#include <Windows.h>
#include <iostream>
#include <stdlib.h>
#include "..\..\inc\types.h"
#include "nano_db.h"
#include "switch_db.h"
#include "..\..\inc\analyzer.h"

namespace analyser
{

/* Возвращаемые значения:
	= 0 - блок завершился выходом за диапазон (не достаточно памяти или неправильный блок)
	< 0 - блок заверешился ошибкой анализа (неправильный блок)
	> 0 - блок завершился инструкцией завершения блока
*/
#ifdef ARMADILLO
	int blocks_analyser::block_process (block *node, nano_db *nanos)
#else
	int blocks_analyser::block_process (block *node)
#endif
{
	u8 *ptr, *end;
	bool last_was_marker = false;
	u8 nanolen;
	u32 EIP;
	u64 orig_bytes;
	u32 temp_test = 0;
	char buf[512];
	switch_db switch_tables;
	std::vector<u32> eip_stack;

	ptr = base + (node->address - ImageBase - code_section_rva);
	end = base + code_size;
	eip_stack.clear();

	try
	{
#ifdef _DEBUG
		printf("================Start of block 0x%X=========================\n", code_section_va + (ptr - base));
#endif
		while (ptr < end)
		{
			EIP = code_section_va + (ptr - base);
			res = medi_disassemble(ptr, &instr, &params);
			if (EIP == 0x42BB15 || EIP == 0x430A89)//0x406A17)
				EIP = EIP;
			if (params.errcode)
			{
				sprintf_s(buf, sizeof(buf), "%X: fail: %d, len: %d\n", EIP, params.errcode, res);
				printf(buf);
				return -1; // Ошибка анализа: ошибка дизассемблирования
			}
			else
			{
#ifdef _DEBUG
				reallen = medi_dump(&instr, buff, sizeof(buff), DUMP_OPTION_IMM_UHEX | DUMP_OPTION_DISP_HEX);
				buff[reallen] = 0;
				printf("%X: %s\n", EIP, buff);
#endif
				node->size += res;
				node->count++;
				if (instr.groups & (GRP_BRANCH | GRP_SWITCH))
				{
					// Конец блока
#ifdef _DEBUG
					printf("=====================End of block===============================\n");
#endif
					node->id = instr.id;
					switch (instr.id)
					{
						case ID_INT:
						{
							node->is_analyzed = true;
							if (instr.ops[0].value.imm.imm8 == 3)
							{
								// INT 3
#ifdef ARMADILLO
								nanos->get_orig_bytes(EIP, &nanolen, &orig_bytes);
								if (nanolen)
								{
									if (*(u16 *)orig_bytes == 0x03EB)
									{
										ptr += 5;
										last_was_marker = true;
									}
									else
									{
										last_was_marker = false;
									}
									memcpy(ptr, (const void *)orig_bytes, nanolen);
								}
								else
								{
									printf("Nanomite not found!");
#endif
									return 6; // Анализ блок завершён: найдена инструкция INT 3
#ifdef ARMADILLO
								}
								res = medi_disassemble(ptr, &instr, &params);
								reallen = medi_dump(&instr, buff, OUT_BUFF_SIZE, DUMP_OPTION_IMM_UHEX | DUMP_OPTION_DISP_HEX);
								buff[reallen] = 0;
								printf("%X: %s\n", EIP, buff);
#endif
							}
							else
							{
								// Анализ блок завершён: найдена инструкция INT N
								node->is_analyzed = true;
								return 12;
							}
						}
						case ID_JMP:
						{
							if (instr.flags & INSTR_FLAG_MODRM)
							{
								switch (instr.ops[0].value.addr.mod)
								{
									case ADDR_MOD_DISP:
									{
										node->is_analyzed = true;
										return 5;
									}
									case ADDR_MOD_BASE:
									case ADDR_MOD_IDX:
									case ADDR_MOD_BASE | ADDR_MOD_IDX:
									{
										node->is_analyzed = false;
										return 3;
									}
									case ADDR_MOD_IDX | ADDR_MOD_DISP:
									{
										// обработка switch таблицы
										u8 scale = instr.ops[0].value.addr.scale;
										u32 taddr = instr.ops[0].value.imm.imm32;
										//u32 eip_old;
										u32 tsize = 1;
										int i = 0;
										bool bound_found = false, table_back_oriented = false;
										#define MAX_JUMPS_BACK 3 // макс. кол-во прыжков назад при поиске прыжка-ограничителя
										#define MAX_JUMPS_BACK2 1 // макс. кол-во прыжков назад при поиске CMP/AND

										/**
										while (i++ < MAX_JUMPS_BACK)
										{
											eip_old = eip_stack[eip_stack.size() - 1];
											eip_stack.pop_back();
											ptr = base + (eip_old - code_section_va);
											res = disassemble(ptr, &instr, &params);
											if (!params.errcode)
											{
												switch (instr.id)
												{
												case ID_JA:
												case ID_JB:
													bound_found = true;
													break;
												case ID_NEG:
													table_back_oriented = true;
													break;
												}
											}
											else
											{
												throw std::exception("Unexpected error while disasm!");
											}
										}
										/**/
										//switch_tables.add_table(taddr, scale, tsize);
										/*
										res = disassemble(ptr, &instr, &params);
										reallen = dump(&instr, buff, OUT_BUFF_SIZE, DUMP_OPTION_IMM_UHEX | DUMP_OPTION_DISP_HEX);
										buff[reallen] = 0;
										printf("%X: %s\n", EIP, buff);
										*/
										node->is_analyzed = false;
										return 4; // Анализ блока завершён: switch jump
									}
									default:
										return -1;
								}
							}
							else
							{
								// Анализ блока завершён: безусловный переход
								node->is_analyzed = true;
								return 2;
							}
						}
						case ID_JO:
						case ID_JNO:
						case ID_JB:
						case ID_JAE:
						case ID_JZ:
						case ID_JNZ:
						case ID_JBE:
						case ID_JA:
						case ID_JS:
						case ID_JNS:
						case ID_JP:
						case ID_JNP:
						case ID_JL:
						case ID_JGE:
						case ID_JLE:
						case ID_JG:
						case ID_JCXZ:
						{
							// Анализ блока завершён: условный переход
							node->is_analyzed = true;
							return 1;
						}
						case ID_CALL:
						{
							if (instr.flags & INSTR_FLAG_MODRM)
							{
								switch (instr.ops[0].value.addr.mod)
								{
									case 0:
									{
										temp_test = 0;
										break;
									}
									case ADDR_MOD_BASE:
									{
										temp_test = 1;
										break;
									}
									case ADDR_MOD_IDX:
									{
										temp_test = 2;
										break;
									}
									case ADDR_MOD_BASE | ADDR_MOD_IDX:
									{
										temp_test = 3;
										break;
									}
									case ADDR_MOD_DISP:
									{
										// CALL DS:[X] - IAT (or something else?)
										node->is_analyzed = true;
										return 5;
									}
									case ADDR_MOD_BASE | ADDR_MOD_IDX | ADDR_MOD_DISP:
									{
										// x64 only?
										temp_test = 7;
										return -1;
									}
									default:
									{
										temp_test = 8;
										return -1;
									}
								}
							}
							else
							{
								// Анализ блока завершён: инструкция CALL
								node->is_analyzed = true;
								return 8;
							}
						}
						case ID_RETN:
						case ID_RETF:
						{
							node->is_analyzed = true;
							// Анализ блока завершён: инструкция RET
							return 11;
						}
						case ID_IRET:
						{
							node->is_analyzed = true;
							// Анализ блока завершён: инструкция IRET
							return 13;
						}
						case ID_SYSCALL:
						case ID_SYSENTER:
						{
							node->is_analyzed = true;
							// Анализ блока НЕ завершён: инструкция SYSENTER/SYSCALL
							break;
						}
						case ID_SYSEXIT:
						case ID_SYSRET:
						{
							node->is_analyzed = true;
							return 14;
						}
						default:
						{
							temp_test = 20;
						}
					}
				}
				ptr += res;
				eip_stack.push_back(EIP);
			}
		}
	}
	catch(char * str)
	{
		std::cout << "Exception raised: " << str << '\n';
	}

	return 0;
}

#ifdef ARMADILLO
	void blocks_analyser::ananlyze_from_va (u32 v_start_addr, nano_db *nanos)
#else
	void blocks_analyser::ananlyze_from_va (u32 v_start_addr)
#endif
{
	block_ptr node;//, *tmp_node;
	int res;
	u32 vaddr, tmp_addr;
	ref temp;
	std::vector<ref> addresses;
	BLOCKS::iterator it;

	temp.from = 0;
	temp.to = v_start_addr;
	addresses.push_back(temp);
	while (true)
	{
		if (addresses.empty())
			break; // Analyze finished
		temp = addresses.back();
		vaddr = temp.to;
		addresses.pop_back();
		tmp_addr = temp.from;
		temp.from = vaddr;
		it = main_tree.find(vaddr);
		if (it == main_tree.end())// ISSUE: зацикливание на 0x42AFBC
		{
			node = new block(vaddr, tmp_addr);
			main_tree[vaddr] = node;
#ifdef ARMADILLO
			res = block_process(node, nanos);
#else
			res = block_process(node);
#endif
			temp.to = node->address + node->size;
			switch (res)
			{
			case -1:
				node->is_valid = false;
				break;
			case 1:
				addresses.push_back(temp);
			case 2:
				temp.to += instr.ops[0].value.imm.imm32;
				addresses.push_back(temp);
				break;
			case 3:
				// сосать член (писать эмулятор/эвристику)
				break;
			case 4:
				// TODO: Kill youself - switch jump :D
				// TODO: добавить в список заданий все элементы таблицы
				break;
			case 5:
				// TODO: обработать импорт
				// ISSUE: в случае API заверешния потока/процесса сделать break,
				// чтобы не анализировать адрес возврата
			case 6:
				addresses.push_back(temp);
				break;
			case 7:
			case 11:
			case 12:
			case 13:
			case 14:
				// End of the block chain - Nothing to do
				break;
			case 8:
				addresses.push_back(temp);
				// проход на основную ветку кидаем на вершину, т.к возврата может и не быть
				if (instr.ops[0].value.imm.imm32)
				{
					temp.to += instr.ops[0].value.imm.imm32;
					addresses.push_back(temp);
				}
				break;
			case 10:
				// поделить блок на два
				//tmp_node = new block(vaddr);
				// TODO: изменить размер первого блока
				tmp_addr = node->address + node->size + instr.ops[0].value.imm.imm8; // Проверить!
				temp.to = tmp_addr;
				addresses.push_back(temp);
				//tmp_node->address = tmp_addr;
				//node->size = tmp_addr - node->address;
				//node->type = 0;
				// TODO: добавить референс на себя в tmp_node и референс с node, в node сделать референс на tmp_node
				// продолжить анализ со следующей инструкции
				temp.to = node->address + node->size;
				addresses.push_back(temp);
				break;
			default:
				break;
			}
		}
		else
		{
			// TODO: Добавить референсы
			if (it->second->is_valid)
			{
				it->second->RefFrom.push_back(temp.from);
				it = main_tree.find(vaddr);
				if (it != main_tree.end())
				{
					it->second->RefTo.push_back(temp.to);
				}
				else
				{
					// TODO: осознать, что произошло
				}
			}
		}
	}
}

blocks_analyser::blocks_analyser (u8 *header, u32 *file_offset, u32 *size)
{
	u8 *ptr, *end;
	u16 j;
	PIMAGE_DOS_HEADER dos_header;
	PIMAGE_NT_HEADERS nt_headers;
	PIMAGE_SECTION_HEADER section;

	params.arch = ARCH_COMMON;
	params.sf_prefixes = sf_prefixes;
	params.mode = DISASSEMBLE_MODE_32;
	params.base = 0;

	ptr = header;
	end = ptr + IN_BUFF_SIZE;
	dos_header = (PIMAGE_DOS_HEADER)ptr;

	if (dos_header->e_magic == IMAGE_DOS_SIGNATURE)
	{
		ptr += dos_header->e_lfanew;
		nt_headers = (PIMAGE_NT_HEADERS32)ptr;
		if (nt_headers->Signature == IMAGE_NT_SIGNATURE)
		{
			ImageBase = nt_headers->OptionalHeader.ImageBase;
			EntryPoint = nt_headers->OptionalHeader.AddressOfEntryPoint;
			NumberOfSections = nt_headers->FileHeader.NumberOfSections;
			/* TODO: Вставить обработку импорта */
			/**/
			ptr += sizeof(IMAGE_NT_HEADERS);
			for (j = 0; j < NumberOfSections && ptr < end; j++)
			{
				section = (PIMAGE_SECTION_HEADER)ptr;
				if (section->Characteristics & IMAGE_SCN_MEM_EXECUTE)
				{
					*file_offset = section->PointerToRawData;
					code_section_rva = section->VirtualAddress;
					code_size = section->SizeOfRawData;
					break;
				}
				else
				{
					ptr += sizeof(IMAGE_SECTION_HEADER);
				}
			}
		}
	}

	code_section_va = ImageBase + code_section_rva;
	EP = EntryPoint + ImageBase;
	*size = code_size;
}

void blocks_analyser::code_load (u8 *pointer_to_code, u32 size)
{
	base = pointer_to_code;
	code_size = size;
}

blocks_analyser::~blocks_analyser ()
{
	free(base);
}

}
/**
int ROR (u32 r32, u8 val)
{
	u32 mask;

	__asm {
		mov eax,r32
		mov cl,val
		ror eax,cl
	}
	/**
	val = val % 32;
	mask = ~(0xFFFFFFFF << (32 - val));
	return ((r32 >> val) & mask) | (r32 << (32 - val));
	/**
}
/**/
int main(int argc, char **argv)
{
	u8 *base;
	u8 *ptr_nano = 0;
	u32 addr = 0, size = 0;
	u32 nano_offset, nano_size;
	FILE *fdump;
	/**/
#ifdef ARMADILLO
	if (argc != 3)
#else
	if (argc != 2)
#endif
	{
		// Armadillo__.exe 2184896
		printf("invalid number of params");
		return 0;
	}

	fdump = fopen(argv[1], "r+b");
	if (!fdump)
	{
		printf("can not open file %s", argv[1]);
		return 0;
	}
	// Reading header
	fseek(fdump, SEEK_TO, SEEK_SET);
	base = (uint8_t *)malloc(IN_BUFF_SIZE);
	fread(base, sizeof(uint8_t), IN_BUFF_SIZE, fdump);
	analyser::blocks_analyser blocks(base, &addr, &size);
	free(base);

	if (!addr)
	{
		printf("executable section not found!");
		return 0;
	}
	if (!size)
	{
		printf("executable section is zero sized!");
		return 0;
	}
	/**/
	// Reading code section
	fseek(fdump, addr, SEEK_SET);
	base = (uint8_t *)malloc(size);
	fread(base, sizeof(uint8_t), size, fdump);
	blocks.code_load(base, size);
	/**/
#ifdef ARMADILLO
	// Reading nanomites table
	nano_offset = atoi(argv[2]);
	nano_size = NANO_BUFF_SIZE;
	fseek(fdump, nano_offset, SEEK_SET);
	ptr_nano = (u8 *)malloc(nano_size);
	fread(ptr_nano, sizeof(uint8_t), nano_size, fdump);
	nano_db nanos(ptr_nano);
	free(ptr_nano);
#endif
#ifdef ARMADILLO
	blocks.ananlyze_from_va(blocks.EP, &nanos);
#else
	blocks.ananlyze_from_va(blocks.EP);
#endif

	return 0;
}
