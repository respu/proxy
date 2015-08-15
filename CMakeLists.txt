project(proxy)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)

set(Boost_USE_STATIC_LIBS        OFF)
set(Boost_USE_MULTITHREADED      ON)

find_package(Boost 1.55.0 REQUIRED COMPONENTS system thread log)
if(Boost_FOUND)
    add_definitions(-DBOOST_LOG_DYN_LINK)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(${PROJECT_NAME} ${SRC_LIST})
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
endif()


