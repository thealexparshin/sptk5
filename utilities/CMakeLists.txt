INCLUDE_DIRECTORIES (${SPTK_SOURCE_DIR})
INCLUDE_DIRECTORIES (/usr/local/include)

LINK_DIRECTORIES (${SPTK_BINARY_DIR})

IF (UNIX)
    LINK_LIBRARIES (pthread rt)
ENDIF (UNIX)

ADD_EXECUTABLE (wsdl2cxx wsdl2cxx.cpp)
TARGET_LINK_LIBRARIES (wsdl2cxx sputil5 spwsdl5 spdb5)

FILE (GLOB utilities "${CMAKE_SOURCE_DIR}/utilities/wsdl2cxx")
INSTALL(TARGETS wsdl2cxx
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
  )

ADD_SUBDIRECTORY (load_balance)

