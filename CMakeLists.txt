cmake_minimum_required(VERSION 2.8)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(DEFINED ENV{DOLCESDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{DOLCESDK}/share/dolce.toolchain.cmake" CACHE PATH "toolchain file")
    else()
    message(FATAL_ERROR "Please define DOLCESDK to point to your SDK path!")
    endif()
endif()

project(QuickBackup)

include("${DOLCESDK}/share/dolce.cmake" REQUIRED)

set(VITA_APP_NAME "Quick Backup")
set(VITA_TITLEID  "GRVA00012")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O3 -fno-builtin -Wno-pointer-sign")

link_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
	)

add_executable(${PROJECT_NAME}
	src/main.c
	)

target_link_libraries(${PROJECT_NAME}
	SceLibc_stub
	SceLibKernel_stub
	SceSysmem_stub
	SceKernelThreadMgr_stub
	SceKernelModuleMgr_stub
	SceProcessMgr_stub
	SceSysmodule_stub
	SceAppMgrUser_stub
	SceRtabi_stub
	SceIncomingDialog_stub
	SceAppMgr_stub
    )
	
set_target_properties(${PROJECT_NAME}
    PROPERTIES LINK_FLAGS "-nostdlib"
    )

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/boot_param.bin
    COMMAND dolce-make-bootparam app_memsize 0x1000 attribute 0x02 ${CMAKE_CURRENT_BINARY_DIR}/boot_param.bin
    )

dolce_create_self(eboot.bin ${PROJECT_NAME} UNSAFE UNCOMPRESSED BOOT_PARAM ${CMAKE_CURRENT_BINARY_DIR}/boot_param.bin)

set(DOLCE_MKSFOEX_FLAGS "${DOLCE_MKSFOEX_FLAGS} -d ATTRIBUTE=38912 -s CATEGORY=gdc -d LAREA_TYPE=1")

dolce_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} eboot.bin
    VERSION ${VITA_VERSION}
    NAME ${VITA_APP_NAME}
    )
