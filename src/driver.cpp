#include "argument_config.h"
#include "entry_manager.h"
#include <iostream>
#include <unordered_map>
#include <vector>

argument_config parse_args(size_t argc, char *argv[])
{
    std::vector<std::string> parsed_args;
    for (size_t i = 1; i < argc; ++i) {
        parsed_args.emplace_back(argv[i]);
    }
    if (parsed_args.size() == 0) {
        throw std::invalid_argument("Not enough arguments");
    }
    argument_config config{};
    config.start_dir_entry = parsed_args[0];
    for (size_t i = 1; i < parsed_args.size(); ++i) {
        if (parsed_args[i] == "-inum") {
            try {
                config.inode = std::stoull(parsed_args.at(i + 1));
                config.has_inum = true;
                ++i;
            } catch (std::logic_error &) {
                throw std::invalid_argument("Wrong -inum argument, see usage");
            }
        } else if (parsed_args[i] == "-name") {
            try {
                config.name = std::move(parsed_args.at(i + 1));
                config.has_name = true;
                ++i;
            } catch (std::logic_error &) {
                throw std::invalid_argument("Wrong -name argument, see usage");
            }
        } else if (parsed_args[i] == "-size") {
            try {
                config.size = std::stoll(parsed_args.at(i + 1).substr(1));
                char c = parsed_args[i + 1][0];
                if (c == '=') {
                    config.size_sign = 0;
                } else if (c == '+') {
                    config.size_sign = 1;
                } else if (c == '-') {
                    config.size_sign = -1;
                } else {
                    throw std::invalid_argument("");
                }
                config.has_size = true;
                ++i;
            } catch (std::logic_error &) {
                throw std::invalid_argument("Wrong -size argument, see usage");
            }
        } else if (parsed_args[i] == "-nlinks") {
            try {
                config.nlinks = std::stoull(parsed_args.at(i + 1));
                config.has_nlinks = true;
                ++i;
            } catch (std::logic_error &) {
                throw std::invalid_argument(
                    "Wrong -nlinks argument, see usage");
            }
        } else if (parsed_args[i] == "-exec") {
            try {
                config.exec = std::move(parsed_args.at(i + 1));
                config.has_exec = true;
                ++i;
            } catch (std::logic_error &) {
                throw std::invalid_argument("Wrong -exec argument");
            }
        } else {
            throw std::invalid_argument("Unknown argument: " + parsed_args[i]);
        }
    }
    return config;
}

void process(size_t argc, char *argv[])
{
    entry_manager manager{parse_args(argc, argv)};

    manager.walk_dirs(manager.config.start_dir_entry);
}

const std::string usage =
    R"BLOCK(
Usage: findy search_path [-inum num] [-name name] [-size [-+=]size] [-nlinks num] [-exec path]
)BLOCK";

int main(int argc, char *argv[])
{
    try {
        process(static_cast<size_t>(argc), argv);
    } catch (std::logic_error &e) {
        std::cerr << e.what() << std::endl;
        std::cout << usage << std::endl;
    }
}
