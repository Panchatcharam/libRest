cmake_minimum_required(VERSION 3.5)

project(Rest)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../common/config)

include(BuildConfig)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../include/)

set(CMAKE_STATIC_LIBRARY_PREFIX "lib")
set(LIB_INSTALL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../install)

# Set libs to be installed
file(GLOB InstallLibs "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/log4cplus/liblog4cplus*")

# Find required libraries
set( Libraries liblog4cplus.so )

#Curl libraries
find_package(CURL REQUIRED)
if(CURL_FOUND)
  include_directories( ${CURL_INCLUDE_DIR} )
  message(STATUS "Found Curl version ${CURL_VERSION_STRING}")
  set( Libraries ${Libraries} ${CURL_LIBRARIES} )
else(CURL_FOUND)
  message(FATAL_ERROR "Missing the CURL library and development files.")
endif(CURL_FOUND)

# Find OpenSSL libraries
find_package(OpenSSL REQUIRED)
if(OPENSSL_FOUND)
    include_directories( ${OPENSSL_INCLUDE_DIR} )
    set( Libraries ${Libraries} ${OPENSSL_LIBRARIES} )
    message(STATUS "Found OpenSSL version ${OPENSSL_VERSION}")
else(OPENSSL_FOUND)
    message(FATAL_ERROR "Missing the OpenSSL library and development files.")
endif(OPENSSL_FOUND)

# Find Thread libraries
find_package(Threads REQUIRED)
if( CMAKE_THREAD_LIBS_INIT )
    set( Libraries ${Libraries} ${CMAKE_THREAD_LIBS_INIT} )
    message(STATUS "Found Thread Libraray ${CMAKE_THREAD_LIBS_INIT}")
elseif(CMAKE_THREAD_LIBS_INIT)
    message(FATAL_ERROR "Missing Thread dependencies.")
endif(CMAKE_THREAD_LIBS_INIT)

# Locate library dependencies
set(LibraryDirs ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/log4cplus )

set(ClInclude
   #${CMAKE_CURRENT_SOURCE_DIR}/main.h
   ${CMAKE_CURRENT_SOURCE_DIR}/CommonDefs.h
   ${CMAKE_CURRENT_SOURCE_DIR}/CurlHttpWrapper.h
   #${CMAKE_CURRENT_SOURCE_DIR}/JsonParser.h
   ${CMAKE_CURRENT_SOURCE_DIR}/RestAPIFacade.h
   ${CMAKE_CURRENT_SOURCE_DIR}/Base64.h
   ${CMAKE_CURRENT_SOURCE_DIR}/sha256.h)

set(ClCompile
   #${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/CurlHttpWrapper.cpp
   #${CMAKE_CURRENT_SOURCE_DIR}/JsonParser.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/RestAPIFacade.cpp
   #${CMAKE_CURRENT_SOURCE_DIR}/jsoncpp.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/Base64.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/sha256.cpp)
   
# Set link directories
link_directories(${LibraryDirs})

# Create Executable
#add_executable(Rest ${ClInclude} ${ClCompile})

# Create static library
add_library(Rest SHARED ${ClCompile} ${ClInclude})

target_link_libraries(Rest ${Libraries})

set_target_properties(Rest PROPERTIES PUBLIC_HEADER "CommonDefs.h;RestAPIFacade.h")

# Set rpath for the library
set_target_properties(Rest PROPERTIES INSTALL_RPATH  "$ORIGIN/../:$ORIGIN:$ORIGIN/lib/")

# install library and header
install(TARGETS Rest 
        LIBRARY DESTINATION ${LIB_INSTALL_PATH}/lib
        PUBLIC_HEADER DESTINATION ${LIB_INSTALL_PATH}/include)

#install library directory
install(FILES ${InstallLibs} DESTINATION ${LIB_INSTALL_PATH}/lib)
