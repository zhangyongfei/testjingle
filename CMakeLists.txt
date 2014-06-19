cmake_minimum_required(VERSION 2.8)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/CMakeUtility)
include(CMakeUtility)

Solution(jingle ON)

AddHDirWin32(
"E:/project/Expat 2.1.0/Source/lib"
"D:/work/tools/Expat 2.1.0/Source/lib"
${CMAKE_CURRENT_LIST_DIR}/third_party/)
AddHDirLinux(
/home/zyf/github/extends/include
)

AddLibDirsWin32(
"E:/project/Expat 2.1.0/Bin"
"D:/work/tools/Expat 2.1.0/Bin")
AddLibDirsLinux(
/home/zyf/github/extends/lib)

AddSrcDir(SOURCE_LIST
${CMAKE_CURRENT_LIST_DIR}/xmpp
${CMAKE_CURRENT_LIST_DIR}/base
${CMAKE_CURRENT_LIST_DIR}/xmllite
${CMAKE_CURRENT_LIST_DIR}/p2p/base
${CMAKE_CURRENT_LIST_DIR}/p2p/client
${CMAKE_CURRENT_LIST_DIR}/base/jsoncpp
${CMAKE_CURRENT_LIST_DIR}/session/tunnel 
${CMAKE_CURRENT_LIST_DIR}/session/fileshare
${CMAKE_CURRENT_LIST_DIR}/session/media
${CMAKE_CURRENT_LIST_DIR}/media/base
${CMAKE_CURRENT_LIST_DIR}/media/sctp
${CMAKE_CURRENT_LIST_DIR}/media/devices
${CMAKE_CURRENT_LIST_DIR}/test)

AddSrcDirWin32(SOURCE_LIST
${CMAKE_CURRENT_LIST_DIR}/base/win32
${CMAKE_CURRENT_LIST_DIR}/media/devices/win32)
AddSrcDirLinux(SOURCE_LIST
${CMAKE_CURRENT_LIST_DIR}/base/linux
${CMAKE_CURRENT_LIST_DIR}/media/devices/linux
${CMAKE_CURRENT_LIST_DIR}/sound
)

SET(LIBS_LIST)
AddLibsWin32(LIBS_LIST
usrsctplib
libexpat
Secur32
ws2_32
Iphlpapi
wininet
Winmm
crypt32
Shlwapi
strmiids)

AddLibsLinux(LIBS_LIST
usrsctplib 
pthread
expat
dl
X11
Xext
crypto
ssl
rt
Xrender
Xcomposite)

MESSAGE(STATUS "${LIBS_LIST}")

#add_executable(register example/register.cc ${ALL_FILES})
#TARGET_LINK_LIBRARIES(register ${LIBS_LIST})

#add_executable(login example/login_main.cc ${ALL_FILES})
#TARGET_LINK_LIBRARIES(login ${LIBS_LIST})

#add_executable(fileshare example/filesharedemo.cc ${ALL_FILES})
#TARGET_LINK_LIBRARIES(fileshare ${LIBS_LIST})

AddSrc(SOURCE_LIST
${CMAKE_CURRENT_LIST_DIR}/example/testdemo.cc)

AppPrj(testdemo SOURCE_LIST LIBS_LIST)

AddSubPrj(./third_party/usrsctplib)
#add_dependencies(testdemo usrsctplib)



