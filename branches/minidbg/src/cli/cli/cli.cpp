#include "stdafx.h"
// isatty
#ifdef _MSC_VER
#include <io.h>
#else
#include <unistd.h>
#endif
#include <boost/program_options.hpp>
#include "debugger_cli.h"
#include "pdbparser.h"

namespace po = boost::program_options;

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

int main(int argc, char* argv[])
{
	try
	{
		// try to detemine if we are in interactive console session
#ifdef _MSC_VER
		bool interactive = _isatty(_fileno(stdin)) != 0;
#else
		bool interactive = isatty(fileno(stdin)) != 0;
#endif
		bool quiet = !interactive;
		typedef std::map<std::string, po::options_description>::iterator MapIter;
		typedef std::vector<std::string>::const_iterator VectorIter;
		std::map<std::string, po::options_description> options_modules;

		// initialize option sets
		po::options_description generic_options("Generic options");
		generic_options.add_options()
			("help,?", "show this help screen")
			("verbose", "show more information")
			("help-module", po::value< std::vector<std::string> >()->multitoken()->zero_tokens(), "show help module list or help for given module(s)")
			("version", "print version")
			("quiet,q", "do not display introductory message")
		;
		options_modules.insert(std::make_pair("generic", generic_options));

		po::options_description config_options("Configuration");
		config_options.add_options()
			("input-scripts,s", po::value< std::vector<std::string> >()->multitoken()->zero_tokens()->composing(), "input scripts")
		;
		options_modules.insert(std::make_pair("config", config_options));

		po::options_description hidden_options("Hidden options");
		hidden_options.add_options()
			("input-processes", po::value< std::vector<std::string> >()->multitoken(), "processes to analyze")
		;
		options_modules.insert(std::make_pair("hidden", hidden_options));

		po::options_description result_options;
		result_options.add(generic_options).add(config_options).add(hidden_options);
		po::options_description visible_options;
		visible_options.add(generic_options);

		po::positional_options_description pos_opt_desc;
		pos_opt_desc.add("input-processes", -1);

		// load variables from config file
		po::variables_map vars;
		po::store(po::basic_command_line_parser<char>(argc, argv).
			options(result_options).positional(pos_opt_desc).run(), vars);
		std::ifstream config_file(CONFIG_FILE_PATH);
		if (config_file.good())
			po::store(po::parse_config_file(config_file, result_options, true), vars);
		config_file.close();
		po::notify(vars);

		// show welcome message
		if (!vars.count("quiet") && !quiet)
		{
			std::cout << "opendbg " << OPENDBG_VERSION_STR << " cli\n";
			if (!(vars.count("help") || vars.count("help-module") || vars.count("input-processes") || vars.count("input-scripts")))
			{
				std::cout << "Copyright (C) 2009 opendbg developers group.\n";
				std::cout << "This program comes with ABSOLUTELY NO WARRANTY; for details type `show w'.\n";
				std::cout << "This is free software, and you are welcome to redistribute it\n";
				std::cout << "under certain conditions; type `show c' for details.\n";
			}
		}
		else
		{
			quiet = true;
		}
		// show help
		if (vars.count("help"))
		{
			std::cout << visible_options << "\n";
			if (vars.count("verbose"))
			{
				std::cout << "Additional debugger options can be specified in " << CONFIG_FILE_PATH << " file.\n";
				std::cout << "For the full list of options, please see cli --help-module config\n";
			}
			return 1;
		}
		if (vars.count("help-module"))
		{
			const std::vector<std::string>& modules = vars["help-module"].as< std::vector<std::string> >();
			if (!modules.empty())
			{
				MapIter iter;
				for(VectorIter it = modules.begin(); it != modules.end(); it++)
				{
					if ((iter = options_modules.find(*it)) != options_modules.end())
						std::cout << iter->second << "\n";
					else
						std::cout << "module " << *it << " not found\n";
				}
			}
			else
			{
				std::vector<std::string> module_names;
				for(MapIter it = options_modules.begin(); it != options_modules.end(); it++)
					module_names.push_back(it->first);
				std::cout << "available help modules: " << module_names << "\n";
			}
			return 1;
		}
		if (vars.count("version"))
		{
			return 1;
		}
		// start debugger cli
		debugger_cli cli;
		if (interactive)
			cli.set_interactive(false);
		std::stringstream ss;
		if (vars.count("input-processes"))
		{
			const std::vector<std::string>& processes = vars["input-processes"].as< std::vector<std::string> >();
			for(VectorIter it = processes.begin(); it != processes.end(); ++it)
			{
				ss << "start " << *it << "\n";
				cli.interpret(ss);
				ss.clear();
			}
		}
		if (vars.count("input-scripts"))
		{
			const std::vector<std::string>& scripts = vars["input-scripts"].as< std::vector<std::string> >();
			for(VectorIter it = scripts.begin(); it != scripts.end(); ++it)
			{
				ss << "load " << *it << "\n";
				cli.interpret(ss);
				ss.clear();
			}
		}
		cli.set_interactive(interactive);
		cli.start();
	}
	catch (po::multiple_values&)
	{
		std::cerr << "error: too many values\n";
		return 1;
	}
	catch (po::multiple_occurrences&)
	{
		std::cerr << "error: some parameter occurs more than once\n";
	}
	catch (po::unknown_option& e)
	{
		std::cerr << "error: unknown option " << e.what() << "\n";
	}
	catch (po::invalid_command_line_syntax& e)
	{
		switch (e.kind())
		{
		case po::invalid_command_line_syntax::missing_parameter:
			std::cerr << "error: " << e.what();
			break;
		default:
			std::cerr << "error: " << e.what();
			break;
		}
	}
	catch (po::error& e)
	{
		std::cerr << "error: unknown error parsing command-line arguments: " << e.what() << "\n";
	}
	catch(pdb::pdb_error& e)
	{
		std::cerr << "pdb error: " << e.what() << "\n";
	}
	catch(std::exception& e)
	{
		std::cerr << "error: " << e.what() << "\n";
		return 1;
	}
	catch(...)
	{
		std::cerr << "exception of unknown type!\n";
	}
	return 0;
}
