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

set(wxBUILD_SHARED OFF)
# set(wxBUILD_SAMPLES OFF CACHE BOOL "" FORCE)
# set(wxBUILD_DEMOS OFF CACHE BOOL "" FORCE)
# set(wxBUILD_TOOLS OFF CACHE BOOL "" FORCE)
# set(wxUSE_GUI ON CACHE BOOL "" FORCE)
# set(wxUSE_NO_MANIFEST ON CACHE BOOL "" FORCE)
# set(wxUSE_HTML OFF CACHE BOOL "" FORCE)
# set(wxUSE_MEDIACTRL OFF CACHE BOOL "" FORCE)
# set(wxUSE_XRC OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(wxWidgets)

add_executable(${PROJ_NAME} ${SOURCE_FILES})

target_include_directories(${PROJ_NAME} PRIVATE    
    ../src/include    
    ../src/source    
    include    
    .    
    ${wxWidgets_SOURCE_DIR}/include)

add_definitions(-DwxNO_UNSAFE_WXUSINGDLL)
target_compile_definitions(${PROJ_NAME} PRIVATE _UNICODE UNICODE)

target_link_options(${PROJ_NAME} PRIVATE /SUBSYSTEM:WINDOWS)

# Link wxWidgets libraries
target_link_libraries(${PROJ_NAME} PRIVATE
    wx::core
    wx::base
)

set_target_properties(${PROJ_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
)
