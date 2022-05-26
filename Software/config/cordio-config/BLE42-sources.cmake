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

if(USE_EXACTLE)
target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/include/common
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/include/ble
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/include
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/uecc
)
target_sources(${PROJECT_NAME}
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/bb/bb_ble_adv_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/bb/bb_ble_adv_slave.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/bb/bb_ble_conn_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/bb/bb_ble_conn_slave.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/bb/bb_ble_conn.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/bb/bb_ble_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/bb/bb_ble_dtm.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/bb/bb_ble_whitelist.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/bb/bb_ble_reslist.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/bb/bb_ble_periodiclist.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/bb/bb_ble_pdufilt.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/init/init_ctr.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/init/init.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_act_adv_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_act_adv_slave.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_act_conn_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_act_conn.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_act_enc_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_act_enc.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_act_init_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_isr_adv_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_isr_adv_slave.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_isr_conn_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_isr_conn_slave.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_isr_conn.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_isr_init_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_main_adv_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_main_adv_slave.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_main_conn_data.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_main_conn_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_main_conn_slave.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_main_conn.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_main_enc_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_main_enc_slave.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_main_init_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_main_priv.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_main_sc.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_pdu_adv_slave.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_pdu_conn.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_pdu_enc.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_sm_adv_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_sm_adv_slave.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_sm_conn_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_sm_conn_slave.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_sm_init_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_sm_llcp_conn_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_sm_llcp_conn_slave.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_sm_llcp_conn.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_sm_llcp_enc_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lctr/lctr_sm_llcp_enc_slave.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_cmd_adv_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_cmd_adv_priv.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_cmd_adv_slave.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_cmd_conn_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_cmd_conn_priv.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_cmd_conn.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_cmd_enc_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_cmd_enc_slave.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_cmd_sc.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_cmd_vs_adv_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_cmd_vs_adv_slave.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_cmd_vs_conn_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_cmd_vs_conn.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_cmd_vs_enc_slave.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_cmd_vs_sc.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_cmd_vs.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_cmd.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_evt_adv_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_evt_adv_slave.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_evt_conn_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_evt_conn_priv.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_evt_conn.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_evt_enc_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_evt_enc_slave.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_evt_sc.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_evt_vs.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_evt.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_init_adv_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_init_adv_priv.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_init_adv_slave.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_init_conn_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_init_conn_priv.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_init_conn.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_init_enc_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_init_enc_slave.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_init_sc.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_init.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lhci/lhci_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/ll/ll_init_adv_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/ll/ll_init_adv_slave.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/ll/ll_init_conn_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/ll/ll_init_conn_slave.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/ll/ll_init_enc_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/ll/ll_init_enc_slave.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/ll/ll_init_init_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/ll/ll_init_priv.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/ll/ll_init_sc.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/ll/ll_init.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/ll/ll_main_adv_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/ll/ll_main_adv_slave.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/ll/ll_main_conn_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/ll/ll_main_conn_slave.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/ll/ll_main_conn.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/ll/ll_main_dtm.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/ll/ll_main_enc_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/ll/ll_main_enc_slave.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/ll/ll_main_priv.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/ll/ll_main_sc.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/ll/ll_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/ll/ll_math.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lmgr/lmgr_events.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lmgr/lmgr_main_conn.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lmgr/lmgr_main_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lmgr/lmgr_main_priv.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lmgr/lmgr_main_sc.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lmgr/lmgr_main_slave.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/lmgr/lmgr_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/sch/sch_ble.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/sch/sch_rm.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/ble/sch/sch_tm.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/common/bb/bb_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/common/chci/chci_tr.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/common/sch/sch_list.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/sources/common/sch/sch_main.c
	)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/hci/exactle
	)

target_sources(${PROJECT_NAME}
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/hci/common/hci_core.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/hci/common/hci_tr.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/hci/exactle/hci_cmd.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/hci/exactle/hci_cmd_enc.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/hci/exactle/hci_cmd_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/hci/exactle/hci_core_ps.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/hci/exactle/hci_evt.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/hci/exactle/hci_vs.c
	)
else()
target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/hci/dual_chip
	)


target_sources(${PROJECT_NAME}
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/hci/common/hci_core.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/hci/common/hci_tr.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/hci/dual_chip/hci_cmd.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/hci/dual_chip/hci_core_ps.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/hci/dual_chip/hci_evt.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/hci/dual_chip/hci_vs.c
)
endif()

target_include_directories(${PROJECT_NAME}
  PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/include
  PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/af
  )

target_sources(${PROJECT_NAME}
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio//ble-profiles/sources/af/app_disc.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio//ble-profiles/sources/af/app_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio//ble-profiles/sources/af/app_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio//ble-profiles/sources/af/app_master_leg.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio//ble-profiles/sources/af/app_server.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio//ble-profiles/sources/af/app_slave.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio//ble-profiles/sources/af/app_slave_leg.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio//ble-profiles/sources/af/app_terminal.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio//ble-profiles/sources/af/common/app_db.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio//ble-profiles/sources/af/common/app_hw.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio//ble-profiles/sources/af/common/app_ui.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio//ble-profiles/sources/af/common/ui_console.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio//ble-profiles/sources/af/common/ui_lcd.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio//ble-profiles/sources/af/common/ui_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio//ble-profiles/sources/af/common/ui_platform.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio//ble-profiles/sources/af/common/ui_timer.c
	)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/include
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles
	)

target_sources(${PROJECT_NAME}
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/anpc/anpc_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/bas/bas_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/blpc/blpc_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/blps/blps_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/cpp/cpps_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/cscp/cscps_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/dis/dis_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/fmpl/fmpl_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/gap/gap_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/gatt/gatt_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/glpc/glpc_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/glps/glps_db.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/glps/glps_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/hid/hid_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/hrpc/hrpc_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/hrps/hrps_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/htpc/htpc_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/htps/htps_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/paspc/paspc_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/plxpc/plxpc_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/plxps/plxps_db.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/plxps/plxps_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/rscp/rscps_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/scpps/scpps_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/sensor/gyro_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/sensor/temp_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/tipc/tipc_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/udsc/udsc_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/uribeacon/uricfg_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/wdxc/wdxc_ft.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/wdxc/wdxc_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/wdxc/wdxc_stream.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/wdxs/wdxs_au.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/wdxs/wdxs_dc.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/wdxs/wdxs_ft.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/wdxs/wdxs_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/wdxs/wdxs_phy.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/wdxs/wdxs_stream.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/wpc/wpc_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/wspc/wspc_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/profiles/wsps/wsps_main.c
)


target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/services
	)

target_sources(${PROJECT_NAME}
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/services/svc_alert.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/services/svc_batt.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/services/svc_bps.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/services/svc_core.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/services/svc_cps.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/services/svc_cscs.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/services/svc_dis.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/services/svc_gls.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/services/svc_gyro.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/services/svc_hid.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/services/svc_hrs.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/services/svc_hts.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/services/svc_ipss.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/services/svc_plxs.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/services/svc_px.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/services/svc_rscs.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/services/svc_scpss.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/services/svc_temp.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/services/svc_time.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/services/svc_uricfg.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/services/svc_wdxs.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/services/svc_wp.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-profiles/sources/services/svc_wss.c
)
target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/include
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/att
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/cfg
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/dm
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/hci
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/l2c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/smp
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/platform/include
	)

target_sources(${PROJECT_NAME}
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/att/att_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/att/att_uuid.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/att/attc_disc.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/att/attc_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/att/attc_proc.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/att/attc_read.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/att/attc_sign.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/att/attc_write.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/att/atts_ccc.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/att/atts_csf.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/att/atts_dyn.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/att/atts_ind.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/att/atts_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/att/atts_proc.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/att/atts_read.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/att/atts_sign.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/att/atts_write.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/cfg/cfg_stack.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/dm/dm_adv.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/dm/dm_adv_leg.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/dm/dm_conn.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/dm/dm_conn_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/dm/dm_conn_master_leg.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/dm/dm_conn_slave.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/dm/dm_conn_slave_leg.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/dm/dm_conn_sm.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/dm/dm_dev.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/dm/dm_dev_priv.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/dm/dm_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/dm/dm_priv.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/dm/dm_scan.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/dm/dm_scan_leg.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/dm/dm_sec.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/dm/dm_sec_lesc.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/dm/dm_sec_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/dm/dm_sec_slave.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/hci/hci_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/l2c/l2c_coc.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/l2c/l2c_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/l2c/l2c_master.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/l2c/l2c_slave.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/smp/smp_act.c
 	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/smp/smp_db.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/smp/smp_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/smp/smp_non.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/smp/smp_sc_act.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/smp/smp_sc_main.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/smp/smpi_act.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/smp/smpi_sc_act.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/smp/smpi_sc_sm.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/smp/smpi_sm.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/smp/smpr_act.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/smp/smpr_sc_act.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/smp/smpr_sc_sm.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/stack/smp/smpr_sm.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/sec/common/sec_aes.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/sec/common/sec_aes_rev.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/sec/common/sec_ccm_hci.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/sec/common/sec_cmac_hci.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/sec/common/sec_ecc_debug.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/sec/common/sec_ecc_hci.c
	PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/ble-host/sources/sec/common/sec_main.c
)
