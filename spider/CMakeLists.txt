cmake_minimum_required(VERSION 3.10)


add_executable(SpiderApp 
			main.cpp 
			link.h
			link.cpp 
			crawler.h 
			crawler.cpp 
			../config/config.h 
			../config/config.cpp 
			../config/config.ini 
			../database/database.h 
			../database/database.cpp
)

target_compile_features(SpiderApp PRIVATE cxx_std_17)

target_link_libraries(SpiderApp pqxx)

target_link_libraries(SpiderApp ${OPENSSL_LIBRARIES})

target_include_directories(SpiderApp PRIVATE ${Boost_INCLUDE_DIRS})

target_link_directories(SpiderApp PRIVATE ${Boost_LIBRARY_DIR})

#target_link_libraries(SpiderApp Boost::locale)
