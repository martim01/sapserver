cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(sapserver LANGUAGES CXX VERSION 1.2.1)
include(FetchContent)
message(STATUS "----pml/sapserver ----")

execute_process(COMMAND ${CMAKE_COMMAND} -DNAMESPACE=sapserver -DMAJOR=${PROJECT_VERSION_MAJOR} -DMINOR=${PROJECT_VERSION_MINOR} -DPATCH=${PROJECT_VERSION_PATCH} -P ${CMAKE_CURRENT_SOURCE_DIR}/version.cmake)

SET(DIR_BASE ${CMAKE_SOURCE_DIR} CACHE STRING "base location for external libraries' source")
SET(DIR_LOG ${DIR_BASE}/external/log CACHE STRING "location of pml log")
SET(DIR_ASIO ${DIR_BASE}/external/asio CACHE STRING "location of asio")

add_library(sapserver SHARED "src/parser.cpp" "src/receiver.cpp" "src/saphandler.cpp" "src/sapserver.cpp" "src/sapserverimpl.cpp" "src/sender.cpp" ${CMAKE_BINARY_DIR}/src/sapserver_version.cpp)

target_include_directories(sapserver PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(sapserver PUBLIC ${CMAKE_BINARY_DIR}/include)

#Get log
FetchContent_Declare(log GIT_REPOSITORY "https://github.com/martim01/log.git"  SOURCE_DIR ${DIR_LOG})
FetchContent_Declare(asio GIT_REPOSITORY "https://github.com/chriskohlhoff/asio.git"  SOURCE_DIR ${DIR_ASIO})

FetchContent_GetProperties(log)
if(NOT log_POPULATED)
	FetchContent_MakeAvailable(asio)
endif()

FetchContent_GetProperties(asio)
if(NOT asio_POPULATED)
	FetchContent_Populate(asio)
endif()


target_include_directories(sapserver PUBLIC ${PROJECT_SOURCE_DIR}/include)
include_directories(${DIR_LOG}/include)
target_include_directories(sapserver PUBLIC ${DIR_ASIO}/asio/include)



list(APPEND flags "-fPIC" "-Wall" "-fpermissive"  "-std=c++17")

if(CMAKE_BUILD_TYPE MATCHES Release)
   list(APPEND flags "-O3")
   target_compile_definitions(sapserver PUBLIC NDEBUG DLL_EXPORTS _MSL_STDINT_H)
else()
   list(APPEND flags "-g")
   target_compile_definitions(sapserver PUBLIC DEBUG DLL_EXPORTS _MSL_STDINT_H)
endif()

target_compile_options(sapserver PRIVATE ${flags})
target_compile_definitions(sapserver PUBLIC ASIO_STANDALONE)

target_link_libraries(sapserver optimized pml_log debug pml_logd)
target_compile_options(sapserver PRIVATE ${flags})



#linux specific
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	target_compile_definitions(sapserver PRIVATE __GNU__)
endif()

set_target_properties(sapserver PROPERTIES DEBUG_POSTFIX "d")
set_target_properties(sapserver PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/)


#linux specific
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	install(TARGETS sapserver LIBRARY DESTINATION /usr/local/lib)
endif()
