#include "stdafx.h"
#include <io.h>
#include "DebuggerCLI.h"

namespace po = boost::program_options;
using namespace std;

#define OPENDBG_VERSION_STR "0.1-dev"
#define CONFIG_FILE_PATH "cli.conf"

template<typename T>
std::ostream& operator<<(std::ostream& out, std::vector<T> const& v)
{
	if (!v.empty())
	{
		typedef std::ostream_iterator<T> out_iter;
		copy(v.begin(), v.end() - 1, out_iter( out, " " ));
		out << v.back();
	}
	return out;
}

int __cdecl _tmain(int argc, _TCHAR* argv[])
{
	try
	{
#ifdef _MSC_VER
		bool interactive = _isatty(_fileno(stdin)) != 0;
#else
		bool interactive = isatty(fileno(stdin)) != 0;
#endif
		bool quiet = !interactive;
		typedef map<string, po::options_description>::iterator MapIter;
		typedef vector<string>::const_iterator VectorIter;
		map<string, po::options_description> options_modules;

		po::options_description generic_options("Generic options");
		generic_options.add_options()
			("help,?", "show this help screen")
			("verbose", "show more information")
			("help-module", po::value< vector<string> >()->multitoken()->zero_tokens(), "show help module list or help for given module(s)")
			("version", "print version")
			("quiet,q", "do not display introductory message")
		;
		options_modules.insert(make_pair("generic", generic_options));

		po::options_description config_options("Configuration");
		config_options.add_options()
			("input-scripts,s", po::value< vector<string> >()->multitoken()->zero_tokens()->composing(), "input scripts")
		;
		options_modules.insert(make_pair("config", config_options));

		po::options_description hidden_options("Hidden options");
		hidden_options.add_options()
			("input-processes", po::value< vector<string> >()->multitoken(), "processes to analyze")
		;
		options_modules.insert(make_pair("hidden", hidden_options));

		po::options_description result_options;
		result_options.add(generic_options).add(config_options).add(hidden_options);
		po::options_description visible_options;
		visible_options.add(generic_options);

		po::positional_options_description pos_opt_desc;
		pos_opt_desc.add("input-processes", -1);

		po::variables_map vars;
		po::store(po::basic_command_line_parser<_TCHAR>(argc, argv).
			options(result_options).positional(pos_opt_desc).run(), vars);
		ifstream config_file(CONFIG_FILE_PATH);
		if (config_file.good())
			po::store(po::parse_config_file(config_file, result_options, true), vars);
		config_file.close();
		po::notify(vars);

		if (!vars.count("quiet") && !quiet)
		{
			cout << "opendbg " << OPENDBG_VERSION_STR << " cli" << endl;
			if (!(vars.count("help") || vars.count("help-module") || vars.count("input-processes") || vars.count("input-scripts")))
			{
				cout << "Copyright (C) 2009 Opendbg developers group." << endl;
				cout << "This program comes with ABSOLUTELY NO WARRANTY; for details type `show w'." << endl;
				cout << "This is free software, and you are welcome to redistribute it" << endl;
				cout << "under certain conditions; type `show c' for details." << endl;
			}
		}
		else
		{
			quiet = true;
		}
		if (vars.count("help"))
		{
			cout << visible_options << endl;
			if (vars.count("verbose"))
			{
				cout << "Additional debugger options can be specified in " << CONFIG_FILE_PATH << " file." << endl;
				cout << "For the full list of options, please see cli --help-module config" << endl;
			}
			return 1;
		}
		if (vars.count("help-module"))
		{
			const vector<string>& modules = vars["help-module"].as< vector<string> >();
			if (!modules.empty())
			{
				MapIter iter;
				for(VectorIter it = modules.begin(); it != modules.end(); it++)
				{
					if ((iter = options_modules.find(*it)) != options_modules.end())
						cout << iter->second << endl;
					else
						cout << "module " << *it << " not found" << endl;
				}
			}
			else
			{
				vector<string> module_names;
				for(MapIter it = options_modules.begin(); it != options_modules.end(); it++)
					module_names.push_back(it->first);
				cout << "available help modules: " << module_names << endl;
			}
			return 1;
		}
		if (vars.count("version"))
		{
			return 1;
		}
		DebuggerCLI cli;
		if (interactive)
			cli.SetInteractive(false);
		std::stringstream ss;
		if (vars.count("input-processes"))
		{
			const vector<string>& processes = vars["input-processes"].as< vector<string> >();
			for(VectorIter it = processes.begin(); it != processes.end(); ++it)
			{
				ss << "start " << *it << endl;
				cli.Interpret(ss);
				ss.clear();
			}
		}
		if (vars.count("input-scripts"))
		{
			const vector<string>& scripts = vars["input-scripts"].as< vector<string> >();
			for(VectorIter it = scripts.begin(); it != scripts.end(); ++it)
			{
				ss << "load " << *it << endl;
				cli.Interpret(ss);
				ss.clear();
			}
		}
		cli.SetInteractive(interactive);
		cli.Start();
	}
	catch (po::multiple_values&)
	{
		cerr << "error: too many values" << endl;
		return 1;
	}
	catch (po::multiple_occurrences&)
	{
		cerr << "error: some parameter occurs more than once" << endl;
	}
	catch (po::unknown_option& e)
	{
		cerr << "error: unknown option " << e.what() << endl;
	}
	catch (po::invalid_command_line_syntax& e)
	{
		switch (e.kind())
		{
		case po::invalid_command_line_syntax::missing_parameter:
			cerr << "error: " << e.what();
			break;
		default:
			cerr << "error: " << e.what();
			break;
		}
	}
	catch (po::error& e)
	{
		cerr << "error: unknown error parsing command-line arguments: " << e.what() << endl;
	}
	catch(pdb::pdb_error& e)
	{
		cerr << "pdb error: " << e.what() << endl;
	}
	catch(exception& e)
	{
		cerr << "error: " << e.what() << endl;
		return 1;
	}
	catch(...)
	{
		cerr << "exception of unknown type!\n";
	}
	return 0;
}
