cmake_minimum_required(VERSION 2.6)
project(libsteam)

#libsteam

include_directories("include")
add_library(steam
	src/Steam2/serverclient.c
	src/Steam2/dsclient.c
	src/Util/socketutil.c
	src/Util/crypto.c
)

#libsteam-test

add_executable(libsteam-test
	test/test.c
)
target_link_libraries(libsteam-test steam)