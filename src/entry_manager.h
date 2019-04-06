#ifndef WALKER_H
#define WALKER_H
#include "argument_config.h"
#include <string>
#include <sys/stat.h>

struct entry_manager {
    argument_config config;
    void process_entry(std::string const &exact_filename,
                       std::string const &path,
                       struct stat const &stat_info) const;

    void walk_dirs(std::string const &directory_name) const;

private:
    static void execute(std::string const &executable_name, std::string arg);
};

#endif // WALKER_H
