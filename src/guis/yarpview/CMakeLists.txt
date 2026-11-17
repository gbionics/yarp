# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(YARP_COMPILE_yarpview)

  include(YarpUseQt)

  set(QtYARPViewPlugin_QMLDIR "robotology/yarp/view")

  add_subdirectory(plugin)
  add_subdirectory(src)
  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

endif()
