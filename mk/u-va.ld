OUTPUT_ARCH( "riscv" )

ENTRY( _start )

PHDRS
{
  text PT_LOAD;
  data PT_LOAD;
}

SECTIONS
{
  . = ALIGN(4096);
  .text : {
    PROVIDE(_text_start = .);
    *(.text.init) *(.text .text.*)
    PROVIDE(_text_end = .);
  }:text

  . = ALIGN(16);
  .rodata : {
    PROVIDE(_rodata_start = .);
    *(.rodata .rodata.*)
    PROVIDE(_rodata_end = .);
  }:text

  . = ALIGN(4096);
  .data : {
    PROVIDE(_data_start = .);
    *(.sdata .sdata.*) *(.data .data.*)
    PROVIDE(_data_end = .);
  }:data

  . = ALIGN(16);
  .bss :{
    PROVIDE(_bss_start = .);
    *(.sbss .sbss.*) *(.bss .bss.*)
    PROVIDE(_bss_end = .);
  }:data
}
