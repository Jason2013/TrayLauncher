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

# set(wxWidgets_ROOT_DIR C:/chenchang/study/TrayLauncher-source/source-archive/wxWidgets-3.2.6-install)
include(FetchContent)
FetchContent_Declare(wxWidgets
    GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
    GIT_TAG v3.2.6
)

# set(wxBUILD_TESTS OFF CACHE BOOL "" FORCE)
# set(wxBUILD_SAMPLES OFF CACHE BOOL "" FORCE)
# set(wxBUILD_DEMOS OFF CACHE BOOL "" FORCE)
# set(wxBUILD_TOOLS OFF CACHE BOOL "" FORCE)
# set(wxUSE_GUI ON CACHE BOOL "" FORCE)
# set(wxUSE_NO_MANIFEST ON CACHE BOOL "" FORCE)
# set(wxUSE_HTML OFF CACHE BOOL "" FORCE)
# set(wxUSE_MEDIACTRL OFF CACHE BOOL "" FORCE)
# set(wxUSE_XRC OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(wxWidgets)

# Find wxWidgets
# find_package(wxWidgets REQUIRED COMPONENTS core base)

# Include wxWidgets use file
#include(${wxWidgets_USE_FILE})

add_executable(${PROJ_NAME} ${SOURCE_FILES})

# target_include_directories(MyApp PRIVATE
#     ${wxWidgets_SOURCE_DIR}/include
# )

target_include_directories(${PROJ_NAME} PRIVATE    
    ../src/include    
    ../src/source    
    include    
    .    
    ${wxWidgets_SOURCE_DIR}/include)

target_compile_definitions(${PROJ_NAME} PRIVATE _UNICODE UNICODE WXUSINGDLL)

target_link_options(${PROJ_NAME} PRIVATE /SUBSYSTEM:WINDOWS)

# Link wxWidgets libraries
# target_link_libraries(${PROJ_NAME} ${wxWidgets_LIBRARIES})
target_link_libraries(${PROJ_NAME} PRIVATE
    wx::core
    wx::base
)
