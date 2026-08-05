# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\SmartBackupManager_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\SmartBackupManager_autogen.dir\\ParseCache.txt"
  "SmartBackupManager_autogen"
  )
endif()
