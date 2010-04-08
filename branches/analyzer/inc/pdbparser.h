/*
    *
    * Copyright (c) 2009
    * d1mk4 <d1mk4nah@gmail.com>
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


#ifndef PDBPARSER_H__
#define PDBPARSER_H__

#include <windows.h>
#include <cstdio>
#include <exception>
#include <string>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <xstring>
#include <utility>
#include <map>
#include <vector>
#include "defines.h"
#include "dia2.h"

namespace pdb
{

#ifdef OLE2ANSI
	#ifndef _T
			#define _T(x)      x
	#endif

	typedef char char_type;
#else
	#ifndef _T
			#define _T(x)      L ## x
	#endif
	typedef wchar_t char_type;
#endif

typedef std::basic_string<char_type> string_type;

class pdb_error : public std::exception
{
public:
	explicit pdb_error(const std::string& msg)
	 : m_msg(msg)
	{}

	virtual ~pdb_error() throw() // destroy the object
	{}

	virtual const char* what() const throw() // return pointer to message string
	{
		return m_msg.c_str();
	}

private:
	std::string m_msg;
};

class sym_info
{
public:
	sym_info()
	 : m_name(0),
	   m_rva(0),
	   m_offset(0),
	   m_section(0)
	{
	}

	sym_info(const string_type& name, u32 rva, u32 offset, u32 section)
	 : m_name(name),
	   m_rva(rva),
	   m_offset(offset),
	   m_section(section)
	{
	}

	sym_info(const string_type& name)
	 : m_name(name),
	   m_rva(0),
	   m_offset(0),
	   m_section(0)
	{
	}

	string_type get_name()
	{
		return m_name;
	}

	u32 get_rva()
	{
		return m_rva;
	}

	u32 get_offset()
	{
		return m_offset;
	}

	u32 get_section()
	{
		return m_section;
	}

private:
	string_type  m_name;
	u32        m_rva;
	u32        m_offset;
	u32        m_section;
};

class member_info
{
public:
	member_info()
	 : m_name(),
	   m_offset(0)
	{
	}

	member_info(const string_type& name, u32 offset)
	 : m_name(name),
	   m_offset(offset)
	{
	}

	string_type get_name()
	{
		return m_name;
	}

	u32 get_offset()
	{
		return m_offset;
	}

private:
	string_type  m_name;
	u32        m_offset;
};

class type_info
{
	typedef std::pair<string_type, member_info> member_info_pair;
	typedef std::map<string_type, member_info> member_info_map;

public:
	type_info()
	 : m_name(),
	   m_members()
	{
	}

	type_info(const string_type& name)
	 : m_name(name),
	   m_members()
	{
	}

	type_info(const string_type& name, const member_info_map& members)
	 : m_name(name),
	   m_members(members)
	{
	}

	const string_type get_name()
	{
		return m_name;
	}

	member_info_map get_members()
	{
		return m_members;
	}

	member_info& get_member(const string_type& name)
	{
		return m_members[name];
	}

private:
	string_type               m_name;
	member_info_map            m_members;
};

class pdb_parser
{
	typedef std::pair<string_type, sym_info> sym_info_pair;
	typedef std::map<string_type, sym_info> sym_info_map;
	typedef std::pair<string_type, type_info> type_info_pair;
	typedef std::map<string_type, type_info> type_info_map;

	typedef std::pair<string_type, member_info> member_info_pair;
	typedef std::map<string_type, member_info> member_info_map;

public:
	pdb_parser(const string_type& filename);

	void init_symbols(IDiaSymbol* global);
	void init_types(IDiaSymbol* global);
	member_info_map get_type_detail(IDiaSymbol* sym, int deep);
	member_info get_type_location(IDiaSymbol* sym);

	type_info& get_type(const string_type& name)
	{
		return m_types[name];
	}

	sym_info& get_symbol(const string_type& name)
	{
		return m_symbols[name];
	}

private:
	sym_info_map  m_symbols;
	type_info_map m_types;
};

} // namespace pdb

#endif // PDBPARSER_H__