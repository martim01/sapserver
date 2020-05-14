cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(sapserver LANGUAGES CXX)

add_library(sapserver SHARED "src/parser.cpp" "src/receiver.cpp" "src/saphandler.cpp" "src/sapserver.cpp" "src/sapserverimpl.cpp" "src/sender.cpp")

target_include_directories(sapserver PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(sapserver PUBLIC ${PROJECT_SOURCE_DIR}/../log/include)

# ASIO
find_path(ASIO_INCLUDE_DIR NAMES "asio.hpp" PATHS ${CMAKE_CURRENT_SOURCE_DIR}/external/asio/asio/include)
message(${CMAKE_CURRENT_SOURCE_DIR}/external/asio/asio/include)

if((NOT ASIO_INCLUDE_DIR) OR (NOT EXISTS ${ASIO_INCLUDE_DIR}))
    # we couldn't find the header files for FOO or they don't exist
    message("Unable to find Asio")
    

    # we have a submodule setup for asio, assume it is under external/asio
    # now we need to clone this submodule
    execute_process(COMMAND git submodule update --init -- external/asio
                    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
else()
	message("Found Asio")
endif()

# set ASIO_INCLUDE_DIR properly
set(ASIO_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/external/asio/asio/include CACHE PATH "asio include directory")


target_include_directories(sapserver PUBLIC ${ASIO_INCLUDE_DIR})


list(APPEND flags "-Wall" "-s" "-O3" "-fexceptions" "-std=c++14")
target_compile_options(sapserver PRIVATE ${flags})



#linux specific
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	target_compile_definitions(sapserver PRIVATE __GNU__)
endif()

set_target_properties(sapserver PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib/)
target_link_libraries(sapserver pml_log)

#linux specific
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	install(TARGETS sapserver LIBRARY DESTINATION /usr/local/lib)
endif()
