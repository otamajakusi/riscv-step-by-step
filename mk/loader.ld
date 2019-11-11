OUTPUT_ARCH( "riscv" )

ENTRY( _start )

MEMORY
{
  rom   (wxa!ri) : ORIGIN = 0x20010000, LENGTH = 0x6a120
}

PHDRS
{
  rom PT_LOAD;
}

SECTIONS
{
  .text : ALIGN(0x1000) {
    PROVIDE(_text_start = .);
    *(.text.init) *(.text .text.*)
    PROVIDE(_text_end = .);
  } >rom AT>rom :rom

  .rodata : {
    PROVIDE(_rodata_start = .);
    *(.rodata .rodata.*)
    PROVIDE(_rodata_end = .);
  } >rom AT>rom :rom

  .data : ALIGN(0x4) {
    PROVIDE(_data_start = .);
    *(.sdata .sdata.*) *(.data .data.*)
    PROVIDE(_data_end = .);
  } >rom AT>rom :rom

  .bss : ALIGN(0x4) {
    PROVIDE(_bss_start = .);
    *(.sbss .sbss.*) *(.bss .bss.*)
    PROVIDE(_bss_end = .);
  } >rom AT>rom :rom
}
