# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  [[CMakeFiles\vole_gui_autogen.dir\AutogenUsed.txt]]
  [[CMakeFiles\vole_gui_autogen.dir\ParseCache.txt]]
  "vole_gui_autogen"
  )
endif()
