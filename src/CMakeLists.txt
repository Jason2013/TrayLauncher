# set_source_files_properties(main.cpp PROPERTIES FOLDER "Source Files")
# set_source_files_properties(main.h PROPERTIES FOLDER "Header Files")

set(PROJ_NAME TrayLauncher)

set(SOURCE_FILES 
    source/FileStrFnc.cpp
    source/GDIWavePic.cpp
    source/Hotkey.cpp
    source/language.cpp
    source/MenuData.cpp
    source/MenuWithIcon.cpp
    source/MsgMap.cpp
    source/OwnerDrawMenu.cpp
    source/pugixml/pugixml.cpp
    source/RunDlg.cpp
    source/SettingFile.cpp
    source/stdafx.cpp
    source/TrayStart.cpp
    source/WavePic.cpp
    source/xmlmenudata.cpp
    )

set_source_files_properties(${SOURCE_FILES} PROPERTIES COMPILE_OPTIONS "/FI\"stdafx.h\"")
set(wxWidgets_ROOT_DIR C:/chenchang/study/TrayLauncher-source/source-archive/wxWidgets-3.2.6-install)

# Find wxWidgets
find_package(wxWidgets REQUIRED COMPONENTS core base)

# Include wxWidgets use file
include(${wxWidgets_USE_FILE})

add_executable(${PROJ_NAME} ${SOURCE_FILES})

target_include_directories(${PROJ_NAME} PRIVATE ../src/include ../src/source include .)

target_compile_definitions(${PROJ_NAME} PRIVATE _UNICODE UNICODE WXUSINGDLL)

target_link_options(${PROJ_NAME} PRIVATE /SUBSYSTEM:WINDOWS)

# Link wxWidgets libraries
target_link_libraries(${PROJ_NAME} ${wxWidgets_LIBRARIES})
