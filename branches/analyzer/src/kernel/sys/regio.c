/*
    *    
    * Copyright (c) 2008 
    * ntldr <ntldr@freed0m.org> PGP key ID - 0xC48251EB4F8E4E6E
    *

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <ntifs.h>
#include "defines.h"


HANDLE reg_open_key(
		 IN PWCHAR key_name,
		 IN HANDLE par_key
		 )
{
	OBJECT_ATTRIBUTES obj;
	UNICODE_STRING    u_name;
	HANDLE            h_key = NULL;

	RtlInitUnicodeString(
		&u_name, key_name
		);

	InitializeObjectAttributes(
		&obj, 
		&u_name, 
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, 
		par_key, 
		NULL
		);

	ZwOpenKey(&h_key, KEY_ALL_ACCESS, &obj);

	return h_key;
}

HANDLE reg_create_key(
		 IN PWCHAR key_name,
		 IN HANDLE par_key
		 )
{
	OBJECT_ATTRIBUTES obj;
	UNICODE_STRING    u_name;
	HANDLE            h_key = NULL;

	RtlInitUnicodeString(
		&u_name, 
		key_name
		);

	InitializeObjectAttributes(
		&obj, 
		&u_name, 
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, 
		par_key, 
		NULL
		);

	ZwCreateKey(
		&h_key,
		KEY_ALL_ACCESS,
		&obj,
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		NULL
		);

	return h_key;
}

int reg_enum_key(
	  IN  HANDLE h_key,
	  IN  ULONG  index,
	  OUT PWCHAR key_name
	  )
{
	WCHAR                  inf_buff[MAX_PATH];
	PKEY_BASIC_INFORMATION info = (void*)&inf_buff;
	NTSTATUS               status;
	ULONG                  res_len;

	status = ZwEnumerateKey(
		         h_key, 
				 index,
				 KeyBasicInformation, 
				 info, 
				 sizeof(inf_buff), 
				 &res_len
				 );

	if (NT_SUCCESS(status))
	{
		memcpy(key_name, info->Name, info->NameLength);

		key_name[info->NameLength >> 1] = 0;

		return 1;
	}

	return 0;
}

int reg_query_val(
	  IN  HANDLE reg_key,
	  IN  PWCHAR val_name,
	  OUT PVOID  dat_buff,
	  IN  ULONG  buf_size
	  )
{
	WCHAR                          inf_buff[MAX_PATH];
	PKEY_VALUE_PARTIAL_INFORMATION info = (void*)&inf_buff;
	UNICODE_STRING                 u_name;
	NTSTATUS                       status;
	ULONG                          res_len;

	memset(inf_buff, 0, sizeof(inf_buff));

	RtlInitUnicodeString(
		&u_name, 
		val_name
		);

	status = ZwQueryValueKey(
		        reg_key,
				&u_name,
				KeyValuePartialInformation,
				info,
				sizeof(inf_buff),
				&res_len
				);

	if (NT_SUCCESS(status))
	{
		if (info->DataLength < buf_size) {
			buf_size = info->DataLength;
		}

		memcpy(dat_buff, info->Data, buf_size);

		return 1;
	}

	return 0;
}

int reg_set_val(
	  IN HANDLE reg_key,
	  IN PWCHAR val_name,
	  IN ULONG  dat_type,
	  IN PVOID  dat_buff,
	  IN ULONG  dat_size
	  )
{
	UNICODE_STRING u_name;
	NTSTATUS       status;

	RtlInitUnicodeString(
		&u_name, 
		val_name
		);

	status = ZwSetValueKey(
		       reg_key,
			   &u_name,
			   0,
			   dat_type,
			   dat_buff,
			   dat_size
			   );

	return NT_SUCCESS(status);
}

int delete_key_recurse(
		   IN HANDLE par_key,
		   IN PWCHAR sub_key
		   )
{
	HANDLE h_key;
	WCHAR  name[MAX_PATH];
	ULONG  idx = 0;
	int    res = 0;

	if (h_key = reg_open_key(sub_key, par_key))
	{
		while (reg_enum_key(h_key, idx++, name))
		{
			if (delete_key_recurse(h_key, name)) {
				idx--;
			}
		}

		res = NT_SUCCESS(ZwDeleteKey(h_key));

		ZwClose(h_key);
	}

	return res;
}

