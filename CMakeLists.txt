# The following code is subject to the terms and conditions defined in the
# file 'COPYING' which is part of the source code distribution.

cmake_minimum_required(VERSION 2.6)
project(libsteam)

#libsteam

include_directories("include")
add_library(steam
	src/Steam2/serverclient.c
	src/Steam2/dsclient.c
	src/Steam2/authclient.c
	src/Util/socketutil.c
	src/Util/crypto.c
	src/Common/blob.c
)

#libsteam-test

add_executable(libsteam-test
	test/test.c
)
target_link_libraries(libsteam-test steam crypto)
set_target_properties (libsteam-test PROPERTIES
	COMPILE_FLAGS "--std=gnu99 -Wall"
)
set_target_properties(steam PROPERTIES
	COMPILE_FLAGS "--std=gnu99 -Wall"
)