#ifndef _REGIO_
#define _REGIO_

HANDLE reg_open_key(
		 IN PWCHAR key_name,
		 IN HANDLE par_key
		 );

HANDLE reg_create_key(
		 IN PWCHAR key_name,
		 IN HANDLE par_key
		 );

int reg_enum_key(
	  IN  HANDLE h_key,
	  IN  ULONG  index,
	  OUT PWCHAR key_name
	  );

int reg_query_val(
	  IN  HANDLE reg_key,
	  IN  PWCHAR val_name,
	  OUT PVOID  dat_buff,
	  IN  ULONG  buf_size
	  );


int reg_set_val(
	  IN HANDLE reg_key,
	  IN PWCHAR val_name,
	  IN ULONG  dat_type,
	  IN PVOID  dat_buff,
	  IN ULONG  dat_size
	  );

void delete_key_recurse(
		   IN HANDLE par_key,
		   IN PWCHAR sub_key
		   );

#endif