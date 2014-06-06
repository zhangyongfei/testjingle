cmake_minimum_required(VERSION 2.8)
PROJECT(jingle)

set(CMAKE_BUILD_TYPE DEBUG)
if(MSVC)
set(CMAKE_C_FLAGS "-DHAVE_SCTP -DWIN32 -D_DEBUG -DDEBUG -DFEATURE_ENABLE_SSL")
set(CMAKE_CXX_FLAGS "-DHAVE_SCTP -DWIN32 -D_DEBUG -DDEBUG -DFEATURE_ENABLE_SSL /EHsc")
else()
set(CMAKE_C_FLAGS "-O0 -ggdb -DHAVE_SCTP -D_DEBUG -DDEBUG -DFEATURE_ENABLE_SSL -DPOSIX -DLINUX -DHAVE_OPENSSL_SSL_H")
set(CMAKE_CXX_FLAGS "-O0 -ggdb -DHAVE_SCTP -D_DEBUG -DDEBUG -DFEATURE_ENABLE_SSL -DPOSIX -DLINUX -DHAVE_OPENSSL_SSL_H")
set(CMAKE_C_FLAGS_DEBUG "-O0 -ggdb")
set(CMAKE_C_FLAGS_RELEASE "-O0 -ggdb")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O0 -ggdb")
endif()

string(REGEX MATCH ".*/" curpath ${CMAKE_CURRENT_LIST_FILE})
message(${curpath})
if(MSVC)
set(buildpath ${curpath}/build)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${buildpath})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${buildpath})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${buildpath})
else()
set(buildpath $ENV{PWD})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${buildpath}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${buildpath}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${buildpath}/bin)
endif()


if(MSVC)
include_directories(
"E:/project/Expat 2.1.0/Source/lib" 
"D:/work/tools/Expat 2.1.0/Source/lib" 
${curpath}
${curpath}/third_party/)
link_directories("E:/project/Expat 2.1.0/Bin" "D:/work/tools/Expat 2.1.0/Bin" ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
else()
include_directories(
/home/zyf/github/extends/include 
${curpath} 
${curpath}/third_party/)
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} /home/zyf/github/extends/lib)
endif()

AUX_SOURCE_DIRECTORY(./xmpp SOURCE_LIST)
AUX_SOURCE_DIRECTORY(./base SOURCE_LIST)
if(MSVC)
AUX_SOURCE_DIRECTORY(./os/win32 SOURCE_LIST)
AUX_SOURCE_DIRECTORY(./devices/win32 SOURCE_LIST)
else()
AUX_SOURCE_DIRECTORY(./os/linux SOURCE_LIST)
AUX_SOURCE_DIRECTORY(./devices/linux SOURCE_LIST)
AUX_SOURCE_DIRECTORY(./sound SOURCE_LIST)
endif()
AUX_SOURCE_DIRECTORY(./xmllite SOURCE_LIST)
AUX_SOURCE_DIRECTORY(./p2p/base SOURCE_LIST)
AUX_SOURCE_DIRECTORY(./p2p/client SOURCE_LIST)
AUX_SOURCE_DIRECTORY(./base/jsoncpp SOURCE_LIST)
AUX_SOURCE_DIRECTORY(./session/tunnel SOURCE_LIST)
AUX_SOURCE_DIRECTORY(./session/fileshare SOURCE_LIST)
AUX_SOURCE_DIRECTORY(./session/media SOURCE_LIST)
AUX_SOURCE_DIRECTORY(./media/base SOURCE_LIST)
AUX_SOURCE_DIRECTORY(./media/sctp SOURCE_LIST)
AUX_SOURCE_DIRECTORY(./media/devices SOURCE_LIST)
AUX_SOURCE_DIRECTORY(./test SOURCE_LIST)

set(COMMON_PATH
${curpath}/xmpp/*.h
${curpath}/base/*.h
${curpath}/xmllite/*.h
${curpath}/p2p/base/*.h
${curpath}/p2p/client/*.h
${curpath}/base/jsoncpp/*.h
${curpath}/session/tunne/*.h
${curpath}/session/fileshare/*.h
${curpath}/session/media/*.h
${curpath}/media/base/*.h
${curpath}/media/sctp/*.h
${curpath}/media/devices/*.h
)

if(MSVC)
set(RELATIVE_PATH 
${COMMON_PATH}
${curpath}/devices/win32/*.h
${curpath}/os/win32/*.h)
set(LIBS_LIST
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
else()
set(RELATIVE_PATH 
${COMMON_PATH}
${curpath}/devices/linux/*.h
${curpath}/sound/*.h
${curpath}/os/linux/*.h)
set(LIBS_LIST
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
endif()

file(GLOB_RECURSE HEADER_LIST ${RELATIVE_PATH})
set(ALL_FILES ${HEADER_LIST} ${SOURCE_LIST})

#add_executable(register example/register.cc ${ALL_FILES})
#TARGET_LINK_LIBRARIES(register ${LIBS_LIST})

#add_executable(login example/login_main.cc ${ALL_FILES})
#TARGET_LINK_LIBRARIES(login ${LIBS_LIST})

#add_executable(fileshare example/filesharedemo.cc ${ALL_FILES})
#TARGET_LINK_LIBRARIES(fileshare ${LIBS_LIST})

add_executable(testdemo example/testdemo.cc ${ALL_FILES})
TARGET_LINK_LIBRARIES(testdemo ${LIBS_LIST})
add_dependencies(testdemo usrsctplib)

add_subdirectory(./third_party/usrsctplib)



