cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(sapserver LANGUAGES CXX)

SET(DIR_BASE $ENV{HOME} CACHE STRING "base location for libraries")
SET(DIR_LOG ${DIR_BASE}/log CACHE STRING "location of pml log")
SET(DIR_ASIO ${DIR_BASE}/asio CACHE STRING "location of asio")

SET(BUILD_LOG ON CACHE BOOL "set to OFF if pml::log built by parent project")

add_library(sapserver SHARED "src/parser.cpp" "src/receiver.cpp" "src/saphandler.cpp" "src/sapserver.cpp" "src/sapserverimpl.cpp" "src/sender.cpp")

target_include_directories(sapserver PUBLIC ${PROJECT_SOURCE_DIR}/include)


#Get log
find_path(TEMP_DIR7 NAMES "CMakeLists.txt" PATHS ${DIR_LOG})
if((NOT TEMP_DIR7) OR (NOT EXISTS ${TEMP_DIR7}))
	message("log not found - clone from GitHub")
	execute_process(COMMAND git clone https://github.com/martim01/log/ ${DIR_LOG})
endif()
unset(TEMP_DIR7)

find_path(TEMP_DIR8 NAMES "CMakeLists.txt" PATHS ${DIR_LOG})
if((NOT TEMP_DIR8) OR (NOT EXISTS ${TEMP_DIR8}))
	message(FATAL_ERROR "Failed to clone pml::Log")
endif()
unset(TEMP_DIR8)

if(${BUILD_LOG})
	add_subdirectory(${DIR_LOG} ${DIR_LOG}/build)
else()
	message("Restgoose - pml::log being build elsewhere")
endif()


# ASIO
message(${DIR_ASIO}/asio/include)
find_path(TEMP_DIR NAMES "asio.hpp" PATHS ${DIR_ASIO}/asio/include)
if((NOT TEMP_DIR) OR (NOT EXISTS ${TEMP_DIR}))
	message("asio not found - clone from GitHub")
	execute_process(COMMAND git clone https://github.com/chriskohlhoff/asio ${DIR_ASIO})
endif()

find_path(TEMP_DIR2 NAMES "asio.hpp" PATHS ${TEMP_DIR})
if((NOT TEMP_DIR2) OR (NOT EXISTS ${TEMP_DIR2}))
	message(FATAL_ERROR "Failed to clone pml::Log")
endif()
unset(TEMP_DIR2)
message("Asio found at " ${TEMP_DIR})
target_include_directories(sapserver PUBLIC ${PROJECT_SOURCE_DIR}/include)
include_directories(${DIR_LOG}/include)
target_include_directories(sapserver PUBLIC ${DIR_ASIO}/asio/include)



list(APPEND flags "-fPIC" "-Wall" "-fpermissive"  "-std=c++14")

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
