#include <string>
#include <iostream>
#include <vector>
#include <map>

int main(){
    std::vector< char > st;
    std::map<char, int> mp;
    std::string res = "";
    mp['('] = 0;
    mp['+'] = mp['-'] = 1;
    mp['*'] = mp['/'] = 2;
    mp['^'] = 3;
    std::string exp = "a+2-3*7+(12*5-3)*(34-3)";
    for(int i=0;i<exp.size(); ++i){
        if(mp.find(exp[i]) != mp.end()){
            while(st.size() > 0 && mp[st.back()] >= mp[exp[i]]){
                res.push_back(st.back());
                st.pop_back();
            }
            st.push_back(exp[i]);
        }
        else if(exp[i] == '('){
            st.push_back(exp[i]);
        }
        else if(exp[i] == ')'){
            while(st.size() > 0 && st.back() != '('){
                res.push_back(st.back());
                st.pop_back();
            }
            st.pop_back();
        }
        else{
           res.push_back(exp[i]);
        }
    }
    while(!st.empty()){
        res.push_back(st.back());
        st.pop_back();
    }
    std::cout << res << std::endl;
    return 0;
}