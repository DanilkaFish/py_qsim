// #ifndef _COMMON
// #define _COMMON



class Qubit{
public:
    Qubit(): num{0} { }
    Qubit(int num): num{num} {}
    int num;
};


// class Qubits{
// public:
//     using container=std::vector<Qubit>; 
//     using iterator=typename container::iterator;
//     using const_iterator=typename container::const_iterator;

//     iterator begin() { return qubs.begin(); }
//     iterator end() { return qubs.end(); }
//     const_iterator begin() const { return qubs.begin(); }
//     const_iterator end() const { return qubs.end(); }

//     Qubits(): qubs{} {}
//     Qubits(const Qubits& qubs): qubs{qubs.qubs} {}
//     explicit Qubits(int n) {
//         for (int i=0; i<n; i++){
//             this->insert(i);
//         }
//     }
//     Qubits(std::initializer_list<Qubit> tot): qubs{tot} {}
//     const Qubit& operator[](int i) const { return qubs[i]; }
//     Qubit& operator[](int i) { return qubs[i]; }
//     int size() const noexcept {  return qubs.size(); }
//     void insert(const Qubit& qub) {
//         if (std::find(qubs.begin(), qubs.end(), qub) == qubs.end()){
//             this->qubs.push_back(qub); 
//         }
//     }
//     void push_back(const Qubit& qub) { this->qubs.push_back(qub); }
//     std::string to_str(){ std::string s; for (auto x: qubs) s += std::to_string(x.num) + ','; return s;}
// private:
//     container qubs;
// };

// int qubs_to_int(const Qubits& qubs);

// class PauliString{
// public:
//     PauliString()=default;
//     PauliString(const std::string& s, const Qubits& qubs);
//     Qubit       get_qn(int i ) const { return qubs[i]; }
//     Qubits      get_qubs() const { return qubs; }
//     Qubits      get_sorted_qubs() const { 
//         Qubits qubs_copy(qubs);
//         std::sort(qubs_copy.begin(), qubs_copy.end());
//         return qubs_copy;
//     }
//     std::string get_name() const;
//     char        get_ch(int i ) const { return s[i]; }
//     int         size()         const { return s.size(); }
// private:
//     std::vector<std::pair<char, Qubit>> ps;
//     Qubits qubs;
//     std::string s;
// };
// bool qubs_in_qubs(const Qubits& qubsl, const Qubits& qubsr);



// #endif