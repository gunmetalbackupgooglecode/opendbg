#ifndef _DBGSYM_H__
#define _DBGSYM_H__

#include <windows.h>
#include <tchar.h>

#define _NO_CVCONST_H
#include <dbghelp.h>

#include <stdio.h>

#include <vector>
#include <exception>
#include <string>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <xstring>
#include <utility>

#include <map>

template <typename T>
class dbgsym_traits
{
public:
#ifdef UNICODE
	typedef SYMBOL_INFOW sym_info_type;
#else
	typedef SYMBOL_INFO sym_info_type;
#endif
	typedef T                                                       char_type;
	typedef std::basic_string<char_type>                            string_type;
	typedef size_t                                                  size_type;
	typedef ptrdiff_t                                               difference_type;
	typedef dbgsym_traits<char_type>                                class_type;
	typedef int                                                     int_type;
	typedef bool                                                    bool_type;
	typedef unsigned long                                           ulong_type;
	typedef HANDLE                                                  handle_type;
	typedef HINSTANCE                                               module_type;
	typedef DWORD                                                   error_type;
	typedef bool_type (__stdcall *enum_callback_type)(sym_info_type*, ulong_type, void*);

public:
	static handle_type get_current_process();
	static ulong_type get_file_size(handle_type h);
	static ulong_type sym_init();
	static ulong_type sym_cleanup();
	static ulong_type get_sym_options();
	static ulong_type set_sym_options(ulong_type opt);
	static ulong_type sym_load_module(string_type const& modname);
	static ulong_type sym_unload_module(ulong_type modbase);
	static ulong_type sym_enum_symbols(ulong_type modbase, string_type const& search_mask, enum_callback_type callback, void* arg);
};

template <>
class dbgsym_traits<char>
{
public:
	typedef char                                                    char_type;
	typedef std::basic_string<char_type>                            string_type;
	typedef size_t                                                  size_type;
	typedef ptrdiff_t                                               difference_type;
	typedef dbgsym_traits<char_type>                                class_type;
	typedef int                                                     int_type;
	typedef bool                                                    bool_type;
	typedef unsigned long                                           ulong_type;
	typedef HANDLE                                                  handle_type;
	typedef HINSTANCE                                               module_type;
	typedef DWORD                                                   error_type;
	typedef SYMBOL_INFO                                             sym_info_type;
	typedef PSYM_ENUMERATESYMBOLS_CALLBACK                          enum_callback_type;
	typedef IMAGEHLP_MODULE                                         module_info_type;

public:
	static handle_type get_current_process()
	{
		return ::GetCurrentProcess();
	}

	static ulong_type get_file_size(handle_type h)
	{
		ulong_type dwHigh;
		ulong_type dwLow   =   ::GetFileSize(h, &dwHigh);

		if( 0xFFFFFFFF == dwLow &&
			ERROR_SUCCESS != ::GetLastError())
			dwHigh = 0xFFFFFFFF;

		return (static_cast<ulong_type>(dwHigh) << 32) | dwLow;
	}

	static ulong_type sym_init()
	{
		return ::SymInitialize(get_current_process(), 0, false);
	}

	static ulong_type sym_cleanup()
	{
		return ::SymCleanup(get_current_process());
	}

	static ulong_type get_sym_options()
	{
		return ::SymGetOptions();
	}

	static ulong_type set_sym_options(ulong_type opt)
	{
		return ::SymSetOptions(opt);
	}

	static ulong_type sym_load_module(string_type const& modname)
	{
		ulong_type baseaddr = 0, filesize = 0;
		return ::SymLoadModule(get_current_process(), 0, modname.c_str(), 0, baseaddr, filesize);
	}

	static ulong_type sym_unload_module(ulong_type modbase)
	{
		return ::SymUnloadModule64(get_current_process(), modbase);
	}

	static ulong_type sym_enum_symbols(ulong_type modbase, string_type const& search_mask, void* callback, void* arg)
	{
		return ::SymEnumSymbols(get_current_process(), modbase, NULL, static_cast<enum_callback_type>(callback), arg);
	}

	static module_info_type sym_get_module_info(ulong_type modbase)
	{
		module_info_type module_info;
		memset(&module_info, 0, sizeof(module_info));
		::SymGetModuleInfo(get_current_process(), modbase, &module_info);
		return module_info;
	}
};

template <>
class dbgsym_traits<wchar_t>
{
public:
	typedef wchar_t                                                 char_type;
	typedef std::basic_string<char_type>                            string_type;
	typedef size_t                                                  size_type;
	typedef ptrdiff_t                                               difference_type;
	typedef dbgsym_traits<char_type>                                class_type;
	typedef int                                                     int_type;
	typedef bool                                                    bool_type;
	typedef unsigned long                                           ulong_type;
	typedef HANDLE                                                  handle_type;
	typedef HINSTANCE                                               module_type;
	typedef DWORD                                                   error_type;
	typedef SYMBOL_INFOW                                            sym_info_type;
	typedef PSYM_ENUMERATESYMBOLS_CALLBACKW                         enum_callback_type;
	typedef IMAGEHLP_MODULEW                                      module_info_type;

public:
	static handle_type get_current_process()
	{
		return ::GetCurrentProcess();
	}

	static ulong_type get_file_size(handle_type h)
	{
		DWORD   dwHigh;
		DWORD   dwLow   =   ::GetFileSize(h, &dwHigh);

		if( 0xFFFFFFFF == dwLow &&
			ERROR_SUCCESS != ::GetLastError())
			dwHigh = 0xFFFFFFFF;

		return (static_cast<ulong_type>(dwHigh) << 32) | dwLow;
	}

	static ulong_type sym_init()
	{
		return ::SymInitializeW(get_current_process(), 0, false);
	}

	static ulong_type sym_cleanup()
	{
		return ::SymCleanup(get_current_process());
	}

	static ulong_type get_sym_options()
	{
		return ::SymGetOptions();
	}

	static ulong_type set_sym_options(ulong_type opt)
	{
		return ::SymSetOptions(opt);
	}

	static ulong_type sym_load_module(string_type const& modname)
	{
		ulong_type baseaddr = 0, filesize = 0;
		std::string modname_;
		wcstombs(&modname_[0], modname.c_str(), modname.size());
		return ::SymLoadModule(get_current_process(), 0, modname_.c_str(), 0, baseaddr, filesize);
	}

	static ulong_type sym_unload_module(ulong_type modbase)
	{
		return ::SymUnloadModule(get_current_process(), modbase);
	}

	static ulong_type sym_enum_symbols(ulong_type modbase, string_type const& search_mask, void* callback, void* arg)
	{
		return ::SymEnumSymbolsW(get_current_process(), modbase, search_mask.c_str(), reinterpret_cast<enum_callback_type>(callback), arg);
	}

	static module_info_type sym_get_module_info(ulong_type modbase)
	{
		module_info_type module_info;
		memset(&module_info, 0, sizeof(module_info));
		::SymGetModuleInfoW(get_current_process(), modbase, &module_info);
		return module_info;
	}
};

template <typename C, typename T = dbgsym_traits<C> >
class module_properties
{
public:
	typedef C                                            char_type;
	typedef T                                            traits_type;
	typedef typename traits_type::ulong_type             ulong_type;
	typedef typename traits_type::string_type            string_type;
	typedef typename traits_type::bool_type              bool_type;
	typedef typename traits_type::module_info_type       module_info_type;

public:
	void init()
	{
		if (!traits_type::sym_init())
			throw std::runtime_error("Error: sym_init");

		m_options = traits_type::get_sym_options();
		m_options |= SYMOPT_DEBUG;
		traits_type::set_sym_options(m_options);
	}

	module_properties()
	{
		init();
	}

	~module_properties()
	{
		// Unload symbols for the module
		if (!traits_type::sym_unload_module(m_modbase))
			throw std::runtime_error("Error: sym_unload_module");

		if (!traits_type::sym_cleanup())
			throw std::runtime_error("Error: sym_cleanup");
	}

	module_properties(string_type const& modname)
	 : m_modname(modname)
	{
		init();

		m_modbase = traits_type::sym_load_module(modname);
		if(!m_modbase)
			throw std::runtime_error("Error: sym_load_module");

		m_module_info = traits_type::sym_get_module_info(m_modbase);
	}

	// Get information about loaded symbols 
	module_info_type get_properties()
	{
		return m_module_info;
	}

	string_type get_modname()
	{
		return m_modname;
	}

	ulong_type get_modbase()
	{
		return m_modbase;
	}

	ulong_type get_options()
	{
		return m_options;
	}

	ulong_type get_timestamp()
	{
		return m_modbase.TimeDateStamp;
	}

private:
	ulong_type        m_modbase;
	string_type       m_modname;
	module_info_type  m_module_info;
	ulong_type        m_options;
};

template <typename T>
class sym_info
{
public:
	typedef char                         char_type;
	typedef std::basic_string<char_type> string_type;
	typedef unsigned long                ulong_type;
#ifdef UNICODE
	typedef SYMBOL_INFOW                 sym_info_type;
#else
	typedef SYMBOL_INFO                  sym_info_type;
#endif
	typedef size_t                       size_type;
	typedef sym_info_type*               pointer;

public:
	sym_info()
		: m_size(( sizeof(sym_info_type)+ MAX_SYM_NAME*sizeof(char_type)
		+sizeof(ulong_type) - 1 )/sizeof(ulong_type))
	{
		m_obj = (pointer)new char[m_size];
	}

	sym_info(const sym_info_type& rhs)
		: m_size(( sizeof(sym_info_type)+ MAX_SYM_NAME*sizeof(char_type)
		+sizeof(ulong_type) - 1 )/sizeof(ulong_type))
	{
		m_obj = (pointer)new char[m_size];
		string_type name(rhs.Name);
		memcpy(m_obj, &rhs, m_size);
		strcpy(m_obj->Name, name.c_str());
	}

	sym_info(const sym_info& rhs)
		: m_size(rhs.m_size)
	{
		m_obj = new sym_info_type(*rhs.m_obj);
	}

	void swap(sym_info& other) throw()
	{
		std::swap<sym_info_type*>(m_obj, other.m_obj);
	}

	sym_info& operator=(sym_info rhs)
	{
		swap(rhs);
		return *this;
	}

	~sym_info()
	{
		delete m_obj;
	}

	pointer get_pointer()
	{
		return m_obj;
	}

	string_type get_name()
	{
		return string_type(m_obj->Name);
	}

private:
	const size_type m_size;
	pointer         m_obj;
};

template <typename C, typename T = dbgsym_traits<C> >
class dbgsym_sequence
{
public:
	typedef C                                            char_type;
	typedef T                                            traits_type;
	typedef dbgsym_sequence<C, T>                        class_type;
	typedef module_properties<C, T>                      module_properties;
	typedef typename traits_type::ulong_type             ulong_type;
	typedef typename traits_type::string_type            string_type;
	typedef typename traits_type::bool_type              bool_type;
	typedef sym_info<char_type>                          sym_info_type;
	typedef typename traits_type::sym_info_type          sym_info_type_;
	typedef std::pair<string_type, sym_info_type>        sym_info_pair_type;
	typedef std::map<string_type, sym_info_type>         sym_info_map_type;
	typedef typename sym_info_map_type::iterator         iterator;
	typedef typename sym_info_map_type::const_iterator   const_iterator;
	typedef typename sym_info_map_type::key_type         key_type;

public:
	explicit dbgsym_sequence(string_type const& module_name)
	 : m_modname(module_name)
	{
		module_properties mod_info(module_name);

		if (!traits_type::sym_enum_symbols(mod_info.get_modbase(), "", reinterpret_cast<traits_type::enum_callback_type>(enum_sym_callback), this))
			throw std::runtime_error("Error: sym_enum_symblos");
	}

	dbgsym_sequence(module_properties& mod_info)
	{
		if (!traits_type::sym_enum_symbols(mod_info.get_modbase(), "", reinterpret_cast<traits_type::enum_callback_type>(enum_sym_callback), this))
			throw std::runtime_error("Error: sym_enum_symblos");
	}

	~dbgsym_sequence()
	{
	}

	static bool_type __stdcall enum_sym_callback(sym_info_type_* sym_info, ulong_type symsize, void* arg)
	{
		class_type* pthis = static_cast<class_type*>(arg);
		if( sym_info != 0 )
		{
			sym_info_type obj(*sym_info);
			pthis->m_sym_map.insert(sym_info_pair_type(obj.get_name(), obj));
		}

		return true; // Continue enumeration
	}

	sym_info_type& operator[](const key_type& key)
	{
		return m_sym_map[key];
	}

	const_iterator begin() const
	{
		return m_sym_map.begin();
	}

	const_iterator end() const
	{
		return m_sym_map.end();
	}

private:
	string_type m_search_mask;
	string_type m_modname;
	sym_info_map_type m_sym_map;
};

/////////////////////////////////////////////////////////////////////////////////
//// main 
////
//
//void print_sym(const std::pair<std::string, sym_info<char> >& sym)
//{
//	std::cout << "name: " << sym.first << "\n";
//}
//
//int main( int argc, const char* argv[] )
//{
//	module_properties<char> mod_info("ntoskrnl.exe");
//
//	dbgsym_sequence<char> dbg_sym(mod_info);
//
//	sym_info<char> t = dbg_sym["wctomb"];
//
//	return 0;
//}

#endif //_DBGSYM_H__
