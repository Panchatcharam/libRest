This is the restAPI library which uses libcurl and openSSL for making sucure http connection to server.
Presently we use http connection with security added in the header.  

The following are the steps involved to build RestAPI lib using CMake
1. Create a new directory called build at the source directory level (i.e. build include lib source README.md)
2. cd to build/
3. run below command to create build files
   i. To create eclipse project with DEBUG build run below, use Release for Release build
      #cmake -G"Eclipse CDT4 - Unix Makefiles" -D CMAKE_BUILD_TYPE=Debug ../source/src/
   ii. To create simple Makefile project run below
      #cmake -G"Unix Makefiles" -D CMAKE_BUILD_TYPE=Debug ../source/src/
4. Build by typing make or make install
5. make install will create new directory 'install' at the soruce level which will contain all the dependencies.
