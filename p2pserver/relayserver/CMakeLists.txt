cmake_minimum_required(VERSION 2.8)

string(REGEX MATCH ".*/" RELAYSERVER_CURPATH ${CMAKE_CURRENT_LIST_FILE})
message(${RELAYSERVER_CURPATH})

set(RELAYSERVER_COMMON_PATH
)

set(RELAYSERVER_SOURCE_LIST
${RELAYSERVER_CURPATH}/../../p2p/base/relayserver.cc
${RELAYSERVER_CURPATH}/../../p2p/base/relayserver.h

${RELAYSERVER_CURPATH}/../../p2p/base/stun.h
${RELAYSERVER_CURPATH}/../../p2p/base/stun.cc
${RELAYSERVER_CURPATH}/../../base/socketaddress.h
${RELAYSERVER_CURPATH}/../../base/socketaddress.cc
${RELAYSERVER_CURPATH}/../../base/asyncudpsocket.h
${RELAYSERVER_CURPATH}/../../base/asyncudpsocket.cc
${RELAYSERVER_CURPATH}/../../base/thread.h
${RELAYSERVER_CURPATH}/../../base/thread.cc
${RELAYSERVER_CURPATH}/../../base/bytebuffer.h
${RELAYSERVER_CURPATH}/../../base/bytebuffer.cc
${RELAYSERVER_CURPATH}/../../base/ipaddress.h
${RELAYSERVER_CURPATH}/../../base/ipaddress.cc
${RELAYSERVER_CURPATH}/../../base/common.h
${RELAYSERVER_CURPATH}/../../base/common.cc
${RELAYSERVER_CURPATH}/../../base/logging.h
${RELAYSERVER_CURPATH}/../../base/logging.cc
${RELAYSERVER_CURPATH}/../../base/crc32.h
${RELAYSERVER_CURPATH}/../../base/crc32.cc
${RELAYSERVER_CURPATH}/../../base/messagedigest.h
${RELAYSERVER_CURPATH}/../../base/messagedigest.cc
${RELAYSERVER_CURPATH}/../../base/messagehandler.h
${RELAYSERVER_CURPATH}/../../base/messagehandler.cc
${RELAYSERVER_CURPATH}/../../base/messagequeue.h
${RELAYSERVER_CURPATH}/../../base/messagequeue.cc
${RELAYSERVER_CURPATH}/../../base/timeutils.h
${RELAYSERVER_CURPATH}/../../base/timeutils.cc
${RELAYSERVER_CURPATH}/../../base/md5.h
${RELAYSERVER_CURPATH}/../../base/md5.cc
${RELAYSERVER_CURPATH}/../../base/sha1.h
${RELAYSERVER_CURPATH}/../../base/sha1.cc
${RELAYSERVER_CURPATH}/../../base/physicalsocketserver.h
${RELAYSERVER_CURPATH}/../../base/physicalsocketserver.cc
${RELAYSERVER_CURPATH}/../../base/nethelpers.h
${RELAYSERVER_CURPATH}/../../base/nethelpers.cc
${RELAYSERVER_CURPATH}/../../base/helpers.h
${RELAYSERVER_CURPATH}/../../base/helpers.cc
${RELAYSERVER_CURPATH}/../../base/stream.h
${RELAYSERVER_CURPATH}/../../base/stream.cc
${RELAYSERVER_CURPATH}/../../base/stringencode.h
${RELAYSERVER_CURPATH}/../../base/stringencode.cc
${RELAYSERVER_CURPATH}/../../base/asyncsocket.cc
${RELAYSERVER_CURPATH}/../../base/asyncsocket.h
${RELAYSERVER_CURPATH}/../../base/signalthread.h
${RELAYSERVER_CURPATH}/../../base/signalthread.cc
${RELAYSERVER_CURPATH}/../../base/optionsfile.cc
${RELAYSERVER_CURPATH}/../../base/optionsfile.h
${RELAYSERVER_CURPATH}/../../p2p/base/basicpacketsocketfactory.cc
${RELAYSERVER_CURPATH}/../../p2p/base/basicpacketsocketfactory.h
${RELAYSERVER_CURPATH}/../../p2p/base/asyncstuntcpsocket.cc
${RELAYSERVER_CURPATH}/../../p2p/base/asyncstuntcpsocket.h
${RELAYSERVER_CURPATH}/../../base/asynctcpsocket.cc
${RELAYSERVER_CURPATH}/../../base/asynctcpsocket.h
${RELAYSERVER_CURPATH}/../../base/socketadapters.cc
${RELAYSERVER_CURPATH}/../../base/socketadapters.h
${RELAYSERVER_CURPATH}/../../base/asynctcpsocket.cc
${RELAYSERVER_CURPATH}/../../base/asynctcpsocket.h
${RELAYSERVER_CURPATH}/../../base/httpcommon.cc
${RELAYSERVER_CURPATH}/../../base/httpcommon.h
${RELAYSERVER_CURPATH}/../../base/base64.cc
${RELAYSERVER_CURPATH}/../../base/base64.h
${RELAYSERVER_CURPATH}/../../base/socketaddresspair.cc
${RELAYSERVER_CURPATH}/../../base/socketaddresspair.h
)

if(MSVC)
set(RELAYSERVER_SOURCE_LIST 
${RELAYSERVER_SOURCE_LIST}
${RELAYSERVER_CURPATH}/../../os/win32/winping.cc
${RELAYSERVER_CURPATH}/../../os/win32/winping.h
${RELAYSERVER_CURPATH}/../../os/win32/win32socketinit.cc
${RELAYSERVER_CURPATH}/../../os/win32/win32socketinit.h
${RELAYSERVER_CURPATH}/../../os/win32/win32.cc
${RELAYSERVER_CURPATH}/../../os/win32/win32securityerrors.cc
${RELAYSERVER_CURPATH}/../../os/win32/win32.h)
else()
set(RELAYSERVER_SOURCE_LIST 
${RELAYSERVER_SOURCE_LIST})
endif()

include_directories(${RELAYSERVER_CURPATH})

if(MSVC)
set(RELAYSERVER_RELATIVE_PATH 
${RELAYSERVER_COMMON_PATH})
else()
set(RELAYSERVER_RELATIVE_PATH 
${RELAYSERVER_COMMON_PATH})
endif()

file(GLOB_RECURSE RELAYSERVER_HEADER_LIST ${RELAYSERVER_RELATIVE_PATH})
set(RELAYSERVER_ALL_FILES ${RELAYSERVER_HEADER_LIST} ${RELAYSERVER_SOURCE_LIST})

add_executable(relayserver relayserver_main.cc ${RELAYSERVER_ALL_FILES})
TARGET_LINK_LIBRARIES(relayserver ${LIBS_LIST})


