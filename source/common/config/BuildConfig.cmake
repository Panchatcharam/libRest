if (WIN32)
message(STATUS "Compiling on Windows")
elseif(UNIX)
    if (NOT APPLE AND ${CMAKE_SYSTEM_NAME} STREQUAL "Linux") 
    
        message(STATUS "Compiling on Linux")

        if ( CMAKE_SIZEOF_VOID_P EQUAL 8 )
            add_definitions( -DPLATFORM64 )
        else()      
            add_definitions( -DPLATFORM32 )
        endif()

        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -ldl -v -pedantic -Werror -o0 -g -std=c++11" )
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated -Wno-error=sign-compare")
    
    endif()
endif()
