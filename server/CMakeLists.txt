cmake_minimum_required(VERSION 3.10)

add_executable(ServerApp 
        main.cpp 
        server.h 
        server.cpp 
        search.h 
        search.cpp 
        utils.h 
        utils.cpp 
        ../config/config.h 
		../config/config.cpp 
		../config/config.ini 
        ../spider/link.h 
        ../spider/link.cpp
)


target_compile_features(ServerApp PRIVATE cxx_std_17)

target_link_libraries(ServerApp pqxx)

target_link_libraries(ServerApp ${OPENSSL_LIBRARIES})

target_include_directories(ServerApp PRIVATE ${Boost_INCLUDE_DIRS})

target_link_directories(ServerApp PRIVATE ${Boost_LIBRARY_DIR})