﻿file(GLOB_RECURSE source_files
    "src/*.cpp"
    "src/*.c"
    "include/*.hpp"
)

set(output_target ptalkPingPongTest)
add_executable(${output_target} ${source_files})
target_link_libraries(${output_target} PRIVATE mainframe.networking mainframe.utils mainframe.ptalk nlohmann_json::nlohmann_json fmt::fmt)

MFAddExecutable(${output_target})
