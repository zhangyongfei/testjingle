cmake_minimum_required(VERSION 2.8)

string(REGEX MATCH ".*/" TURNSERVER_CURPATH ${CMAKE_CURRENT_LIST_FILE})
message(${TURNSERVER_CURPATH})

set(TURNSERVER_COMMON_PATH
)

set(TURNSERVER_SOURCE_LIST
${TURNSERVER_CURPATH}/../../p2p/base/turnserver.cc
${TURNSERVER_CURPATH}/../../p2p/base/turnserver.h

${TURNSERVER_CURPATH}/../../p2p/base/stun.h
${TURNSERVER_CURPATH}/../../p2p/base/stun.cc
${TURNSERVER_CURPATH}/../../base/socketaddress.h
${TURNSERVER_CURPATH}/../../base/socketaddress.cc
${TURNSERVER_CURPATH}/../../base/asyncudpsocket.h
${TURNSERVER_CURPATH}/../../base/asyncudpsocket.cc
${TURNSERVER_CURPATH}/../../base/thread.h
${TURNSERVER_CURPATH}/../../base/thread.cc
${TURNSERVER_CURPATH}/../../base/bytebuffer.h
${TURNSERVER_CURPATH}/../../base/bytebuffer.cc
${TURNSERVER_CURPATH}/../../base/ipaddress.h
${TURNSERVER_CURPATH}/../../base/ipaddress.cc
${TURNSERVER_CURPATH}/../../base/common.h
${TURNSERVER_CURPATH}/../../base/common.cc
${TURNSERVER_CURPATH}/../../base/logging.h
${TURNSERVER_CURPATH}/../../base/logging.cc
${TURNSERVER_CURPATH}/../../base/crc32.h
${TURNSERVER_CURPATH}/../../base/crc32.cc
${TURNSERVER_CURPATH}/../../base/messagedigest.h
${TURNSERVER_CURPATH}/../../base/messagedigest.cc
${TURNSERVER_CURPATH}/../../base/messagehandler.h
${TURNSERVER_CURPATH}/../../base/messagehandler.cc
${TURNSERVER_CURPATH}/../../base/messagequeue.h
${TURNSERVER_CURPATH}/../../base/messagequeue.cc
${TURNSERVER_CURPATH}/../../base/timeutils.h
${TURNSERVER_CURPATH}/../../base/timeutils.cc
${TURNSERVER_CURPATH}/../../base/md5.h
${TURNSERVER_CURPATH}/../../base/md5.cc
${TURNSERVER_CURPATH}/../../base/sha1.h
${TURNSERVER_CURPATH}/../../base/sha1.cc
${TURNSERVER_CURPATH}/../../base/physicalsocketserver.h
${TURNSERVER_CURPATH}/../../base/physicalsocketserver.cc
${TURNSERVER_CURPATH}/../../base/nethelpers.h
${TURNSERVER_CURPATH}/../../base/nethelpers.cc
${TURNSERVER_CURPATH}/../../base/helpers.h
${TURNSERVER_CURPATH}/../../base/helpers.cc
${TURNSERVER_CURPATH}/../../base/stream.h
${TURNSERVER_CURPATH}/../../base/stream.cc
${TURNSERVER_CURPATH}/../../base/stringencode.h
${TURNSERVER_CURPATH}/../../base/stringencode.cc
${TURNSERVER_CURPATH}/../../base/asyncsocket.cc
${TURNSERVER_CURPATH}/../../base/asyncsocket.h
${TURNSERVER_CURPATH}/../../base/signalthread.h
${TURNSERVER_CURPATH}/../../base/signalthread.cc
${TURNSERVER_CURPATH}/../../base/optionsfile.cc
${TURNSERVER_CURPATH}/../../base/optionsfile.h
${TURNSERVER_CURPATH}/../../p2p/base/basicpacketsocketfactory.cc
${TURNSERVER_CURPATH}/../../p2p/base/basicpacketsocketfactory.h
${TURNSERVER_CURPATH}/../../p2p/base/asyncstuntcpsocket.cc
${TURNSERVER_CURPATH}/../../p2p/base/asyncstuntcpsocket.h
${TURNSERVER_CURPATH}/../../base/asynctcpsocket.cc
${TURNSERVER_CURPATH}/../../base/asynctcpsocket.h
${TURNSERVER_CURPATH}/../../base/socketadapters.cc
${TURNSERVER_CURPATH}/../../base/socketadapters.h
${TURNSERVER_CURPATH}/../../base/asynctcpsocket.cc
${TURNSERVER_CURPATH}/../../base/asynctcpsocket.h
${TURNSERVER_CURPATH}/../../base/httpcommon.cc
${TURNSERVER_CURPATH}/../../base/httpcommon.h
${TURNSERVER_CURPATH}/../../base/base64.cc
${TURNSERVER_CURPATH}/../../base/base64.h
)

if(MSVC)
set(TURNSERVER_SOURCE_LIST 
${TURNSERVER_SOURCE_LIST}
${TURNSERVER_CURPATH}/../../os/win32/winping.cc
${TURNSERVER_CURPATH}/../../os/win32/winping.h
${TURNSERVER_CURPATH}/../../os/win32/win32socketinit.cc
${TURNSERVER_CURPATH}/../../os/win32/win32socketinit.h
${TURNSERVER_CURPATH}/../../os/win32/win32.cc
${TURNSERVER_CURPATH}/../../os/win32/win32securityerrors.cc
${TURNSERVER_CURPATH}/../../os/win32/win32.h)
else()
set(TURNSERVER_SOURCE_LIST 
${TURNSERVER_SOURCE_LIST})
endif()

include_directories(${TURNSERVER_CURPATH})

if(MSVC)
set(TURNSERVER_RELATIVE_PATH 
${TURNSERVER_COMMON_PATH})
else()
set(TURNSERVER_RELATIVE_PATH 
${TURNSERVER_COMMON_PATH})
endif()

file(GLOB_RECURSE TURNSERVER_HEADER_LIST ${TURNSERVER_RELATIVE_PATH})
set(TURNSERVER_ALL_FILES ${TURNSERVER_HEADER_LIST} ${TURNSERVER_SOURCE_LIST})

add_executable(turnserver turnserver_main.cc ${TURNSERVER_ALL_FILES})
TARGET_LINK_LIBRARIES(turnserver ${LIBS_LIST})


