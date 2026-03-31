/******************************************************************************
AUTHOR: Shaik Mohammad Aadil
ROLL NUMBER: 2401CS12
COURSE: CS2206 Computer Architecture
DECLARATION OF AUTHORSHIP:
This code represents my original conceptual work, architecture, and design. It was designed and 
implemented to satisfy the requirements of the CS2206 MiniProject. 
I have followed the SIMPLEX ISA specification with the provided marking scheme.
*****************************************************************************/



#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
using namespace std;
// #define size_t uit

struct inst
{
    int opcode;
    int operand_type;
};
void printHex(int value)
{
    cout << setfill('0') << setw(8) << hex << uppercase << value;
}

void trimmer(string &label)
{
    label.erase(0,label.find_first_not_of(" \t")); // Front Trimming
    label.erase(label.find_last_not_of(" \t") + 1);// Back Trimming
}
//For Valid Label Indentifier
bool label_validator(string &label)
{
    if(label.empty()) return false;

    // first character must be a letter
    if(!isalpha(label[0])) return false;

    // remaining characters must be alphanumeric
    for(char c : label)
    {
        if(!isalnum(c) && c != '_')
            return false;
    }

    return true;
}
//Operand validator
bool operand_validator(string &operand)
{
    if(operand.empty()) return false;
    char *end;
    strtol(operand.c_str(), &end, 0); 
    return (*end == '\0');
}
int main(int argc, char* argv[])
 {
    //Setup Input and Base Names
    string input_file = (argc > 1) ? argv[1] : "main.asm";
    string base_name = input_file;
    size_t lastdot = input_file.find_last_of(".");
    if (lastdot != string::npos) {
        base_name = input_file.substr(0, lastdot);
    }

    //Define specific dynamic filenames
    string lst_filename = base_name + ".lst";
    string log_filename = base_name + ".log";
    string obj_filename = base_name + ".obj";

    ifstream file(input_file);
    if (!file.is_open()) 
    {
        cerr << "Error: Could not open " << input_file << endl;
        return 1;
    }

    vector<string> instruct;
    map<string,inst> instruction;
    map<string,int> label;
    map<string,bool> label_identifier;
    int pc = 0;
    map<int,string> error;
    map<int,string> warning;
    int warning_cnt = 0;
    int error_cnt = 0;
    vector<int> Machine_Code;
    string line;

    //Filling the instruction table
    instruction["ldc"]    = {0,1};
    instruction["adc"]    = {1,1};
    instruction["ldl"]    = {2,1};
    instruction["stl"]    = {3,1};
    instruction["ldnl"]   = {4,1};
    instruction["stnl"]   = {5,1};
    instruction["add"]    = {6,0};
    instruction["sub"]    = {7,0};
    instruction["shl"]    = {8,0};
    instruction["shr"]    = {9,0};
    instruction["adj"]    = {10,1};
    instruction["a2sp"]   = {11,0};
    instruction["sp2a"]   = {12,0};
    instruction["call"]   = {13,2};
    instruction["return"] = {14,0};
    instruction["brz"]    = {15,2};
    instruction["brlz"]   = {16,2};
    instruction["br"]     = {17,2};
    instruction["HALT"]   = {18,0};

    int addr = 0; // Use this to track memory locations (starts at address zero) 
    int line_num = 0; // Use this for error reporting 

    //Single pass
    while (getline(file, line)) 
    {
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        line_num++; // Track every line for error reporting 

        //Removing Comment
        size_t comment = line.find(';');
        if (comment != string::npos) line = line.substr(0, comment);

        //Skipping Blank Lines
        if (line.find_first_not_of(" \t") == string::npos)
        {
            continue;
        }

        //Trimming the sapces in instructions at start and end 
        trimmer(line);
        instruct.push_back(line);

        size_t colon = line.find(':');
        if(colon != string::npos)
        {
            string labl = line.substr(0,colon);
            trimmer(labl);

            string rest = line.substr(colon + 1);
            trimmer(rest);

            string mnemonic, operand;
            stringstream ss(rest);
            ss >> mnemonic >> operand;

            //Hanlding SET instruction 
            if(mnemonic == "SET")
            {
                if(operand_validator(operand))
                {
                    int value = strtol(operand.c_str(), nullptr, 0);

                    if(label_identifier[labl])
                    {
                        error_cnt++;
                        string msg = "Dupliacte Label of ";
                        msg += labl;
                        error[line_num] = msg;
                    }
                    else
                    {
                        label_identifier[labl] = true;
                        label[labl] = value; // SET uses the specific value provided 
                    }
                }
                else
                {
                    error_cnt++;
                    string msg = "Invalid Operand ";
                    error[line_num] = msg;
                }
                // SET does not increment the memory address 
                continue;
            }
            //Check if lLabel is valid
            if(!(label_validator(labl)))
            {
                error_cnt++;
                string msg = "Invalid Label of ";
                msg += labl;
                error[line_num] = msg;
            }
            //Check if Label already exists
            if(label_identifier[labl])
            {
                error_cnt++;
                string msg = "Dupliacte Label of ";
                msg += labl;
                error[line_num] = msg;
            }
            else
            {
                label_identifier[labl] = true;
                label[labl] = addr; //Regular labels point to the current memory address
            }

            //If line is empty (label only) then warning
            if (rest.empty())
            {
                warning_cnt++;
                string msg = "Unused Label of ";
                msg += labl;
                warning[line_num] = msg;
                continue; // Move to next line without incrementing addr
            }
            
            // Re-parse mnemonic from 'rest' to see if addr needs to increment
            line = rest;
        }

        // Only increment memory address if an instruction or 'data' is present 
        stringstream ss_final(line);
        string final_mnemonic;
        ss_final >> final_mnemonic;
        if (!final_mnemonic.empty())
        {
            addr++; 
        }
    }
    file.close();

    //2nd pass 
    addr = 0;      // Current memory address (starts at 0) 
    line_num = 0; // Current file line (for error reporting)
    file.open(input_file);

    while (getline(file, line)) 
    {
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        line_num++;

        //Remove comments
        size_t comment = line.find(';');
        if (comment != string::npos)
        {
            line = line.substr(0, comment);
        }

        //Skip Blank Lines
        if (line.find_first_not_of(" \t") == string::npos)
        {
            continue;
        }

        trimmer(line);

        //Removing Label IF Present 
        size_t colon = line.find(':');
        if(colon != string::npos)
        {
            line = line.substr(colon + 1);
            trimmer(line);

            // If line is empty after label, it generates no code
            if(line.empty())
            {
                Machine_Code.push_back(-1);
                continue;
            }
        }

        //Parsing Instruction
        string mnemonic, operand, extra;
        stringstream ss(line);

        ss >> mnemonic;
        ss >> operand;

        if(ss >> extra)
        {
            error_cnt++;
            string msg = "Extra token instruction ";
            error[line_num] = msg; // Displays error by line
        }
        else
        {
            //Checking mnemonic validity 
            if(instruction.find(mnemonic) == instruction.end() && mnemonic != "data" && mnemonic != "SET")
            {
                error_cnt++;
                error[line_num] = "Unknown instruction: " + mnemonic;
                continue;
            }
            else
            {
                if(mnemonic == "SET")
                {
                    //Already processed in 1st pass; generates no machine code
                    Machine_Code.push_back(-1);
                    continue;
                }
                else if(mnemonic == "data")
                {
                    if(operand_validator(operand))
                    {
                        int value = strtol(operand.c_str(), nullptr, 0);
                        Machine_Code.push_back(value);
                        addr++; // Reserve memory location
                    }
                    else
                    {
                        error_cnt++;
                        string msg = "Invalid Operand ";
                        error[line_num] = msg;
                    }
                }
                else
                {
                    auto Inst = instruction[mnemonic];
                    //Checking Operand requirement
                    if(Inst.operand_type != 0 && operand.empty())
                    {
                        error_cnt++;
                        string msg = "Operand not found";
                        error[line_num] = msg;
                    }
                    else if(Inst.operand_type == 0 && !operand.empty())
                    {
                        error_cnt++;
                        string msg = "Unexpected Operand ";
                        error[line_num] = msg;
                    }
                    else
                    {
                        int value = 0;
                        if(Inst.operand_type == 1) // Non-branch operand 
                        {
                            if(operand_validator(operand))
                            {
                                value = strtol(operand.c_str(), nullptr, 0);
                            }
                            else if(label.find(operand) != label.end())
                            {
                                value = label[operand]; //Use label value directly
                            }
                            else
                            {
                                error_cnt++;
                                error[line_num] = "Invalid Operand " + operand;
                            }
                        }
                        else if(Inst.operand_type == 2) //Branch displacement 
                        {
                            if(label.find(operand) != label.end())
                            {
                                //Calculating displacement
                                value = label[operand] - (addr + 1);
                            }
                            else
                            {
                                error_cnt++;
                                string msg = "Unidentifed Label of " + operand;
                                error[line_num] = msg;
                            }
                        }
                        
                        //Encoding: Opcode in bottom 8 bits,24-bit operand above 
                        int code = ((value & 0xFFFFFF) << 8) | Inst.opcode;

                        Machine_Code.push_back(code);
                        addr++; // Move to next instruction address
                    }
                }
            }
        }
    }
    file.close();


    map<int,string> address_label;
    for(auto &p : label)
    {
        address_label[p.second] = p.first;
    }

    if(!error_cnt)
    {
        //Writing any warnigns if they exist
        ofstream err(log_filename,ios::trunc);
        if(error_cnt == 0)
        {
            err << "No errors found." << endl;

            if(warning_cnt)
            {
                err << endl << "Warnings:" << endl;

                for(auto &p : warning)
                {
                    err << "Line " << p.first << ": " << p.second << endl;
                }
            }
        }
        err << "--- Program Executed without Errors ---" << endl;
        err.close();
        //Writing into OBJ file
        ofstream obj(obj_filename, ios::binary | ios::trunc);
        for(int x : Machine_Code)
        {
            if (x != -1)
            { //Only write actual instructions/data
                obj.write((char*)&x, sizeof(int));
            }
        }
        obj.close();

        //Writing into Listing File
        ofstream lst(lst_filename, ios::trunc);
        lst << "Address   MachineCode   Instruction" << endl;
        lst << "--------------------------------------" << endl;

        int current_addr = 0;
        //We iterate through Machine_Code because it represents our actual memory layout
        for(int i = 0; i < Machine_Code.size(); i++)
        {
            //Print Address (8-character hex)
            lst << setfill('0') << setw(8) << hex << uppercase << current_addr << "\t";
            //Print Machine Code if it's not a label-only line
            if(Machine_Code[i] != -1) 
            {
                lst << setfill('0') << setw(8) << hex << uppercase << Machine_Code[i] << "\t";
                current_addr++; //Only increment address for actual data/instructions
            } 
            else
            {
                lst << "\t\t"; //Leave blank for label-only lines
            }
            //Printing the source instruction text
            if (i < instruct.size())
            {
                lst << " " << instruct[i] << endl;
            } 
            else 
            {
                lst << endl;
            }
        }
        lst.close();
    }
    else
    {
        //Now is the case with errors WE DO NOT produce obj and listing file in this case
        ofstream err(log_filename,ios::trunc);
        if(warning_cnt > 0)
        {
            err << endl << "Warnings:" << endl;

            for(auto &p : warning)
            {
                err << "Line " << p.first << ": " << p.second << endl;
            }
        } 

        err << "Errors found:" << endl;

        for(auto &p : error)
        {
            err << "Line " << p.first << ": " << p.second << endl;
        }

        err.close();

        //Clear the contents in obj and listing file to avoid problems
        ofstream obj(obj_filename, ios::binary | ios::trunc);
        obj.close();
        ofstream lst(lst_filename, ios::trunc);
        lst.close();
    }

    return 0;
}