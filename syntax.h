#include "lexical.h"
#include "emit.h"
#include <vector>
#include <string>
#include <set>
#include <map>

class Syntax {

    public:
        Syntax(std::vector< std::pair<std::string, std::string>> tokens, std::vector<std::string> t);
        bool Start();
        void SetNumberST(std::vector< std::pair<int, int>> tokens);
        void List();
        void Run();
            
    private:
        std::set<std::string> binary_operator = 
        {"operator+",
         "operator-",
         "operator=",
         "operator*",
         "operator,",
         "operator/",
         "operator>",
         "operator<",
         "operator%",
         "operator||",
         "operator&&",
         "operator.",
         "operator!=",
         "operator=="};
        
        std::set<std::string> base_type = 
        {"int",
         "str",
         "double",
         "boolean"};



        std::set<std::string> braket = 
        {"service{", "service}",
         "service[", "service]",
         "service(", "service)"};
        //TODO set of unary operators
        std::set<std::string> unary =
        {"operator++", "operator--",
         "operator+", "operator-", 
         "operator!"};
        std::set<std::string> openb = {"service(", "service{", "service["};
        std::set<std::string> closeb = {"service]", "service)", "service}"};
        std::vector<std::pair<int,int>> number_st;
        long long gidx = 0;
        std::vector< std::pair<std::string, std::string>> distrib;
        //serivce function
        bool sfind(std::set<std::string>& mset, std::string elem);
        bool eq(std::string elem);
        bool check_blnc();
        void set_bln(std::string elem);
        // data
        int str_balance = 0;
        std::map<std::string, int> blnc;
        std::vector<std::string> superlines;
        // Rec functions
        void BEGIN();
        void USING();
        void OPERATOR_END();
        void LIB();
        void IDENT();
        void FUNCTION();
        void SERVICE();
        void CYCLE_WHILE();
        void SERVICE_FOPEN();
        void CYCLE_FOR();
        void CONDITION_BEGIN();
        void BINARY_OPERATOR();
        void CONST_RVALUE();
        void BKT();
        void CYCLE();
        void CONDITION_ELSE();
        void FLOAT_PART();
        void CONST_FLOAT_PART();
        void UNARY();
        void STRING_LIT();
        void RETURN();
        void VAR();

        //reconstruction BKT-method
        void OPEN_FIGURE();
        void OPEN_SQUARE();
        void OPEN_ROUND();
        void CLOSE_FIGURE();
        void CLOSE_SQUARE();
        void CLOSE_ROUND();
        
        void DEF();
        void BEXP();
        
        void EXP();
        void EXP_ID();
        void EXP_BIN();
        void EXP_CONST();
        void EXP_BKT_CLOSE();
        void EXP_BKT_OPEN();

        void FOR();
        void FOR_DEF();

        void WHILE();

        void ASSIGNMENT();

        void ARG_DEF();
        bool LAST_DEF_IS_CONST = false;
        bool SIGN_FOR_EXIT = false;


        void IF();
        void ENDIF();
        void ELSE();

        void FID();
        void FBIN();
        void DEFID();
        void PRINT();
        void SCAN();

        void FixBranch();

        //printing
        std::string OUT_GREEN = "\e[32;1m",
                    OUT_RED = "\e[0;31m",
                    OUT_BOLD = "\e[12;1m",
                    OUT_PURP = "\e[35m",
                    OUT_END = "\e[0m";
        std::string out(std::string WORD, std::string COLOR);
        void error_print(std::pair<int,int> t);

        //SEMANTIC PART
        
        struct VRBL{
            std::string name = "VRBL_1287";
            std::string type = "undefined";
            std::string object_type = "custom_obj";
            std::string value = "None";
            VRBL(std::string A, std::string B){
                name = A;
                type = B;
            }
            VRBL(){
                name = "VRBL_1287";
                type = "undefined";
            }
            //value? 
        };
        bool if_exist(std::vector<VRBL>);

        std::vector< std::vector<VRBL> > TID;
        bool sclone(std::string name);
        VRBL* findclone(std::string name);

        std::vector<std::pair<std::string, std::string>> expression;
        void out_exp();

        std::map<std::string, int> priority;

        std::vector<std::pair<std::string, std::string>> poliz_exp;

        
        void ExpToPolize();
        std::string CalculatePolize();
        int function_balance_ = 0;

        enum OpStackCmd { CMD_WHILE, CMD_IF, CMD_FOR };

        struct OpStackElem
        {
            OpStackCmd command;
            int address_1;
            int address_2;
        };

        std::vector<OpStackElem> OpStack;

        Emitter CG;

};