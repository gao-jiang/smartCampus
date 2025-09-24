cmake_minimum_required(VERSION 3.10)

project(smartCampus)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(smartCampus
    main.cpp
    src/LocationManager.cpp
    src/RankingManager.cpp
    src/ShuttleService.cpp
    src/ScheduleManager.cpp
    src/NavigationGraph.cpp
    src/SearchManager.cpp
    src/SmartCampusAssistant.cpp
    src/SmartCampusMenu.cpp
)

include_directories(include)
