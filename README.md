# SIMPLEX ISA Toolchain & Web IDE

**Author:** Shaik Mohammad Aadil
**Project Status:** 🚀 Live & Deployed

## 🌐 Live Demo

Access the full web-based integrated development environment here:
> **IDE Link:** [https://simplex-isa-ide.onrender.com/](https://simplex-isa-ide.onrender.com/)

---

## 🚀 Overview

This project provides a comprehensive, full-stack implementation of the **SIMPLEX Instruction Set Architecture**. It bridges the gap between low-level assembly logic and modern software delivery by providing a complete toolchain from assembly to execution.

The toolchain includes:
- A **Two-Pass Assembler** written in C++ for robust label resolution and binary generation.
- A **Hardware Emulator** that simulates a 4-register machine with strict memory protection and step-by-step tracing.
- A **Web IDE** built with Node.js and the Monaco Editor, providing a professional "code and run" experience directly in your browser.

The system is architected for high reliability, utilizing a **unique request ID system** in the backend. This allows multiple concurrent users to assemble and emulate code simultaneously without any data collisions.

---

## 🛠️ System Components

### 1. Two-Pass Assembler (`asm.cpp`)
The assembler transforms human-readable assembly into machine code using a classic 2-pass strategy:
- **Pass 1:** Scans source code to identify labels, calculate memory addresses, and resolve the `SET` pseudo-instruction.
- **Pass 2:** Generates the final 32-bit machine code (24-bit operand, 8-bit opcode).
- **Diagnostics:** Full error reporting for duplicate labels, invalid mnemonics, missing operands, and syntax errors.
- **Outputs:** Generates `.obj` (binary), `.lst` (listing with mnemonics), and `.log` (build reports).

### 2. Hardware Emulator (`emu.cpp`)
A cycle-accurate simulator for the SIMPLEX architecture:
- **Registers:** A (Accumulator), B (Internal Stack), PC (Program Counter), and SP (Stack Pointer).
- **Integrity:** Implements SEGFAULT triggers for out-of-bounds memory access and PC overflows.
- **Monitoring:** Generates a detailed `.trace` file for every execution step, logging register transitions ($A$, $B$, $PC$, $SP$) and memory updates.
- **Execution Control:** Automatic infinite loop detection (halts after 100,000 steps).

### 3. Web IDE (`server.js` & `index.ejs`)
- **Editor:** Integrated Monaco Editor (the core of VS Code) providing high-performance text manipulation and syntax highlighting.
- **Integrated Pipeline:** The Node.js backend manages the C++ compilation and execution lifecycle automatically.
- **Triple Output View:** Simultaneously view the **Assembly Listing**, **Step-by-Step Trace**, and **Emulator Console STDOUT**.

---

## 📖 SIMPLEX ISA Reference

The machine uses a 32-bit word size. Instructions are encoded with the **bottom 8 bits as the opcode** and the **upper 24 bits as the signed operand**.

| Mnemonic | Opcode | Operand | Formal Specification | Description |
| :--- | :--- | :--- | :--- | :--- |
| **ldc** | 0 | value | $B:=A; A:=$ value | Load constant |
| **adc** | 1 | value | $A:=A + value$ | Add constant |
| **ldl** | 2 | offset | $B:=A; A:=mem[SP+off]$ | Load local |
| **stl** | 3 | offset | $mem[SP+off]:=A; A:=B$ | Store local |
| **ldnl** | 4 | offset | $A:=mem[A+off]$ | Load non-local |
| **stnl** | 5 | offset | $mem[A+off]:=B$ | Store non-local |
| **add** | 6 | - | $A:=B+A$ | Addition |
| **sub** | 7 | - | $A:=B-A$ | Subtraction |
| **shl** | 8 | - | $A:=B \ll A$ | Shift left |
| **shr** | 9 | - | $A:=B \gg A$ | Shift right |
| **adj** | 10 | value | $SP:=SP+value$ | Adjust SP |
| **a2sp** | 11 | - | $SP:=A; A:=B$ | A to SP |
| **sp2a** | 12 | - | $B:=A; A:=SP$ | SP to A |
| **call** | 13 | offset | $B:=A; A:=PC; PC:=PC+off$ | Call procedure |
| **return** | 14 | - | $PC:=A; A:=B$ | Return |
| **brz** | 15 | offset | if $A==0, PC:=PC+off$ | Branch if zero |
| **brlz** | 16 | offset | if $A<0, PC:=PC+off$ | Branch if less than zero |
| **br** | 17 | offset | $PC:=PC+off$ | Unconditional branch |
| **HALT** | 18 | - | Stop Emulator | Halt execution |
| **data** | - | value | Reserve word with value | Data directive |
| **SET** | - | value | Assign value to label | Pseudo-instruction |

---

## 🎮 How to use the IDE

1. **Write Code:** Use the left-hand editor to write your SIMPLEX assembly.
2. **Assemble & Run:** Click the green **"Run"** button in the header.
3. **Analyze Results:**
   - **Assembly Log:** Check for syntax errors and view the generated Machine Code for each line.
   - **Step-By-Step Trace:** View the `.trace` file data showing exactly how registers changed at every step.
   - **Emulator STDOUT:** See the final memory dump and register values ($A$, $B$, $PC$, $SP$).

---

## 🛡️ Technical Claims

- [x] **Two-Pass Logic:** Correctly resolves forward label references and `SET` directives in the first pass.
- [x] **Advanced Listing:** The `.lst` file generates a professional view with Hex Addresses, Hex Machine Code, and original Source Mnemonics.
- [x] **Fault Detection:** Emulator catches illegal memory access and PC out-of-bounds errors.
- [x] **Concurrency Support:** Server-side execution is thread-safe via unique filename hashing.
- [x] **Infinite Loop Safety:** Protects server resources from non-terminating assembly code.

---

**Local Deployment:** Compile source with `g++ asm.cpp -o asm` and `g++ emu.cpp -o emu`, then run `node server.js`.