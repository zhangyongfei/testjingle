cmake_minimum_required(VERSION 2.8)

string(REGEX MATCH ".*/" STUNSERVER_CURPATH ${CMAKE_CURRENT_LIST_FILE})
message(${STUNSERVER_CURPATH})

set(STUNSERVER_COMMON_PATH
)

set(STUNSERVER_SOURCE_LIST
${STUNSERVER_CURPATH}/../../p2p/base/stunserver.cc
${STUNSERVER_CURPATH}/../../p2p/base/stunserver.h

${STUNSERVER_CURPATH}/../../p2p/base/stun.h
${STUNSERVER_CURPATH}/../../p2p/base/stun.cc
${STUNSERVER_CURPATH}/../../base/socketaddress.h
${STUNSERVER_CURPATH}/../../base/socketaddress.cc
${STUNSERVER_CURPATH}/../../base/asyncudpsocket.h
${STUNSERVER_CURPATH}/../../base/asyncudpsocket.cc
${STUNSERVER_CURPATH}/../../base/thread.h
${STUNSERVER_CURPATH}/../../base/thread.cc
${STUNSERVER_CURPATH}/../../base/bytebuffer.h
${STUNSERVER_CURPATH}/../../base/bytebuffer.cc
${STUNSERVER_CURPATH}/../../base/ipaddress.h
${STUNSERVER_CURPATH}/../../base/ipaddress.cc
${STUNSERVER_CURPATH}/../../base/common.h
${STUNSERVER_CURPATH}/../../base/common.cc
${STUNSERVER_CURPATH}/../../base/logging.h
${STUNSERVER_CURPATH}/../../base/logging.cc
${STUNSERVER_CURPATH}/../../base/crc32.h
${STUNSERVER_CURPATH}/../../base/crc32.cc
${STUNSERVER_CURPATH}/../../base/messagedigest.h
${STUNSERVER_CURPATH}/../../base/messagedigest.cc
${STUNSERVER_CURPATH}/../../base/messagehandler.h
${STUNSERVER_CURPATH}/../../base/messagehandler.cc
${STUNSERVER_CURPATH}/../../base/messagequeue.h
${STUNSERVER_CURPATH}/../../base/messagequeue.cc
${STUNSERVER_CURPATH}/../../base/timeutils.h
${STUNSERVER_CURPATH}/../../base/timeutils.cc
${STUNSERVER_CURPATH}/../../base/md5.h
${STUNSERVER_CURPATH}/../../base/md5.cc
${STUNSERVER_CURPATH}/../../base/sha1.h
${STUNSERVER_CURPATH}/../../base/sha1.cc
${STUNSERVER_CURPATH}/../../base/physicalsocketserver.h
${STUNSERVER_CURPATH}/../../base/physicalsocketserver.cc
${STUNSERVER_CURPATH}/../../base/nethelpers.h
${STUNSERVER_CURPATH}/../../base/nethelpers.cc
${STUNSERVER_CURPATH}/../../base/stream.h
${STUNSERVER_CURPATH}/../../base/stream.cc
${STUNSERVER_CURPATH}/../../base/stringencode.h
${STUNSERVER_CURPATH}/../../base/stringencode.cc
${STUNSERVER_CURPATH}/../../base/asyncsocket.cc
${STUNSERVER_CURPATH}/../../base/asyncsocket.h
${STUNSERVER_CURPATH}/../../base/signalthread.h
${STUNSERVER_CURPATH}/../../base/signalthread.cc
)

if(MSVC)
set(STUNSERVER_SOURCE_LIST 
${STUNSERVER_SOURCE_LIST}
${STUNSERVER_CURPATH}/../../os/win32/winping.cc
${STUNSERVER_CURPATH}/../../os/win32/winping.h
${STUNSERVER_CURPATH}/../../os/win32/win32socketinit.cc
${STUNSERVER_CURPATH}/../../os/win32/win32socketinit.h
${STUNSERVER_CURPATH}/../../os/win32/win32.cc
${STUNSERVER_CURPATH}/../../os/win32/win32.h)
else()
set(STUNSERVER_SOURCE_LIST 
${STUNSERVER_SOURCE_LIST})
endif()

include_directories(${STUNSERVER_CURPATH})

if(MSVC)
set(STUNSERVER_RELATIVE_PATH 
${STUNSERVER_COMMON_PATH})
else()
set(STUNSERVER_RELATIVE_PATH 
${STUNSERVER_COMMON_PATH})
endif()

file(GLOB_RECURSE STUNSERVER_HEADER_LIST ${STUNSERVER_RELATIVE_PATH})
set(STUNSERVER_ALL_FILES ${STUNSERVER_HEADER_LIST} ${STUNSERVER_SOURCE_LIST})

add_executable(stunserver stunserver_main.cc ${STUNSERVER_ALL_FILES})
TARGET_LINK_LIBRARIES(stunserver ${LIBS_LIST})


