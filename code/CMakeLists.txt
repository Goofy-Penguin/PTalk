file(GLOB_RECURSE source_files
	"src/*.cpp"
	"src/*.c"
	"include/*.hpp"
)

# dll
set(output_target mainframe.ptalk)
add_library(${output_target} ${source_files})
target_include_directories(${output_target} PUBLIC "include")
target_link_libraries(${output_target} PUBLIC mainframe.networking mainframe.utils nlohmann_json::nlohmann_json fmt::fmt)

if (build_tests)
	add_subdirectory("tests")
endif()
