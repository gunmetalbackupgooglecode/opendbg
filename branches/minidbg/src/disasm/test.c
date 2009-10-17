#include <stdio.h>
#include <stdlib.h>
#include "disasm.h"

int main(int argc, char **argv)
{
	uint8_t *ptr, *end;
	uint8_t sf_prefixes[MAX_INSTRUCTION_LEN];
	char buff[70];
	int reallen;
	unsigned int res;
	struct INSTRUCTION instr;
	struct PARAMS params;
	FILE *fp;

	params.arch = ARCH_COMMON;
	params.sf_prefixes = sf_prefixes;
	params.mode = DISASSEMBLE_MODE_32;

	ptr = malloc(14231285);
	end = ptr + 14231285;

	fp = fopen("asm_com.bin", "rb");
	fread(ptr, 14231285, 1, fp);
	fclose(fp);

	while(ptr < end)
	{
		res = disassemble(ptr, &instr, &params);
		if (!IS_OK(res))
		{
			printf("Fail: %d\n", GET_ERR(res));
			res &= 0xFF;
		}
		else
		{
			reallen = dump(&instr, buff, 70, OPTION_IMM_UHEX | OPTION_DISP_HEX);
			//reallen = dump_dbg(&instr, NULL, buff, 0x200);
			buff[reallen] = '\n';
			buff[reallen + 1] = 0;
			printf(buff);
		}
		ptr += res;
	}

	return 0;
}