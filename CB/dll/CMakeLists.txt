set_source_files_properties(main.cpp PROPERTIES FOLDER "Source Files")
set_source_files_properties(main.h PROPERTIES FOLDER "Header Files")
add_library(TL SHARED main.cpp main.h)
target_compile_definitions(TL PRIVATE BUILD_DLL)
