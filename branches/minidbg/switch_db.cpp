#include "switch_db.h"

namespace analyser
{

switch_db::switch_db ()
{
	switches.clear();
}

switch_db::~switch_db()
{
	switches.clear();
}

void switch_db::add_table (u32 address, u8 scale, u32 size)
{
	switch_table table;

	table.address = address;
	table.scale = scale;
	table.size = size;

	switches.push_back(table);
}

bool switch_db::verify_addr (u32 *address)
{
	switch_tables::iterator i;
	u32 addr1 = *address;

	for (i = switches.begin(); i < switches.end(); i++)
	{
		if (addr1 >= i->address && addr1 < (i->address * i->size * i->scale))
		{
			*address = i->address * i->size * i->scale;
			return true;
		}
	}

	return false;
}

}
