#include "nano_db.h"

//namespace nano
//{

	nano_db::nano_db (u8 *nano_data)
	{
		nanomite temp;
		u32 addr;
		u8 *ptr = nano_data;
		u8 *end = nano_data + NANO_BUFF_SIZE;

		while ((addr = *(u32 *)ptr) && (ptr < end))
		{
			temp.address = addr;
			ptr += sizeof(u32);
			temp.length = *ptr;
			ptr += sizeof(u32);
			temp.nano_bytes = *(u64 *)ptr;
			nano_table.push_back(temp);
		}
	}

	nano_db::~nano_db()
	{
		nano_table.clear();
	}

	void nano_db::get_orig_bytes (u32 address, u8 *nano_len, u64 *orig_bytes)
	{
		nano_elem::iterator i;
		//u8 len;

		for (i = nano_table.begin(); i < nano_table.end(); i++)
		{
			if (i->address == address)
			{
				*orig_bytes = i->nano_bytes;
				*nano_len = i->length;
				nano_table.erase(i);

				//return nano_len;
			}
		}

		//return 0;
	}

	void nano_db::delete_by_addr (u32 address)
	{
		nano_elem::iterator i;

		for (i = nano_table.begin(); i < nano_table.end(); i++)
		{
			if (i->address == address)
			{
				nano_table.erase(i);
				break;
			}
		}
	}

//}