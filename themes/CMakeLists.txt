ADD_EXECUTABLE (tar2h tar2h.cpp ../src/sputil/net/CachedSSLContext.cpp ../sptk5/net/CachedSSLContext.h)
TARGET_LINK_LIBRARIES (tar2h sputil5)

ADD_SUBDIRECTORY (Blue)
ADD_SUBDIRECTORY (Dark)
ADD_SUBDIRECTORY (Default)
ADD_SUBDIRECTORY (E17)
ADD_SUBDIRECTORY (Flat)
ADD_SUBDIRECTORY (Keramic)
ADD_SUBDIRECTORY (OSX)
ADD_SUBDIRECTORY (OSX_Flat)
ADD_SUBDIRECTORY (Opera)
