find_package(Threads)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(cyboto
    physical_components_manager.cpp
    functions_manager.cpp
    emergency_situation_manager.cpp
    physical_component.cpp
    base_function.cpp
    core.cpp
    base_component.cpp
    parsers.cpp
    common.cpp
    event_manager.cpp
    trigger.cpp
    function_factory.cpp
)

target_link_libraries(cyboto Threads::Threads)

