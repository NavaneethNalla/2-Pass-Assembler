// Name : Navaneeth Nalla
// Roll Number : 2301AI14
// Declaration of authorship : I hereby certify that the source code I am submitting is entirely my own original work except, where otherwise indicated.
#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <set>
#include <map>
#include <vector>
using namespace std;

vector<string> maincode[10000];
vector<string> machine_code(10000);
int intmachinecode[10000];
set<pair<int, string>> errors;
map<string, int> labels;
set<pair<int, string>> warnings;
set<string> labelsused;
map<string, int> no_operand_instr;
map<string, char> bin_hex;
map<string, char> operand_instr;
map<string, int> pcoffset_instr;
void initialize();
int string_to_dec(string s, int no_of_bits);
int validoperand(string s);
string dec_to_2scomplement(int data, int no_of_bits);
void SET_labels();
void printing_warnings();
bool isvalidlabel(string s);
void find_unusedlabels();
void log_file(string filename);
void list_file(string filename);
void obj_file(string filename);
void second_pass();
void first_pass(string s, int line_num, int *p);
void printing_errors();
void process_arguments(int argc, char **argv);
void handle_errors_and_warnings(string file_name);
void generate_machine_code(int instruction_type, string mnemonic, string operand, int itr, string label_to_be_used);
int main(int argc, char **argv)
{
    // Initialize the program and set up instruction formats
    initialize();

    // Process command-line arguments
    if (argc != 2)
    {
        cout << "Error: Incorrect number of arguments. " << endl;
        // Exit the program if the argument format is wrong
    }
    // If file argument is valid, proceed with file processing

    // Open source file for reading

    fstream input_file;
    input_file.open(argv[1], ios::in);
    string file_name = "";

    for (int i = 0; i < strlen(argv[1]); i++)
    {
        int v = 0;

        if (argv[1][i] == '.')
        {

            break;
        }
        file_name += argv[1][i];
    }

    if (input_file.is_open())
    {

        int line_num = 1;
        int pc = 0;
        string cline;

        while (getline(input_file, cline))
        {

            string line_without_comment = cline.substr(0, cline.find(';'));

            first_pass(line_without_comment, line_num, &pc);
            line_num++;
        }

        SET_labels();
        if (errors.empty())
        {
            second_pass();
        }

        find_unusedlabels();
    }

    else
    {

        errors.insert({0, "Error : unable to open the file"});
    }

    handle_errors_and_warnings(file_name);

    input_file.close();

    // Handle any errors or warnings and generate output files

    return 0;
}

// Function to handle errors and warnings after processing
void handle_errors_and_warnings(string file_name)
{
    // Write log file with labels, errors, and warnings
    log_file(file_name);

    // Generate listing file (human-readable format of the code)
    list_file(file_name);

    // If there are no errors, generate object code
    if (errors.empty())
    {
        if (!warnings.empty())
        {
            printing_warnings();
        }
        obj_file(file_name);
    }
    else
    {
        printing_errors();
        if (!warnings.empty())
        {
            printing_warnings();
        }
    }
}

void initialize()
{

    // Initialize instructions without operands
    no_operand_instr["add"] = 6;
    no_operand_instr["sub"] = 7;
    no_operand_instr["shl"] = 8;
    no_operand_instr["shr"] = 9;
    no_operand_instr["a2sp"] = 11;
    no_operand_instr["sp2a"] = 12;
    no_operand_instr["return"] = 14;
    no_operand_instr["HALT"] = 18;

    // Initialize instructions with operands
    operand_instr["ldc"] = 0;
    operand_instr["adc"] = 1;
    operand_instr["ldl"] = 2;
    operand_instr["stl"] = 3;
    operand_instr["ldnl"] = 4;
    operand_instr["stnl"] = 5;
    operand_instr["adj"] = 10;
    operand_instr["call"] = 13;
    operand_instr["brz"] = 15;
    operand_instr["brlz"] = 16;
    operand_instr["br"] = 17;
    operand_instr["SET"] = 19;
    operand_instr["data"] = 20;

    // Initialize instructions with PC-relative offset
    pcoffset_instr["call"] = 13;
    pcoffset_instr["brz"] = 15;
    pcoffset_instr["brlz"] = 16;
    pcoffset_instr["br"] = 17;

    // Map binary strings to hex digits
    bin_hex["0000"] = '0';
    bin_hex["0001"] = '1';
    bin_hex["0010"] = '2';
    bin_hex["0011"] = '3';
    bin_hex["0100"] = '4';
    bin_hex["0101"] = '5';
    bin_hex["0110"] = '6';
    bin_hex["0111"] = '7';
    bin_hex["1000"] = '8';
    bin_hex["1001"] = '9';
    bin_hex["1010"] = 'A';
    bin_hex["1011"] = 'B';
    bin_hex["1100"] = 'C';
    bin_hex["1101"] = 'D';
    bin_hex["1110"] = 'E';
    bin_hex["1111"] = 'F';
}

int validoperand(string str)
{

    if (labels.find(str) != labels.end())
    {

        return 1;
    } // 1 - label , 2 - invalid

    for (char ch : str)
    {

        if (!(ch >= 'a' and ch <= 'f') and !(ch >= 'A' and ch <= 'F') and !isdigit(ch))
        {

            return 2;
        }
    }
}

bool isvalidlabel(string str)
{

    if (isdigit(str[0]))
    {

        return false;
    }

    for (char ch : str)
    {

        if (!isalpha(ch) and !isdigit(ch) and ch != '_')
        {

            return false;
        }
    }

    return true;
}

int string_to_dec(string s, int no_of_bits)
{

    if (no_of_bits == 32 or no_of_bits == 24)
    {

        if (s == "0")
        {

            return 0;
        }

        int len = s.length();
        int a = 0;
        int val = 0;

        // if string is a existing label
        if (validoperand(s) == 1)
        {

            return a = labels[s];
        }

        if (s[0] == '-' or s[0] == '+')
        {

            // string is octal
            if (s[1] == '0')
            {

                for (int i = 2; i < s.length(); i++)
                {
                    char ch = s[i];

                    if (!(ch >= '0' and ch <= '7'))
                    {

                        a = 1;
                    }
                }

                if (a == 0)
                {

                    int m = 1;

                    int i = len - 1;
                    while (i >= 2)
                    {

                        i = i - 1;
                        val += ((s[i] - '0') * m);
                        m *= 8;
                    }

                    if (s[0] == '-')
                    {

                        val *= -1;
                    }
                }

                else
                {
                    errors.insert({0, "Not a valid Number Format, Use Suitable Prefix For hex = '0x', oct = '0'"});
                }
            }

            else
            {

                // string is decimal
                int b = 0;

                for (int i = 1; i < s.length(); i++)
                {

                    if (!(s[i] >= '0' and s[i] <= '9'))
                    {

                        b = 1;
                    }
                }

                if (b == 0)
                {

                    val = stoi(s.substr(1, len - 1));

                    if (s[0] == '-')
                    {

                        val *= -1;
                    }
                }
                else
                {
                    errors.insert({0, "Not a valid Number Format,Use Suitable Prefix For hex = '0x', oct = '0'"});
                }
            }

            return val;
        }

        // string is hexadecimal

        else if (s[0] == '0' && s[1] == 'x')
        {

            int c = 0;
            char ch;

            for (int i = 2; i < s.length(); i++)
            {

                ch = s[i];

                if (!((ch >= '0' and s[i] <= '9') or (ch >= 'a' and ch <= 'f') or (ch >= 'A' and ch <= 'F')))
                {

                    c = 1;
                }
            }

            if (c == 0)
            {

                string s1 = s.substr(2, len - 2);
                int i = 0;
                while (s1.length() < (no_of_bits) / 4)
                {
                    i++;
                    s1.insert(0, "0");
                }
                stringstream ss;
                unsigned int x;
                ss << std::hex << s1;
                ss >> x;
                int j;
                j = static_cast<int>(x);
                val = j;
            }
            else
            {
                errors.insert({0, "Not a valid Number Format,Use Suitable Prefix For hex = '0x', oct = '0'"});
            }
        }

        else if (validoperand(s) == 2)
        {

            errors.insert({0, "Not a valid input"});
            return 0;
        }

        else if (s[0] == '0')
        {

            int d = 0;
            char chh;

            for (int i = 1; i < s.length(); i++)
            {

                chh = s[i];

                if (!(chh >= '0' and chh <= '7'))
                {

                    d = 1;
                }
            }

            // string is octal
            if (d == 0)
            {

                int m = 1;

                int i = len - 1;
                while (i >= 1)
                {

                    i = i - 1;
                    val += ((s[i] - '0') * m);
                    m *= 8;
                }
            }
            else
            {
                errors.insert({0, "Not a valid Number Format,Use Suitable Prefix For hex = '0x', oct = '0'"});
            }
        }

        else
        {

            // string is decimal
            int e = 0;
            for (char ch : s)
            {

                if (!isdigit(ch))
                {

                    e = 1;
                }
            }
            if (e == 0)
            {

                val = stoi(s);
            }
            else
            {

                errors.insert({0, "Not a valid Number Format,Use Suitable Prefix For hex = '0x', oct = '0'"});
            }
        }
        return val;
    }

    else
    {

        // string is mnemonic
        int val = 0;
        if (no_operand_instr.count(s))
        {

            val = no_operand_instr[s];
        }

        else if (operand_instr.count(s))
        {

            val = operand_instr[s];
        }
        return val;
    }

    return 0;
}

void first_pass(string s, int linenum, int *pc)
{
    if (!s.empty())
    {
        string s1;

        for (char ch : s)
        {
            s1 += ch;
            if (ch == ':')
            {
                s1 += ' ';
            }
        }

        string cont_words[5] = {"", "", "", "", ""};
        stringstream s2(s1);
        int i = 0;
        int len_words[5] = {0, 0, 0, 0, 0};

        while (s2 >> cont_words[i])
        {
            len_words[i] = cont_words[i].length();
            i++;
        }

        for (int i = 0; i < 5; i++)
        {
            if (!cont_words[i].empty())
            {
                maincode[(*pc)].push_back(cont_words[i]);
            }
        }

        // Check if there's a label (ending with ':')
        if (cont_words[0].back() == ':')
        {
            string label = cont_words[0].substr(0, len_words[0] - 1);

            bool invalidLabelSyntax = false;

            // Check if there are extra colons inside the label
            for (int i = len_words[0]; i < s1.length(); i++)
            {
                if (s1[i] == ':')
                {
                    invalidLabelSyntax = true;
                    break;
                }
            }

            if (invalidLabelSyntax)
            {
                errors.insert({linenum, "Invalid syntax: Label contains extra colons"});
            }
            else if (!isvalidlabel(label))
            {
                errors.insert({linenum, "Invalid label name: Must be alphanumeric and start with an alphabet"});
            }
            else
            {

                if (labels.count(label) > 0)
                {
                    errors.insert({linenum, "Duplicate label found"});
                }
                else
                {
                    // Insert label into label table
                    labels.insert({label, *pc});

                    if (!(len_words[1] == 0 and len_words[2] == 0))
                    {

                        (*pc)++;
                    }
                }
            }

            // Process mnemonic and operands if present
            if (!cont_words[1].empty() && cont_words[1].back() != ':')
            {
                string mnemonic = cont_words[1];

                // Instructions without operands
                if (no_operand_instr.count(mnemonic))
                {
                    if (!cont_words[2].empty())
                    {
                        errors.insert({linenum, "Unexpected operand: No operand allowed"});
                    }
                }
                // Instructions  with one operand
                else if (operand_instr.count(mnemonic))
                {
                    if (!cont_words[3].empty())
                    {
                        errors.insert({linenum, "Unexpected operand: Only one operand allowed"});
                    }
                    else if (cont_words[2].empty())
                    {
                        errors.insert({linenum, "No operand provided"});
                    }
                }
                // Instructions  with PC-relative offset
                else if (pcoffset_instr.count(mnemonic))
                {
                    if (!isvalidlabel(cont_words[2]))
                    {
                        errors.insert({linenum, "Invalid operand"});
                    }
                }
                else if (mnemonic == "data" or mnemonic == "SET")
                {
                }
                else
                {
                    // Invalid instruction
                    errors.insert({linenum, "No such  instruction"});
                }
            }
        }
        else
        {
            // Handle case without label, just mnemonic and operands
            string mnemonic = cont_words[0];

            // Instr without operands
            if (no_operand_instr.count(mnemonic))
            {
                if (!cont_words[1].empty())
                {
                    errors.insert({linenum, "Unexpected operand: No operand allowed"});
                }
            }
            // Instr with one operand
            else if (operand_instr.count(mnemonic))
            {
                if (!cont_words[2].empty())
                {
                    errors.insert({linenum, "Unexpected operand: Only one operand allowed"});
                }
                else if (cont_words[1].empty())
                {
                    errors.insert({linenum, "No operand provided"});
                }
            }
            // Instr with PC-relative offset
            else if (pcoffset_instr.count(mnemonic))
            {
                if (!isvalidlabel(cont_words[1]))
                {
                    errors.insert({linenum, "Invalid operand"});
                }
            }
            else if (mnemonic == "data" or mnemonic == "SET")
            {
            }
            else
            {
                // Invalid instruction
                errors.insert({linenum, "No such instruction"});
            }

            (*pc)++;
        }
    }
}

string dec_to_2scomplement(int data, int num_bits)
{

    string bin;

    if (data >= 0)
    {
        bin = bitset<32>(data).to_string(); // Generate a 32-bit binary representation
        bin = bin.substr(32 - num_bits);
    }

    else
    {
        data = data * (-1);
        bin = bitset<32>(data).to_string(); // Generate a 32-bit binary representation
        bin = bin.substr(32 - num_bits);
        // invert the bits
        for (int i = 0; i < num_bits; ++i)
        {
            bin[i] = (bin[i] == '0') ? '1' : '0';
        }

        // add 1

        for (int i = num_bits - 1; i >= 0; --i)
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

    for (int i = 0; i < num_bits; i = i + 4)
    {

        string part = bin.substr(i, 4);
        hex_string += bin_hex[part];
    }
    return hex_string;
}

void second_pass()
{
    for (int i = 0; i < 100000; ++i)
    {
        if (maincode[i].empty())
        {
            break;
        }

        if (maincode[i].size() == 1)
        {
            generate_machine_code(0, maincode[i][0], "", i, "");
            continue;
        }

        string mnemonic, operand;
        bool l1 = false, l2 = false;

        string s1 = maincode[i][0];
        string s2 = maincode[i][1];

        if (s1.back() == ':')
        {
            l1 = true;
        }

        if (s2.back() == ':')
        {
            l2 = true;
        }

        if (l1 && l2)
        {

            mnemonic = maincode[i][2];
            int sizee;
            sizee = maincode[i].size();
            if (no_operand_instr.count(mnemonic))
            {
                generate_machine_code(0, mnemonic, "", i, "");
            }

            else
            {
                if (sizee >= 4)
                {
                    operand = maincode[i][3];
                }

                if (isvalidlabel(operand) && labels.count(operand))
                {
                    labelsused.insert(operand);
                }
                generate_machine_code(1, mnemonic, operand, i, s2.substr(0, s2.length() - 1));
            }
        }

        else if (l1 && !l2)
        {
            mnemonic = maincode[i][1];

            int sizee;
            sizee = maincode[i].size();

            if (no_operand_instr.count(mnemonic))
            {
                generate_machine_code(0, mnemonic, "", i, "");
            }
            else
            {
                if (sizee >= 3)
                {
                    operand = maincode[i][2];
                }

                if (isvalidlabel(operand) && labels.count(operand))
                {
                    labelsused.insert(operand);
                }
                generate_machine_code(1, mnemonic, operand, i, s1.substr(0, s1.length() - 1));
            }
        }
        // Case 3: No label, just mnemonic and operand
        else
        {
            mnemonic = maincode[i][0];

            int sizee;

            sizee = maincode[i].size();

            if (no_operand_instr.count(mnemonic))
            {
                generate_machine_code(0, mnemonic, "", i, "");
            }
            else
            {
                if (maincode[i].size() >= 2)
                {
                    operand = maincode[i][1];
                }

                if (isvalidlabel(operand) && labels.count(operand))
                {
                    labelsused.insert(operand);
                }
                generate_machine_code(1, mnemonic, operand, i, "");
            }
        }
    }
}

void generate_machine_code(int instrType, string mnemonic, string operand, int itr, string label_to_be_used)
{

    int value = 0;

    // Case when the instruction doesn't have an operand
    if (instrType == 0)
    {
        intmachinecode[itr] = no_operand_instr[mnemonic];

        machine_code[itr] = dec_to_2scomplement(no_operand_instr[mnemonic], 32);
    }
    // Case when the instruction has an operand
    else if (instrType == 1)
    {

        // Handle specific cases like 'data' or 'SET'
        if (mnemonic == "data" || mnemonic == "SET")
        {
            labelsused.insert(label_to_be_used);
            value = string_to_dec(operand, 32);
            intmachinecode[itr] = value;
            machine_code[itr] = dec_to_2scomplement(value, 32);
        }
        else
        {

            int opcodeValue = string_to_dec(mnemonic, 8);
            int operandValue = string_to_dec(operand, 24);

            if (pcoffset_instr.find(mnemonic) != pcoffset_instr.end())
            {
                if (isvalidlabel(operand))
                {

                    operandValue = (operandValue - itr) - 1;
                }
            }

            // Construct the full machine code
            intmachinecode[itr] = (operandValue << 8) | opcodeValue;
            machine_code[itr] = dec_to_2scomplement(operandValue, 24) + dec_to_2scomplement(opcodeValue, 8);
        }
    }
}

void SET_labels()
{

    for (int i = 0; i < 100000; ++i)
    {

        if (maincode[i].empty())
        {
            break;
        }

        if (maincode[i].size() == 3 && maincode[i][1] == "SET")
        {

            string label = maincode[i][0];
            if (label.back() == ':')
            {
                label.pop_back();
            }

            if (labels.find(label) != labels.end())
            {

                labels[label] = string_to_dec(maincode[i][2], 32);
            }
        }

        else if (maincode[i].size() == 4 && maincode[i][2] == "SET")
        {

            string label = maincode[i][1];
            if (label.back() == ':')
            {
                label.pop_back();
            }

            if (labels.find(label) != labels.end())
            {

                labels[label] = string_to_dec(maincode[i][3], 32);
            }
        }
    }
}

void find_unusedlabels()
{
    for (const auto &label_s : labels)
    {

        string label_name = label_s.first;

        if (!labelsused.count(label_name))
        {

            warnings.insert({0, "An unused label"});
        }
    }
}

void printing_warnings()
{

    for (const auto &war : warnings)
    {
        cout << "Line " << war.first << " : " << war.second << "\n";
    }
    cout << "\n";
}

void printing_errors()
{

    for (const auto &err : errors)
    {
        cout << "Line " << err.first << " : " << err.second << "\n";
    }
    cout << "\n";
}

void log_file(string filename)
{

    fstream log_fptr;
    log_fptr.open(filename + ".log", ios::out);

    if (log_fptr.is_open())
    {
        log_fptr << "LabelName => Value\n";
        for (const auto &str : labels)
        {
            log_fptr << str.first << " => " << str.second << "\n";
        }

        log_fptr << "\nUsedLabelName\n";
        for (const auto &used_label : labelsused)
        {
            if (!used_label.empty())
            {
                log_fptr << used_label << "\n";
            }
        }

        log_fptr << "\nWarnings \n";
        for (const auto &warn : warnings)
        {
            log_fptr << "Line " << warn.first << " : " << warn.second << "\n";
        }

        log_fptr << "\nErrors \n";
        for (const auto &err : errors)
        {
            log_fptr << "Line " << err.first << " : " << err.second << "\n";
        }
    }
    log_fptr.close();
    return;
}

void list_file(string filename)
{

    fstream list_fptr;
    list_fptr.open(filename + ".lst", ios::out);

    if (list_fptr.is_open())
    {
        for (int i = 0; i < 100000; i++)
        {
            if (maincode[i].empty())
            {
                break;
            }

            list_fptr << dec_to_2scomplement(i, 32) << " ";

            list_fptr << machine_code[i] << " ";

            for (int j = 0; j < maincode[i].size(); j++)
            {
                list_fptr << maincode[i][j] << " ";
            }
            list_fptr << "\n";
        }
    }
    list_fptr.close();

    return;
}

void obj_file(string filename)
{
    int p = 0;
    FILE *obj_fptr;

    filename = filename + ".o";
    obj_fptr = fopen(filename.c_str(), "wb");

    for (int i = 0; i < 100000; i++)
    {
        if (maincode[i].empty())
        {

            break;
        }

        p = p + 1;
    }
    fwrite(intmachinecode, sizeof(int), p, obj_fptr);

    fclose(obj_fptr);
    return;
}
