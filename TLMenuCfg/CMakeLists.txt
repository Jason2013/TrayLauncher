set_source_files_properties(main.cpp PROPERTIES FOLDER "Source Files")
set_source_files_properties(main.h PROPERTIES FOLDER "Header Files")

set(PROJ_NAME TLMenuCfg)

set(SOURCE_FILES 
		../src/source/FileStrFnc.cpp
		../src/source/FileStrFnc.h
		../src/source/MenuData.cpp
		../src/source/MenuData.h
		../src/source/SettingFile.cpp
		../src/source/SettingFile.h
		../src/source/language.cpp
		../src/source/language.h
		../src/source/pugixml/pugiconfig.hpp
		../src/source/pugixml/pugixml.cpp
		../src/source/pugixml/pugixml.hpp
		../src/source/xmlmenudata.cpp
		../src/source/xmlmenudata.h
        src/MenuItemData.cpp
        # wx_pch.h
		TLMenuCfgApp.cpp
		TLMenuCfgApp.h
		TLMenuCfgMain.cpp
		TLMenuCfgMain.h
		include/MenuItemData.h
		resource.rc
    )

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
