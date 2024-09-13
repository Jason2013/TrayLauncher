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

add_executable(${PROJ_NAME} ${SOURCE_FILES} source/TrayStart.rc)

target_include_directories(${PROJ_NAME} PRIVATE include source source/rc)

target_compile_definitions(${PROJ_NAME} PRIVATE _UNICODE UNICODE WXUSINGDLL)

target_link_options(${PROJ_NAME} PRIVATE /SUBSYSTEM:WINDOWS)

target_link_libraries(${PROJ_NAME} Shlwapi.lib Comctl32.lib)
