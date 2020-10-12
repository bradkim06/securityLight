# CMake에서 주석은 #을 사용해 작성합니다
set(AUTHOR_NAME "bradkim06")
cmake_minimum_required(VERSION 3.14)    # CMake 버전을 명시
set(CMAKE_C_STANDARD 99)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_definitions(__MSP430F6736__)
set(CCS_BASE_ROOT /mnt/c/ti/ccsv8/ccs_base)
set(PROJECT_ROOT /mnt/c/Users/User/workspace_v8/securityLight)
set(CG_TOOL_ROOT /mnt/c/ti/ccsv8/tools/compiler/ti-cgt-msp430_18.12.0.LTS)
# set(EMSWLIB_ROOT
#     /mnt/c/ti/msp/EnergyMeasurementDesignCenter_1.40.00.03/EnergyMeasurementDesignCenter/example_projects/EnergyMeasurementDesignCenterWorkspace/EVM430-F6736_CT_1V_1C_60Hz/src/lib/EM)
# set(MATHLIB_ROOT
#     /mnt/c/ti/msp/EnergyMeasurementDesignCenter_1.40.00.03/EnergyMeasurementDesignCenter/example_projects/EnergyMeasurementDesignCenterWorkspace/EVM430-F6736_CT_1V_1C_60Hz/src/lib/mathlib)
project(securityLight)


file(GLOB_RECURSE SRC CONFIGURE_DEPENDS
    "*.h"
    "*.c"
    )
SET(App ${SRC})
ADD_EXECUTABLE(App ${SRC})
include_directories(
    ${CCS_BASE_ROOT}/msp430/include
    ${PROJECT_ROOT}
    ${PROJECT_ROOT}/App
    ${PROJECT_ROOT}/App/comm
    ${PROJECT_ROOT}/App/shell
    ${PROJECT_ROOT}/include
    ${PROJECT_ROOT}/driverlib/MSP430F5xx_6xx
    ${PROJECT_ROOT}/driverlib/MSP430F5xx_6xx/inc
    # ${EMSWLIB_ROOT}/include
    # ${MATHLIB_ROOT}/include
    ${PROJECT_ROOT}/EM_Config
    ${PROJECT_ROOT}/hal/MSP430F5xx_6xx
    ${CG_TOOL_ROOT}/include
    )
