add_compile_definitions(BOOST_ASIO_HAS_MOVE)
add_compile_definitions(BOOST_THREAD_PROVIDES_FUTURE)
add_compile_definitions(BOOST_THREAD_PROVIDES_FUTURE_UNWRAP)
add_compile_definitions(BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION)

add_compile_definitions(MLN_NET_USE_JSONPARSER_BOOSTJSON)
add_compile_definitions(MLN_USE_BEAST_WEBSOCKET)

#ADD_DEFINITIONS(-DBoost_USE_STATIC_LIBS=ON)
#SET(Boost_ADDITIONAL_VERSIONS "1.78" "1.78.0")

add_compile_definitions(METABOLOTS_USE_PARAM_CHECKER)

if(WIN32)
    #SET(Boost_USE_STATIC_LIBS        ON)  # only find static libs
else()
    set(CMAKE_MODULE_PATH "/usr/local/share/cmake/redis++/;/usr/lib/x86_64-linux-gnu/cmake/Hiredis/;${CMAKE_MODULE_PATH}")
endif()
# set(Boost_USE_DEBUG_LIBS         OFF) # ignore debug libs and
# set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
set(Boost_USE_MULTITHREADED      ON)
# set(Boost_USE_STATIC_RUNTIME    OFF)


macro(configure_files srcDir destDir)
    message(STATUS "Configuring directory ${destDir}")
    make_directory(${destDir})

    file(GLOB templateFiles RELATIVE ${srcDir} "${srcDir}/*")
    foreach(templateFile ${templateFiles})
        set(srcTemplatePath ${srcDir}/${templateFile})
        if(NOT IS_DIRECTORY ${srcTemplatePath})
            message(STATUS "Configuring file ${templateFile}")
            configure_file(
                    ${srcTemplatePath}
                    ${destDir}/${templateFile}
                    @ONLY)
        endif(NOT IS_DIRECTORY ${srcTemplatePath})
    endforeach(templateFile)
endmacro(configure_files)



find_package(redis++ CONFIG REQUIRED)

find_package(Boost 1.80 REQUIRED system date_time thread chrono json context coroutine)
if(Boost_FOUND)
    message(STATUS "---------------------")
    message(STATUS "Boost_FOUND: ${Boost_FOUND}")
    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(STATUS "---------------------")

    include_directories(${Boost_INCLUDE_DIRS} 
        "${CMAKE_SOURCE_DIR}/mlnserver/"
        "${CMAKE_SOURCE_DIR}/include/"
    )
    message("${CMAKE_SOURCE_DIR}/include/")

    add_executable(mlnserver
        configuration.cpp
        main.cpp
        mysqlConn/mysqlConnMgr.cpp
        redisConnector.cpp 
        mysqlConn/mysqlConn.cpp
        lobbyServerCoro.cpp
        sessionManager.cpp 
        tableManager.cpp 
        myPacketHandler/handlePlayer.cpp
        myPacketHandler/handleAccount.cpp
    )

     add_subdirectory("boost_mysql")

     target_compile_definitions(mlnserver PRIVATE BOOST_ASIO_DISABLE_CONCEPTS)

     if(MSVC)
        target_compile_definitions(mlnserver
            PRIVATE
                WIN32_LEAN_AND_MEAN
                NOMINMAX
                _CRT_SECURE_NO_WARNINGS 
        )
     endif()

     target_link_libraries(mlnserver
        ${Boost_LIBRARIES}
        redis++::redis++
        Boost::mysql
    )
    
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/netconfig.json
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )

     configure_files("${CMAKE_CURRENT_SOURCE_DIR}/myDesignTable/Json" "${CMAKE_CURRENT_BINARY_DIR}/myDesignTable/Json" )
    
elseif()
    message("Boost NOT Found!")
endif()

