# SPDX-FileCopyrightText: 2026 Generative Bionics S.R.L.
# SPDX-License-Identifier: BSD-3-Clause

add_executable(yarpview_video_test
  video.cpp
  ../plugin/videoproducer.cpp
  ../plugin/videoproducer.h
  ../plugin/signalhandler.cpp
  ../plugin/signalhandler.h
  ../plugin/ImagePort.cpp
)
target_include_directories(yarpview_video_test PRIVATE ../plugin)
target_link_libraries(yarpview_video_test PRIVATE
  YARP::YARP_os YARP::YARP_init YARP::YARP_sig
  Qt${YARP_QT_MAJOR_VERSION}::Gui Qt${YARP_QT_MAJOR_VERSION}::Multimedia
)
add_test(NAME yarpview_video COMMAND yarpview_video_test)
