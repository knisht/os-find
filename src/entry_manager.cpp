#include "entry_manager.h"
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>

#include <vector>

const size_t BUF_SIZE = 1024;

void entry_manager::execute(std::string const &executable_name, std::string arg)
{
    arg.push_back('\0');
    std::string duplicate = executable_name + '\0';
    std::vector<char *> args{&(duplicate[0]), &(arg[0]), nullptr};
    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << strerror(errno) << std::endl;
        return;
    }
    if (pid == 0) {
        int result = execve(executable_name.c_str(), args.data(), environ);
        if (result == -1) {
            std::cerr << "Cannot execute given program" << std::endl;
        }
        exit(EXIT_SUCCESS);
    } else {
        int result = waitpid(pid, nullptr, 0);
        if (result == -1) {
            std::cerr << strerror(errno) << std::endl;
        }
    }
}

void entry_manager::process_entry(std::string const &exact_filename,
                                  std::string const &path,
                                  struct stat const &stat_info) const
{
    if (config.has_inum && config.inode != stat_info.st_ino) {
        return;
    }
    if (config.has_name && config.name != exact_filename) {
        return;
    }
    if (config.has_size) {
        if ((config.size_sign > 0 && stat_info.st_size < config.size) ||
            (config.size_sign == 0 && stat_info.st_size != config.size) ||
            (config.size_sign < 0 && stat_info.st_size > config.size)) {
            return;
        }
    }
    if (config.has_nlinks && config.nlinks != stat_info.st_nlink) {
        return;
    }
    if (config.has_exec) {
        execute(config.exec, path);
        return;
    }
    std::cout << path << std::endl;
}

struct linux_dirent {
    unsigned long d_ino;
    off_t d_off;
    unsigned short d_reclen;
    char const d_name[1]; // entry name cannot be empty, so this is correct
                          // replacement for variable-size arrays
};

// for raii
struct fd_wrapper {
    int fd;
    fd_wrapper(char const *directory_name)
    {
        fd = open(directory_name, O_RDONLY | O_DIRECTORY);
        if (fd == -1) {
            std::cerr << directory_name << ": " << strerror(errno) << std::endl;
        }
    }

    fd_wrapper(fd_wrapper const &) = delete;

    ~fd_wrapper()
    {
        if (fd != -1) {
            int result = close(fd);
            if (result == -1) {
                std::cerr << strerror(errno) << std::endl;
            }
        }
    }
};

void entry_manager::walk_dirs(std::string const &directory_name) const
{
    fd_wrapper wrapper{directory_name.c_str()};
    if (wrapper.fd == -1) {
        return;
    }
    char buf[BUF_SIZE];
    struct stat stat_info;
    for (;;) {
        long nread = syscall(SYS_getdents, wrapper.fd, buf, BUF_SIZE);
        if (nread == -1) {
            std::cerr << directory_name << ":" << strerror(errno) << std::endl;
            break;
        }
        if (nread == 0) {
            break;
        }
        for (int bpos = 0; bpos < nread;) {
            linux_dirent *dir = reinterpret_cast<linux_dirent *>(buf + bpos);
            char d_type = *(buf + bpos + dir->d_reclen - 1);
            std::string entry_name(dir->d_name);
            std::string current_path = directory_name + "/" + entry_name;
            if (d_type == DT_REG) {
                if (stat(current_path.c_str(), &stat_info) != 0) {
                    std::cerr << "Could not retrieve information about "
                              << current_path << ": " << strerror(errno)
                              << std::endl;
                } else {
                    process_entry(entry_name, current_path, stat_info);
                }
            } else if (d_type == DT_DIR && entry_name != "." &&
                       entry_name != "..") {
                walk_dirs(current_path);
            }
            bpos += dir->d_reclen;
        }
    }
}
