cmake_minimum_required(VERSION 3.19)
project(SmartBackupManager LANGUAGES CXX)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)

qt_standard_project_setup()

qt_add_executable(SmartBackupManager
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    .gitignore
    core/backup/BackupService.h
    core/backup/BackupService.cpp
    core/hashing/HashService.h
    core/hashing/HashService.cpp
    core/backup/BackupWorker.h
    core/backup/BackupWorker.cpp
    database/DatabaseManager.cpp
    database/DatabaseManager.h
    models/BackupRecord.h


)

target_link_libraries(SmartBackupManager
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Sql
)

include(GNUInstallDirs)

install(TARGETS SmartBackupManager
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET SmartBackupManager
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
