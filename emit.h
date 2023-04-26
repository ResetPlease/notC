#pragma once
#ifndef EMIT_H
#define EMIT_H

#include <string>
#include <iostream>
#include <vector>

enum ArgType { NONE, STRING, INTEGER, BOOLEAN };

class Value
{
    public:
        Value();
        Value(int arg);
        Value(std::string arg);
        Value(bool arg);
        static Value Add(Value a, Value b);
        static Value Sub(Value a, Value b);
        static Value Mul(Value a, Value b);
        static Value Div(Value a, Value b);
        static Value LT(Value a, Value b);
        static Value GT(Value a, Value b);
        static Value EQ(Value a, Value b);
        bool toBool();
        void Parse(std::string s);
        ArgType type;
        std::string str;
        int integer;
        bool boolean;
};

class Command {
    public:
        std::string cmd;
        Value arg;
};

class Emitter {

    private:
        std::vector<Command> program;

    public:
        Emitter();

        void Emit(std::string cmd);
        void Emit(std::string cmd, int argint);
        void Emit(std::string cmd, std::string argstring);
        void Emit(std::string cmd, bool boolint);
        void List();
        int GetCounter();
        void FixJump(int cmd, int param);
        void Run();
};



#endif