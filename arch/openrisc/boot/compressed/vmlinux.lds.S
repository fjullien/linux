#include <asm-generic/vmlinux.lds.h>

OUTPUT_FORMAT("elf32-or1k", "elf32-or1k", "elf32-or1k")

ENTRY(_start)	/* Defined in head.S */

SECTIONS
{
/*  . = (CONFIG_MEM_BASE + CONFIG_BOOT_LINK_OFFSET) | CONFIG_KERNEL_REGION_BASE;*/

        /* Read-only sections, merged into text segment: */
        . = 0 ;

  _text = .;
  .text : { *(.text) } = 0
  .rodata : { *(.rodata) *(.rodata.*) }
  _etext = .;

  . = ALIGN(32 / 8);
  .data : { *(.data) }
  . = ALIGN(32 / 8);
  _edata  =  .;

  . = ALIGN(32 / 8);
  __bss_start = .;
  .bss : { *(.bss) *(.sbss) }
  . = ALIGN(32 / 8);
  _ebss = .;
  end = . ;
  _end = . ;

}
