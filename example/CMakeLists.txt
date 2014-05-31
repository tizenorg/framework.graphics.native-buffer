CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(TC C)

INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}/include
)

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs REQUIRED
	turbojpeg
	opengl-es-20
	x11
)

FOREACH(flag ${pkgs_CFLAGS})
	SET(EXTRA_FLAGS "${EXTRA_FLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_FLAGS} -Wall -g -Wno-write-strings")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_FLAGS} -g")

##### TC1 - native buffer test
SET(TC1_SRCS
	native-buffer-test.c
	native-buffer-test-common.c
)

SET(TC1_BIN "native-buffer-test")

ADD_EXECUTABLE(${TC1_BIN} ${TC1_SRCS})

SET_TARGET_PROPERTIES(${TC1_BIN} PROPERTIES INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)
TARGET_LINK_LIBRARIES(${TC1_BIN}
	${pkgs_LDFLAGS}
	native-buffer
)

INSTALL(TARGETS ${TC1_BIN} DESTINATION bin)
