#include "pdbparser.h"

namespace pdb
{
	pdb_parser::pdb_parser(const string_type& filename)
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
				throw pdb_error("symbols are not loaded from");

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
		u32 celt = 0;

		while (SUCCEEDED(pEnumSymbols->Next(1, &pSymbol, &celt)) && (celt == 1))
		{
			BSTR name;
			u32 rva, offset, section;

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
		u32 loc_type;

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

		return member_info(_T(""), 0);
	}

	pdb_parser::member_info_map pdb_parser::get_type_detail(IDiaSymbol* sym, int deep)
	{
		IDiaEnumSymbols *enum_chids;
		member_info_map members;

		u32 celt = 0;
		u32 symtag;

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
		u32 celt = 0;

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

}
