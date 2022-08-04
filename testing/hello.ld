/* Tell the linker that we want an x86_64 Binary output file */
/*OUTPUT_FORMAT(binary)*/
OUTPUT_ARCH(i386:x86-64)

/* We want the symbol start to be our entry point */
ENTRY(start)

SECTIONS
{
    . = 0x800000;
    .text : {
        *(.text .text.*)
    } :text

    .rodata : {
        *(.rodata .rodata.*)
    } :rodata

    .data : {
        *(.data .data.*)
    } :data
}