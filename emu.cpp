#include <bits/stdc++.h>
using namespace std;
int pc = 0;
int sp = 0;
int regA = 0;
int regB = 0;
string filename = "";
string option = "";
size_t size;
int integer_machineCode[100000];
vector<string> machinecode(100000);
vector<string> machinecode2(100000);
map<int, string> no_operand_instr;
map<int, string> operand_instr;
map<int, string> pcoffset_instr;
int inst_count = 0;
set<pair<int, pair<string, string>>> isa;
map<string, int> bin_hex;
void initial();
string dec_to_2scomp(int code, int bits);
int str_dec(string code, int bits);
int str_dec24(string code);
void exe();
int main(int argc, char **argv)
{
    if (argc != 3)
    {
        cout << "Enter in correct format: .\\emu options filename.o" << endl;
        cout << "-isa   ISA(Instruction Set Architecture)" << endl;
        cout << "-trace    trace of the code" << endl;
        cout << "-write    memory writes" << endl;
        cout << "-wipe   wipe written flags before execution" << endl;
        cout << "-before    memory dump before execution" << endl;
        cout << "-after    memory dump after execution" << endl;
        cout << "-read     memory writes" << endl;
        return 0;
    }
    initial();
    for (int i = 0; i < strlen(argv[2]); i++)
    {
        if (argv[2][i] == '.')
        {
            break;
        }
        filename += argv[2][i];
    }
    for (int i = 0; i < strlen(argv[1]); i++)
    {
        option += argv[1][i];
    }
    FILE *fptr = nullptr;
    string f_name = filename;
    f_name += ".o";
    fptr = fopen(f_name.c_str(), "rb");
    if (fptr != nullptr)
    {
        size = fread(integer_machineCode, sizeof(int), 100000, fptr);
        for (int i = 0; i < 1000; i++)
        {
            string wse = dec_to_2scomp(integer_machineCode[i], 32);
            machinecode[i] = wse;
        }
        if (option == "-wipe")
        {
            cout << "Program_Counter(PC), Stake_Pointer(SP), Reg A, Reg B are reset\n";
            regA = 0;
            regB = 0;
            pc = 0;
            sp = 0;
        }
        else if (option == "-before")
        {
            cout << "Memory dump before code execution \n";
            for (int i = 0; i < 50; i++)
            {
                if (!(i % 4))
                {
                    cout << dec_to_2scomp(i, 32) << " " << machinecode[i] << " ";
                }
                else
                {
                    cout << machinecode[i] << " ";
                }
                if (i % 4 == 3)
                {
                    cout << endl;
                }
            }
            cout << "\n\n";
        }
        else if (option == "-isa")
        {
            cout << "\nOpcode Mnemonic Operand\n";
            for (auto i = isa.begin(); i != isa.end(); i++)
            {
                cout << i->first << "   " << i->second.first << "   " << i->second.second << endl;
            }
            cout << "       " << "SET" << "     value\n\n";
        }
        exe();
        for (int i = 0; i < 1000; i++)
        {
            string wse = dec_to_2scomp(integer_machineCode[i], 32);
            machinecode2[i] = wse;
        }
        if (option == "-trace")
        {
            cout << "Program Counter: " << dec_to_2scomp(pc, 32) << " Stack Pointer: " << dec_to_2scomp(sp, 32) << " Register A: " << dec_to_2scomp(regA, 32) << " Register B: " << dec_to_2scomp(regB, 32) << " Instruction Count: " << inst_count << endl;
        }
        else if (option == "-after")
        {
            cout << "Memory dump after code execution\n\n";
            for (int i = 0; i < 50; i++)
            {
                if (!(i % 4))
                {
                    cout << dec_to_2scomp(i, 32) << " " << machinecode2[i] << " ";
                }
                else
                {
                    cout << machinecode2[i] << " ";
                }
                if (i % 4 == 3)
                {
                    cout << endl;
                }
            }
            cout << "\n\n";
        }
        if (inst_count == size && option == "-trace")
            cout << "\nProgram Executed\n"
                 << endl;
        fclose(fptr);
    }
    else
    {
        cout << filename << " doesn't exist" << endl;
    }
    return 0;
}
void exe()
{
    for (pc = 0; pc < size; pc++)
    {
        inst_count++;
        int opcode = str_dec(machinecode[pc].substr(6, 2), 8);
        int operand = str_dec24(machinecode[pc].substr(0, 6));
        if (no_operand_instr.find(opcode) != no_operand_instr.end())
        {
            if (option == "-trace")
            {
                cout << "Program Counter: " << dec_to_2scomp(pc, 32) << " Stack Pointer: " << dec_to_2scomp(sp, 32) << " Register A: " << dec_to_2scomp(regA, 32) << " Register B: " << dec_to_2scomp(regB, 32) << " Instruction Count: " << inst_count << endl;
            }
            if (opcode == 6)
            {
                regA = regB + regA;
            }
            else if (opcode == 7)
            {
                regA = regB - regA;
            }
            else if (opcode == 8)
            {
                regA = (regB << regA);
            }
            else if (opcode == 9)
            {
                regA = (regB >> regA);
            }
            else if (opcode == 11)
            {
                sp = regA;
                regA = regB;
            }
            else if (opcode == 12)
            {
                regB = regA;
                regA = sp;
            }
            else if (opcode == 14)
            {
                pc = regA;
                regA = regB;
            }
            else if (opcode == 18)
            {
                if (option == "-trace")
                    ;
                cout << "\nProgram Execution Finished\n";
                return;
            }
            else
            {
                cout << "\nInvalid instruction, Execution stopped\n";
                return;
            }
        }
        else if (operand_instr.find(opcode) != operand_instr.end())
        {
            if (option == "-trace")
            {
                cout << "Program Counter: " << dec_to_2scomp(pc, 32) << " Stack Pointer: " << dec_to_2scomp(sp, 32) << " Register A: " << dec_to_2scomp(regA, 32) << " Register B: " << dec_to_2scomp(regB, 32) << " Instruction Count: " << inst_count << endl;
            }
            if (pcoffset_instr.find(opcode) != pcoffset_instr.end())
            {
                if (opcode == 13)
                {
                    regB = regA;
                    regA = pc;
                    pc = pc + operand;
                }
                else if (opcode == 15)
                {
                    if (regA == 0)
                    {
                        pc = pc + operand;
                    }
                }
                else if (opcode == 16)
                {
                    if (regA < 0)
                    {
                        pc = pc + operand;
                    }
                }
                else if (opcode == 17)
                {
                    pc = pc + operand;
                }
                else
                {
                    cout << "\nInvalid instruction, Execution stopped\n";
                    return;
                }
            }
            else
            {
                if (opcode == 0)
                {
                    regB = regA;
                    regA = operand;
                }
                else if (opcode == 1)
                {
                    regA = regA + operand;
                }
                else if (opcode == 2)
                {
                    regB = regA;
                    if (sp + operand < 0 || sp + operand > 100000)
                    {
                        if (option == "-trace")
                        {
                            cout << "Error: Invalid Memeory Access\n";
                        }
                        return;
                    }
                    regA = integer_machineCode[sp + operand];
                    if (option == "-read")
                    {
                        cout << "Reading Memory " << dec_to_2scomp(sp + operand, 32) << " finds " << dec_to_2scomp(regA, 32) << endl;
                    }
                }
                else if (opcode == 3)
                {
                    if (sp + operand < 0 || sp + operand > 100000)
                    {
                        if (option == "-trace")
                        {
                            cout << "Error: Invalid Memory Access\n";
                        }
                        return;
                    }
                    if (option == "-write")
                    {
                        cout << "Writing Memeory " << dec_to_2scomp(sp + operand, 32) << " was " << dec_to_2scomp(integer_machineCode[sp + operand], 32);
                    }
                    integer_machineCode[sp + operand] = regA;
                    if (option == "-write")
                    {
                        cout << " now " << dec_to_2scomp(regA, 32) << endl;
                    }
                    regA = regB;
                }
                else if (opcode == 4)
                {
                    if (regA + operand < 0 || regA + operand > 100000)
                    {
                        if (option == "-trace")
                        {
                            cout << "Error: Invalid Memory Access\n";
                            return;
                        }
                    }
                    if (option == "-read")
                    {
                        cout << "Reading Memeory " << dec_to_2scomp(regA + operand, 32) << "finds, " << dec_to_2scomp(integer_machineCode[regA + operand], 32) << endl;
                    }
                    regA = integer_machineCode[regA + operand];
                }
                else if (opcode == 5)
                {
                    if (regA + operand < 0 || regA + operand > 100000)
                    {
                        if (option == "-trace")
                        {
                            cout << "Error: Invalid Memory Access\n";
                            return;
                        }
                    }
                    if (option == "-write")
                    {
                        cout << "Writing Memeory " << dec_to_2scomp(regA + operand, 32) << " was " << dec_to_2scomp(integer_machineCode[regA + operand], 32);
                    }
                    integer_machineCode[regA + operand] = regB;
                    if (option == "-write")
                    {
                        cout << "now " << dec_to_2scomp(regB, 32) << endl;
                    }
                }
                else if (opcode == 10)
                {
                    sp = sp + operand;
                }
                else
                {
                    cout << "Invalid instruction, Execution stopped\n";
                    return;
                }
            }
        }
        if (pc >= size)
        {
            if (option == "-trace")
            {
                cout << "Error: Invlaid Memeory Access\n";
            }
            return;
        }
    }
}
int str_dec(string code, int bits)
{
    unsigned int y;
    string s1 = code;
    stringstream ss;
    ss << hex << s1;
    ss >> y;
    return static_cast<int>(y);
}
string dec_to_2scomp(int val, int no_of_bits)
{
    string bin;
    if (val >= 0)
    {
        bin = bitset<32>(val).to_string(); // Generate a 32-bit binary representation
        bin = bin.substr(32 - no_of_bits);
    }
    else
    {
        val = val * (-1);
        bin = bitset<32>(val).to_string(); // Generate a 32-bit binary representation
        bin = bin.substr(32 - no_of_bits);
        for (int i = 0; i < no_of_bits; i++)
        {
            bin[i] = (bin[i] == '0') ? '1' : '0';
        }
        for (int i = no_of_bits - 1; i >= 0; i--)
        {
            if (bin[i] == '0')
            {
                bin[i] = '1';
                break;
            }
            bin[i] = '0';
        }
    }
    string hex_string;

    hex_string = "";

    for (int i = 0; i < no_of_bits; i = i + 4)
    {
        string part = bin.substr(i, 4);
        hex_string += bin_hex[part];
    }
    return hex_string;
}
int str_dec24(string code)
{
    unsigned int y;
    string s1 = code;
    if (s1[0] == '8' || s1[0] == '9' || (s1[0] >= 'A' && s1[0] <= 'F'))
    {
        while (s1.length() < 8)
        {
            s1.insert(0, "F");
        }
    }
    else
    {
        while (s1.length() < 8)
        {
            s1.insert(0, "0");
        }
    }
    stringstream ss;
    ss << hex << s1;
    ss >> y;
    return static_cast<int>(y);
}
void initial()
{
    no_operand_instr = {
        {6, "add"},
        {7, "sub"},
        {8, "shl"},
        {9, "shr"},
        {11, "a2sp"},
        {12, "sp2a"},
        {14, "return"},
        {18, "HALT"}};

    operand_instr = {
        {0, "ldc"},
        {1, "adc"},
        {2, "ldl"},
        {3, "stl"},
        {4, "ldnl"},
        {5, "stnl"},
        {10, "adj"},
        {13, "call"},
        {15, "brz"},
        {16, "brlz"},
        {17, "br"},
        {19, "SET"},
        {20, "data"}};

    pcoffset_instr = {
        {13, "call"},
        {15, "brz"},
        {16, "brlz"},
        {17, "br"}};

    isa.insert({0, {"ldc", "value"}});
    isa.insert({1, {"adc", "value"}});
    isa.insert({2, {"ldl", "value"}});
    isa.insert({3, {"stl", "value"}});
    isa.insert({4, {"ldnl", "value"}});
    isa.insert({5, {"ldnl", "value"}});
    isa.insert({6, {"add", ""}});
    isa.insert({7, {"sub", ""}});
    isa.insert({8, {"shl", ""}});
    isa.insert({9, {"shr", ""}});
    isa.insert({10, {"adj", "value"}});
    isa.insert({11, {"a2sp", ""}});
    isa.insert({12, {"sp2a", ""}});
    isa.insert({13, {"call", "offset"}});
    isa.insert({14, {"return", ""}});
    isa.insert({15, {"brz", "offset"}});
    isa.insert({16, {"brlz", "offset"}});
    isa.insert({17, {"br", "offset"}});
    isa.insert({18, {"HALT", ""}});

    bin_hex = {
        {"0000", '0'},
        {"0001", '1'},
        {"0010", '2'},
        {"0011", '3'},
        {"0100", '4'},
        {"0101", '5'},
        {"0110", '6'},
        {"0111", '7'},
        {"1000", '8'},
        {"1001", '9'},
        {"1010", 'A'},
        {"1011", 'B'},
        {"1100", 'C'},
        {"1101", 'D'},
        {"1110", 'E'},
        {"1111", 'F'}};
    return;
}