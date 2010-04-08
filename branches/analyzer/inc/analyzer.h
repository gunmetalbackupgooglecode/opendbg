#ifndef _ANALYSER_H
#define _ANALYSER_H

#include <cstdlib>
#include <vector>
#include <map>
#include <Windows.h>
#include "../disasm/mediana.h"
#include "../../inc/types.h"

#define OUT_BUFF_SIZE 0x200
#define IN_BUFF_SIZE  0x1000
#define SEEK_TO       0x0
#define INT3		  0xCC
#define ARMADILLO

namespace analyser
{

class block
{
public:
	typedef std::vector<u32> REFS;
	block (u32 addr, u32 referer): is_valid(true), address(addr), size(0), count(0), is_analyzed(false)
	{
		if (referer)
		{
			RefFrom.push_back(referer);
		}
	}
//private:
	bool is_analyzed; // проанализирован ли блок?
	bool is_valid; // является ли корректным кодом?
	u32 address; // VA начала блока
	u32 address_end; // VA конца блока (адрес последнего байта последней инструкции блока)
	u32 count; // кол-во инструкций в блоке
	u32 size; // размер блока в байтах
	u32 delta; // дельта стэка
	u32 crc; // CRC значение блока
	u8 type; // тип блока (почему сделан обрыв на конце блока:
	// 0.  Начало нового блока (начало цикла, общее продолжение двух ветвей)
	// 1.  Команда типа Jcc/JCXZ
	// 2.  Команда JMP без зависимостей
	// 3.  Команда JMP с регистрами
	// 4.  Команда JMP с регистром и базовым адресом (switch)
	// 5.  Команда JMP/CALL ds:[X] - импорт
	// 6.  Команда INT 3
	// 7.  Вызов функции завершения текущего потока/процесса
	// 8.  Команда CALL с явно заданным внутри возвратом
	// 9.  Команда CALL без явно заданного возврата
	// 10. Команда группы LOOP
	// 11. Команда RET
	// 12. Команда INT N (не 3)
	// 13. Команда IRET
	// 14. SYSEXIT/SYSRET
	// Для команд типа Jcc/JCXZ (контрольные значения для анализа инструкций SWITCH)
	u32 regs; // На основе каких регистров делается принятие решения надо ли делать прыжок
	u32 val; // Значение с которым осуществлялось сравнение
	u16 id; // ID команды которая осуществляла переход
	REFS RefTo; // список всех ссылок из блока
	REFS RefFrom; // список адресов команд, данных ссылающихся на блок
	char label[16]; // метка в начале блока
	u32 flags; // различные флаги
};

typedef class block* block_ptr;
typedef struct {
	u32 from;
	u32 to;
} ref;
typedef std::map<u32,block_ptr> BLOCKS;
typedef std::pair <u32,block_ptr> block_pair;
class blocks_analyser
{
private:
	BLOCKS main_tree;
	// Mediana disassembler vars
	u8 sf_prefixes[MAX_INSTRUCTION_LEN];
	char buff[OUT_BUFF_SIZE];
	int reallen;
	u32 res;
	struct INSTRUCTION instr;
	struct DISASM_INOUT_PARAMS params;
	// END Mediana disassembler vars

public:
	u8 *base;
	u16 NumberOfSections;
	u32 ImageBase, EntryPoint, EP;
	u32 code_size, code_section_va, code_section_rva;

	blocks_analyser (u8 *header, u32 *file_offset, u32 *size);
	~blocks_analyser ();
	void code_load (u8 *pointer_to_code, u32 size);
#ifdef ARMADILLO
	void blocks_analyser::ananlyze_from_va (u32 v_start_addr, nano_db *nanos);
	int block_process (block *node, nano_db *nanos);
#else
	void blocks_analyser::ananlyze_from_va (u32 v_start_addr);
	int block_process (block *node);
#endif
};

}

#endif