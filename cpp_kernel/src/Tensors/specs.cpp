#include "state.h"

State State::from_label(const std::string& str, const Qubits& qubs){
    int l = str.size();
    if (l != qubs.size()){
        throw QException("Number of qubits should be equal to the length of label");
    }
    State s(qubs);
    int pos = 0;
    for(int i=0; i < l; i++){
        switch (str[i])
        {
        case '0':
            break;
        case '1':
            pos += 1<<i;
            break;
        default:
            throw QException("state label can consist only from 0 and 1");
            break;
        }
    } 
    s[0] = 0;
    s[pos] = 1;
    return s;
}

