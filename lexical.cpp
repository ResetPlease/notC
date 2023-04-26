#include "lexical.h"
#include <iostream>
#include <string>
#include <set>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>

std::string Lexical::source_code = "";
std::map< std::string, std::string > Lexical::reserve;
std::string Lexical::current_path = "";
char Lexical::pathbuf[100];
std::vector< std::pair<std::string, std::string> > Lexical::distrib;
std::vector<std::pair<int,int>> Lexical::number_of_st;
int Lexical::numst = 1;
int Lexical::smvl = 1;
std::vector<std::string> Lexical::lines;

void Lexical::set_up() {
    getwd(pathbuf);
    current_path = std::string(pathbuf);
    std::ifstream file(current_path + "/reserve.set");
    std::string line;
    std::string token;
    std::string about;
    if(file.is_open()){
        while (getline(file, line))
        {
            token = "";
            about = "";
            if(line[0] == '#' && line[1] == '!'){
                continue;
            }
            int j = 0;
            for(int i = 0; i < line.size(); ++i){
                if(line[i] == ' ')
                {
                    j = i;
                    break;
                }
                token += line[i];
            }
            for(int k = j+1; k<line.size(); ++k ){
                if(line[k] == '\n'){
                    continue;
                }
                about += line[k];
            }
            if(token == ""){
                token = " ";
            }
            reserve[token] = about;
        }    
    }
    else{
        std::cout << "Error file read to path: " <<  current_path + "/reserve.set"  << std::endl;
    }
    file.close();
    std::cout << "Reserve: \n";
    for(auto it : reserve){
        std::cout << it.first << "\t" << it.second << std::endl;
    }
    std::cout << std::endl;
}

bool Lexical::all_digit(std::string elem){
    for(int i=0;i<elem.size(); ++i){
        if(elem[i] < '0' || elem[i] > '9'){
            return false;
        }
    }
    return true;
}

void Lexical::split_as_token(std::string path) {
    std::string token = "";
    std::ifstream file(path);
    std::string myline = "";
    char symbol;
    std::string about = "";
    if(file.is_open()){
        file >> std::noskipws;
        while (file >> symbol)
        {   
            myline += symbol;
            if(symbol == '\n'){
                lines.push_back(myline);
                myline = "";
                numst++; smvl = 1; symbol = ' ';}
            std::string k(1,symbol);
            if(reserve.find(k) != reserve.end()){
                if(reserve.find(token) != reserve.end()){
                    distrib.push_back({token, reserve[token]});
                    number_of_st.push_back( {numst,smvl});
                    smvl += token.size();
                }
                else if(token != "" && token != " "){
                    if((long long)distrib.size()-2 >= 0 && distrib[distrib.size()-2].first == "using") {
                        distrib.push_back({token, "lib"});
                        number_of_st.push_back( {numst,smvl});
                        smvl += token.size();
                    }
                    else if(all_digit(token)){
                        distrib.push_back( {token, "const"} );
                        number_of_st.push_back( {numst,smvl});
                        smvl += token.size();
                    }
                    else
                    {
                        distrib.push_back( {token, "id"} );
                        number_of_st.push_back( {numst,smvl});
                        smvl += token.size();
                    }
                }
                token = "";
                if(symbol == '\"'){
                    token += symbol;
                    while(file >> symbol){
                        token+=symbol;
                        myline += symbol;
                        if(symbol == '\\' && file.peek() == '\"'){
                            file >> symbol;
                            myline += symbol;
                            token += symbol;
                        }
                        else if(symbol == '\"'){
                            distrib.push_back({token, "string_literal"});
                            number_of_st.push_back( {numst,smvl});
                            smvl += token.size();
                            token = "";
                            break;
                        }
                    }
                }
                else if(symbol == '-' && file.peek() == '>'){
                    distrib.push_back( {"->", "operator->"});
                    number_of_st.push_back( {numst,smvl});
                    smvl += token.size()+1;      
                    file >> symbol;
                    myline += symbol;
                }
                else if(symbol == '!' && file.peek() == '='){
                    distrib.push_back( {"!=", "operator!="});
                    number_of_st.push_back( {numst,smvl});
                    smvl += token.size()+1;      
                    file >> symbol;
                    myline += symbol;
                }
                else if( symbol == '=' && file.peek() == '='){
                    distrib.push_back( {"==", "operator=="});
                    number_of_st.push_back( {numst,smvl});
                    smvl += token.size()+1;   
                    file >> symbol;
                    myline += symbol;
                }
                else if(symbol == '|' && file.peek() == '|'){
                    distrib.push_back( {"||", "operator||"});
                    number_of_st.push_back( {numst,smvl});
                    smvl += token.size()+1;      
                    file >> symbol;
                    myline += symbol;
                }
                else if(symbol == '&' && file.peek() == '&'){
                    distrib.push_back( {"&&", "operator&&"});
                    number_of_st.push_back( {numst,smvl});
                    smvl += token.size()+1;      
                    file >> symbol;
                    myline += symbol;

                }
                else if(symbol == '+' && file.peek() == '+'){
                    distrib.push_back( {"++", "operator++"});    
                    number_of_st.push_back( {numst,smvl});
                    smvl += token.size()+1;  
                    file >> symbol;
                    myline += symbol;
                }
                else if(symbol == '-' && file.peek() == '-'){
                    distrib.push_back( {"--", "operator--"});    
                    number_of_st.push_back( {numst,smvl});
                    smvl += token.size()+1;  
                    file >> symbol;
                    myline += symbol;
                }
                else{
                    distrib.push_back( {k, reserve[k]});
                    number_of_st.push_back( {numst,smvl});
                    smvl += token.size()+1;  
                }
                continue;
            }
            token += symbol;
        }
        lines.push_back(myline);
    }
    else{
        std::cout << "Error file read to path: " <<  path  << std::endl;
    }
    std::cout << "Code: \n";
    std::string res = "";
    for(auto it : distrib) {
        if(it.first == " ") continue;
        res += (it.first + "\t" + it.second + "\n");
        std::cout << it.first << "\t" << it.second << std::endl;
    }
    std::string name = "";
    for(int i=path.size()-4; i>=0;--i){
        if(path[i] == '/') break;
        name += path[i];
    }
    std::reverse(name.begin(), name.end());
    write(current_path + "/" + name + ".lex", res);
}

int64_t Lexical::write(std::string path, std::string text) {
    std::ofstream file(path);
    file << text;
    file.close();
    return text.size();
}

std::vector< std::pair<std::string, std::string>> Lexical::run(std::string path) {
    set_up();
    split_as_token(path);
    std::vector< std::pair<std::string, std::string>> original;
    std::vector<std::pair<int,int>> place;
    for(int i=0;i<distrib.size(); ++i){
        if(distrib[i].second != "space")
        {
            original.push_back(distrib[i]);
            place.push_back({number_of_st[i].first, number_of_st[i].second});
        }
    }
    for(int i=0;i<lines.size(); ++i){
        std::cout << lines[i] << std::endl;
    }
    number_of_st.clear();
    number_of_st = place;
    return original;
}