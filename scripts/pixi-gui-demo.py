# SPDX-FileCopyrightText: Generative Bionics S.R.L.
# SPDX-License-Identifier: BSD-3-Clause

"""Launch a GUI with fake devices on a temporary, private YARP network."""

import argparse
import os
from pathlib import Path
import signal
import socket
import subprocess
import tempfile
import time


def demo(gui, duration):
    environment = os.environ.get("PIXI_ENVIRONMENT_NAME", "default")
    build = Path(__file__).resolve().parents[1] / "build" / f"pixi-{environment}"
    suffix = ".exe" if os.name == "nt" else ""
    executables = {name: build / "bin" / (name + suffix)
                   for name in ("yarp", "yarpserver", "yarpdev", gui)}
    for path in executables.values():
        if not path.is_file():
            raise RuntimeError(f"Missing {path}. Run 'pixi run -e {environment} build-gui-demos' first.")

    with tempfile.TemporaryDirectory(prefix="yarp-pixi-demo-") as directory:
        env = os.environ.copy()
        env["YARP_CONFIG_HOME"] = directory
        env["YARP_DATA_HOME"] = str(Path(directory) / "data")
        env["YARP_NAMESPACE"] = "/pixi-demo"
        env["YARP_DATA_DIRS"] = os.pathsep.join(filter(None, (
            str(build / "share" / "yarp"), env.get("YARP_DATA_DIRS"))))
        children = []

        def start(name, *args):
            print(f"Starting {name}: {' '.join(args)}", flush=True)
            child = subprocess.Popen(
                [str(executables[name]), *args], env=env,
                start_new_session=(os.name != "nt"))
            children.append((name, child))
            return child

        def check_children(ignore=None):
            for name, child in children:
                if child is ignore:
                    continue
                if child.poll() is not None:
                    raise RuntimeError(f"{name} exited with status {child.returncode}.")

        def wait_for_port(port):
            deadline = time.monotonic() + 30
            while time.monotonic() < deadline:
                check_children()
                try:
                    result = subprocess.run(
                        [str(executables["yarp"]), "exists", port], env=env,
                        stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
                        timeout=2)
                    if result.returncode == 0:
                        return
                except subprocess.TimeoutExpired:
                    pass
                time.sleep(0.2)
            raise RuntimeError(f"Timed out waiting for YARP port {port}.")

        try:
            with socket.socket() as listener:
                listener.bind(("127.0.0.1", 0))
                server_port = listener.getsockname()[1]
            start("yarpserver", "--ip", "127.0.0.1", "--socket", str(server_port),
                  "--write", "--portdb", ":memory:", "--subdb", ":memory:")
            wait_for_port("/pixi-demo")

            if gui == "yarpmotorgui":
                start("yarpdev", "--device", "deviceBundler",
                      "--wrapper_device", "controlBoard_nws_yarp",
                      "--attached_device", "fakeMotionControl", "--name", "/pixi-demo/arm",
                      "--GENERAL::Joints", "6")
                wait_for_port("/pixi-demo/arm/rpc:i")
                config = Path(directory) / "yarpmotorgui.ini"
                config.write_text("robot pixi-demo\nparts (arm)\n", encoding="utf-8")
                window = start(gui, "--from", str(config), "--skip_description_server", "--skip")
                wait_for_port("/yarpmotorgui0//pixi-demo/arm/rpc:o")
            else:
                start("yarpdev", "--device", "deviceBundler",
                      "--wrapper_device", "frameGrabber_nws_yarp",
                      "--attached_device", "fakeFrameGrabber", "--name", "/pixi-demo/camera",
                      "--fakeFrameGrabber_rpc_port", "/pixi-demo/camera/control",
                      "--width", "320", "--height", "240", "--period", "0.033",
                      "--mode", "line")
                wait_for_port("/pixi-demo/camera")
                window = start(gui, "--name", "/pixi-demo/view")
                wait_for_port("/pixi-demo/view")
                subprocess.run(
                    [str(executables["yarp"]), "connect", "/pixi-demo/camera",
                     "/pixi-demo/view"], env=env, check=True, timeout=10)

            print("Demo ready. Close the GUI or press Ctrl+C to stop it.", flush=True)
            deadline = None if duration is None else time.monotonic() + duration
            while window.poll() is None:
                check_children(ignore=window)
                if deadline is not None and time.monotonic() >= deadline:
                    return 0
                time.sleep(0.2)
            if window.returncode != 0:
                raise RuntimeError(f"{gui} exited with status {window.returncode}.")
            return 0
        finally:
            # Stop clients before their devices and the name server.
            for _, child in reversed(children):
                if child.poll() is None:
                    try:
                        if os.name == "nt":
                            child.terminate()
                        else:
                            child.send_signal(signal.SIGINT)
                    except ProcessLookupError:
                        pass
                    try:
                        child.wait(timeout=5)
                    except subprocess.TimeoutExpired:
                        child.kill()
                        child.wait()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("gui", choices=("yarpmotorgui", "yarpview"))
    parser.add_argument("--duration", type=float,
                        help="Automatically close the demo after this many seconds")
    args = parser.parse_args()
    if args.duration is not None and args.duration <= 0:
        parser.error("--duration must be positive")

    def interrupt(signum, frame):
        raise KeyboardInterrupt

    signal.signal(signal.SIGTERM, interrupt)
    try:
        return demo(args.gui, args.duration)
    except KeyboardInterrupt:
        return 130
    except (OSError, RuntimeError, subprocess.SubprocessError) as error:
        parser.exit(1, f"Demo failed: {error}\n")


if __name__ == "__main__":
    raise SystemExit(main())
