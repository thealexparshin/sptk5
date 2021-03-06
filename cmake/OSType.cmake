IF (UNIX)
    EXEC_PROGRAM(uname OUTPUT_VARIABLE SYSTEM_NAME)
    SET (OS_TYPE ${SYSTEM_NAME})
    IF (${SYSTEM_NAME} STREQUAL "Linux")
        SET (LINUX TRUE)
    ELSE ()
        SET (BSD TRUE)
    ENDIF ()
ELSE ()
    SET (OS_TYPE "Windows")
ENDIF ()

MESSAGE ("OS Type:            ${OS_TYPE}")