#ifndef _NANO_DB_H
#define _NANO_DB_H

#include <vector>
#include <Windows.h>
#include <stdlib.h>
#include "../../inc/types.h"

#define NANO_BUFF_SIZE 10000000

//namespace nano
//{

class nano_db
{
private:
	typedef struct {
		u32 address;
		u8 length;
		u64 nano_bytes;
	} nanomite;
	typedef std::vector<nanomite> nano_elem;
	nano_elem nano_table;

public:
	nano_db (u8 *nano_data);
	~nano_db ();
	// Returns length of nanomite and delete it from table
	void get_orig_bytes (u32 address, u8 *nano_len, u64 *orig_bytes);
	void delete_by_addr (u32 address);
};

//}

#endif