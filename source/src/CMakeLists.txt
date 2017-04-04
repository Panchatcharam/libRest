cmake_minimum_required(VERSION 3.5)

project(Rest)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../common/config)

include(BuildConfig)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../include/)

set(CMAKE_STATIC_LIBRARY_PREFIX "lib")
set(LIB_INSTALL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../install)

set(DependLibs
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/curl/libcurl.so
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/curl/libcurl.so.4.4.0
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/openssl/libssl.so
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/openssl/libssl.so.1.0.0
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/openssl/libcrypto.so
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/openssl/libcrypto.so.1.0.0
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/log4cplus/liblog4cplus.so
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/log4cplus/liblog4cplus-1.2.so.5
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/log4cplus/liblog4cplus-1.2.so.5.1.4
    )

set(Libraries
   libcurl.so
   libssl.so
   libcrypto.so
   liblog4cplus.so
   )

set(LibraryDirs
   ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/curl
   ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/openssl
   ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/log4cplus
   )
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
   
find_package(Threads)

link_directories(${LibraryDirs})

# Create Executable
#add_executable(Rest ${ClInclude} ${ClCompile})

# Create static library
add_library(Rest SHARED ${ClCompile} ${ClInclude})

target_link_libraries(Rest ${Libraries} ${CMAKE_THREAD_LIBS_INIT})

set_target_properties(Rest PROPERTIES PUBLIC_HEADER "CommonDefs.h;RestAPIFacade.h")

# Set rpath for the library
set_target_properties(Rest PROPERTIES INSTALL_RPATH  "$ORIGIN/../:$ORIGIN:$ORIGIN/lib/")

# install library and header
install(TARGETS Rest 
        LIBRARY DESTINATION ${LIB_INSTALL_PATH}/lib
        PUBLIC_HEADER DESTINATION ${LIB_INSTALL_PATH}/include)

#install library directory
install(FILES ${DependLibs} DESTINATION ${LIB_INSTALL_PATH}/lib)
