#ifndef _SWITCH_DB_H
#define _SWITCH_DB_H

#include <vector>
#include <Windows.h>
#include <stdlib.h>
#include "types.h"

namespace analyser
{

class switch_db
{
private:
	typedef struct {
		u32 address;
		u8 scale;
		u32 size;
	} switch_table;
	typedef std::vector<switch_table> switch_tables;
	switch_tables switches;

public:
	switch_db ();
	~switch_db ();
	void add_table (u32 address, u8 scale, u32 size);
	bool verify_addr (u32 *address);
};

}

#endif