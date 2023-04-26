#include "emit.h"
#include <iostream>
#include <map>
#include <functional>

Value::Value() {
    type = NONE;
}


Value::Value(int arg) {
    type = INTEGER;
    integer = arg;
}

Value::Value(std::string arg) {
    type = STRING;
    str = arg;
}

Value::Value(bool arg) {
    type = BOOLEAN;
    boolean = arg;
}

Value Value::Add(Value a, Value b)
{
    if (a.type!=b.type) { return Value(); }
    switch (a.type)
    {
        case INTEGER: return Value(a.integer+b.integer);
        case STRING: return Value(a.str+b.str);
    }
    return Value();
}

Value Value::Sub(Value a, Value b)
{
    if (a.type!=b.type) { return Value(); }
    switch (a.type)
    {
        case INTEGER: return Value(a.integer-b.integer);
    }
    return Value();
}

Value Value::Mul(Value a, Value b)
{
    if (a.type!=b.type) { return Value(); }
    switch (a.type)
    {
        case INTEGER: return Value(a.integer*b.integer);
    }
    return Value();
}

Value Value::Div(Value a, Value b)
{
    if (a.type!=b.type) { return Value(); }
    switch (a.type)
    {
        case INTEGER: return Value(a.integer/b.integer);
    }
    return Value();
}


Value Value::LT(Value a, Value b)
{
    switch (a.type)
    {
        case INTEGER: return Value(a.integer<b.integer);
        case STRING: return Value(a.str<b.str);
        case BOOLEAN: return Value(a.boolean < b.boolean);
    }
}

Value Value::GT(Value a, Value b)
{
    switch (a.type)
    {
        case INTEGER: return Value(a.integer>b.integer);
        case STRING: return Value(a.str>b.str);
        case BOOLEAN: return Value(a.boolean>b.boolean);
    }
}

Value Value::EQ(Value a, Value b)
{
    switch (a.type)
    {
        case INTEGER: return Value(a.integer==b.integer);
        case STRING: return Value(a.str==b.str);
        case BOOLEAN: return Value(a.boolean==b.boolean);
    }
}

bool Value::toBool()
{
    switch (type)
    {
        case INTEGER: return integer != 0;
        case BOOLEAN: return boolean;
    }
}

void Value::Parse(std::string s)
{
    switch (type)
    {
        case INTEGER: integer = std::stoi(s); break;
        case STRING: str = s; break;
        case BOOLEAN: boolean = s == "true" || s == "True"; break;
    }
}

std::ostream& operator<<(std::ostream& os, Value& v)
{
    switch(v.type)
    {
        case NONE: break;
        case INTEGER: os << v.integer; break;
        case BOOLEAN: os << v.boolean; break;
        case STRING: os << v.str; break;
    }
    return os;
}

Emitter::Emitter() { }

void Emitter::Emit(std::string cmd)
{
    Value v;
    program.push_back({ cmd : cmd, arg : v });
}

void Emitter::Emit(std::string cmd, int argint)
{
    Value v(argint);
    program.push_back({ cmd : cmd, arg: v  });
}

void Emitter::Emit(std::string cmd, std::string argstring)
{
    Value v(argstring);
    program.push_back({ cmd : cmd, arg : v });
}

void Emitter::Emit(std::string cmd, bool argbool)
{
    Value v(argbool);
    program.push_back({ cmd : cmd, arg : v });
}

void Emitter::List()
{
    int i=0;
    for(auto c : program)
    {
        std::cout << i++ << '\t' << c.cmd << '\t' << c.arg << '\n';
    }
}

int Emitter::GetCounter()
{
    return program.size();
}

void Emitter::FixJump(int cmd, int param)
{
    Value v(param);
    program[cmd].arg = v;
}

void Emitter::Run() 
{
    std::map<std::string, Value> TID;
    std::vector<Value> stack;
    int PC = 0;

    std::map<std::string, std::function<void(Value)> > cmdtbl;
    std::map<std::string, std::function<Value(Value,Value)> > optbl;
    
    cmdtbl["PUSH"] = [&] (Value arg) { stack.push_back(arg); };
    cmdtbl["LOAD"] = [&] (Value arg) { stack.push_back(TID[arg.str]); };
    cmdtbl["STORE"] = [&] (Value arg) { TID[arg.str] = stack.back(); stack.pop_back(); };
    cmdtbl["PRINT"] = [&] (Value arg) { std::cout << stack.back() << '\n'; stack.pop_back(); };
    cmdtbl["SCAN"] = [&] (Value arg) {
        std::string s;
        std::cin >> s;
        TID[arg.str].Parse(s);
    };
    cmdtbl["JMP"] = [&] (Value arg) { PC = arg.integer - 1; };
    cmdtbl["JZ"] = [&] (Value arg) { 
        auto a = stack.back();
        stack.pop_back();
        if (!a.toBool()) 
        {
            PC = arg.integer - 1;
        }

    };

    optbl["+"] = Value::Add;
    optbl["-"] = Value::Sub;
    optbl["*"] = Value::Mul;
    optbl["/"] = Value::Div;
    optbl["<"] = Value::LT;
    optbl[">"] = Value::GT;
    optbl["=="] = Value::EQ;
    
    

    while(PC < program.size()) {
        auto arg = program[PC].arg;
        auto cmd = program[PC].cmd;
        if (cmdtbl.find(cmd)!=cmdtbl.end())
        {
            cmdtbl[cmd](arg);
        }
        if (optbl.find(cmd)!=optbl.end())
        {
            auto a = stack.back(); stack.pop_back();
            auto b = stack.back(); stack.pop_back();
            auto c = optbl[cmd](b,a);
            stack.push_back(c);
        }
        PC++;
    }

}