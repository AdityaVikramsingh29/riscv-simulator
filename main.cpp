#include <bits/stdc++.h>
using namespace std;
#define ll long long

vector<bitset<8>>mem(0x50002);
inline void ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch)
                                    { return !std::isspace(ch); }));
}

inline void rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch)
                         { return !std::isspace(ch); })
                .base(),
            s.end());
}

inline void trim(std::string &s)
{
    rtrim(s);
    ltrim(s);
}
string getfilename(const string &input_line) {
    stringstream ss(input_line);
    string command, remaining;
    ss >> command;          
    getline(ss, remaining);   
    trim(remaining);          
    return remaining;
}

void reset(map<string, int> &label_lineno, string &line, vector<string> &instructions, int &lineno ,vector<ll>&reg, ll&pc,set<ll>&brkpts)
{
    label_lineno.clear();
    instructions.clear();
    lineno=0;
    line="";    
    pc=0;
    reg.clear();
    reg.resize(32,0);
    brkpts.clear();
    mem.clear();
    // just for testing
    // mem[0x10000]=239;
    // mem[0x10001]=205;
    // mem[0x10002]=171;
    // mem[0x10003]=137;
    // mem[0x10004]=103;
    // mem[0x10005]=69;
    // mem[0x10006]=35;
    // mem[0x10007]=17;
    // reg[1]=0x10000;
}

ll to_signed_int(string&s)
{
    long long number = 0;

    // Convert binary string to an integer
    for (int i = 0; i < 12; ++i) {
        if (s[i] == '1') {
            number += (1LL << (11 - i));
        }
    }

    // Check if the number is negative (12th bit is the sign bit)
    if (s[0] == '1') {
        // If the sign bit is set, convert to negative by subtracting 2^12
        number -= (1LL << 12);
    }

    return number;
    // ll ans=0;
    // for(int i=0;i<s.length()-1;i++)
    // {
    //     ans+=pow(2,i)*(s[i]=='1');
    // }
    // if(s[11]=='1')
    // ans=-ans;
    // return ans;
}

ll to_unsigned_int(string&s)
{
    ll ans=0;
    for(int i=0;i<s.length();i++)
    {
        ans+=pow(2,i)*(s[i]=='1');
    }
    return ans;
}

string removefirstword(string &input_line) {
    trim(input_line);
    size_t first_space_pos = input_line.find(' ');
    string ans = input_line.substr(first_space_pos + 1);
    trim(ans);
    return ans;
}

string toBinary(ll num, int num_digits)
{
    if (num < 0)
    {
        num = ((1LL) << num_digits) + num;
    }
    string binary;
    while (num > 0)
    {
        if (num % 2 == 1)
            binary += '1';
        else
            binary += '0';
        num /= 2;
    }
    reverse(binary.begin(), binary.end());
    while (binary.length() < num_digits)
    {
        binary = '0' + binary;
    }
    return binary;
}

string getFirstWord(string &line)
{
    istringstream iss(line);
    string firstWord;
    int ind = -1;
    for (int i = 0; i < line.length(); i++)
    {
        if (line[i] == ' ' || line[i] == ',' || line[i] == ':' || (line[i] == '\n'))
        {
            ind = i;
            break;
        }
    }
    if (ind == -1)
        ind = line.length();
    firstWord = line.substr(0, ind);
    return firstWord;
}

string to_hex(string bin_ins)
{

    map<string, string> map4bits;

    for (int i = 0; i <= 15; i++)
    {
        if (i <= 9)
        {
            map4bits[toBinary(i, 4)] = '0' + i;
        }
        else
        {
            map4bits[toBinary(i, 4)] = 'a' + (i - 10);
        }
    }
    string hexstring;
    for (int i = 0; i < 32; i += 4)
    {
        string binary4 = bin_ins.substr(i, 4);
        hexstring += map4bits[binary4];
    }

    return hexstring;
}
void R_format(map<string, string> &m_opcode,
              map<string, char> &m_format,
              map<string, int> &m_funct3, map<string, string> &alias, string &line, string &command, int lineno, ofstream& myfile, vector<ll>&reg,ll &pc)
{
    // cout<<"function \n";
    string temp = line;
    int i = 0;
    string cmd, rs1, rs2, rd, funct3, funct7, opcode;
    while (true)
    {
        string firstWord = getFirstWord(temp);
        // cout << alias[firstWord] << "\n";
        if (i == 0)
        {
            cmd = firstWord;
        }
        else if (i == 1)
        {
            rd = firstWord;
        }
        else if (i == 2)
        {
            rs1 = firstWord;
        }
        else if (i == 3)
        {
            rs2 = firstWord;
        }
        i++;
        if (i == 4)
            break;
        if (i == 1)
        {
            int ind_sp = temp.find(' ');
            if (ind_sp == string::npos)
            {
                cerr << "missing value in line " << lineno << "\n";
                myfile.close();
                exit(0);
            }
            temp = temp.substr(ind_sp + 1);
            trim(temp);
        }
        else
        {
            int ind_cm = temp.find(',');
            if (ind_cm == string::npos)
            {
                cerr << "missing register or immediate value in line " << lineno << "\n";
                myfile.close();
                exit(0);
            }
            temp = temp.substr(ind_cm + 1);
            trim(temp);
        }
    }
    rd = alias[rd];
    rs1 = alias[rs1];
    rs2 = alias[rs2];
    if (alias.find(rs1) == alias.end() || alias.find(rs2) == alias.end() || alias.find(rd) == alias.end() || (int)stoi(rs1.substr(1)) > 31 || (int)stoi(rs2.substr(1)) > 31 || (int)stoi(rd.substr(1)) > 31)
    {
        cerr << "incorrect register value in line " << lineno << "\n";
        myfile.close();
        exit(0);
    }
    opcode = m_opcode[cmd];
    int rs1i=stoi((alias[rs1]).substr(1));
    int rs2i=stoi((alias[rs2]).substr(1));
    int rdi=stoi((alias[rd]).substr(1));
    pc+=4;
    if(rdi==0)
    return ;
    rs1 = toBinary(stoi((alias[rs1]).substr(1)), 5);
    rs2 = toBinary(stoi((alias[rs2]).substr(1)), 5);
    rd = toBinary(stoi((alias[rd]).substr(1)), 5);
    ll val2=reg[rs2i];
    ll val1=reg[rs1i];
    // cout<<val1<<" "<<val2<<",\n";
    if(cmd.compare("add")==0)
    {
        reg[rdi]=val1+val2;
    }
    else if(cmd.compare("sub")==0)
    {
        reg[rdi]=val1-val2;
    }
    else if(cmd.compare("xor")==0)
    {
        reg[rdi]=val1^val2;
    }
    else if(cmd.compare("or")==0)
    {
        reg[rdi]=val1|val2;
    }
    else if(cmd.compare("and")==0)
    {
        reg[rdi]=val1&val2;
    }
    else if(cmd.compare("sll")==0)
    {
        cout << val1 << " " << val2 << " " << endl;
        if(val2<0||val2>63)return;
        reg[rdi]=val1 << val2;
    }
    else if(cmd.compare("srl")==0)
    {
        reg[rdi]=val1>>val2;;
    }    
    // cout<<reg[rdi]<<",\n";
    // 
    // funct3 = toBinary(m_funct3[cmd], 3);

    // funct7 = "0000000";
    // if (cmd.compare("sub") == 0 || cmd.compare("sra") == 0)
    //     funct7 = "0100000";
    // string binenc = funct7 + rs2 + rs1 + funct3 + rd + opcode;
    // string hexenc = to_hex(binenc);
    // myfile << hexenc<<"\n";
}

void S_format_case(string &cmd, string &rs1, string &rs2, string &imm, string &line, int lineno,ofstream& myfile)
{
    string temp = line;
    int i = 0;
    rs1 = "", rs2 = "", imm = "";

    trim(temp);
    if(temp[temp.length()-1]!=')')
    {
        cerr << "missing paranthesis in line " << lineno << "\n";
        myfile.close();
        exit(0);
    }
    while (i < temp.length() && temp[i] != ' ')
    {
        i++;
    }
    cmd = temp.substr(0, i);
    temp = temp.substr(i);
    trim(temp);
    i = 0;
    while (i < temp.length() && temp[i] != ',')
    {
        i++;
    }
    rs2 = temp.substr(0, i);
    i++;
    temp = temp.substr(i);
    trim(temp);
    i = 0;
    while (i < temp.length() && temp[i] != '(')
    {
        i++;
    }
    if(i==temp.length())
    {
        cerr << "missing paranthesis in line " << lineno << "\n";
        myfile.close();
        exit(0);
    }
    imm = temp.substr(0, i);
    i++;
    temp = temp.substr(i);
    trim(temp);
    i = 0;
    while (i < temp.length() && temp[i] != ')')
    {
        i++;
    }
    rs1 = temp.substr(0, i);
    // cout<<"imm is "<<imm<< ",\n";
    if (imm.compare("") == 0)
    {
        cerr << "missing immediate value in line " << lineno << "\n";
        myfile.close();
        exit(0);
    }
    else if (rs1.compare("") == 0 || rs2.compare("") == 0)
    {
        cerr << "missing register value in line " << lineno << "\n";
        myfile.close();
        exit(0);
    }
    // using start check to start checking the starting point of numerical values after ignoring negative sign
    int start_check = 0;
    if (imm.length() > 0 && imm[0] == '-')
        start_check = 1;
    for (int i = start_check; i < imm.length(); i++)
    {
        char c = imm[i];
        if (c < '0' || c > '9')
        {
            cerr << "incorrect immediate value in line " << lineno << "\n";
            myfile.close();
            exit(0);
        }
    }
}

void store_type(ll addr,ll val, int no_bytes)
{
    for(int i=0;i<no_bytes;i++)
    {
        ll temp=(val&0xff);
        val=val>>8;
        mem[addr+i]=bitset<8>(temp);
    }
}
void store_handle(ll rs1,ll rs2,ll diff,string &cmd,vector<ll>&reg, ll &pc)
{
    pc+=4;
    ll val=reg[rs2];
    ll addr=reg[rs1]+diff;
    if(cmd.compare("sb")==0)
    {
        store_type(addr,val,1);
    }
    else if(cmd.compare("sh")==0)
    {
        store_type(addr,val,2);
    }
    else if(cmd.compare("sw")==0)
    {
        store_type(addr,val,4);
    }
    else if(cmd.compare("sd")==0)
    {
        store_type(addr,val,8);
    }
}

void S_format(map<string, string> &m_opcode, map<string, char> &m_format, map<string, int> &m_funct3, map<string, string> &alias, string &line, string &command, int lineno, ofstream& myfile,vector<ll>&reg, ll &pc)
{
    string temp = line;
    int i = 0;
    string cmd, rs1, rs2, funct3, imm, opcode;
    if (command.compare("sb") == 0 || command.compare("sh") == 0 || command.compare("sw") == 0 || command.compare("sd") == 0)
    {
        S_format_case(cmd, rs1, rs2, imm, line, lineno,myfile);
    }
    // cout << alias[rs2] << "\n";
    // cout << alias[rs1] << "\n";
    // cout << imm << "\n";
    rs1 = alias[rs1];
    rs2 = alias[rs2];
    if (alias.find(rs1) == alias.end() || alias.find(rs2) == alias.end() || (int)stoi(rs1.substr(1)) > 31 || (int)stoi(rs2.substr(1)) > 31)
    {
        cerr << "incorrect register value\n in line " << lineno << "\n";
        myfile.close();
        exit(0);
    }
    opcode = m_opcode[cmd];
    // cout << opcode << "\n";
    // cout << funct3 << "\n";

    ll value_imm = stoi(imm);
    if (value_imm < -2048 || value_imm > 2047)
    {
        cerr << "the immediate value is out of bounds of 12 bits in line no " << lineno << endl;
        myfile.close();
        exit(0);
    }

    store_handle(stoi((rs1).substr(1)),stoi((rs2).substr(1)),value_imm,cmd,reg,pc);

    rs1 = toBinary(stoi((alias[rs1]).substr(1)), 5);
    rs2 = toBinary(stoi((alias[rs2]).substr(1)), 5);
    funct3 = toBinary(m_funct3[cmd], 3);
    // imm = toBinary(stoi(imm), 12);
    // string t = imm;
    // reverse(t.begin(), t.end());
    // string imm1 = t.substr(0, 5);
    // string imm2 = t.substr(5);

    // reverse(imm1.begin(), imm1.end());
    // reverse(imm2.begin(), imm2.end());
    // string binenc = imm2 + rs2 + rs1 + funct3 + imm1 + opcode;
    // string hexenc = to_hex(binenc);
    // myfile << hexenc<<"\n";
}

void B_format_case(string &cmd, string &rs1, string &rs2, string &imm, string &line, int lineno, map<string, int> &label_lineno,ofstream& myfile,string& label)
{
    string temp = line;
    int i = 0;
    rs1 = "", rs2 = "", imm = "";
    while (true)
    {
        string firstWord = getFirstWord(temp);
        // cout << alias[firstWord] << "\n";
        if (i == 0)
        {
            cmd = firstWord;
        }
        else if (i == 1)
        {
            rs1 = firstWord;
        }
        else if (i == 2)
        {
            rs2 = firstWord;
        }
        else if (i == 3)
        {
            label = firstWord;
            if (label_lineno.find(label) == label_lineno.end())
            {
                cout << "Incorrect Label Used" << "\n";
                myfile.close();
                exit(0);
            }
            // cout << lineno;
            int val = (label_lineno[label] - lineno - 1) * 4;
            // cout << val;
            imm = to_string(val);
        }
        i++;
        if (i == 4)
            break;
        if (i == 1)
        {
            int ind_sp = temp.find(' ');
            if (ind_sp == string::npos)
            {
                cerr << "missing value in line " << lineno << "\n";
                myfile.close();
                exit(0);
            }
            temp = temp.substr(ind_sp + 1);
            trim(temp);
        }
        else
        {
            int ind_cm = temp.find(',');
            if (ind_cm == string::npos)
            {
                cerr << "missing register or immediate value in line " << lineno << "\n";
                myfile.close();
                exit(0);
            }
            temp = temp.substr(ind_cm + 1);
            trim(temp);
        }
    }
}

void B_format(map<string, string> &m_opcode, map<string, char> &m_format, map<string, int> &m_funct3, map<string, int> &label_lineno, map<string, string> &alias, string &line, string &command, int lineno, ofstream& myfile,vector<ll>&reg,ll &pc)
{

    string temp = line;
    int i = 0;
    string label;
    string cmd, rs1, rs2, funct3, imm, opcode;
    if (command.compare("beq") == 0 || command.compare("bne") == 0 || command.compare("blt") == 0 || command.compare("bge") == 0 || command.compare("bltu") == 0 || command.compare("bgeu") == 0)
    {
        B_format_case(cmd, rs1, rs2, imm, line, lineno, label_lineno,myfile,label);
    }
    rs1 = alias[rs1];
    rs2 = alias[rs2];
    if (alias.find(rs1) == alias.end() || alias.find(rs2) == alias.end() || (int)stoi(rs1.substr(1)) > 31 || (int)stoi(rs2.substr(1)) > 31)
    {
        cerr << "incorrect register value\n in line " << lineno << "\n";
        myfile.close();
        exit(0);
    }

    opcode = m_opcode[cmd];
    ll val1=reg[stoi((alias[rs1]).substr(1))];
    ll val2=reg[stoi((alias[rs2]).substr(1))];
    // cout<<val1<<val2<<",\n";
    ll dest=4*(label_lineno[label]-1);
    // cout<<dest<<",\n";
    if(cmd.compare("beq")==0&&val1==val2)
    {
        pc=dest;
    } 
    else if(cmd.compare("bne")==0&&val1!=val2)
    {
        pc=dest;
    }
    else if(cmd.compare("blt")==0&&val1<val2)
    {
        pc=dest;
    }
    else if(cmd.compare("bge")==0&&val1>=val2)
    {
        pc=dest;
    }
    else if(cmd.compare("bltu")==0)
    {
        unsigned ll temp1=val1;
        if(val1<0)
        {
            temp1=ULLONG_MAX-1+val1;
        }
        unsigned ll temp2=val2;
        if(val2<0)
        {
            temp2=ULLONG_MAX-1+val2;
        }
        // string op1=toBinary(val1,63);
        // string op2=toBinary(val2,63);
        // temp1=(1LL<<62)-1;
        // temp1+=temp1+1;
        // cout<<to_string(temp1)<<','<<to_string(temp2)<<",\n";
        // string op1=to_string(temp1);
        // string op2=to_string(temp2);
        // cout<<"op1"<<op1<<"op2"<<op2<<"difff"<<op1.compare(op2)<<",\n";
        if(temp1<temp2)
        pc=dest;
        else pc+=4;
    }
    else if(cmd.compare("bgeu")==0)
    {
        unsigned ll temp1=val1;
        if(val1<0)
        {
            temp1=ULLONG_MAX-1+val1;
        }
        unsigned ll temp2=val2;
        if(val2<0)
        {
            temp2=ULLONG_MAX-1+val2;
        }
        if(temp1>=temp2)
        pc=dest;
        else pc+=4;
    }
    else
    {
        pc+=4;
    }
    return ;
}

void I_format_case1(string &cmd, string &rs1, string &rd, string &imm, string &line, int lineno,ofstream& myfile)
{
    string temp = line;
    int i = 0;
    rs1 = "", rd = "", imm = "";
    while (true)
    {
        string firstWord = getFirstWord(temp);
        // cout << alias[firstWord] << "\n";
        if (i == 0)
        {
            cmd = firstWord;
        }
        else if (i == 1)
        {
            rd = firstWord;
        }
        else if (i == 2)
        {
            rs1 = firstWord;
        }
        else if (i == 3)
        {
            imm = firstWord;
        }
        i++;
        if (i == 4)
            break;
        if (i == 1)
        {
            int ind_sp = temp.find(' ');
            if (ind_sp == string::npos)
            {
                cerr << "missing value in line " << lineno << "\n";
                myfile.close();
                exit(0);
            }
            temp = temp.substr(ind_sp + 1);
            trim(temp);
        }
        else
        {
            int ind_cm = temp.find(',');
            if (ind_cm == string::npos)
            {
                cerr << "missing register or immediate value in line " << lineno << "\n";
                myfile.close();
                exit(0);
            }
            temp = temp.substr(ind_cm + 1);
            trim(temp);
        }
    }
    // cout<<"imm is "<<imm<< ",\n";
    if (imm.compare("") == 0)
    {
        cerr << "missing immediate value in line " << lineno << "\n";
        myfile.close();
        exit(0);
    }
    else if (rs1.compare("") == 0 || rd.compare("") == 0)
    {
        cerr << "missing register value in line " << lineno << "\n";
        myfile.close();
        exit(0);
    }
    int start_check = 0;
    if (imm.length() > 0 && imm[0] == '-')
        start_check = 1;
    for (int i = start_check; i < imm.length(); i++)
    {
        char c = imm[i];
        if (c < '0' || c > '9')
        {
            cerr << "incorrect immediate value in line " << lineno << "\n";
            myfile.close();
            exit(0);
        }
    }
    
}

void I_format_case2(string &cmd, string &rs1, string &rd, string &imm, string &line, int lineno,ofstream& myfile)
{
    string temp = line;
    int i = 0;
    rs1 = "", rd = "", imm = "";

    trim(temp);
    if(temp[temp.length()-1]!=')')
    {
        cerr << "missing paranthesis in line " << lineno << "\n";
        myfile.close();
        exit(0);
    }
    while (i < temp.length() && temp[i] != ' ')
    {
        i++;
    }
    cmd = temp.substr(0, i);
    temp = temp.substr(i);
    trim(temp);
    i = 0;
    while (i < temp.length() && temp[i] != ',')
    {
        i++;
    }
    rd = temp.substr(0, i);
    i++;
    temp = temp.substr(i);
    trim(temp);
    i = 0;
    while (i < temp.length() && temp[i] != '(')
    {
        i++;
    }
    if(i==temp.length())
    {
        cerr << "missing paranthesis in line " << lineno << "\n";
        myfile.close();
        exit(0);
    }
    imm = temp.substr(0, i);
    i++;
    temp = temp.substr(i);
    trim(temp);
    i = 0;
    while (i < temp.length() && temp[i] != ')')
    {
        i++;
    }
    rs1 = temp.substr(0, i);
    if (cmd.compare("jalr") == 0)
    {
        string t = rs1;
        rs1 = imm;
        imm = t;
        int value_imm = stoi(imm);
        if (value_imm < -2048 || value_imm > 2047)
        {
            cerr << "the immediate value is out of bounds of 12 bits in line no " << lineno << endl;
            myfile.close();
            exit(0);
        }
    }
    // cout<<cmd<<","<<rd<<","<<rs1<<","<<imm<<",\n";
    // cout<<"imm is "<<imm<< ",\n";

    if (imm.compare("") == 0)
    {
        cerr << "missing immediate value in line " << lineno << "\n";
        myfile.close();
        exit(0);
    }
    else if (rs1.compare("") == 0 || rd.compare("") == 0)
    {
        cerr << "missing register value in line " << lineno << "\n";
        myfile.close();
        exit(0);
    }

    // using start check to start checking the starting point of numerical values after ignoring negative sign
    int start_check = 0;
    if (imm.length() > 0 && imm[0] == '-')
        start_check = 1;
    for (int i = start_check; i < imm.length(); i++)
    {
        char c = imm[i];
        if (c < '0' || c > '9')
        {
            cerr << "incorrect immediate value in line " << lineno << "\n";
            myfile.close();
            exit(0);
        }
    }
}

ll getvalue(ll source, ll no_bytes)
{
    ll ans=0;
    // for(int i=8-no_bytes;i<8;i++)
    // {
    //     ll temp=(ll)(mem[source+i].to_ullong());
    //     ans=temp+(ans<<8);
    //     cout<<ans<<",";
    // }
    for(int i=0;i<no_bytes;i++)
    {
        ll temp=(ll)(mem[source+i].to_ullong());
        ans=ans+(temp<<8*i);
        // cout<<ans<<",";
    }
    // cout<<endl;
    return ans;
}

void load_handle(string &cmd,string&rs1,string &rd, string &imm,vector<ll>&reg,ll&pc)
{
    ll addr=reg[stoi(rs1.substr(1))];
    ll diff=stoi(imm);
    ll dest=stoi(rd.substr(1));
    ll source=diff+addr;

    pc+=4;
    if(cmd.compare("lb")==0)
    {
        reg[dest]=getvalue(source,1);
        if((reg[dest]>>7)&1)
        {
            reg[dest]|=0xffffffffffffff00;
        }
    }
    if(cmd.compare("lh")==0)
    {
        reg[dest]=getvalue(source,2);
        if((reg[dest]>>15)&1)
        {
            reg[dest]|=0xffffffffffff0000;
        }
    }
    if(cmd.compare("lw")==0)
    {
        reg[dest]=getvalue(source,4);
        if((reg[dest]>>31)&1)
        {
            reg[dest]|=0xffffffff00000000;
        }
    }
    if(cmd.compare("ld")==0)
    {
        reg[dest]=getvalue(source,8);
    }
    if(cmd.compare("lbu")==0)
    {
        reg[dest]=getvalue(source,1)&0xff;
    }
    if(cmd.compare("lhu")==0)
    {
        reg[dest]=getvalue(source,2)&0xffff;
    }
    if(cmd.compare("lwu")==0)
    {
        reg[dest]=getvalue(source,4)&0xffffffff;
    }
    // cout<<mem[0x10000]<<endl;
    return ;
}

void I_format(map<string, string> &m_opcode,
              map<string, char> &m_format,
              map<string, int> &m_funct3, map<string, string> &alias, string &line, string &command, int lineno, ofstream& myfile,vector<ll>&reg,ll &pc)
{
    string temp = line;
    int i = 0;
    string cmd, rs1, rd, funct3, imm, opcode;
    if (command.compare("addi") == 0 || command.compare("xori") == 0 || command.compare("ori") == 0 || command.compare("andi") == 0 || command.compare("slli") == 0 || command.compare("srli") == 0 || command.compare("srai") == 0)
    {
        I_format_case1(cmd, rs1, rd, imm, line, lineno,myfile);
    }
    else
    {
        // adding format of case 2
        I_format_case2(cmd, rs1, rd, imm, line, lineno,myfile);
        
        // else
        // I_format_case2(cmd, imm, rd, rs1, line,lineno);
    }
    // cout << alias[rs1] << "\n";
    // cout << alias[rd] << "\n";
    // cout << imm << "\n";
    rs1 = alias[rs1];
    rd = alias[rd];
    if (alias.find(rs1) == alias.end() || alias.find(rd) == alias.end() || (int)stoi(rs1.substr(1)) > 31 || (int)stoi(rd.substr(1)) > 31)
    {
        cerr << "incorrect register value\n in line " << lineno << "\n";
        myfile.close();
        exit(0);
    }
    opcode = m_opcode[cmd];

    if(command.compare("jalr")==0)
        {
            ll value_imm=stoi(imm);
            value_imm*=2;

            reg[stoi((alias[rd]).substr(1))]=pc+4;
            pc=reg[stoi((alias[rs1]).substr(1))]+value_imm; 
            // cout<<hex<<pc<<",\n"       ;
            return ;
        }

    set<string>loadtype={"ld","lw","lb","lh","lhu","lbu","lwu"};
    if(loadtype.find(cmd)!=loadtype.end())
    {load_handle(cmd,rs1,rd,imm,reg,pc); return ;}

    int rs1i=stoi((alias[rs1]).substr(1));
    int rdi=stoi((alias[rd]).substr(1));
    rs1 = toBinary(stoi((alias[rs1]).substr(1)), 5);
    rd = toBinary(stoi((alias[rd]).substr(1)), 5);
    funct3 = toBinary(m_funct3[cmd], 3);
    pc+=4;
    set<string> case1 = {"addi", "xori", "ori", "andi", "slli", "srli", "srai"};
    if (case1.count(cmd) > 0)
        imm = toBinary(stoi(imm), 12);
    else
    {
        char bit = '0';
        int start = 64;
        // "entering case 2";
        if (cmd.compare("lb") == 0)
        {
            imm = toBinary(stoi(imm), 40);
            reverse(imm.begin(), imm.end());
            bit = imm[7];
            start = 8;
        }
        else if (cmd.compare("lh") == 0)
        {
            imm = toBinary(stoi(imm), 40);
            reverse(imm.begin(), imm.end());
            bit = imm[15];
            start = 16;
        }
        else if (cmd.compare("lw") == 0)
        {
            imm = toBinary(stoi(imm), 40);
            reverse(imm.begin(), imm.end());
            bit = imm[31];
            start = 32;
        }
        else if (cmd.compare("ld") == 0)
        {
            imm = toBinary(stoi(imm), 40);
            reverse(imm.begin(), imm.end());
        }
        else
        {
            imm = toBinary(stoi(imm), 40);
            reverse(imm.begin(), imm.end());
        }
        for (int i = start; i < 40; i++)
        {
            imm[i] = bit;
        }

        if (cmd.compare("lbu") == 0 || cmd.compare("lhu") == 0 || cmd.compare("lwu") == 0)
        {
            for (int i = start; i < 40; i++)
            {
                imm[i] = '0';
            }
        }
        imm = imm.substr(0, 12);
        reverse(imm.begin(), imm.end());
        // cout<<"imm value is  "<<imm<<endl;
    }
    int cnt = 12;
    for (int i = 0; i < 12; i++)
    {
        if (imm[i] == '0')
            cnt--;
        else
            break;
    }
    set<string> shiftops = {"slli", "srli", "srai"};
    if (shiftops.count(command))
    {
        if (cnt > 6)
        {
            cerr << " immediate value for shifting is out of range on lineno " << lineno << "\n";
            myfile.close();
            exit(0);
        }
        // else
        // {
        //     if (command.compare("slli") == 0)
        //     {
        //         imm.replace(0, 6, "000000");
        //     }
        //     else if (command.compare("srli") == 0)
        //     {
        //         imm.replace(0, 6, "000000");
        //     }
        //     else
        //     {
        //         imm.replace(0, 6, "010000");
        //     }
        // }
    }
    
    ll val1=reg[rs1i];
    // reverse(imm.begin(),imm.end());
    ll value_imm=to_signed_int(imm);

    // cout<<"imm is"<<imm<<","<<val1<<",\n";
    if(cmd.compare("andi")==0)
    {
        reg[rdi]=value_imm&val1;
    }
    else if(cmd.compare("xori")==0)
    {
        reg[rdi]=value_imm^val1;
    }
    else if(cmd.compare("ori")==0)
    {
        reg[rdi]=value_imm|val1;
    }
    else if(cmd.compare("addi")==0)
    {
        reg[rdi]=value_imm+val1;
    }
    else if(cmd.compare("slli")==0)
    {
        reg[rdi]=val1<<value_imm;
    }
    else if(cmd.compare("srli")==0)
    {
        if(val1<0)
        {
            unsigned ll temp1=val1;
            reg[rdi]=temp1>>value_imm;
        }
        else
        reg[rdi]=val1>>value_imm;
    }
    else if(cmd.compare("srai")==0)
    {
        reg[rdi]=val1/(pow(2,value_imm));
    }
    else{

    }  

}


void U_format_case(string &cmd, string &rd, string &imm, string &line, int lineno,ofstream& myfile)
{
    string temp = line;
    int i = 0;
    rd = "", imm = "";cmd="";
    while (true)
    {
        string firstWord = getFirstWord(temp);
        if (i == 0)
        {
            cmd = firstWord;
        }
        else if (i == 1)
        {
            rd = firstWord;
        }
        else if (i == 2)
        {
            imm = firstWord;
        }
        // cout<<cmd<<rd<<imm<<",\n";
        i++;
        if (i == 3)
            break;
        if (i == 1)
        {
            int ind_sp = temp.find(' ');
            if (ind_sp == string::npos)
            {
                cerr << "missing value in line " << lineno + 1 << "\n";
                myfile.close();
                exit(0);
            }
            temp = temp.substr(ind_sp + 1);
            trim(temp);
        }
        else
        {
            int ind_cm = temp.find(',');
            if (ind_cm == string::npos)
            {
                cerr << "missing register or immediate value or label in line " << lineno + 1 << "\n";
                myfile.close();
                exit(0);
            }
            temp = temp.substr(ind_cm + 1);
            trim(temp);
        }
    }
}

void U_format(map<string, string> &m_opcode,
              map<string, char> &m_format,
              map<string, int> &m_funct3, map<string, string> &alias, string &line, string &command, int lineno, ofstream& myfile,vector<ll>&reg, ll &pc)
{
    string temp = line;
    int i = 0;
    string cmd, rd, imm, opcode;
    U_format_case(cmd,rd,imm,line,lineno,myfile);
    rd = alias[rd];
    if (alias.find(rd) == alias.end() || (int)stoi(rd.substr(1)) > 31)
    {
        cerr << "incorrect register value\n in line " << lineno + 1 << "\n";
        myfile.close();
        exit(0);
    }
    opcode = m_opcode[cmd];
    ll value_imm=0;
    imm=imm.substr(2);
    for(int i=0;i<=imm.length()-1;i++)
    {
        value_imm=value_imm*16+imm[i]-'0';
    }
    if(value_imm<0||value_imm>(ll)((1LL<<32)-1))
    {
        cerr << "incorrect immediate value in line " << lineno + 1 << "\n";
        myfile.close();
        exit(0);
    }
    value_imm=value_imm&(0xfffff);
    ll curr=value_imm<<12;
    pc+=4;
    if(curr>>31&1)
    {
        ll temp=0xFFFFFFFF00000000;
        curr=temp|curr;
    }
    reg[stoi(rd.substr(1))]=curr;
    // rd = toBinary(stoi(rd.substr(1)),5);
    // imm=toBinary(value_imm,32);
    // imm=imm.substr(12,20);
    // string binenc = imm + rd + opcode;
    // string hexenc = to_hex(binenc);
    // myfile << hexenc<<"\n";
}
void J_format_case(string &cmd, string &rd, string &imm, string &line, int lineno, map<string, int> &label_lineno,ofstream& myfile,string &label)
{
    string temp = line;
    int i = 0;
    rd = "", imm = "";
    while (true)
    {
        string firstWord = getFirstWord(temp);
        // cout << alias[firstWord] << "\n";
        if (i == 0)
        {
            cmd = firstWord;
        }
        else if (i == 1)
        {
            rd = firstWord;
        }
        else if (i == 2)
        {
            label = firstWord;
            if (label_lineno.find(label) == label_lineno.end())
            {
                if (label.size() == 0)
                {
                    cerr << "missing register or immediate value or label in line " << lineno + 1 << "\n";
                    myfile.close();
                    exit(0);
                }
                cout << "Incorrect Label Used in line number " << lineno + 1 << "\n";
                myfile.close();
                exit(0);
            }
            int val = (label_lineno[label] - lineno - 1) * 4;
            // cout << val;
            imm = to_string(val);
        }
        i++;
        if (i == 3)
            break;
        if (i == 1)
        {
            int ind_sp = temp.find(' ');
            if (ind_sp == string::npos)
            {
                cerr << "missing value in line " << lineno + 1 << "\n";
                myfile.close();
                exit(0);
            }
            temp = temp.substr(ind_sp + 1);
            trim(temp);
        }
        else
        {
            int ind_cm = temp.find(',');
            if (ind_cm == string::npos)
            {
                cerr << "missing register or immediate value or label in line " << lineno + 1 << "\n";
                myfile.close();
                exit(0);
            }
            temp = temp.substr(ind_cm + 1);
            trim(temp);
        }
    }
}

void J_format(map<string, string> &m_opcode, map<string, char> &m_format, map<string, int> &label_lineno, map<string, string> &alias, string &line, string &command, int lineno, ofstream& myfile,vector<ll>&reg,ll &pc)
{

    string temp = line;
    int i = 0;
    string cmd, rd, funct3, imm, opcode, label;
    if (command.compare("jal") == 0)
    {
        J_format_case(cmd, rd, imm, line, lineno, label_lineno,myfile,label);
    }
    rd = alias[rd];

    if (alias.find(rd) == alias.end() || (int)stoi(rd.substr(1)) > 31)
    {
        cerr << "incorrect register value\n in line " << lineno + 1 << "\n";
        myfile.close();
        exit(0);
    }

    opcode = m_opcode[cmd];

    reg[stoi((alias[rd]).substr(1))]=pc+4;
    pc=4*(label_lineno[label]-1);
    // rd = toBinary(stoi((alias[rd]).substr(1)), 5);
    // imm = toBinary(stoi(imm), 21);
    // string t = imm;
    // reverse(t.begin(), t.end());
    // imm = t.substr(12, 8) + t[11] + t.substr(1, 10) + t[20];
    // reverse(imm.begin(), imm.end());
    // string binenc = imm + rd + opcode;
    // string hexenc = to_hex(binenc);
    // myfile << hexenc<<"\n";
}

void single_line_execute(map<string, string> &m_opcode, map<string, char> &m_format, map<string,int> &m_funct3,map<string, int> &label_lineno, map<string, string> &alias, ofstream& myfile,vector<ll>&reg,ll &pc,vector<string>&instructions,set<ll>&brkpts){
    string newline = instructions[pc/4];
    // cout << newline << ",\n";
    int i=pc/4;
        string firstWord = getFirstWord(newline);
        int lineno = pc/4 + 1;
        if (label_lineno.find(firstWord) == label_lineno.end() && m_opcode.find(firstWord) == m_opcode.end())
        {
            cerr << "Command not found: " << firstWord << " in line " << lineno << endl;
            return ;
        }
        else
        {
            char format = m_format[m_opcode[firstWord]];
            if (format == 'R')
            {
                R_format(m_opcode, m_format, m_funct3, alias, newline, firstWord, lineno,myfile,reg,pc);
            }
            else if (format == 'I')
            {
                I_format(m_opcode, m_format, m_funct3, alias, newline, firstWord, lineno,myfile,reg,pc);
            }
            else if (format == 'S')
            {
                S_format(m_opcode, m_format, m_funct3, alias, newline, firstWord, lineno,myfile,reg,pc);
            }
            else if (format == 'B')
            {
                B_format(m_opcode, m_format, m_funct3, label_lineno, alias, newline, firstWord, i,myfile,reg,pc);
            }
            else if (format == 'J')
            {
                J_format(m_opcode, m_format, label_lineno, alias, newline, firstWord, i,myfile,reg,pc);
            }
            else if (format == 'U')
            {
                U_format(m_opcode, m_format, label_lineno, alias, newline, firstWord, i,myfile,reg,pc);
            }
        }
        trim(newline);
        cout << "Executed " << newline  << " PC = 0x"<<hex<<setw(8)<<setfill('0')<< 4*i << "\n";
        reg[0]=0;
        return;
}
void run(map<string, string> &m_opcode, map<string, char> &m_format, map<string,int> &m_funct3,map<string, int> &label_lineno, map<string, string> &alias, ofstream& myfile,vector<ll>&reg,ll &pc,vector<string>&instructions,set<ll>&brkpts){
    // ll prev_pc=pc;
    do
    {
        single_line_execute(m_opcode, m_format,m_funct3,label_lineno, alias,myfile, reg, pc, instructions, brkpts);
    }while(brkpts.find(pc/4)==brkpts.end());
    if(pc!=4*instructions.size())
    cout<<"Execution stopped at breakpoint\n";
    return;
}
void regs(vector<ll>&reg)
{
    cout<<"Registers\n";
    for(int i=0;i<32;i++)
    {
        cout<<"x"<<dec<<i<<": 0x"<<hex<<setw(16)<<setfill('0')<<reg[i]<<"\n";
    }
    return ;
}
void show_memory(ll addr, ll no_bytes)
{    
    for(int i=0;i<no_bytes;i++)
    {
        cout<<"Memory[0x"<<hex<<setw(5)<<(addr+i)<<"]";
        cout<<" = 0x"<<hex<<mem[addr+i].to_ullong()<<"\n";
    }
}
int main()
{
    map<string, string> m_opcode;
    map<string, char> m_format;
    map<string, int> m_funct3;
    m_opcode["add"] = "0110011";
    m_opcode["sub"] = "0110011";
    m_opcode["and"] = "0110011";
    m_opcode["or"] = "0110011";
    m_opcode["xor"] = "0110011";
    m_opcode["sll"] = "0110011";
    m_opcode["srl"] = "0110011";
    m_opcode["sra"] = "0110011";
    m_opcode["addi"] = "0010011";
    m_opcode["xori"] = "0010011";
    m_opcode["ori"] = "0010011";
    m_opcode["andi"] = "0010011";
    m_opcode["slli"] = "0010011";
    m_opcode["srli"] = "0010011";
    m_opcode["srai"] = "0010011";
    m_opcode["jalr"] = "1100111";

    m_opcode["lb"] = "0000011";
    m_opcode["lh"] = "0000011";
    m_opcode["lw"] = "0000011";
    m_opcode["ld"] = "0000011";
    m_opcode["lbu"] = "0000011";
    m_opcode["lhu"] = "0000011";
    m_opcode["lwu"] = "0000011";

    m_opcode["sb"] = "0100011";
    m_opcode["sh"] = "0100011";
    m_opcode["sw"] = "0100011";
    m_opcode["sd"] = "0100011";

    m_opcode["beq"] = "1100011";
    m_opcode["bne"] = "1100011";
    m_opcode["blt"] = "1100011";
    m_opcode["bge"] = "1100011";
    m_opcode["bltu"] = "1100011";
    m_opcode["bgeu"] = "1100011";
    m_opcode["jal"] = "1101111";

    m_opcode["lui"]="0110111";

    m_format["0110011"] = 'R';
    m_format["0010011"] = 'I';
    m_format["0000011"] = 'I';
    m_format["0100011"] = 'S';
    m_format["1100111"] = 'I';
    m_format["1100011"] = 'B';
    m_format["1101111"] = 'J';
    m_format["0110111"]='U';

    m_funct3["add"] = 0;
    m_funct3["sub"] = 0;
    m_funct3["xor"] = 4;
    m_funct3["or"] = 6;
    m_funct3["and"] = 7;
    m_funct3["sll"] = 1;
    m_funct3["srl"] = 5;
    m_funct3["sra"] = 5;

    m_funct3["addi"] = 0;
    m_funct3["xori"] = 4;
    m_funct3["ori"] = 6;
    m_funct3["andi"] = 7;
    m_funct3["slli"] = 1;
    m_funct3["srli"] = 5;
    m_funct3["srai"] = 5;

    m_funct3["lb"] = 0;
    m_funct3["lh"] = 1;
    m_funct3["lw"] = 2;
    m_funct3["ld"] = 3;
    m_funct3["lbu"] = 4;
    m_funct3["lhu"] = 5;
    m_funct3["lwu"] = 6;

    m_funct3["jalr"] = 0;

    m_funct3["sb"] = 0;
    m_funct3["sh"] = 1;
    m_funct3["sw"] = 2;
    m_funct3["sd"] = 3;

    m_funct3["beq"] = 0;
    m_funct3["bne"] = 1;
    m_funct3["blt"] = 4;
    m_funct3["bge"] = 5;
    m_funct3["bltu"] = 6;
    m_funct3["bgeu"] = 7;
    m_funct3["jal"] = 0;

    map<string, string> alias;
    alias["zero"] = "x0";
    alias["ra"] = "x1";
    alias["sp"] = "x2";
    alias["gp"] = "x3";
    alias["tp"] = "x4";
    alias["s0"] = "x8";
    alias["fp"] = "x8";
    alias["s1"] = "x9";

    for (int i = 0; i < 32; i++)
    {
        alias["x" + to_string(i)] = "x" + to_string(i);
    }

    for (int i = 0; i <= 2; ++i)
    {
        alias["t" + to_string(i)] = "x" + to_string(5 + i);
    }

    for (int i = 0; i <= 7; ++i)
    {
        alias["a" + to_string(i)] = "x" + to_string(10 + i);
    }

    for (int i = 2; i <= 11; ++i)
    {
        alias["s" + to_string(i)] = "x" + to_string(18 + (i - 2));
    }
    for (int i = 3; i <= 6; ++i)
    {
        alias["t" + to_string(i)] = "x" + to_string(28 + (i - 3));
    }
    ofstream myfile;
    myfile.open ("output.hex");
    map<string, int> label_lineno;
    string line;
    vector<string> instructions;
    int lineno = 0;
    vector<ll>reg;
    ll pc=0;
    
    set<ll>brkpts;
    reset(label_lineno,line,instructions,lineno,reg,pc,brkpts);
    // cout<<mem[0x10006];
    while(1){
        string input_line = "";
        string first_command;
        cin>>first_command;
        if(first_command.compare("load") == 0){
            ll memptr=0x10000;
            lineno=0;
            reset(label_lineno,line,instructions,lineno,reg,pc,brkpts);
            string file_name;
            cin>>file_name;
            ifstream infile(file_name);
            string line;
            while(getline(infile,line)){
            int i = 0, n = line.length();
            string word = "";
            while(i < n && line[i] != ' '){
            word += line[i];
            i++;
            }
            if(word.compare(".dword")==0){
            line = line.substr(i);
            vector<int> v;
            stringstream ss(line);            
            for (int i; ss >> i;) {
                v.push_back(i);
                if (ss.peek() == ',')
                    ss.ignore();
            }
            for (size_t i = 0; i < v.size(); i++){
                for(int j=0;j<=7;j++){
                mem[memptr++] = v[i] & (0b11111111);
                v[i] = v[i] >> 8;
                }
            }
            }
            else if(word.compare(".word")==0){
            line = line.substr(i);
            vector<int> v;
            stringstream ss(line);            
            for (int i; ss >> i;) {
                v.push_back(i);
                if (ss.peek() == ',')
                    ss.ignore();
            }
            for (size_t i = 0; i < v.size(); i++){
                for(int j=0;j<=3;j++){
                mem[memptr++] = v[i] & (0b11111111);
                v[i] = v[i] >> 8;
                }
            }
            }
            else if(word.compare(".half")==0){
            line = line.substr(i);
            vector<int> v;
            stringstream ss(line);            
            for (int i; ss >> i;) {
                v.push_back(i);
                if (ss.peek() == ',')
                    ss.ignore();
            }
            for (size_t i = 0; i < v.size(); i++){
                for(int j=0;j<=1;j++){
                mem[memptr++] = v[i] & (0b11111111);
                v[i] = v[i] >> 8;
                }
            }
            }
            else if(word.compare(".byte")==0){
            line = line.substr(i);
            vector<int> v;
            stringstream ss(line);            
            for (int i; ss >> i;) {
                v.push_back(i);
                if (ss.peek() == ',')
                    ss.ignore();
            }
            for (size_t i = 0; i < v.size(); i++){
                for(int j=0;j<=0;j++){
                mem[memptr++] = v[i] & (0b11111111);
                v[i] = v[i] >> 8;
                }
            }
            }
                else if(word.compare(".data")==0 || word.compare(".text")==0){
                continue;
                }
                else if(word.compare(".data")!=0 && line.compare(".text")!=0 &&word.compare(".dword")!=0&&word.compare("")!=0){
                lineno++;
                if (line.find(':') != -1)
                    {
                        int mini = min(line.find(':'), line.find(' '));
                        string label = line.substr(0, mini);
                        line = line.substr(mini + 1);
                        label_lineno[label] = lineno;
                        trim(line);
                    }
                    instructions.push_back(line);
                }
            }
            brkpts.insert(instructions.size());
        }
        else if(first_command.compare("exit") == 0){
            cout << "Exited the Simulator";
            exit(0);
        }
        else if(first_command.compare("run") == 0){
            run(m_opcode, m_format, m_funct3,label_lineno, alias,myfile,reg,pc,instructions,brkpts);
        }
        else if(first_command.compare("regs") == 0){
            regs(reg);
        }
        else if(first_command.compare("mem") == 0){
            string addr;
            cin>>addr;
            ll no_bytes;
            cin>>no_bytes;
            ll address=stoi(addr.substr(2),0,16);
            show_memory(address,no_bytes);
        }
        else if(first_command.compare("step") == 0){
            if(pc/4==instructions.size())
            {
                cout<<"Nothing to step"<<"\n";
                continue;
            }
            single_line_execute(m_opcode, m_format, m_funct3, label_lineno, alias, myfile, reg, pc, instructions, brkpts);
        }
        else if(first_command.compare("show-stack") == 0){
            cout << "show-stack";
        }
        else if(first_command.compare("break") == 0){
            int val;
            cin>>val;
            brkpts.insert(val-1);
            cout<<"Breakpoint set at line "<<dec<<val<<"\n";
        }
        else if(first_command.compare("del") == 0){
            string x;
            cin>>x;
            int line_tbr;
            cin>>line_tbr;
            line_tbr--;
            if(brkpts.find(line_tbr)==brkpts.end())
            {
                cout<<"Breakpoint not present\n";
            }
            else
            brkpts.erase(line_tbr);
        }
        else {
            cout << "Incorrect functionality" << endl;
        }
        cout<<endl;
    }
    return 0;
}