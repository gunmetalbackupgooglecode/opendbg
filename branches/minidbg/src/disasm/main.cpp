#include <stdio.h>
#include "udis86.h"

int
main() {
    uint8_t raw[] = { 0x49, 0x49, 0x2A, 0x00, 0xAA, 0x71, 0x00, 0x00,
                      0x3B, 0x5B, 0x33, 0xE6, 0x48, 0x32, 0x18, 0x30 };
    uint8_t len = 9;
    ud_t ud_obj;
    ud_init(&ud_obj);
    ud_set_mode(&ud_obj, 16);
    ud_set_input_buffer(&ud_obj, raw, len);
    ud_set_syntax(&ud_obj, UD_SYN_INTEL);
    ud_set_mode(&ud_obj, 32);
    
    while (ud_disassemble(&ud_obj)) {
        printf("\t%s\n", ud_insn_asm(&ud_obj));
    }
    return 0;
}