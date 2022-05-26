/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief  Linker script describing the memory layout for the nRF51840.
 *
 *  Copyright (c) 2019 Packetcraft, Inc. 
 * 
 *  Copyright (c) 2022 Delft University of Technology.
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

OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
ENTRY(Reset_Handler)
SEARCH_DIR(.)

MEMORY
{
  FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 1024K
  RAM   (rw) : ORIGIN = 0x20000000, LENGTH =  256K
  FW0   (rwx) : ORIGIN = 0x12000000, LENGTH = 0x80
  FW1   (rwx) : ORIGIN = 0x12000080, LENGTH = 0x80
  NVMEM (rwx) : ORIGIN = 0x12000100, LENGTH = 0x7ff00
}

STACK_SIZE = 4096;

SECTIONS
{
  /********************/
  /*** Text Segment ***/
  /********************/

  . = ORIGIN(FLASH);
  _stext = .;

  .text_vect :
  {
    KEEP(*(.isr_vector))
  } > FLASH

  .text :
  {
    *(.text*)

    KEEP(*(.init))
    KEEP(*(.fini))

    *(.rodata*)
  } > FLASH

  . = ALIGN(4);
  _etext = .;
  
  .firmware :
  {
    . = ALIGN(4096);
    _sfirmware_rom = .;
    *(.firmware_rom)
    . = ALIGN(4);
    _efirmware_rom = .;
    . = ALIGN(4096);
  } > FLASH

  /********************/
  /*** Data Segment ***/
  /********************/

  .data_no_restore :
  {
        . = ALIGN(4);
        _sdata_norestore = .;
        *(.norestore_data)
        *(.norestore_data*)
        . = ALIGN(4);
        INCLUDE "exclude_checkpoint_data.ld" ;
        . = ALIGN(4);
        _edata_norestore = .;
  } > RAM AT>FLASH

   .data_application :
    {
        . = ALIGN(4);
        _sdata_application = .;
        *ckpt-app/*(.data)
        *ckpt-app/*(.data*)
        . = ALIGN(4);
        _edata_application = .;
    } > RAM AT>FLASH

    .data_network :
    {
        . = ALIGN(4);
        _sdata_network = .;
        *cordio/*(.data)
        *cordio/*(.data*)
        . = ALIGN(4);
        _edata_network = .;
    } > RAM AT>FLASH

  .data :
  {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;
  } > RAM AT>FLASH
  
      /* used by startup to initialize data */
    _init_data = LOADADDR(.data_no_restore);
    
  /*******************/
  /*** BSS Segment ***/
  /*******************/

.bss_no_restore (NOLOAD):
{
        . = ALIGN(4);
        _sbss_norestore = .;
        *(.norestore_bss)
        *(.norestore_bss*)
        . = ALIGN(4);
        INCLUDE "exclude_checkpoint_bss.ld" ;
        . = ALIGN(4);
        _ebss_norestore = .;
} > RAM

    .bss_application (NOLOAD):
    {
        . = ALIGN(4);
        _sbss_application = .;
        *ckpt-app/*(.bss)
        *ckpt-app/*(.bss*)
        *ckpt-app/*(COMMON)
        . = ALIGN(4);
        _ebss_application = .;
    } > RAM

    .bss_network (NOLOAD):
    {
        . = ALIGN(4);
        _sbss_network = .;
        *cordio/*(.bss)
        *cordio/*(.bss*)
        *cordio/*(COMMON)
        . = ALIGN(4);
        _ebss_network = .;
    } > RAM

  .bss (NOLOAD):
  {
        . = ALIGN(4);
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
  } > RAM

    .restore_stack (NOLOAD):
    {
        . = ALIGN(4);
        . = . + 1K; /* Reserve 1K for the restore stack (TODO check actual requirement) */
        . = ALIGN(4);
        _erestore_stack = .;
    } > RAM
    
    
        .nvm (NOLOAD):
    {
        . = ALIGN(4);

        _data_checkpoint_0_start = .;
        . = ALIGN(4);
        . = DEFINED(_checkpoint_data_allocate_checkpoint_ld) ? . + (_edata - _edata_norestore) : . ;
        . = ALIGN(4);
        _data_checkpoint_1_start = .;
        . = . + _data_checkpoint_1_start - _data_checkpoint_0_start;
        _data_checkpoint_1_end = .;

        /* .bss checkpoint */
        _bss_checkpoint_0_start = .;
        . = ALIGN(4);
        . = DEFINED(_checkpoint_data_allocate_checkpoint_ld) ? . + (_ebss - _ebss_norestore) : . ;
        . = ALIGN(4);
        _bss_checkpoint_1_start = .;
        . = . + _bss_checkpoint_1_start - _bss_checkpoint_0_start;
        _bss_checkpoint_1_end = .;
        
        
        _data_application_checkpoint_0_start = .;
        . = ALIGN(4);
        . = DEFINED(_checkpoint_application_allocate_checkpoint_ld) ? . + (_edata_application - _sdata_application) : . ;
        . = ALIGN(4);
        _data_application_checkpoint_1_start = .;
        . = . + _data_application_checkpoint_1_start - _data_application_checkpoint_0_start;
        _data_application_checkpoint_1_end = .;
        
        /* .bss_application checkpoint */
        _bss_application_checkpoint_0_start = .;
        . = ALIGN(4);
        . = DEFINED(_checkpoint_application_allocate_checkpoint_ld) ? . + (_ebss_application - _sbss_application) : . ;
        . = ALIGN(4);
        _bss_application_checkpoint_1_start = .;
        . = . + _bss_application_checkpoint_1_start - _bss_application_checkpoint_0_start;
        _bss_application_checkpoint_1_end = .;
        
        
        _data_network_checkpoint_0_start = .;
        . = ALIGN(4);
        . = DEFINED(_checkpoint_network_allocate_checkpoint_ld) ? . + (_edata_network - _sdata_network) : . ;
        . = ALIGN(4);
        _data_network_checkpoint_1_start = .;
        . = . + _data_network_checkpoint_1_start - _data_network_checkpoint_0_start;
        _data_network_checkpoint_1_end = .;
        
        /* .bss_network checkpoint */
        _bss_network_checkpoint_0_start = .;
        . = ALIGN(4);
        . = DEFINED(_checkpoint_network_allocate_checkpoint_ld) ? . + (_ebss_network - _sbss_network) : . ;
        . = ALIGN(4);
        _bss_network_checkpoint_1_start = .;
        . = . + _bss_network_checkpoint_1_start - _bss_network_checkpoint_0_start;
        _bss_network_checkpoint_1_end = .;

        . = ALIGN(4);
        _snvm = .;
        *(.nvm)
        *(.nvm*)
        . = ALIGN(4);
        _envm = .;

        /* _sfirmware_0 = .;
        . = ALIGN(4);
        . = DEFINED(_allocate_firmware_ld) ? . + (_efirmware_rom - _sfirmware_rom) : . ;
        . = ALIGN(4);
        _efirmware_0 = .;

        _sfirmware_1 = .;
        . = ALIGN(4);
        . = DEFINED(_allocate_firmware_ld) ? . + (_efirmware_rom - _sfirmware_rom) : . ;
        . = ALIGN(4);
        _efirmware_1 = .; */
    } > NVMEM

        .fw0 (NOLOAD):
    {
        _sfirmware_0 = .;
        . = . + 0x80;
        _efirmware_0 = .;
    } > FW0

        .fw1 (NOLOAD):
    {
        _sfirmware_1 = .;
        . = . + 0x80;
        _efirmware_1 = .;
    } > FW1
  /*************/
  /*** Stack ***/
  /*************/

  __stack_top__ = ORIGIN(RAM) + LENGTH(RAM);
  __stack_limit__ = __stack_top__ - STACK_SIZE;
  
  _estack = __stack_top__;
  
  /************/
  /*** Heap ***/
  /************/

  __heap_start__ = _erestore_stack;
  __heap_end__ = __stack_limit__;

  ASSERT(__stack_limit__ >= __heap_end__, "RAM region overflowed")
}
