#pragma once
#ifndef LEXICAL_H
#define LEXICAL_H

#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <set>
#include <unistd.h>
#include <map>

namespace Lexical {
    extern std::map<std::string, std::string> reserve;
    extern std::string source_code;
    extern std::vector< std::pair<std::string, std::string> > distrib;
    extern std::string current_path;
    extern std::vector<std::pair<int,int>> number_of_st;
    extern char pathbuf[100];
    extern int numst;
    extern int smvl;
    extern std::vector<std::string> lines;
    
    int64_t write(std::string path, std::string text);
    void set_up();
    void split_as_token(std::string path);
    bool all_digit(std::string elem);
    
    std::vector< std::pair<std::string, std::string> > run(std::string path);
}
#endif