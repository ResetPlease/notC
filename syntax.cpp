#include "syntax.h"
#include <vector>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <map>

Syntax::Syntax(std::vector< std::pair<std::string, std::string>> tokens, std::vector<std::string> t) : distrib(tokens), superlines(t) {
    blnc["service("] = 0;
    blnc["service["] = 0;
    blnc["service{"] = 0;
    priority["."] = 8;
    priority["*"] = priority["/"] = priority["%"] = 7;
    priority["+"] = priority["-"] = 6;
    priority[">"] = priority["<"] = 5;
    priority["=="] = priority["!="] = 4;
    priority["&&"] = 3;
    priority["||"] = 2;
    priority["="] = 1;
    priority["("] = 0;
    priority[";"] = -1;
    TID.push_back(std::vector<VRBL>());
}

void Syntax::ExpToPolize(){
    std::vector<std::pair<std::string, std::string>> mystack;
    poliz_exp.clear();
    for(int i=0;i<expression.size(); ++i){
        if(expression[i].second == "id" ||
           expression[i].second == "string_literal" ||
           expression[i].second == "const"){
               if(expression[i].second == "id"){
                   VRBL* tempr = findclone(expression[i].first);
                   std::cout << "COUT::: " << expression[i].first << std::endl;
                   if(tempr != nullptr){
                    poliz_exp.push_back({tempr->type,expression[i].first});
                    CG.Emit("LOAD",expression[i].first);
                   }
                   else{
                    error_print(number_st[gidx]);
                    std::cout << "SyntaxError{NULL_PTR IN EXPRESSION}{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
                    exit(1);
                   }
               }
               else if(expression[i].second == "string_literal"){
                   poliz_exp.push_back({"str",expression[i].first});
                   CG.Emit("PUSH",expression[i].first);
               }
               else{
                   if(expression[i].first == "false" || expression[i].first == "true"){
                       poliz_exp.push_back({"boolean",expression[i].first});
                       CG.Emit("PUSH",expression[i].first == "true");
                   }
                   else{
                       poliz_exp.push_back({"int",expression[i].first});
                       CG.Emit("PUSH",std::stoi(expression[i].first));
                   }
               }
           }
        else if(sfind(binary_operator, expression[i].second)){
            while(mystack.size() > 0 && priority[mystack.back().first] >= priority[expression[i].first]){
                auto op = mystack.back();
                poliz_exp.push_back(op);
                CG.Emit(op.first);
                mystack.pop_back();
            }
            mystack.push_back(expression[i]);
        }
        else if(expression[i].second == "service("){
            mystack.push_back(expression[i]);
        }
        else if(expression[i].second == "service)"){
            while(mystack.size() > 0 && mystack.back().second != "service("){
                auto op = mystack.back();
                poliz_exp.push_back(op);
                CG.Emit(op.first);
                mystack.pop_back();
            }
            mystack.pop_back();
        }
    }
    while(!mystack.empty()){
        auto op = mystack.back();
        poliz_exp.push_back(op);
        CG.Emit(op.first);
        mystack.pop_back();
    }
    for(int i=0;i<poliz_exp.size(); ++i){
        std::cout << poliz_exp[i].first << " ";
    }
    std::cout << std::endl;
    std::cout << "END TO POL\n";
}

std::string Syntax::CalculatePolize(){
    std::vector< std::pair<std::string, std::string> > mystack;
    for(int i=0;i<poliz_exp.size();++i){
        if(poliz_exp[i].first == "undefined"){
            error_print(number_st[gidx]);
            std::cout << "SemanticError{undefined type of varialbe}{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << expression[i].first << std::endl;
            exit(1);
        }
        else if(poliz_exp[i].first == "str" ||
           poliz_exp[i].first == "int" ||
           poliz_exp[i].first == "boolean" ||
           poliz_exp[i].first == "double"){
            mystack.push_back(poliz_exp[i]);
        }
        else if(sfind(binary_operator, poliz_exp[i].second)){
            std::pair<std::string, std::string> a,b;
            b = mystack.back();
            mystack.pop_back();
            a = mystack.back();
            mystack.pop_back();
            if(poliz_exp[i].first == "."){
                if(a.first == "int" && b.first == "int"){
                    mystack.push_back({"double","double"});
                }
                else{
                    error_print(number_st[gidx]);
                    std::cout << "SemanticError{this operation is not allowed with types}{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << a.first << " " << b.first << std::endl;
                    exit(1);
                }
            }
            else if(poliz_exp[i].first == "*" ||
                    poliz_exp[i].first == "/"){
                if(a.first == "int" && b.first == "int")
                    {
                        mystack.push_back({"int", "int"});
                    }
                else if(a.first == "int" && b.first == "double" ){
                        mystack.push_back({"double", "double"});
                    }
                else if(a.first == "double" && b.first == "int"){
                        mystack.push_back({"double", "double"});
                    }
                else if(a.first == "double" && b.first == "double"){
                        mystack.push_back({"double", "double"});
                    }
                else{
                    error_print(number_st[gidx]);
                    std::cout << "SemanticError{this operation is not allowed with types}{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << a.first << " " << b.first << std::endl;
                    exit(1);
                }
            }
            else if(poliz_exp[i].first == "%"){
                if(a.first == "int" && b.first == "int")
                {
                    mystack.push_back({"int", "int"});
                }
                else{
                    error_print(number_st[gidx]);
                    std::cout << "SemanticError{this operation is not allowed with types}{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << a.first << " " << b.first << std::endl;
                    exit(1);
                }
            }
            else if(poliz_exp[i].first == "+"){
                if(a.first == "int" && b.first == "int")
                    {
                        mystack.push_back({"int", "int"});
                    }
                else if(a.first == "int" && b.first == "double" ){
                        mystack.push_back({"double", "double"});
                    }
                else if(a.first == "double" && b.first == "int"){
                        mystack.push_back({"double", "double"});
                    }
                else if(a.first == "double" && b.first == "double"){
                        mystack.push_back({"double", "double"});
                    }
                else if(a.first == "str" && b.first == "str"){
                    mystack.push_back({"str", "str"});
                }
                else{
                    error_print(number_st[gidx]);
                    std::cout << "SemanticError{this operation is not allowed with types}{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << a.first << " " << b.first << std::endl;
                    exit(1);
                }
            }
            else if(poliz_exp[i].first == "-"){
                if(a.first == "int" && b.first == "int")
                    {
                        mystack.push_back({"int", "int"});
                    }
                else if(a.first == "int" && b.first == "double" ){
                        mystack.push_back({"double", "double"});
                    }
                else if(a.first == "double" && b.first == "int"){
                        mystack.push_back({"double", "double"});
                    }
                else if(a.first == "double" && b.first == "double"){
                        mystack.push_back({"double", "double"});
                    }
                else{
                    error_print(number_st[gidx]);
                    std::cout << "SemanticError{this operation is not allowed with types}{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << a.first << " " << b.first << std::endl;
                    exit(1);
                }
            }
            else if(poliz_exp[i].first == ">" ||
                    poliz_exp[i].first == "<" ||
                    poliz_exp[i].first == "==" ||
                    poliz_exp[i].first == "!="){
                if(a.first == "int" && b.first == "int")
                    {
                        mystack.push_back({"boolean", "boolean"});
                    }
                else if(a.first == "int" && b.first == "double" ){
                        mystack.push_back({"boolean", "boolean"});
                    }
                else if(a.first == "double" && b.first == "int"){
                        mystack.push_back({"boolean", "boolean"});
                    }
                else if(a.first == "double" && b.first == "double"){
                        mystack.push_back({"boolean", "boolean"});
                    }
                else if(a.first == "str" && b.first == "str"){
                        mystack.push_back({"boolean", "boolean"});
                }
                else if(poliz_exp[i].first == "==" ||
                    poliz_exp[i].first == "!="){
                    if(a.first == "boolean" && b.first == "boolean"){
                        mystack.push_back({"boolean", "boolean"});
                    }
                    else{
                        error_print(number_st[gidx]);
                        std::cout << "SemanticError{this operation is not allowed with types}{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << a.first << " " << b.first << std::endl;
                        exit(1);
                    }
                }
                else{
                    error_print(number_st[gidx]);
                    std::cout << "SemanticError{this operation is not allowed with types}{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << a.first << " " << b.first << std::endl;
                    exit(1);
                }
            }
            else if(poliz_exp[i].first == "&&" ||
                    poliz_exp[i].first == "||"){
                if(a.first == "boolean" && b.first == "boolean"){
                    mystack.push_back({"boolean", "boolean"});
                }
                else{
                    error_print(number_st[gidx]);
                    std::cout << "SemanticError{this operation is not allowed with types}{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << a.first << " " << b.first << std::endl;
                    exit(1);
                }
            }
            else if(poliz_exp[i].first == "="){
                mystack.push_back({b.first, b.first});
            }
            else{
                error_print(number_st[gidx]);
                std::cout << "SemanticError{this operation is not allowed with types}{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << a.first << " " << b.first << std::endl;
                exit(1);
            }
        }
    }
    std::cout << "END POLIZ\n";
    return mystack.back().first;
}

std::string Syntax::out(std::string word, std::string color){
    word = color + word + OUT_END;
    return word;
}

long long vlanum(long long a){
    a = abs(a);
    long long c = 0;
    while(a>0){
        c++;
        a/=10;
    }
    return c;
}

void Syntax::SetNumberST(std::vector<std::pair<int,int>> v){ number_st = v; }

void Syntax::error_print(std::pair<int,int> T){
    std::cout << std::endl;
    std::cout << T.first << " | " << out(superlines[T.first-1], OUT_BOLD);
    for(int i=0;i<=vlanum(T.first);++i) std::cout << " ";
    std::cout << " ";
    for(int i=0;i<T.second-1;++i) std::cout << out("~", OUT_PURP);
    std::cout << out("^",OUT_RED);
    for(int i=T.second+1;i<superlines[T.first-1].size();++i) std::cout << out("~", OUT_PURP);
    std::cout << std::endl;
}

void Syntax::set_bln(std::string elem){
    if(sfind(closeb, elem)){
        if(elem == "service}")
            blnc["service{"]--;
        else if(elem == "service)")
            blnc["service("]--;
        else if(elem == "service]")
            blnc["service["]--;
        return;
    }
    else if(sfind(openb, elem)){
        blnc[elem]++;
        return;
    }
}

bool Syntax::check_blnc(){
    for(auto it : blnc){
        if(it.second < 0){
            return false;
        }
    }
    return true;
}

bool Syntax::sclone(std::string name){
    for(int i=0;i<TID.size(); ++i){
        for(int j=0;j<TID[i].size(); ++j){
            if(TID[i][j].name == name){
                return false;
            }
        }
    }
    return true;
}

Syntax::VRBL* Syntax::findclone(std::string name){
    for(int i=0;i<TID.size(); ++i){
        for(int j=0;j<TID[i].size(); ++j){
            if(TID[i][j].name == name){
                return &TID[i][j];
            }
        }
    }
    return nullptr;
}


bool Syntax::sfind(std::set<std::string>& mset, std::string elem){
    if(mset.find(elem) != mset.end()){
        return true;
    }
    return false;
}

bool Syntax::eq(std::string elem){
    return (elem == distrib[gidx].second);
}

void Syntax::BEGIN() {
    std::cout << "BEGIN()\n";
    //if(gidx == distrib.size()-1) return;
    std::cout << distrib[gidx].second << std::endl;
    if(distrib[gidx].second == "operator.using"){
        gidx++;
        USING();
    }
    else if(distrib[gidx].second == "structure.function"){
        gidx++;
        FUNCTION();
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
    }
}

void Syntax::ASSIGNMENT(){
    std::string id_d = "";
    if(eq("id") && distrib[gidx+1].second == "operator="){
        id_d = distrib[gidx].first;
        gidx+=2;
    }
    else{
        gidx++;
        IDENT();
        return;
    }
    expression.clear();
    EXP();
    out_exp();
    ExpToPolize();
    findclone(id_d)->type = CalculatePolize();
    CG.Emit("STORE",id_d);
    expression.clear();
    if(eq("operator;")){
            gidx++;
            OPERATOR_END();
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
        exit(1);
    }
}

void Syntax::FUNCTION() {
    std::cout << "FUNCTION()\n";
    std::string fun_id = "";
    if(distrib[gidx].second == "id"){
        if(!sclone(distrib[gidx].first)){
            error_print(number_st[gidx]);
            std::cout << "SemanticError{double definition:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        fun_id = distrib[gidx].first;
        TID.back().push_back({distrib[gidx].first, "undefined"});
        TID.back().back().object_type = "function";
        gidx++;
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{FUN_ARG}{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
        return;
    }
    if(eq("service(")){
        TID.push_back(std::vector<VRBL>());
        gidx++;
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{FUN_ARG}{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
        return;
    }
    SIGN_FOR_EXIT = false;
    LAST_DEF_IS_CONST = false;
    while(!eq("service)")){
        if(eq("description")){
            gidx++;
        }
        else{
            error_print(number_st[gidx]);
            std::cout << "SyntaxError{FUN_ARG}{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        ARG_DEF();
        if(SIGN_FOR_EXIT){
            error_print(number_st[gidx]);
            std::cout << "SyntaxError{wrong order of arguments}{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        if(eq("operator;") && distrib[gidx-1].second != "operator="){
            gidx++;
        }
        else{
            return;
        }
        std::cout << "READ ARG\n";
    }
    if(!eq("service)")){
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{close round brackent not find}{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
        return;
    }
    else{
        gidx++;
    }
    if(eq("operator->")){
        gidx++;
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{not exist operator-> for type of return value}{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
        return;
    }
    if(sfind(base_type, distrib[gidx].second)){
        findclone(fun_id)->type = distrib[gidx].second;
        gidx++;
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{not exist type of return value}{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
        return;
    }
    if(eq("service{")){
        set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
            std::cout << "SyntaxError{out of balance:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
                return;
        }
        gidx++;
        if(eq("operator.return")){
            gidx++;
            RETURN();
        }
        else{
            OPEN_FIGURE();
        }
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{OPEN FIGURE BRACKET}{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
    }
}

void Syntax::ARG_DEF(){
    std::string id_d = "";
    if(eq("id")){
        if(distrib[gidx].second == "id"){
            if(!sclone(distrib[gidx].first)){
                error_print(number_st[gidx]);
                std::cout << "SemanticError{double definition:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
                return;
            }
            TID.back().push_back(VRBL(distrib[gidx].first, "undefined"));
        }
        id_d = distrib[gidx].first;
        gidx++;
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
        return;
    }
    if(eq("operator;")){
        if(LAST_DEF_IS_CONST){
            SIGN_FOR_EXIT = true;
        }
        return;
    }
    if(eq("operator=")){
        gidx++;
        LAST_DEF_IS_CONST = true;
        if(eq("operator;")){
            error_print(number_st[gidx]);
            std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        expression.clear();
        EXP();
        out_exp();
        ExpToPolize();
        findclone(id_d)->type = CalculatePolize();
        expression.clear();
        if(eq("operator;")){
            return;
        }
        else{
            error_print(number_st[gidx]);
            std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
    }

}

void Syntax::USING() {
    //if(gidx == distrib.size()-1) return;
    std::cout << "USING()\n";
    std::cout << distrib[gidx].second << std::endl; 
    if(distrib[gidx].second == "lib"){
        gidx++;
        LIB();
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
    }
}

void Syntax::LIB(){
    std::cout << "LIB()\n";
    //if(gidx == distrib.size()-1) return;
    if(distrib[gidx].second == "operator;"){
        gidx++;
        OPERATOR_END();
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
    }
}

void Syntax::OPERATOR_END(){
    std::cout << "OPERATOR_END()\n";
    if(distrib[gidx].second == "id"){
        if(sclone(distrib[gidx].first)){
            error_print(number_st[gidx]);
            std::cout << "SemanticError{undefined variable:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        //gidx++;
        ASSIGNMENT();
    }
    else if(distrib[gidx].second == "operator.using"){
        gidx++;
        USING();
    }
    else if(distrib[gidx].second == "condition.begin"){
        gidx++;
        IF();
    }
    else if(eq("operator-if")){
        if(!if_exist(TID.back())){
            error_print(number_st[gidx]);
            std::cout << "SyntaxError{this is not endif place}:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        gidx++;
        ENDIF();
    }
    else if(distrib[gidx].second == "description"){
        if(distrib[gidx+1].second == "id"){
            if(!sclone(distrib[gidx+1].first)){
                error_print(number_st[gidx+1]);
                std::cout << "SemanticError{double definition:" << number_st[gidx+1].first << ":" << number_st[gidx+1].second << "}: " << distrib[gidx+1].first << " " << distrib[gidx+1].second << std::endl;
                return;
            }
            TID.back().push_back(VRBL(distrib[gidx+1].first, "undefined"));
        }
        gidx++;
        DEF();
    }
    else if(distrib[gidx].second == "contdition.end"){
        gidx++;
        CONDITION_ELSE();
    }
    else if(distrib[gidx].second == "cycle.for"){
        gidx++;
        //CYCLE();
        FOR();
    }
    else if(distrib[gidx].second == "cycle.while"){
        gidx++;
        //CYCLE();
        WHILE();
    }
    else if(distrib[gidx].second == "operator.print"){
        gidx++;
        PRINT();
    }
    else if(eq("operator.scan")){
        gidx++;
        SCAN();
    }
    else if(eq("operator.return")){
        gidx++;
        RETURN();
    }
    else if(distrib[gidx].second == "service{"){
        set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
            std::cout << "SyntaxError{out of balance:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
                return;
        }
        TID.push_back(std::vector<VRBL>());
        gidx++;
        OPEN_FIGURE();
    }
    else if(distrib[gidx].second == "service}"){
        set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
            std::cout << "SyntaxError{out of balance:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
                return;
        }
        TID.pop_back();
        gidx++;
        CLOSE_FIGURE();
    }
    else if(distrib[gidx].second == "structure.function"){
        gidx++;
        FUNCTION();
    }
    else if(distrib[gidx].second == "operator.END"){
        std::cout << out("ALL OK\n",OUT_GREEN);
        return;
    }
    else if(sfind(unary, distrib[gidx].second)){
        gidx++;
        UNARY();   
    }
    else{
        error_print(number_st[gidx]);
        std::cout << out("SyntaxError", OUT_RED) << "{after " << out("operator;",OUT_GREEN) << " can not stand " << out(distrib[gidx].second, OUT_GREEN) << ":" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
    }
}

void Syntax::IDENT(){
    std::cout << "IDENT()\n";
    if(binary_operator.find(distrib[gidx].second) != binary_operator.end()){
        gidx++;
        BINARY_OPERATOR();
    }    
    else if(distrib[gidx].second == "operator;"){
        gidx++;
        OPERATOR_END();
    }
    else if(distrib[gidx].second == "service("){
        set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
        std::cout << "SyntaxError{out of balance:" << number_st[gidx].first <<":"<<number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        gidx++;
        OPEN_ROUND();
    }
    else if(distrib[gidx].second == "service)"){
        set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
        std::cout << "SyntaxError{out of balance:" << number_st[gidx].first <<":"<<number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        gidx++;
        CLOSE_ROUND();
    }
    else if(distrib[gidx].second == "service["){
        set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
        std::cout << "SyntaxError{out of balance:" << number_st[gidx].first <<":"<<number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        gidx++;
        OPEN_SQUARE();
    }
    else if(distrib[gidx].second == "service]"){
        set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
        std::cout << "SyntaxError{out of balance:" << number_st[gidx].first <<":"<<number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        gidx++;
        CLOSE_SQUARE();
    }
    else if(sfind(unary, distrib[gidx].second) && !eq("operator!")){
        gidx++;
        UNARY();   
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
    }
}

void Syntax::BINARY_OPERATOR() {
    std::cout << "BINARY_OPERATOR()\n";
    if(distrib[gidx].second == "const"){
        gidx++;
        CONST_RVALUE();
    }
    else if(distrib[gidx].second == "id"){
        if(sclone(distrib[gidx].first)){
            error_print(number_st[gidx]);
            std::cout << "SemanticError{undefined variable:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        gidx++;
        IDENT();
    }
    else if(distrib[gidx].second == "service("){
                set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
        std::cout << "SyntaxError{out of balance:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
                gidx++;
                OPEN_ROUND();
    }
    else if(distrib[gidx].second == "service["){
                set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
        std::cout << "SyntaxError{out of balance:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
                gidx++;
                OPEN_SQUARE();
    }
    else if(eq("string_literal")){
        gidx++;
        str_balance++;
        STRING_LIT();
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
    }
}


void Syntax::CYCLE(){
    std::cout << "CYCLE()\n";
    if(distrib[gidx].second == "service("){
        set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
        std::cout << "SyntaxError{out of balance:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        gidx++;
        OPEN_ROUND();
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
    }
}

void Syntax::WHILE(){
    std::cout << "WHILE()\n";
    int while_begin = CG.GetCounter();
    if(eq("service(")){
        set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
        std::cout << "SyntaxError{out of balance:" << number_st[gidx].first <<":"<<number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        gidx++;
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError1{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
        return;
    }
    expression.clear();
    EXP();
    if(eq("operator;")){
        gidx++;
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError3{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
        return;
    }
    out_exp();
    ExpToPolize();
    std::string mt_type = CalculatePolize();
    std::cout << mt_type << std::endl;
    if(mt_type != "boolean"){
        error_print(number_st[gidx]);
        std::cout << "SemanticError{EXP IN WHILE MUST HAVE BOOLEAN TYPE}{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
        exit(1);
    }
    int while_branch = CG.GetCounter();
    CG.Emit("JZ",0);
    OpStack.push_back({ CMD_WHILE, while_begin, while_branch });
    expression.clear();
    if(eq("service)")){
        gidx++;
        CLOSE_ROUND();
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError6{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
        return;
    }
}

void Syntax::FOR(){
    if(eq("service(")){
        set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
        std::cout << "SyntaxError{out of balance:" << number_st[gidx].first <<":"<<number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        gidx++;
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError1{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
        return;
    }
    if(eq("description")){
        gidx++;
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError2{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
        return;
    }
    FOR_DEF();

    int after_init_pos = CG.GetCounter();
    CG.Emit("JMP",0);
     
    if(eq("operator;")){
        gidx++;
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError3{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
        return;
    }
    expression.clear();
    EXP();
    out_exp();
    ExpToPolize();
    std::cout << CalculatePolize() << std::endl;
    expression.clear();

    int after_comp_pos = CG.GetCounter();
    CG.Emit("JZ", 0);
    CG.Emit("JMP", 0);

    if(eq("operator;")){
        gidx++;
    }
    else{
        return;
    }
    std::string id_d = "";
    if(eq("id") && distrib[gidx+1].second == "operator="){
        id_d = distrib[gidx].first;
        gidx+=2;
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError3{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
        return;
    }
    expression.clear();
    EXP();
    out_exp();
    ExpToPolize();
    std::cout << CalculatePolize() << std::endl;
    CG.Emit("STORE",id_d);
    expression.clear();

    CG.Emit("JMP", after_init_pos + 1);
    CG.FixJump(after_init_pos, CG.GetCounter());
    CG.FixJump(after_comp_pos + 1, CG.GetCounter());
 

    OpStack.push_back({ CMD_FOR, after_comp_pos, 0 });
    if(eq("operator;")){
        gidx++;
    }
    else{
        return;
    }
    if(eq("service)")){
        gidx++;
        CLOSE_ROUND();
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError6{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
        return;
    }
    //SEMANTIC CHECK();
}

void Syntax::FOR_DEF(){
    std::string id_d = "";
    if(eq("id")){
        id_d = distrib[gidx].first;
        gidx++;
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
        return;
    }
    if(eq("operator=")){
        gidx++;
        expression.clear();
        EXP();
        out_exp();
        ExpToPolize();
        findclone(id_d)->type = CalculatePolize();
        CG.Emit("STORE", id_d);
        expression.clear();
        if(eq("operator;")){
            return;
        }
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
    }
}

void Syntax::STRING_LIT() {

    if(binary_operator.find(distrib[gidx].second) != binary_operator.end()){
        gidx++;
        BINARY_OPERATOR();
    }
    else if(distrib[gidx].second == "operator;"){
        gidx++;
        OPERATOR_END();
    }
    else if(eq("service]")){
        set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
        std::cout << "SyntaxError{out of balance:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        gidx++;
        CLOSE_SQUARE();
    }
    else if(eq("service)")){
        set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
        std::cout << "SyntaxError{out of balance:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        gidx++;
        CLOSE_ROUND();
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
    }

}

void Syntax::UNARY() {
    std::cout << "UNARY()\n";
    if(distrib[gidx].second == "id"){
        if(sclone(distrib[gidx].first)){
            error_print(number_st[gidx]);
            std::cout << "SemanticError{undefined variable:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        gidx++;
        IDENT();
    }
    else if(sfind(binary_operator, distrib[gidx].second)){
        gidx++;
        BINARY_OPERATOR();
    }
    else if(eq("operator;")){
        gidx++;
        OPERATOR_END();
    }
    else if(eq("service]")){
        set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
        std::cout << "SyntaxError{out of balance:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        gidx++;
        CLOSE_SQUARE();
    }
    else if(eq("service)")){
        set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
        std::cout << "SyntaxError{out of balance:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        gidx++;
        CLOSE_ROUND();
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{id expected:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
    }
}

void Syntax::CONDITION_BEGIN() {
    std::cout << "CONDITION_BEGIN()\n";
    if(distrib[gidx].second == "service("){
        set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
        std::cout << "SyntaxError{out of balance:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        gidx++;
        OPEN_ROUND();
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
    }
}

bool Syntax::if_exist(std::vector<VRBL> v){
    for(auto it : v){
        if(it.type == "if_exist"){
            return true;
        }
    }
    return false;
}

void Syntax::IF(){
    if(eq("service(")){
        set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
            std::cout << "SyntaxError{out of balance:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        gidx++;
        if(eq("service)")){
            error_print(number_st[gidx]);
            std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
        return;
    }
    expression.clear();
    EXP();
    out_exp();
    ExpToPolize();
    std::string type_d = CalculatePolize();
    if(type_d != "boolean"){
        error_print(number_st[gidx]);
        std::cout << "SemanticError{EXP IN IF MUST HAVE BOOLEAN TYPE}{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
        exit(1);
    }
    OpStack.push_back({ CMD_IF, CG.GetCounter(), 0 });
    CG.Emit("JZ", 0);
    expression.clear();
    gidx++;
    if(eq("service)")){
        set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
            std::cout << "SyntaxError{out of balance:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        gidx++;
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
        return;
    }
    if(eq("service{")){
        set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
            std::cout << "SyntaxError{out of balance:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        TID.push_back(std::vector<VRBL>());
        VRBL nw;
        nw.type = "if_exist";
        TID.back().push_back(nw);
        gidx++;
        OPEN_FIGURE();
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
        return;
    }
}

void Syntax::ENDIF(){
    OpStackElem tmp;
    if(eq("service}")){
        set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
            std::cout << "SyntaxError{out of balance:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        gidx++;
        if (!OpStack.empty()) tmp = OpStack.back();
        FixBranch();
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{endif}{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
        return;
    }
    if(eq("operator;")){
        gidx++;
        OPERATOR_END();
        return;
    }
    if(eq("condition.end")){
        gidx++;
        if(eq("service{")){
            set_bln(distrib[gidx].second);
            if(!check_blnc()){
                error_print(number_st[gidx]);
                std::cout << "SyntaxError{out of balance:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
                return;
            }
            gidx++;
            OpStack.push_back(tmp);
            if (!OpStack.empty()) {
                int x = OpStack.back().address_1;
                OpStack.back().address_1 = CG.GetCounter();
                CG.Emit("JMP", 0);
                CG.FixJump(x,CG.GetCounter());
            }
            OPEN_FIGURE();
        }
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{endif}{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
        return;
    }
}

void Syntax::CONDITION_ELSE(){
    std::cout << "COND_ELSE()\n";
    if(distrib[gidx].second == "service{"){
        set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
            std::cout << "SyntaxError{out of balance:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        TID.push_back(std::vector<VRBL>());
        gidx++;
        OPEN_FIGURE();
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
    }
}

void Syntax::CONST_RVALUE(){
    std::cout << "CONST_RVALUE()\n";
    if(distrib[gidx].second == "operator."){
        gidx++;
        FLOAT_PART();
    }
    else if(binary_operator.find(distrib[gidx].second) != binary_operator.end()){
        gidx++;
        BINARY_OPERATOR();
    }
    else if(distrib[gidx].second == "operator;"){
        gidx++;
        OPERATOR_END();
    }
    else if(eq("service]")){
        set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
        std::cout << "SyntaxError{out of balance:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        gidx++;
        CLOSE_SQUARE();
    }
    else if(eq("service)")){
        set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
        std::cout << "SyntaxError{out of balance:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        gidx++;
        CLOSE_ROUND();
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
    }
}

void Syntax::FLOAT_PART(){
    std::cout << "FLOAT_PART()\n";
    if(distrib[gidx].second == "const"){
        gidx++;
        CONST_FLOAT_PART();
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
    }
}

void Syntax::CONST_FLOAT_PART(){
    std::cout << "CONST_FLOAT_PART()\n";
    if(distrib[gidx].second != "operator." && binary_operator.find(distrib[gidx].second) != binary_operator.end()){
        gidx++;
        BINARY_OPERATOR();
    }
    else if(distrib[gidx].second == "operator;"){
        gidx++;
        OPERATOR_END();
    }
    else if(eq("service]")){
        set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
        std::cout << "SyntaxError{out of balance:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        gidx++;
        CLOSE_SQUARE();
    }
    else if(eq("service)")){
        set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
        std::cout << "SyntaxError{out of balance:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        gidx++;
        CLOSE_ROUND();
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
    }
}

void Syntax::RETURN() {
    if(eq("const")){
        gidx++;
        CONST_RVALUE();
    }
    else if(eq("id")){
        if(sclone(distrib[gidx].first)){
            error_print(number_st[gidx]);
            std::cout << "SemanticError{undefined variable:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        gidx++;
        IDENT();
    }
    else if(eq("operator;")){
        gidx++;
        OPERATOR_END();
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
    }
}

void Syntax::VAR(){
    if(eq("id")){
        if(sclone(distrib[gidx].first)){
            error_print(number_st[gidx]);
            std::cout << "SemanticError{undefined variable:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        std::cout << "VAR " << distrib[gidx].first << '\n';
        gidx++;
        IDENT();
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
    }
}

void Syntax::OPEN_FIGURE(){
    if(eq("condition.begin")){
        gidx++;
        IF();
    }
    else if(eq("cycle.for")){
        gidx++;
        //CYCLE();
        FOR();
    }
    else if(eq("operator.print")){
        gidx++;
        PRINT();
    }
    else if(eq("operator.scan")){
        gidx++;
        SCAN();
    }
    else if(eq("cycle.while")){
        gidx++;
        //CYCLE();
        WHILE();
    }
    else if(eq("description")){
        if(distrib[gidx+1].second == "id"){
            if(!sclone(distrib[gidx+1].first)){
                error_print(number_st[gidx+1]);
                std::cout << "SemanticError{double definition:" << number_st[gidx+1].first << ":" << number_st[gidx+1].second << "}: " << distrib[gidx+1].first << " " << distrib[gidx+1].second << std::endl;
                return;
            }
            TID.back().push_back({distrib[gidx+1].first, "undefined"});
        }
        gidx++;
        //VAR();
        DEF();
    }
    else if(eq("id")){
        if(sclone(distrib[gidx].first)){
            error_print(number_st[gidx]);
            std::cout << "SemanticError{undefined variable:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        ASSIGNMENT();
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
    }
}

void Syntax::PRINT()
{
    EXP();
    out_exp();
    ExpToPolize();
    CalculatePolize();
    CG.Emit("PRINT");
    expression.clear();
    if(eq("operator;")){
            gidx++;
            OPERATOR_END();
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
        exit(1);
    }
}

void Syntax::SCAN(){
    std::string id_d = "";
    if(eq("id")){
        id_d = distrib[gidx].first;
        gidx+=1;
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
        exit(1);
    }

    CG.Emit("SCAN", id_d);

    if(eq("operator;")){
        gidx++;
        OPERATOR_END();
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
        exit(1);
    }

}

void Syntax::FixBranch()
{
    if (OpStack.empty()) return; // TODO: Generate errors here?
    auto z = OpStack.back();
    OpStack.pop_back();
    switch (z.command) {
        case CMD_WHILE:
            CG.Emit("JMP",z.address_1);
            CG.FixJump(z.address_2,CG.GetCounter());
            break;
        case CMD_IF:
            CG.FixJump(z.address_1,CG.GetCounter());
            break;
        case CMD_FOR:
            CG.Emit("JMP",z.address_1+2);
            CG.FixJump(z.address_1,CG.GetCounter()); 
            break;
    }
}

void Syntax::CLOSE_FIGURE(){
    FixBranch();
    if(eq("operator;")){
        gidx++;
        OPERATOR_END();
    }
    else if(eq("structure.function")){
        gidx++;
        FUNCTION();
    }
    else if(eq("cycle.for")){
        gidx++;
        //CYCLE();
        FOR();
    }
    else if(eq("operator.print")) {
        gidx++;
        PRINT();
    }
    else if(eq("operator.scan")){
        gidx++;
        SCAN();
    }
    else if(eq("cycle.while")){
        gidx++;
        //CYCLE();
        WHILE();
    }
    else if(eq("condition.begin")){
        gidx++;
        IF();
    }
    else if(eq("service}")){
        set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
            std::cout << "SyntaxError{out of balance:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        TID.pop_back();
        gidx++;
        CLOSE_FIGURE();
    }
    else if(eq("operator.return")){
        gidx++;
        RETURN();
    }
    else if(eq("id")){
        if(sclone(distrib[gidx].first)){
            error_print(number_st[gidx]);
            std::cout << "SemanticError{undefined variable:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        gidx++;
        IDENT();
    }
    else if(eq("description")){
        if(distrib[gidx+1].second == "id"){
            if(!sclone(distrib[gidx+1].first)){
                error_print(number_st[gidx+1]);
                std::cout << "SemanticError{double definition:" << number_st[gidx+1].first << ":" << number_st[gidx+1].second << "}: " << distrib[gidx+1].first << " " << distrib[gidx+1].second << std::endl;
                return;
            }
            TID.back().push_back({distrib[gidx+1].first, "undefined"});
        }
        gidx++;
        //VAR();
        DEF();
    }
    else if(sfind(unary, distrib[gidx].second)){
        gidx++;
        UNARY();
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
    }
}

void Syntax::OPEN_ROUND(){
    if(eq("id")){
        if(sclone(distrib[gidx].first)){
            error_print(number_st[gidx]);
            std::cout << "SemanticError{undefined variable:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        gidx++;
        IDENT();
    }
    else if(eq("description")){
        if(distrib[gidx+1].second == "id"){
            if(!sclone(distrib[gidx+1].first)){
                error_print(number_st[gidx+1]);
                std::cout << "SemanticError{double definition:" << number_st[gidx+1].first << ":" << number_st[gidx+1].second << "}: " << distrib[gidx+1].first << " " << distrib[gidx+1].second << std::endl;
                return;
            }
            TID.back().push_back({distrib[gidx+1].first, "undefined"});
        }
        gidx++;
        //VAR();
        DEF();
    }
    else if(eq("const")){
        gidx++;
        OPEN_ROUND();
    }
    else if(sfind(unary, distrib[gidx].second)){
        gidx++;
        UNARY();
    }
    else if(eq("service(")){
        set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
        std::cout << "SyntaxError{out of balance:" << number_st[gidx].first <<":"<<number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        gidx++;
        OPEN_ROUND();
    }
    else if(eq("service)")){
        set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
        std::cout << "SyntaxError{out of balance:" << number_st[gidx].first <<":"<<number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        gidx++;
        CLOSE_ROUND();
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
    }
}

void Syntax::CLOSE_ROUND(){
    if(sfind(binary_operator, distrib[gidx].second)){
        gidx++;
        BINARY_OPERATOR();
    }
    else if(eq("operator;")){
        gidx++;
        OPERATOR_END();
    }
    else if(eq("service{")){
        set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
        std::cout << "SyntaxError{out of balance:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        TID.push_back(std::vector<VRBL>());
        gidx++;
        OPEN_FIGURE();
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
    }
}

void Syntax::OPEN_SQUARE(){
    if(eq("id")){
        if(sclone(distrib[gidx].first)){
            error_print(number_st[gidx]);
            std::cout << "SemanticError{undefined variable:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        gidx++;
        IDENT();
    }
    else if(eq("string_literal")){
        gidx++;
        STRING_LIT();
    }
    else if(eq("const")){
        gidx++;
        CONST_RVALUE();
    }
    else if(sfind(unary, distrib[gidx].second)){
        gidx++;
        UNARY();
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
    }
}

void Syntax::CLOSE_SQUARE(){
    if(eq("operator;")){
        gidx++;
        OPERATOR_END();
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
    }
}

void Syntax::DEF(){
    std::string id_d = "";
    if(eq("id")){
        id_d = distrib[gidx].first;
        gidx++;
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
        return;
    }
    if(eq("operator;")){
        gidx++;
        OPERATOR_END();
        return;
    }
    if(eq("operator=")){
        gidx++;
        if(eq("operator;")){
            error_print(number_st[gidx]);
            std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
        expression.clear();
        EXP();
        ExpToPolize();
        CG.Emit("STORE",id_d);
        std::string type_d = CalculatePolize();
        if(findclone(id_d) == nullptr){
            std::cout << "ID_D: " << id_d << " " << "NULL\n";
        }
        findclone(id_d)->type = type_d;
        out_exp();
        expression.clear();
        if(eq("operator;")){
            gidx++;
            OPERATOR_END();
        }
        else{
            error_print(number_st[gidx]);
            std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            return;
        }
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
    }
}

// TODO ПОДДЕРЖКА МАСИВОВ
// TODO SEMANTIC IN EXP
// TODO ADD FUNCTIONS IN EXP
// UNARY OPERATORS???
void Syntax::EXP(){      
    function_balance_++;
    if(function_balance_ == 1)
        expression.push_back(distrib[gidx]);
    if(eq("id")){
        if(findclone(distrib[gidx].first) == nullptr){
                error_print(number_st[gidx]);
                std::cout << "SemanticError{undefined variable:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
                exit(1);
        }
        gidx++;
        EXP_ID();
    }
    else if(eq("service(")){
        set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
            std::cout << "SyntaxError{out of balance:" << number_st[gidx].first <<":"<<number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            exit(1);
            return;
        }
        gidx++;
        EXP_BKT_OPEN();
    }
    else if(eq("const") || eq("string_literal")){
        gidx++;
        EXP_CONST();    
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{EXP}{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
        exit(1);
        return;
    }
    function_balance_--;
    // else{
    //     error_print(number_st[gidx]);
    //     std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
    //     return;
    // }
}

void Syntax::EXP_ID(){
    if(function_balance_ == 1)
        expression.push_back(distrib[gidx]);
    if(sfind(binary_operator, distrib[gidx].second)){
        gidx++;
        EXP_BIN();
    }
    else if(eq("service(")){
        if(findclone(distrib[gidx-1].first) == nullptr ||
           findclone(distrib[gidx-1].first)->object_type != "function"){
            error_print(number_st[gidx]);
            std::cout << "SyntaxError{it is not a function}:" << number_st[gidx].first <<":"<<number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            exit(1);
            return;    
        }
        gidx++;
        while(!eq("service)")){
            EXP();
            if(eq("operator;")){
                gidx++;
            }
            else{
                error_print(number_st[gidx]);
                std::cout << "SyntaxError{}:" << number_st[gidx].first <<":"<<number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
                exit(1);
                return;
            }
        }
        if(eq("service)")){
            expression.push_back(distrib[gidx]);
            gidx++;
        }
        else{
            error_print(number_st[gidx]);
            std::cout << "SyntaxError{}:" << number_st[gidx].first <<":"<<number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            exit(1);
            return;
        }
        EXP_BKT_CLOSE();
    }
    else if(eq("operator;")){
        return;
    }
    else if(eq("service)")){
        set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
            std::cout << "SyntaxError{out of balance:" << number_st[gidx].first <<":"<<number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            exit(1);
            return;
        }
        gidx++;
        EXP_BKT_CLOSE();
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
        exit(1);
        return;
    }
}

void Syntax::EXP_CONST(){
    if(function_balance_ == 1)
        expression.push_back(distrib[gidx]);
    if(sfind(binary_operator, distrib[gidx].second)){
        gidx++;
        EXP_BIN();
    }
    else if(eq("service)")){
        set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
            std::cout << "SyntaxError{out of balance:" << number_st[gidx].first <<":"<<number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            exit(1);
            return;
        }
        gidx++;
        EXP_BKT_CLOSE();
    }
    else if(eq("operator;")){
        return;
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError33{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
        exit(1);
        return;
    }
}

void Syntax::EXP_BKT_OPEN(){
    if(function_balance_ == 1)
        expression.push_back(distrib[gidx]);
    if(eq("const") || eq("string_literal")){
        gidx++;
        EXP_CONST();
    }
    else if(eq("id")){
        if(findclone(distrib[gidx].first) == nullptr){
                error_print(number_st[gidx]);
                std::cout << "SemanticError{undefined variable:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
                exit(1);
        }
        gidx++;
        EXP_ID();
    }
    else if(eq("service(")){
        set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
            std::cout << "SyntaxError{out of balance:" << number_st[gidx].first <<":"<<number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            exit(1);
            return;
        }
        gidx++;
        EXP_BKT_OPEN();
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
        exit(1);
        return;
    }
}

void Syntax::EXP_BKT_CLOSE(){
    if(function_balance_ == 1)
        expression.push_back(distrib[gidx]);
    if(sfind(binary_operator, distrib[gidx].second)){
        gidx++;
        EXP_BIN();
    }
    else if(eq("operator;")){
        return;
    }
    else if(eq("service)")){
        set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
            std::cout << "SyntaxError{out of balance:" << number_st[gidx].first <<":"<<number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            exit(1);
            return;
        }
        gidx++;
        EXP_BKT_CLOSE();
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
        exit(1);
        return;
    }
}

void Syntax::EXP_BIN(){
    if(function_balance_ == 1)
        expression.push_back(distrib[gidx]);
    if(eq("const") || eq("string_literal")){
        gidx++;
        EXP_CONST();
    }
    else if(eq("id")){
        if(findclone(distrib[gidx].first) == nullptr){
                error_print(number_st[gidx]);
                std::cout << "SemanticError{undefined variable:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
                exit(1);
        }
        gidx++;
        EXP_ID();
    }
    else if(eq("service(")){
        set_bln(distrib[gidx].second);
        if(!check_blnc()){
            error_print(number_st[gidx]);
            std::cout << "SyntaxError{out of balance:" << number_st[gidx].first <<":"<<number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
            exit(1);
            return;
        }
        gidx++;
        EXP_BKT_OPEN();
    }
    else{
        error_print(number_st[gidx]);
        std::cout << "SyntaxError{EXP_BIN}{:" << number_st[gidx].first << ":" << number_st[gidx].second << "}: " << distrib[gidx].first << " " << distrib[gidx].second << std::endl;
        exit(1);
        return;
    }
}

void Syntax::out_exp(){
    for(int i=0;i<expression.size(); ++i){
        std::cout << expression[i].first << " ";
    }
    std::cout << std::endl;
}

// TODO SEMANTUC TID
 
bool Syntax::Start() {
    BEGIN();
    return true;
}

void Syntax::List() {
    std::cout << "============= BYTE CODE =============" << '\n';
    CG.List();
}

void Syntax::Run() {
    std::cout << "============= EXECUTING =============" << '\n';
    CG.Run();
    std::cout << "============= DONE  =============" << '\n';
}


// SEMANTIC IN EXP
// ADD WHILE CYCLE
// base type {int, string,bool, double, structure(soon)}