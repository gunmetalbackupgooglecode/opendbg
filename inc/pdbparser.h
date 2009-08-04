#ifndef PDBPARSER_H__
#define PDBPARSER_H__

#include <windows.h>

#include <vector>
#include <exception>
#include <string>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <utility>
#include <map>

#include "dia2.h"

typedef unsigned long ulong;

namespace pdb
{

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

	sym_info(const std::wstring& name, ulong rva, ulong offset, ulong section)
	 : m_name(name),
	   m_rva(rva),
	   m_offset(offset),
	   m_section(section)
	{
	}

	sym_info(const std::wstring& name, ulong relative_offset)
	 : m_name(name),
	   m_rva(0),
	   m_offset(0),
	   m_section(0)
	{
	}

private:
	std::wstring m_name;
	ulong        m_rva;
	ulong        m_offset;
	ulong        m_section;
};

class member_info
{
public:
	member_info()
	 : m_name(),
	   m_offset(0)
	{
	}

	member_info(const std::wstring& name, ulong offset)
	 : m_name(name),
	   m_offset(offset)
	{
	}

	const std::wstring get_name()
	{
		return m_name;
	}

	ulong get_offset()
	{
		return m_offset;
	}

private:
	std::wstring m_name;
	ulong        m_offset;
};

class type_info
{
	typedef std::pair<std::wstring, member_info> member_info_pair;
	typedef std::map<std::wstring, member_info> member_info_map;

public:
	type_info()
	 : m_name(),
	   m_members()
	{
	}

	type_info(const std::wstring& name)
	 : m_name(name),
	   m_members()
	{
	}

	type_info(const std::wstring& name, const member_info_map& members)
	 : m_name(name),
	   m_members(members)
	{
	}

	const std::wstring get_name()
	{
		return m_name;
	}

	member_info_map get_members()
	{
		return m_members;
	}

	member_info& get_member(const std::wstring& name)
	{
		return m_members[name];
	}

private:
	std::wstring               m_name;
	member_info_map            m_members;
};

class pdb_parser
{
	typedef std::pair<std::wstring, sym_info> sym_info_pair;
	typedef std::map<std::wstring, sym_info> sym_info_map;
	typedef std::pair<std::wstring, type_info> type_info_pair;
	typedef std::map<std::wstring, type_info> type_info_map;

	typedef std::pair<std::wstring, member_info> member_info_pair;
	typedef std::map<std::wstring, member_info> member_info_map;

public:
	pdb_parser(const std::wstring& filename);

	void init_symbols(IDiaSymbol* global);
	void init_types(IDiaSymbol* global);
	member_info_map get_type_detail(IDiaSymbol* sym, int deep);
	member_info get_type_location(IDiaSymbol* sym);

	type_info& get_type(const std::wstring& name)
	{
		return m_types[name];
	}

	sym_info& get_symbol(const std::wstring& name)
	{
		return m_symbols[name];
	}

private:
	sym_info_map  m_symbols;
	type_info_map m_types;
};

pdb_parser::pdb_parser(const std::wstring& filename)
{
	IDiaDataSource *pSource;
	IDiaSession *pSession;

	IDiaSymbol *pGlobal;

	HRESULT hr = CoInitialize(NULL);

	// Obtain access to the provider
	hr = CoCreateInstance(__uuidof(DiaSource),
												NULL,
												CLSCTX_INPROC_SERVER,
												__uuidof(IDiaDataSource),
												(void **) &pSource);

	if (FAILED(hr))
		throw pdb_error("CoCreateInstance failed - HRESULT");

	if ( FAILED( pSource->loadDataForExe(filename.c_str(), NULL, NULL) ) )
		if ( FAILED( pSource->loadDataFromPdb(filename.c_str()) ) )
			throw pdb_error("symbols are not loaded");

	if (FAILED(hr))
		throw pdb_error("loadDataFromPdb failed - HRESULT");

	hr = pSource->openSession(&pSession);

	if (FAILED(hr))
		throw pdb_error("openSession failed - HRESULT");
	
	// Retrieve a reference to the global scope
	hr = pSession->get_globalScope(&pGlobal);

	if (FAILED(hr))
		throw pdb_error("openSession failed - HRESULT");

	init_symbols(pGlobal);
	init_types(pGlobal);
}

void pdb_parser::init_symbols(IDiaSymbol* global)
{
	IDiaEnumSymbols *pEnumSymbols;
	if (FAILED(global->findChildren(SymTagPublicSymbol, 0, nsNone, &pEnumSymbols)))
		throw pdb_error("child is not founded");

	IDiaSymbol *pSymbol;
	ULONG celt = 0;

	while (SUCCEEDED(pEnumSymbols->Next(1, &pSymbol, &celt)) && (celt == 1))
	{
		BSTR name;
		ulong rva, offset, section;
		
		pSymbol->get_name(&name);
		pSymbol->get_relativeVirtualAddress(&rva);
		pSymbol->get_addressOffset(&offset);
		pSymbol->get_addressSection(&section);
		
		sym_info sym(name, rva, offset, section);
		m_symbols.insert(sym_info_pair(name, sym));

		pSymbol->Release();
	}

	pEnumSymbols->Release();
}

member_info pdb_parser::get_type_location(IDiaSymbol* sym)
{
	ulong loc_type;

	if (sym->get_locationType(&loc_type) != S_OK)
		return member_info(0,0); // It must be a symbol in optimized code

	switch (loc_type)
	{
		case LocIsThisRel:
			BSTR name;
			long offset;
			sym->get_name(&name);
			if (sym->get_offset(&offset) == S_OK)
				return member_info(name, offset);
	}

	return member_info(L"", 0);
}

pdb_parser::member_info_map pdb_parser::get_type_detail(IDiaSymbol* sym, int deep)
{
	IDiaEnumSymbols *enum_chids;
	member_info_map members;

	ulong celt = 0;
	ulong symtag;
	
	if (deep > 2)
		return members;

	if (sym->get_symTag(&symtag) != S_OK)
		return members;

	switch(symtag)
	{
		case SymTagData:
		{
			member_info mem_info = get_type_location(sym);
			if (!mem_info.get_name().empty())
				members.insert(member_info_pair(mem_info.get_name(), mem_info));
		}
		return members;

		
		case SymTagUDT:
		if (SUCCEEDED(sym->findChildren(SymTagNull, NULL, nsNone, &enum_chids)))
		{
			IDiaSymbol* child;
			while (SUCCEEDED(enum_chids->Next(1, &child, &celt)) && (celt == 1))
			{
				member_info_map curr_mem_info = get_type_detail(child, deep+1);
				for (member_info_map::iterator i = curr_mem_info.begin(); i != curr_mem_info.end(); ++i)
					members.insert(member_info_pair((*i).first, (*i).second));

				child->Release();
			}
			enum_chids->Release();
		}
	}
	
	return members;
}

void pdb_parser::init_types(IDiaSymbol* global)
{
	IDiaEnumSymbols *pEnumSymbols;
	if (FAILED(global->findChildren(SymTagUDT, 0, nsNone, &pEnumSymbols)))
		throw pdb_error("child is not founded");

	IDiaSymbol *pSymbol;
	ulong celt = 0;

	while (SUCCEEDED(pEnumSymbols->Next(1, &pSymbol, &celt)) && (celt == 1))
	{
		BSTR name;
		pSymbol->get_name(&name);
		member_info_map members = get_type_detail(pSymbol, 1);
		m_types.insert(type_info_pair(name, type_info(name, members)));
		pSymbol->Release();
	}

	pEnumSymbols->Release();
}

} // namespace pdb

#endif // PDBPARSER_H__