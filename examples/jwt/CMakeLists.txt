INCLUDE_DIRECTORIES (${SPTK_SOURCE_DIR})
INCLUDE_DIRECTORIES (/usr/local/include)

LINK_DIRECTORIES (${SPTK_BINARY_DIR})

SET (JWT_TESTS jwt_test)

IF (UNIX)
    LINK_LIBRARIES (pthread rt)
ENDIF (UNIX)
LINK_LIBRARIES (sputil5)

ADD_EXECUTABLE (jwt_test jwt_test.cpp)

