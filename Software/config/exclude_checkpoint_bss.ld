/*************************************************************************************************/
/*!
 *  Copyright (c) 2022 Delft University of Technology.
 *  Jasper de Winkel, Haozhe Tang and Przemyslaw Pawelczak
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
/*************************************************************************************************/

/* Variables that have to be excluded from the .bss checkpoint */
*(*.m_cb)
*(*.m_prs_box_0)
*(*.m_prs_box_1)
*(*.m_prs_box_2)
*(*.m_prs_box_3)
*(*.m_prs_box_4)
/*      *(*.palRtcTimerCback)           */
*(*.palSysAssertCount)
*(*.PalSysAssertTrapEnable)
*(*.palSysBusyCount)
/*      *(*.palTimerCb)                 */
/*      *(*.palBbTimerIrqCbackTbl)      */
/*      *(*.palBbRadioIrqCbackTbl)      */
/* *(*.bbTxLen) */
/* *(*.bbEncryptTxSuppressMic) */
/* *(*.bbEncryptOutBuf) */
*(*.m_in_critical_region)
/* SEGGER RTT STUFF */
*(*._acDownBuffer)
*(*._acUpBuffer)
*(*._SEGGER_RTT)