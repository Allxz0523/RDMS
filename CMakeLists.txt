cmake_minimum_required(VERSION 3.20)
project(1st_Work)

set(BOOST_ROOT /usr/local/Cellar/boost/1.76.0)
set(Boost_INCLUDE_DIR /usr/local/Cellar/boost/1.76.0/include)
set(Boost_LIBRARY_DIR /usr/local/Cellar/boost/1.76.0/lib)
find_package(Boost 1.76.0  COMPONENTS thread system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

# boost
find_package(Boost 1.71.0 COMPONENTS system filesystem REQUIRED)
if (Boost_FOUND)
    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARY_DIRS}")
    message(STATUS "Boost_VERSION: ${Boost_VERSION}")
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif ()

set(CMAKE_CXX_STANDARD 14)

link_directories(/usr/local/Cellar/boost/1.76.0/lib/)
add_executable(1st_Work main.cpp head.h data_preparation.cpp simulation.cpp data_cleaning.cpp update.cpp deletion_operation.cpp eta_update.cpp)

target_link_libraries(1st_Work boost_thread-mt boost_system)
