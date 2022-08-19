cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 17)
project (mc7010tool LANGUAGES C CXX VERSION 1.0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Disable search for boost-cmake
# Fixes building with Boost >= 1.70.0
set(Boost_NO_BOOST_CMAKE ON)

find_package(Boost REQUIRED program_options)
find_package(CURL 7.32.0 REQUIRED)
find_package(Jsoncpp REQUIRED)
find_package(Rhash REQUIRED)

file(GLOB SRC_FILES
  main.cpp
  src/mc7010.cpp
  src/util.cpp
  )

set(GIT_CHECKOUT FALSE)
if(EXISTS ${PROJECT_SOURCE_DIR}/.git)
  if(NOT EXISTS ${PROJECT_SOURCE_DIR}/.git/shallow)
    find_package(Git)
    if(GIT_FOUND)
      set(GIT_CHECKOUT TRUE)
    else(GIT_FOUND)
      message(WARNING "Git executable not found")
    endif(GIT_FOUND)
  else(NOT EXISTS ${PROJECT_SOURCE_DIR}/.git/shallow)
    message(STATUS "Shallow Git clone detected, not attempting to retrieve version info")
  endif(NOT EXISTS ${PROJECT_SOURCE_DIR}/.git/shallow)
endif(EXISTS ${PROJECT_SOURCE_DIR}/.git)

if(GIT_CHECKOUT)
  execute_process(COMMAND ${GIT_EXECUTABLE} diff --shortstat
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_SHORTSTAT
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_REV_PARSE
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  if(GIT_SHORTSTAT)
    set(GIT_DIRTY ON)
  endif(GIT_SHORTSTAT)

  if(GIT_DIRTY)
    set(PROJECT_VERSION_MINOR ${PROJECT_VERSION_MINOR}M)
  endif(GIT_DIRTY)

  set(PROJECT_VERSION_PATCH ${GIT_REV_PARSE})
  set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})
endif(GIT_CHECKOUT)

set(VERSION_NUMBER ${PROJECT_VERSION})
set(VERSION_STRING "MC7010Tool ${VERSION_NUMBER}")

add_definitions(-D_FILE_OFFSET_BITS=64 -DVERSION_NUMBER="${VERSION_NUMBER}" -DVERSION_STRING="${VERSION_STRING}")

add_executable (${PROJECT_NAME} ${SRC_FILES})


target_include_directories(${PROJECT_NAME}
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE ${Boost_INCLUDE_DIRS}
  PRIVATE ${CURL_INCLUDE_DIRS}
  PRIVATE ${Jsoncpp_INCLUDE_DIRS}
  PRIVATE ${Rhash_INCLUDE_DIRS}
  )

target_link_libraries(${PROJECT_NAME}
  PRIVATE ${Boost_LIBRARIES}
  PRIVATE ${CURL_LIBRARIES}
  PRIVATE ${Jsoncpp_LIBRARIES}
  PRIVATE ${Rhash_LIBRARIES}
  )

if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-long-long -fexceptions")
endif()

set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_SHARE_DIR share CACHE PATH "Installation directory for resource files")

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX} DESTINATION ${INSTALL_BIN_DIR})
