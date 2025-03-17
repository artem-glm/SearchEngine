cmake_minimum_required(VERSION 3.10)

project(SearchEngine)


set(Boost_LIBRARY_DIR "F:\\boost_1_87_0\\lib64-msvc-14.3") 
set(Boost_INCLUDE_DIR "F:\\boost_1_87_0")
set(BOOST_ROOT "F:\\boost_1_87_0")
set(OPENSSL_ROOT_DIR "F:\\Program Files\\OpenSSL-Win64")
set(libpqxx_DIR "F:\\libpqxx-7.9.0")

# Boost:
find_package(Boost 1.87.0 REQUIRED COMPONENTS system thread locale filesystem)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Could not find Boost")
endif()

# OpenSSL
find_package(OpenSSL REQUIRED)

include_directories(${OPENSSL_INCLUDE_DIR})

# PQXX
set(PostgreSQL_ADDITIONAL_VERSIONS "16")

add_subdirectory(${libpqxx_DIR} libpqxx-build)


add_subdirectory(spider)

add_subdirectory(server)
