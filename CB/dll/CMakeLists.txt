set(PROJ_NAME TL)

set_source_files_properties(main.cpp PROPERTIES FOLDER "Source Files")
set_source_files_properties(main.h PROPERTIES FOLDER "Header Files")

add_library(${PROJ_NAME} SHARED main.cpp main.h)

target_compile_definitions(${PROJ_NAME} PRIVATE BUILD_DLL)

set_target_properties(${PROJ_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
)
