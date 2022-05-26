#/*************************************************************************************************/
#/*!
# *  Copyright (c) 2022 Delft University of Technology.
# *  Jasper de Winkel, Haozhe Tang and Przemyslaw Pawelczak
# *
# *  Licensed under the Apache License, Version 2.0 (the "License");
# *  you may not use this file except in compliance with the License.
# *  You may obtain a copy of the License at
# *
# *      http://www.apache.org/licenses/LICENSE-2.0
# *
# *  Unless required by applicable law or agreed to in writing, software
# *  distributed under the License is distributed on an "AS IS" BASIS,
# *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# *  See the License for the specific language governing permissions and
# *  limitations under the License.
# */
#/*************************************************************************************************/

target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/platform/include
)

#--------------------------------------------------------------------------------------------------
#   Includes
#--------------------------------------------------------------------------------------------------

target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/platform/targets/nordic/include
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/uecc
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/nordic-bsp/components/boards
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/nordic-bsp/components/drivers_nrf/nrf_soc_nosd
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/nordic-bsp/components/libraries/bsp
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/nordic-bsp/components/libraries/crypto
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/nordic-bsp/components/libraries/util
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/nordic-bsp/components/toolchain
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/nordic-bsp/components/toolchain/cmsis/include
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/nordic-bsp/external/nrf_cc310/include
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/nordic-bsp/modules/nrfx
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/nordic-bsp/modules/nrfx/drivers/include
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/nordic-bsp/modules/nrfx/drivers/src
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/nordic-bsp/modules/nrfx/drivers/src/prs
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/nordic-bsp/modules/nrfx/hal
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/nordic-bsp/modules/nrfx/mdk
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/nordic-bsp/modules/nrfx/templates
)

#--------------------------------------------------------------------------------------------------
#	BSP
#--------------------------------------------------------------------------------------------------

target_sources(${PROJECT_NAME}
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/platform/targets/nordic/build/retarget_gcc.c
	# PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/nordic-bsp/modules/nrfx/hal/nrf_ecb.c
	# PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/nordic-bsp/modules/nrfx/drivers/src/nrfx_gpiote.c
	# PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/nordic-bsp/modules/nrfx/drivers/src/nrfx_i2s.c
	# PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/nordic-bsp/modules/nrfx/drivers/src/nrfx_spim.c
	# PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/nordic-bsp/modules/nrfx/drivers/src/nrfx_twim.c
	# PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/nordic-bsp/modules/nrfx/drivers/src/nrfx_uarte.c
	# PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/nordic-bsp/modules/nrfx/drivers/src/prs/nrfx_prs.c
	PRIVATE ${CMAKE_SOURCE_DIR}/ckpt-os/thirdparty/nordic-bsp/modules/nrfx/hal/nrf_ecb.c
	PRIVATE ${CMAKE_SOURCE_DIR}/ckpt-os/thirdparty/nordic-bsp/modules/nrfx/drivers/src/nrfx_gpiote.c
	PRIVATE ${CMAKE_SOURCE_DIR}/ckpt-os/thirdparty/nordic-bsp/modules/nrfx/drivers/src/nrfx_i2s.c
	PRIVATE ${CMAKE_SOURCE_DIR}/ckpt-os/thirdparty/nordic-bsp/modules/nrfx/drivers/src/nrfx_spim.c
	PRIVATE ${CMAKE_SOURCE_DIR}/ckpt-os/thirdparty/nordic-bsp/modules/nrfx/drivers/src/nrfx_twim.c
	PRIVATE ${CMAKE_SOURCE_DIR}/ckpt-os/thirdparty/nordic-bsp/modules/nrfx/drivers/src/nrfx_uarte.c
	PRIVATE ${CMAKE_SOURCE_DIR}/ckpt-os/thirdparty/nordic-bsp/modules/nrfx/drivers/src/prs/nrfx_prs.c
)

if(${CHIP} STREQUAL NRF52832_XXAA)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/nordic-bsp/config/nrf52832/config
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/nordic-bsp/modules/nrfx/templates/nRF52832
)

target_sources(${PROJECT_NAME}
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/nordic-bsp/modules/nrfx/mdk/system_nrf52.c

)
if(NOT CHECKPOINTING)
target_sources(${PROJECT_NAME}
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/platform/targets/nordic/build/startup_gcc_nrf52832.c
)
endif()
else() # Default is NRF52840_XXAA

target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/nordic-bsp/config/nrf52840/config
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/nordic-bsp/modules/nrfx/templates/nRF52840
)

target_sources(${PROJECT_NAME}
	# PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/nordic-bsp/modules/nrfx/drivers/src/nrfx_qspi.c
	PRIVATE ${CMAKE_SOURCE_DIR}/ckpt-os/thirdparty/nordic-bsp/modules/nrfx/drivers/src/nrfx_qspi.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/nordic-bsp/modules/nrfx/mdk/system_nrf52840.c
)
if(NOT CHECKPOINTING)
target_sources(${PROJECT_NAME}
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/platform/targets/nordic/build/startup_gcc_nrf52840.c
)
endif()

#LIBS  += \
#	$(BSP_DIR)/external/nrf_cc310/lib/libnrf_cc310_softfp_0.9.10.a

endif()

#--------------------------------------------------------------------------------------------------
#	Platform Abstraction Layer (PAL)
#--------------------------------------------------------------------------------------------------

target_sources(${PROJECT_NAME}
	# PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/platform/targets/nordic/sources/pal_btn.c
	# PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/platform/targets/nordic/sources/pal_cfg.c
	# PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/platform/targets/nordic/sources/pal_codec.c
	# PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/platform/targets/nordic/sources/pal_crypto.c
	# PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/platform/targets/nordic/sources/pal_led.c
	# PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/platform/targets/nordic/sources/pal_i2s.c
	# PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/platform/targets/nordic/sources/pal_rtc.c
	# PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/platform/targets/nordic/sources/pal_spi.c
	# PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/platform/targets/nordic/sources/pal_twi.c
	# PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/platform/targets/nordic/sources/pal_uart.c
	PRIVATE ${CMAKE_SOURCE_DIR}/ckpt-os/platform/targets/nordic/sources/pal_btn.c
	PRIVATE ${CMAKE_SOURCE_DIR}/ckpt-os/platform/targets/nordic/sources/pal_cfg.c
	PRIVATE ${CMAKE_SOURCE_DIR}/ckpt-os/platform/targets/nordic/sources/pal_codec.c
	PRIVATE ${CMAKE_SOURCE_DIR}/ckpt-os/platform/targets/nordic/sources/pal_crypto.c
	PRIVATE ${CMAKE_SOURCE_DIR}/ckpt-os/platform/targets/nordic/sources/pal_led.c
	PRIVATE ${CMAKE_SOURCE_DIR}/ckpt-os/platform/targets/nordic/sources/pal_i2s.c
	PRIVATE ${CMAKE_SOURCE_DIR}/ckpt-os/platform/targets/nordic/sources/pal_rtc.c
	PRIVATE ${CMAKE_SOURCE_DIR}/ckpt-os/platform/targets/nordic/sources/pal_spi.c
	PRIVATE ${CMAKE_SOURCE_DIR}/ckpt-os/platform/targets/nordic/sources/pal_twi.c
	PRIVATE ${CMAKE_SOURCE_DIR}/ckpt-os/platform/targets/nordic/sources/pal_uart.c
)

if(NOT CHECKPOINTING)
target_sources(${PROJECT_NAME}
	# PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/platform/targets/nordic/sources/pal_sys.c
	PRIVATE ${CMAKE_SOURCE_DIR}/ckpt-os/platform/targets/nordic/sources/pal_sys.c
)
endif()

if (NOT ${USE_EXACTLE} EQUAL 0)
target_sources(${PROJECT_NAME}
	# PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/platform/targets/nordic/sources/pal_timer.c
	# PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/platform/targets/nordic/sources/pal_bb.c
	# PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/platform/targets/nordic/sources/pal_bb_ble.c
	# PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/platform/targets/nordic/sources/pal_bb_ble_rf.c
	# PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/platform/targets/nordic/sources/pal_bb_ble_tester.c
	PRIVATE ${CMAKE_SOURCE_DIR}/ckpt-os/platform/targets/nordic/sources/pal_timer.c
	PRIVATE ${CMAKE_SOURCE_DIR}/ckpt-os/platform/targets/nordic/sources/pal_bb.c
	PRIVATE ${CMAKE_SOURCE_DIR}/ckpt-os/platform/targets/nordic/sources/pal_bb_ble.c
	PRIVATE ${CMAKE_SOURCE_DIR}/ckpt-os/platform/targets/nordic/sources/pal_bb_ble_rf.c
	PRIVATE ${CMAKE_SOURCE_DIR}/ckpt-os/platform/targets/nordic/sources/pal_bb_ble_tester.c
)
endif()

if(${CHIP} STREQUAL NRF52840_XXAA)
target_sources(${PROJECT_NAME}
	# PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/platform/targets/nordic/sources/pal_flash.c
	# PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/platform/targets/nordic/sources/pal_bb_154.c
	PRIVATE ${CMAKE_SOURCE_DIR}/ckpt-os/platform/targets/nordic/sources/pal_flash.c
	PRIVATE ${CMAKE_SOURCE_DIR}/ckpt-os/platform/targets/nordic/sources/pal_bb_154.c
)
endif()

#--------------------------------------------------------------------------------------------------
#	Audio
#--------------------------------------------------------------------------------------------------

if (${AUDIO_CAPE} EQUAL 1)

# Board Support

target_sources(${PROJECT_NAME}
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/platform/external/sources/io_exp/pca9557.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/platform/external/sources/codec/max9867.c
)

if (${CODEC} STREQUAL FRAUNHOFER)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/fraunhofer/
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/fraunhofer/basic_op
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/fraunhofer/arm_inc
)

file(GLOB FRAUNHOFER_SRC
" ${CMAKE_SOURCE_DIR}/thirdparty/fraunhofer/*.[c]"
)

target_sources(${PROJECT_NAME} PRIVATE ${FRAUNHOFER_SRC})

endif()

if (${CODEC} STREQUAL BLUEDROID)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/bluedroid/encoder/include
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/bluedroid/decoder/include
)

file(GLOB BLUEDROID_SRC
" ${CMAKE_SOURCE_DIR}/thirdparty/bluedroid/encoder/srce/*.[c]"
" ${CMAKE_SOURCE_DIR}/thirdparty/bluedroid/decoder/srce/*.[c]"
)

target_sources(${PROJECT_NAME} PRIVATE ${BLUEDROID_SRC})

endif()

endif()

#--------------------------------------------------------------------------------------------------
#	Third Party: Security
#--------------------------------------------------------------------------------------------------

target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/uecc
)

target_sources(${PROJECT_NAME}
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/uecc/uECC_ll.c
	)
