# set_source_files_properties(main.cpp PROPERTIES FOLDER "Source Files")
# set_source_files_properties(main.h PROPERTIES FOLDER "Header Files")

set(PROJ_NAME TrayLauncher)

set(HEADER_FILES
    source/appstrings.h
    source/FileStrFnc.h
    source/GDIWavePic.h
    source/Hotkey.h
    source/language.h
    source/MenuData.h
    source/MenuWithIcon.h
    source/MsgMap.h
    source/OwnerDrawMenu.h
    source/Resource.h
    source/RunDlg.h
    source/SettingFile.h
    source/stdafx.h
    source/TrayStart.h
    source/WavePic.h
    )

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

add_executable(${PROJ_NAME} ${SOURCE_FILES} ${HEADER_FILES} source/TrayStart.rc)

target_include_directories(${PROJ_NAME} PRIVATE include source source/rc)

target_compile_definitions(${PROJ_NAME} PRIVATE _UNICODE UNICODE)

target_link_options(${PROJ_NAME} PRIVATE /SUBSYSTEM:WINDOWS)

target_link_libraries(${PROJ_NAME} Shlwapi.lib Comctl32.lib)

set_target_properties(${PROJ_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
)

function(copy_folder folder depend_lst)

    set(_updated_depends "${${depend_lst}}")

    file(GLOB_RECURSE ALL_FILES "${CMAKE_SOURCE_DIR}/${folder}/*")
    foreach(file_path IN LISTS ALL_FILES)
        # message(STATUS "Found file: ${file_path}")
        file(RELATIVE_PATH rel_path "${CMAKE_SOURCE_DIR}" "${file_path}")
        message(STATUS "Found file: ${rel_path}")

        set(_output_path "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/${rel_path}")
        set(_input_path "${CMAKE_SOURCE_DIR}/${rel_path}")

        add_custom_command(
            OUTPUT "${_output_path}"
            COMMAND ${CMAKE_COMMAND} -E echo "Copy ${_input_path} to ${_output_path}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_input_path}" "${_output_path}"
            DEPENDS "${_input_path}"
        )
        list(APPEND _updated_depends "${_output_path}")
    endforeach()

    set(${depend_lst} "${_updated_depends}" PARENT_SCOPE)
endfunction()

set(depends "")

copy_folder(intro depends)
copy_folder(lng depends)
copy_folder(skin depends)

add_custom_target(copy_resources DEPENDS "${depends}")
add_dependencies(${PROJ_NAME} copy_resources)
