# gbionics YARP fork

This repository is a fork of [YARP](https://github.com/robotology/yarp) based on
the YARP 3.12 series. See [README.md](README.md) for the upstream project overview
and documentation.

## Building and running tests with Pixi

Install [Pixi](https://pixi.sh), then run from the repository root:

```bash
pixi run test
```

Pixi installs the build dependencies, including OpenCV, Qt, and ccache, and
builds in `build/pixi-default`, with tests, GUIs, the math library, and fake
devices enabled.
The environment supports Linux x86-64, Linux ARM64, macOS on Apple Silicon,
and Windows x86-64. The lockfile records the resolved dependencies for each platform.
CMake and OpenCV have no upper version bounds.

The C and C++ compilers use ccache to reuse compilation results across builds.
On Linux, mold is used to speed up linking. Pixi selects it through CMake's
[`CMAKE_LINKER_TYPE`](https://cmake.org/cmake/help/latest/variable/CMAKE_LINKER_TYPE.html)
option, which requires CMake 3.29 or newer.
Use `pixi run configure` or `pixi run build` to run those steps individually.
The configure task skips searches for optional packages absent from the Pixi
environment, including MPI, to keep configuration fast. If you add one of these
packages, remove its `CMAKE_DISABLE_FIND_PACKAGE_<Package>` flag from `pixi.toml`
and configure once with that flag set to `OFF` to reset the existing CMake cache.
Builds use Ninja's default parallelism, based on the available CPU cores.
Set `CMAKE_BUILD_PARALLEL_LEVEL` to override the number of build jobs.
Use `pixi run ccache --show-stats` to inspect the compiler cache.

### Choosing Qt 5 or Qt 6

The default environment uses Qt 5.15. Named `qt5` and `qt6` environments select
Qt 5.15 and Qt 6 (6.5 or newer), respectively. They use separate build trees,
`build/pixi-qt5` and `build/pixi-qt6` (the default uses `build/pixi-default`),
so you can switch without clearing CMake's cache. For example:

```bash
pixi run -e qt5 test
pixi run -e qt6 test
pixi run -e qt5 demo-yarpmotorgui
pixi run -e qt6 demo-yarpmotorgui
pixi run -e qt5 demo-yarpview
pixi run -e qt6 demo-yarpview
```

Both demos use the binaries from the selected environment's build tree.
`pixi run -e qt6 demo-yarpview --duration 10` runs a timed Qt 6 demo.
The `yarpview_video` CTest checks frame conversion, row alignment, resizing,
and color picking with either Qt version.

For builds outside Pixi, use `-DYARP_QT_VERSION=5` or `-DYARP_QT_VERSION=6`.
Use separate build directories for the two Qt versions. When unset, CMake
prefers Qt 5 and falls back to Qt 6. The Qt Quick GUIs use
Controls 2; Qt 5 builds require version 5.15 or newer.

FFmpeg is constrained to versions older than 9 for compatibility with YARP 3.12.

## Trying the GUIs with fake devices

```bash
pixi run demo-yarpmotorgui
pixi run demo-yarpview
```

Each command builds the demo GUIs and their dependencies, then starts its own
temporary YARP server and fake device. `demo-yarpmotorgui` opens a six-joint fake control board directly. Try changing the
joint position targets and checking the encoder feedback.
`demo-yarpview` connects a fake camera to the viewer and displays a moving line.
A graphical desktop is required.

Close the GUI or press Ctrl+C in its terminal to stop the demo and its helper
processes. The demos use temporary YARP network settings, leaving your usual
YARP server configuration unchanged. They can run alongside each other.
For a timed run, append `--duration 10` to either command.
