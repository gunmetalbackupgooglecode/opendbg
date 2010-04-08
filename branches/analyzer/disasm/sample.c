#include <stdio.h>
#include <stdlib.h>
#include "mediana.h"

#define OUT_BUFF_SIZE 0x200
/*
#define IN_BUFF_SIZE  14231285
#define SEEK_TO       0x0
*/

#define IN_BUFF_SIZE  0x8B8E
#define SEEK_TO       0x10C0

int main(int argc, char **argv)
{
    int                         reallen;
    unsigned int                res;
    uint8_t                    *base, *ptr, *end;
    uint8_t                     sf_prefixes[MAX_INSTRUCTION_LEN];
    unichar_t                   buff[OUT_BUFF_SIZE];
    FILE                       *fp;
    struct INSTRUCTION          instr;
    struct DISASM_INOUT_PARAMS  params;

    params.arch = ARCH_ALL;
    params.sf_prefixes = sf_prefixes;
    params.mode = DISASSEMBLE_MODE_64;
    params.options = DISASM_OPTION_APPLY_REL | DISASM_OPTION_OPTIMIZE_DISP;
    //params.options = 0;
    params.base = 0x00401000;

    base = malloc(IN_BUFF_SIZE);
    ptr = base;
    end = ptr + IN_BUFF_SIZE;

    fp = fopen("notepad64.exe", "rb");
    fseek(fp, SEEK_TO, SEEK_SET);
    fread(base, IN_BUFF_SIZE, 1, fp);
    fclose(fp);
 
    while(ptr < end)
    {
        res = medi_disassemble(ptr, &instr, &params);
        if (params.errcode)
        {
            printf("%X: fail: %d, len: %d\n", ptr - base, params.errcode, res);
            if (res == 0)
                res++;
        }
        else
        {
            reallen = medi_dump(&instr, buff, OUT_BUFF_SIZE, DUMP_OPTION_IMM_UHEX | DUMP_OPTION_DISP_HEX);
            //reallen = medi_dbg_dump(&instr, params.sf_prefixes, params.sf_prefixes_len, buff, OUT_BUFF_SIZE);
            if (reallen > OUT_BUFF_SIZE)
                buff[OUT_BUFF_SIZE - 1] = 0;
            else
                buff[reallen] = 0;
            //printf("%X: %s\n", ptr - base, buff);
            wprintf(_UT("%X: %s\n"), ptr - base, buff);
        }
        ptr += res;
        params.base += res;
    }

    return 0;
}