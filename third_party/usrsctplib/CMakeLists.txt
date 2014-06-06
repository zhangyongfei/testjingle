cmake_minimum_required(VERSION 2.8)

add_definitions("-DSCTP_DEBUG")
add_definitions("-DSCTP_SIMPLE_ALLOCATOR")
add_definitions("-DSCTP_PROCESS_LEVEL_LOCKS")
add_definitions("-D__Userspace__")
if(MSVC)
add_definitions("-D__Userspace_os_Windows")
else()
add_definitions("-D__Userspace_os_Linux")
endif()
add_definitions("-DINET")
add_definitions("-DINET6")
add_definitions("-D_LIB")

string(REGEX MATCH ".*/" SCTP_CURPATH ${CMAKE_CURRENT_LIST_FILE})
message(${SCTP_CURPATH})

AUX_SOURCE_DIRECTORY(./netinet SCTP_SOURCE_LIST)
AUX_SOURCE_DIRECTORY(./netinet6 SCTP_SOURCE_LIST)
AUX_SOURCE_DIRECTORY(. SCTP_SOURCE_LIST)

set(SCTP_COMMON_PATH
${SCTP_CURPATH}/netinet/*.h
${SCTP_CURPATH}/netinet6/*.h
${SCTP_CURPATH}/*.h
)

include_directories(${SCTP_CURPATH} ${SCTP_CURPATH}/netinet ${SCTP_CURPATH}/netinet6)

if(MSVC)
set(SCTP_RELATIVE_PATH 
${SCTP_COMMON_PATH})
else()
set(SCTP_RELATIVE_PATH 
${SCTP_COMMON_PATH})
endif()

file(GLOB_RECURSE SCTP_HEADER_LIST ${SCTP_RELATIVE_PATH})
set(SCTP_ALL_FILES ${SCTP_HEADER_LIST} ${SCTP_SOURCE_LIST})

add_library(usrsctplib STATIC ${SCTP_ALL_FILES})



