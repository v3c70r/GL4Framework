find_package(Lua REQUIRED)
include_directories(${LUA_INCLUDE_DIRS})
add_library(console LuaConsole.cpp)
target_link_libraries(console ${LUA_LIBRARIES})
