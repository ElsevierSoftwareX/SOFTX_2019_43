#include <iostream>
#include <sstream>
#include <algorithm>
#include <type_traits>

#include "Tools/Exception/exception.hpp"

#include "Argument_handler.hpp"
#include "Tools/Display/bash_tools.h"

using namespace aff3ct::tools;

Argument_handler
::Argument_handler(const int argc, const char** argv)
{
	if (argc <= 0)
	{
		std::stringstream message;
		message << "'argc' has to be greater than 0 ('argc' = " << argc << ").";
		throw invalid_argument(__FILE__, __LINE__, __func__, message.str());
	}

	this->m_program_name = argv[0];

	command.resize(argc-1);

	for (unsigned short i = 0; i < argc-1; ++i)
		this->command[i] = std::string(argv[i+1]);
}

Argument_handler
::~Argument_handler()
{
}

Argument_map_value Argument_handler
::parse_arguments(const Argument_map_info &req_args, const Argument_map_info &opt_args,
                  std::vector<std::string> &warnings, std::vector<std::string> &errors)
{
	Argument_map_value m_arg_v;
	std::vector<bool> command_found_pos(this->command.size(),false);

	auto req_found_pos = this->sub_parse_arguments(req_args, m_arg_v, command_found_pos, errors);
	auto opt_found_pos = this->sub_parse_arguments(opt_args, m_arg_v, command_found_pos, errors);


	for (unsigned i = 0; i < req_found_pos.size(); ++i)
	{
		if (req_found_pos[i])
			continue;

		auto it_arg = req_args.begin();
		std::advance(it_arg, i);
		std::string message = "The " + print_tag(it_arg->first) + " argument is required.";
		errors.push_back(message);
	}

	for (unsigned i = 0; i < command_found_pos.size(); ++i)
	{
		if (command_found_pos[i])
			continue;

		std::string message = "Unknown argument \"" + this->command[i] + "\".";
		warnings.push_back(message);
	}

	return m_arg_v;
}

std::vector<bool> Argument_handler
::sub_parse_arguments(const Argument_map_info &args, Argument_map_value& arg_v, std::vector<bool>& command_found_pos,
                      std::vector<std::string>& messages)
{
	std::vector<bool> args_found_pos(args.size(), false);

	if (this->command.size() != command_found_pos.size())
		std::runtime_error("'command' and 'command_found_pos' vectors have to have the same length.");

	unsigned found_arg_count = 0;

	for (auto it_arg_info = args.begin(); it_arg_info != args.end(); it_arg_info++)
	{
		std::string message;

		// try to find each tag version of each argument
		for (auto it_tag = it_arg_info->first.begin(); it_tag != it_arg_info->first.end(); it_tag++)
		{
			const auto cur_tag = this->print_tag(*it_tag);

			// find all occurances of this tag in the command line
			for (unsigned ix_arg_val = 0; ix_arg_val < this->command.size(); ++ix_arg_val)
			{
				if (cur_tag == this->command[ix_arg_val]) // the tag has been found
				{
					if(it_arg_info->second.type->get_title() == "") // do not wait for a value after the tag
					{
						auto it = arg_v.find(it_arg_info->first);
						if (it == arg_v.end())
							found_arg_count++;

						arg_v[it_arg_info->first] = "";
						command_found_pos[ix_arg_val] = true;
						args_found_pos[std::distance(args.begin(), it_arg_info)] = true;

					}
					else // wait for a value with the tag
					{
						if(ix_arg_val != (this->command.size() -1))
						{
							auto it = arg_v.find(it_arg_info->first);
							if (it == arg_v.end())
								found_arg_count++;

							arg_v[it_arg_info->first] = this->command[ix_arg_val +1];
							command_found_pos[ix_arg_val   ] = true;
							command_found_pos[ix_arg_val +1] = true;
							args_found_pos[std::distance(args.begin(), it_arg_info)] = true;

							// check the found argument
							try
							{
								it_arg_info->second.type->check(this->command[ix_arg_val +1]);
								message = "";
							}
							catch(std::exception& e)
							{
								message = "The " + print_tag(it_arg_info->first) + " argument ";
								message += e.what();
								message += "; given at the position " + std::to_string(ix_arg_val+1) + " '";
								message += this->command[ix_arg_val] + " " + this->command[ix_arg_val +1] + "'.";
							}
						}
					}
				}
			}
		}

		if (message != "")
			messages.push_back(message);
	}

	return args_found_pos;
}

void Argument_handler
::print_usage(const Argument_map_info &req_args) const
{
	std::cout << "Usage: " << this->m_program_name;

	std::vector<std::string> existing_flags;

	for (auto it = req_args.begin(); it != req_args.end(); ++it)
	{
		if (std::find(existing_flags.begin(), existing_flags.end(), it->first.back()) == existing_flags.end())
		{
			if(it->second.type->get_title() != "")
				std::cout << " " + print_tag(it->first.back()) << " <" << it->second.type->get_title() << ">";
			else
				std::cout << " " + print_tag(it->first.back());

			existing_flags.push_back(it->first.back());
		}
	}
	std::cout << " [optional args...]" << std::endl << std::endl;
}

size_t Argument_handler
::find_longest_tags(const Argument_map_info &args) const
{
	size_t max_n_char_arg = 0;
	for (auto it = args.begin(); it != args.end(); ++it)
		max_n_char_arg = std::max(this->print_tag(it->first).size(), max_n_char_arg);

	return max_n_char_arg;
}

void Argument_handler
::print_help(const Argument_map_info &req_args, const Argument_map_info &opt_args) const
{
	this->print_usage(req_args);

	// found first the longest tag to align informations
	size_t max_n_char_arg = std::max(find_longest_tags(req_args), find_longest_tags(opt_args));

	// print arguments
	for (auto it = req_args.begin(); it != req_args.end(); ++it)
		this->print_help(it->first, it->second, max_n_char_arg, true);

	for (auto it = opt_args.begin(); it != opt_args.end(); ++it)
		this->print_help(it->first, it->second, max_n_char_arg, false);

	std::cout << std::endl;
}

void Argument_handler
::print_help(const Argument_tag &tags, const Argument_info &info, const size_t max_n_char_arg, const bool required) const
{
	Format arg_format = 0;

	const auto tab = "    ";

	std::string tags_str = this->print_tag(tags);
	tags_str.append(max_n_char_arg - tags_str.size(), ' ');

	std::cout << tab << format(tags_str, arg_format | Style::BOLD);

	if (info.type->get_title().size())
		std::cout << format(" <" + info.type->get_title() + ">", arg_format | FG::GRAY);

	if (required)
		std::cout << format(" {REQUIRED}", arg_format | Style::BOLD | FG::Color::ORANGE);

	std::cout << std::endl;
	std::cout << format(tab + info.doc, arg_format) << std::endl;
}

void Argument_handler
::print_help_title(const std::string& title) const
{
	Format head_format = Style::BOLD | Style::ITALIC | FG::Color::MAGENTA | FG::INTENSE;

	std::cout << format(title + ":", head_format) << std::endl;
}

void Argument_handler
::print_help(const Argument_map_info &req_args, const Argument_map_info &opt_args, const Argument_map_group& arg_groups) const
{
	this->print_usage(req_args);

	// found first the longest tag to align informations
	size_t max_n_char_arg = std::max(find_longest_tags(req_args), find_longest_tags(opt_args));
	bool title_displayed = false;

	// display each group
	for (auto it_grp = arg_groups.begin(); it_grp != arg_groups.end(); it_grp++)
	{
		title_displayed = false;

		auto& prefix = it_grp->first;

		// display first the required arguments of this group
		for (auto it_arg = req_args.begin(); it_arg != req_args.end(); it_arg++)
		{
			auto& tag = it_arg->first.front();

			if (tag.find(prefix) == 0)
			{
				if (!title_displayed)
				{
					print_help_title(it_grp->second);
					title_displayed = true;
				}

				this->print_help(it_arg->first, it_arg->second, max_n_char_arg, true);
			}
		}

		// display then the optional arguments of this group
		for (auto it_arg = opt_args.begin(); it_arg != opt_args.end(); it_arg++)
		{
			auto& tag = it_arg->first.front();

			if (tag.find(prefix) == 0)
			{
				if (!title_displayed)
				{
					print_help_title(it_grp->second);
					title_displayed = true;
				}

				this->print_help(it_arg->first, it_arg->second, max_n_char_arg, false);
			}
		}

		std::cout << std::endl;
	}


	title_displayed = false;
	// display the other required parameters
	for (auto it_arg = req_args.begin(); it_arg != req_args.end(); it_arg++)
	{
		auto& tag = it_arg->first.front();
		bool found = false;

		for (auto it_grp = arg_groups.begin(); it_grp != arg_groups.end(); it_grp++)
		{
			auto& prefix = it_grp->first;

			if (tag.find(prefix) == 0)
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			if (!title_displayed)
			{
				print_help_title("Other parameter(s)");
				title_displayed = true;
			}

			this->print_help(it_arg->first, it_arg->second, max_n_char_arg, true);
		}
	}

	// display the other optional parameters
	for (auto it_arg = opt_args.begin(); it_arg != opt_args.end(); it_arg++)
	{
		auto& tag = it_arg->first.front();
		bool found = false;

		for (auto it_grp = arg_groups.begin(); it_grp != arg_groups.end(); it_grp++)
		{
			auto& prefix = it_grp->first;

			if (tag.find(prefix) == 0)
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			if (!title_displayed)
			{
				print_help_title("Other parameter(s)");
				title_displayed = true;
			}

			this->print_help(it_arg->first, it_arg->second, max_n_char_arg, false);
		}
	}
}

std::string Argument_handler
::print_tag(const std::string& tag)
{
	return ((tag.size() == 1) ? "-" : "--") + tag;
}

std::string Argument_handler
::print_tag(const Argument_tag &tags)
{
	std::string txt = "\"";
		for (unsigned i = 0; i < tags.size(); i++)
			txt += print_tag(tags[i]) + ((i < tags.size()-1)?", ":"");
	txt += "\"";
	return txt;
}