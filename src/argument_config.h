#ifndef ARGUMENT_CONFIG_H
#define ARGUMENT_CONFIG_H

#include <string>

struct argument_config {
    std::string start_dir_entry;
    std::string name;
    std::string exec;
    ino_t inode;
    nlink_t nlinks;
    off_t size;
    signed char size_sign;
    bool has_name;
    bool has_inum;
    bool has_size;
    bool has_nlinks;
    bool has_exec;
    char padding[2];
};

#endif
