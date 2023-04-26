#include "lexical.h"
#include "syntax.h"
#include <string>
#include <iostream>

int main(int argc, char** argv){
    if(argc < 2){
        std::cout << "Error: path to file is not specified";
        return 0;
    }
    auto rt = Lexical::run(argv[1]);
    rt.push_back({"000", "operator.END"});
    Syntax a(rt, Lexical::lines);
    a.SetNumberST(Lexical::number_of_st);
    a.Start();
    a.List();
    a.Run();
    return 0;
}