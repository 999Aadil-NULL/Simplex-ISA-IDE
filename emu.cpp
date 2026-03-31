/******************************************************************************
AUTHOR: Shaik Mohammad Aadil
ROLL NUMBER: 2401CS12
COURSE: CS2206 Computer Architecture
DECLARATION OF AUTHORSHIP:
This code represents my original conceptual work, architecture, and design. It was designed and 
implemented to satisfy the requirements of the CS2206 MiniProject. 
I have followed the SIMPLEX ISA specification with the provided marking scheme.
*****************************************************************************/


#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

//Function to dump memory (We can dump total Memory as well just replace 1024 / 64 with memory.size())
void dump_memory(const vector<int>& memory)
 {
    for(int i = 0;i < 1024;i += 4)
    { 
        cout << setw(8) << setfill('0') << hex << uppercase << i << ": ";

        for(int j = 0;j < 4;j++)
         {
            if(i + j < memory.size())
            {
                cout << setw(8) << setfill('0') << hex << uppercase << memory[i + j] << " ";
            }
        }
        cout << endl;
    }
}
//Use this if want it go line wise 
// void dump_memory(const vector<int>& memory)

//  {

//     for(int i = 0; i < 64; i++)

//     {

//         // Dumping first 64 words for visibility

//         cout << setw(4) << setfill('0') << hex << uppercase << i << " : ";

//         cout << setw(8) << setfill('0') << hex << uppercase << memory[i] << endl;

//     }

// }

int main(int argc, char* argv[])
{
    //Default to "main.obj" or accept a specific file from command line becuase back end reads main.obj only 
    string obj_file = (argc > 1) ? argv[1] : "main.obj";

    //Same process of making files like in asm.cpp 
    string base_name = obj_file;
    size_t lastdot = obj_file.find_last_of(".");
    if(lastdot != string::npos) base_name = obj_file.substr(0, lastdot);
    string trace_filename = base_name + ".trace";
    
    ifstream obj(obj_file, ios::binary);
    if(!obj.is_open()) 
    { 
        cerr << "Error: Could not open " << obj_file << endl; 
        return 1; 
    }

    
    // Writing into trace file
    ofstream trace(trace_filename, ios::trunc);
    if(!trace.is_open()) 
    {
        cerr << "Error: Could not create trace file " << trace_filename << endl;
        return 1;
    }

    const int MEM_SIZE = 16384;
    vector<int> memory(MEM_SIZE, 0); 


    int x, program_size = 0;
    while(obj.read((char*)&x, sizeof(int)) && program_size < MEM_SIZE)
    {
        memory[program_size++] = x;
    }
    obj.close();

    if(program_size == MEM_SIZE && obj.peek() != EOF)
    {
        cerr << "Warning: Program size exceeds memory limit. Only first " << MEM_SIZE << " words loaded." << endl;  
    }

    //THE MARKER
    cout << "\n--- SIMPLEX Execution Started ---\n";

    int pc = 0, A = 0, B = 0, sp = MEM_SIZE;
    long long steps = 0;
    bool halted = false;

    //Printing Initial Set of Values
    cout << "--- Initial Values ---\n";
    cout << dec << "A=" << A << " B=" << B;
    cout << " PC=" << hex << pc << " SP=" << sp  << endl;

    //Write into .trace file
    trace << "--- Step-by-Step Trace for " << obj_file << " ---" << endl;
    trace << "Initial: PC:0x" << hex << pc << " A:" << dec << A << " B:" << B << " SP:" << sp << endl;

    while(true)
    {
        if(++steps > 100000)
        {
            cout << " \n--- Infinite Loop detected--- \n";
            trace << "ABORT: Infinite Loop detected at step " << steps << endl;
            halted = true;
            break;
        }
        
        // PC Bounds Check: Ensure the next instruction exists in memory
        if(pc < 0 || pc >= MEM_SIZE) {
            cout << "SEGFAULT: PC (0x" << hex << pc << ") is out of bounds!" << endl;
            trace << "SEGFAULT: PC (0x" << hex << pc << ") out of bounds!" << endl;
            halted = true;
            break; 
        }

        int instr = memory[pc];
        int opcode = instr & 0xFF;
        int operand = instr >> 8;
        if(operand & (1 << 23)) operand |= 0xFF000000;

        int old_pc = pc;
        pc++; 

        //Trace 
        trace << "Step: " << setfill(' ') << setw(6) << dec << steps 
              << " | PC: 0x" << setw(4) << setfill('0') << hex << old_pc 
              << " | A: " << dec << A 
              << " | B: " << B 
              << " | SP: " << sp << endl;

        switch(opcode) 
        {
            case 0: //ldc
                B = A; 
                A = operand; 
                break;
            case 1: //adc
                A += operand; 
                break;
            case 2: //ldl
            {
                int addr = sp + operand;
                if(addr >= 0 && addr < MEM_SIZE)
                {
                    B = A;
                    A = memory[addr]; 
                }
                else 
                {
                    cout << "SEGFAULT at PC=" << hex << old_pc << ": Invalid LDL address 0x" << addr << endl;
                    trace << "SEGFAULT at PC=" << hex << old_pc << ": Invalid LDL address 0x" << addr << endl;
                    return 1;
                }
                break;
            }
            case 3: // stl
            {    if(sp + operand >= 0 && sp + operand < MEM_SIZE) 
                {
                    memory[sp + operand] = A;
                    A = B;
                }
                //In case of overflow
                else 
                {
                    cout << "SEGFAULT at PC=" << hex << old_pc << endl;
                    trace << "SEGFAULT at PC=" << hex << old_pc << endl;
                    return 1;
                }
                break;
            }
            case 4: //ldnl
            {
                int addr = A + operand;
                if(addr >= 0 && addr < MEM_SIZE)
                {
                    A = memory[addr]; 
                }
                else
                {
                    cout << "SEGFAULT at PC=" << hex << old_pc << ": Invalid LDNL address 0x" << addr << endl;
                    trace << "SEGFAULT at PC=" << hex << old_pc << ": Invalid LDNL address 0x" << addr << endl;
                    return 1;
                }
                break;
            }
            case 5: // stnl
            {
                int addr = A + operand;
                if(addr >= 0 && addr < MEM_SIZE)
                {
                    memory[addr] = B;
                } 
                else 
                {
                    cout << "SEGFAULT at PC=" << hex << old_pc << ": Invalid STNL address 0x" << addr << endl;
                    trace << "SEGFAULT at PC=" << hex << old_pc << ": Invalid STNL address 0x" << addr << endl;
                    return 1;
                }
                // cout << " >> [MEM UPDATE] Addr 0x" << hex << (A + operand) << " = " << dec << B << endl;
                break;
            }
            case 6: //Add
                A = B + A; 
                break;
            case 7: //Sub
                A = B - A; 
                break;
             case 8: //shl
                A = B << A;
                break;
            case 9: //shr
                A = B >> A;
                break;
            case 10: //adj
                sp += operand; 
                break;
            case 11: //a2sp
                sp = A;
                A = B;
                break;
            case 12: //sp2a
                B = A;
                A = sp;
                break;
            case 13: //call
                B = A; 
                A = pc;
                pc += operand; 
                break;
            case 14: //return
                pc = A; 
                A = B;
                break;
            case 15: //brz
                if(A == 0) pc += operand; 
                break;
            case 16: //brlz
                if(A < 0) pc += operand;
                break;
            case 17: //br
                pc += operand;
                break;
            case 18: // HALT
                //Final Values of Resgisters and counters.
                cout << "--- Final Values ---\n";
                cout << dec << "A=" << A << " B=" << B;
                cout << " PC=" << hex << pc << " SP=" << sp  << endl;


                //Initial Dump (For Left Pane)
                cout << "--- INITIAL MEMORY DUMP ---\n";
                dump_memory(memory);
                //Final Dump (For Right Pane)
                cout << "\n--- FINAL MEMORY DUMP ---\n";
                halted = true;
                dump_memory(memory);
                trace << "HALT reached at step " << steps << endl;
                trace.close();
                return 0;
        }
    }
    //Code gives a final Memory Dump and Output of registers only if there is a HALT statement in the code
    if(!halted)
    {
        //Initial Dump (For Left Pane)
        cout << "--- INITIAL MEMORY DUMP ---\n";
        dump_memory(memory);
        //Final Values of Resgisters and counters.
        cout << "--- Final Values ---\n";
        cout << dec << "A=" << A << " B=" << B;
        cout << " PC=" << hex << pc << " SP=" << sp  << endl;

        cout << "--- Final Memory Dump ---\n";
        dump_memory(memory);
    }

    trace << "HALT Absent" << endl;
    trace.close();
    return 0;
}