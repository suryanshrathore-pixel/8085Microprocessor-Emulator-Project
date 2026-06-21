<!-- TODO: add a banner here -->
# 8085 Microprocessor Emulator

![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Architecture](https://img.shields.io/badge/Architecture-Intel_8085-blue?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Optimized_&_Active-success?style=for-the-badge)

## Executive Summary
Engineered an optimized microprocessor emulator. Refactored CPU execution logic to strict industry standards, ensuring robust memory management, precise register allocation, and highly scalable instruction processing.

## 🚀 Recent Optimizations & Enhancements
This project builds upon an existing emulation foundation, introducing critical architectural refactoring and logic optimization to bring the codebase up to modern C++ industry standards:

* **Control Flow Optimization:** Resolved pointer logic within `JMP` and `CALL` instructions to ensure the Program Counter (PC) correctly branches to destination addresses rather than evaluating memory-held data.
* **Register Mapping Resolution:** Audited and corrected widespread register allocation anomalies (copy-paste inheritance bugs) within the `MOV` instruction set, ensuring deterministic and accurate data transfer between the accumulator and standard registers.
* **Architectural Refactoring:** Standardized inline documentation, decoupled memory initialization logic with strict boundary checking to prevent buffer overflows, and optimized the overall `switch-case` execution flow for maximum computational efficiency.

## 🧠 Technical Architecture
The system utilizes strict Object-Oriented Programming (OOP) principles to provide a highly accurate virtual environment for executing low-level assembly binaries.

* **Decoupled Memory Module:** Separates CPU logic from the memory buffer, allowing for dynamic memory allocation and safe binary parsing.
* **Precise State Management:** Accurately maps internal registers (A, B, C, D, E, H, L), the 16-bit Program Counter (PC), and Stack Pointer (SP).
* **ALU Simulation:** Dynamically updates Status Register flags (Zero, Carry, Sign, Parity) in real-time based on operation outcomes.
* **Comprehensive Instruction Set:** Processes a vast array of 8085 mnemonics, including data transfer (`MOV`, `MVI`, `LXI`) and complex branching.

## 🛠️ Getting Started

### Prerequisites
* A C++11 (or higher) compatible compiler (e.g., GCC, Clang, MSVC).

### Build Instructions
To compile the emulator into a standalone executable, run the following command in your terminal:
***
🤝 Acknowledgment
This project is a heavily optimized and refactored fork. Credit to the original author for the foundational project structure. Subsequent engineering, logic correction, and architectural scaling were developed independently to achieve current production standards.
```bash
g++ main.cpp cpu.cpp -o emulator.exe
```Execution
./emulator.exe

