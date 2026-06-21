#include "cpu.h"
#include <iostream>
#include <fstream>

using namespace emu;

// ============================================================================
// Memory Implementation
// ============================================================================

/**
 * Zero-initializes the processor's memory buffer.
 */
void Memory::initialise() { 
  data = {}; 
}

/**
 * Initializes memory using the provided executable binary file.
 * Includes boundary checking to prevent buffer overflow if the program 
 * exceeds allocated memory limits.
 */
bool Memory::initialise(std::ifstream &input_file) {
  if (!input_file.is_open()) {
    std::cerr << "Error: Cannot open the executable file.\n";
    return false;
  }

  if (input_file.gcount() >= static_cast<long>(data.size())) {
    std::cerr << "Error: Program size exceeds available memory capacity.\n";
    return false;
  }

  input_file.read(reinterpret_cast<char *>(data.data()), data.size());
  return true;
}

// ============================================================================
// CPU Implementation
// ============================================================================

void CPU::reset(Memory &mem) {
  programCounter = A = B = C = D = E = H = L = 0;
  stackPointer = 0xFFFF;
  statusFlags = {};
  mem.initialise();
}

void CPU::execute(int &cycles, Memory &mem) {
  while (cycles > 0) {
    auto lastInstructionLocation = programCounter;
    auto instruction = fetchByte(cycles, mem);
    
    switch (instruction) {

    // NOP / Self-Moves
    case MOV_AA:
    case MOV_BB:
    case MOV_CC:
    case MOV_DD:
    case MOV_EE:
    case MOV_HH:
    case MOV_LL:
      break;

    // Move to A
    case MOV_AB: moveRegister(A, B); break;
    case MOV_AC: moveRegister(A, C); break;
    case MOV_AD: moveRegister(A, D); break;
    case MOV_AE: moveRegister(A, E); break;
    case MOV_AH: moveRegister(A, H); break;
    case MOV_AL: moveRegister(A, L); break;

    // Move to B
    case MOV_BA: moveRegister(B, A); break;
    case MOV_BC: moveRegister(B, C); break;
    case MOV_BD: moveRegister(B, D); break;
    case MOV_BE: moveRegister(B, E); break;
    case MOV_BH: moveRegister(B, H); break;
    case MOV_BL: moveRegister(B, L); break;

    // Move to C
    case MOV_CA: moveRegister(C, A); break;
    case MOV_CB: moveRegister(C, B); break;
    case MOV_CD: moveRegister(C, D); break;
    case MOV_CE: moveRegister(C, E); break;
    case MOV_CH: moveRegister(C, H); break;
    case MOV_CL: moveRegister(C, L); break;

    // Move to D
    case MOV_DA: moveRegister(D, A); break;
    case MOV_DB: moveRegister(D, B); break;
    case MOV_DC: moveRegister(D, C); break;
    case MOV_DE: moveRegister(D, E); break;
    case MOV_DH: moveRegister(D, H); break;
    case MOV_DL: moveRegister(D, L); break;

    // Move to E
    case MOV_EA: moveRegister(E, A); break;
    case MOV_EB: moveRegister(E, B); break;
    case MOV_EC: moveRegister(E, C); break;
    case MOV_ED: moveRegister(E, D); break;
    case MOV_EH: moveRegister(E, H); break;
    case MOV_EL: moveRegister(E, L); break;

    // Move to H
    case MOV_HA: moveRegister(H, A); break;
    case MOV_HB: moveRegister(H, B); break;
    case MOV_HC: moveRegister(H, C); break;
    case MOV_HD: moveRegister(H, D); break;
    case MOV_HE: moveRegister(H, E); break;
    case MOV_HL: moveRegister(H, L); break;

    // Move to L
    case MOV_LA: moveRegister(L, A); break;
    case MOV_LB: moveRegister(L, B); break;
    case MOV_LC: moveRegister(L, C); break;
    case MOV_LD: moveRegister(L, D); break;
    case MOV_LE: moveRegister(L, E); break;
    case MOV_LH: moveRegister(L, H); break;

    // Register Indirect Addressing Mode (Move to Register from Memory)
    case MOV_AM: moveIndirectTo(A, mem, cycles); break;
    case MOV_BM: moveIndirectTo(B, mem, cycles); break;
    case MOV_CM: moveIndirectTo(C, mem, cycles); break;
    case MOV_DM: moveIndirectTo(D, mem, cycles); break;
    case MOV_EM: moveIndirectTo(E, mem, cycles); break;
    case MOV_HM: moveIndirectTo(H, mem, cycles); break;
    case MOV_LM: moveIndirectTo(L, mem, cycles); break;

    // Register Indirect Addressing Mode (Move to Memory from Register)
    case MOV_MA: moveIndirectFrom(A, mem, cycles); break;
    case MOV_MB: moveIndirectFrom(B, mem, cycles); break;
    case MOV_MC: moveIndirectFrom(C, mem, cycles); break;
    case MOV_MD: moveIndirectFrom(D, mem, cycles); break;
    case MOV_ME: moveIndirectFrom(E, mem, cycles); break;
    case MOV_MH: moveIndirectFrom(H, mem, cycles); break;
    case MOV_ML: moveIndirectFrom(L, mem, cycles); break;

    // Move Immediate
    case MVI_A: moveImmediate(A, cycles, mem); break;
    case MVI_B: moveImmediate(B, cycles, mem); break;
    case MVI_C: moveImmediate(C, cycles, mem); break;
    case MVI_D: moveImmediate(D, cycles, mem); break;
    case MVI_E: moveImmediate(E, cycles, mem); break;
    case MVI_H: moveImmediate(H, cycles, mem); break;
    case MVI_L: moveImmediate(L, cycles, mem); break;
    case MVI_M: {
      auto address = readRegisterPair(H, L);
      cycles--;
      auto destination = mem[address];
      moveImmediate(destination, cycles, mem);
    } break;

    // Load/Store Immediate (Register Pair)
    case LXI_B:  loadImmediate(B, C, cycles, mem); break;
    case LXI_D:  loadImmediate(D, E, cycles, mem); break;
    case LXI_H:  loadImmediate(H, L, cycles, mem); break;
    case LXI_SP: loadImmediate(stackPointer, cycles, mem); break;

    // Load/Store Accumulator (From Register Pair)
    case LDAX_B: {
      auto address = readRegisterPair(B, C);
      cycles--;
      A = mem[address];
    } break;
    case LDAX_D: {
      auto address = readRegisterPair(D, E);
      cycles--;
      A = mem[address];
    } break;
    case STAX_B: {
      auto address = readRegisterPair(B, C);
      cycles--;
      mem[address] = A;
    } break;
    case STAX_D: {
      auto address = readRegisterPair(D, E);
      cycles--;
      mem[address] = A;
    } break;

    // Load/Store Accumulator Directly
    case LDA: loadAccumulator(cycles, mem); break;
    case STA: storeAccumulator(cycles, mem); break;
    
    // Load/Store HL Direct
    case LHLD: {
      auto address = fetchWord(cycles, mem);
      H = mem[address];
      L = mem[address + 1];
    } break;
    case SHLD: {
      auto address = fetchWord(cycles, mem);
      mem[address] = L;
      mem[address + 1] = H;
    } break;

    // Exchange HL and DE
    case XCHNG: {
      auto dataHL = readRegisterPair(H, L);
      auto dataDE = readRegisterPair(D, E);
      writeRegisterPair(H, L, dataDE);
      writeRegisterPair(D, E, dataHL);
    } break;

    // Unconditional Jump
    case JMP: unconditionalJump(cycles, mem); break;

    // Conditional Jumps
    case JNZ: jumpIfNotZero(cycles, mem); break;
    case JZ:  jumpIfZero(cycles, mem); break;
    case JNC: jumpIfNotCarry(cycles, mem); break;
    case JC:  jumpIfCarry(cycles, mem); break;
    case JPO: jumpIfOddParity(cycles, mem); break;
    case JP:  jumpIfEvenParity(cycles, mem); break;
    case JPE: jumpIfPositive(cycles, mem); break;
    case JM:  jumpIfMinus(cycles, mem); break;

    // Call Instructions
    case CALL: unconditionalCall(cycles, mem); break;

    // Conditional Calls
    case CNZ: callIfNotZero(cycles, mem); break;
    case CZ:  callIfZero(cycles, mem); break;
    case CNC: callIfNotCarry(cycles, mem); break;
    case CC:  callIfCarry(cycles, mem); break;
    case CPO: callIfOddParity(cycles, mem); break;
    case CP:  callIfEvenParity(cycles, mem); break;
    case CPE: callIfPositive(cycles, mem); break;
    case CM:  callIfMinus(cycles, mem); break;

    default:
      std::cerr << "Error: Unknown instruction encountered (Opcode: "
                << static_cast<int>(instruction)
                << ") at memory location: " << lastInstructionLocation << "\n";
    }
  }
}

/**
 * Reads a 16-bit word from a specified register pair.
 */
Word CPU::readRegisterPair(const Byte &reg1, const Byte &reg2) {
  Word data = reg1 << 8;
  data |= reg2;
  return data;
}

/**
 * Writes 16-bit word data into a specified register pair.
 */
void CPU::writeRegisterPair(Byte &reg1, Byte &reg2, Word &data) {
  reg1 = data >> 8;
  reg2 = static_cast<Byte>(data);
}

/**
 * Fetches a single byte from memory at the current Program Counter.
 */
Byte CPU::fetchByte(int &cycles, const Memory &mem) {
  auto currentData = mem[programCounter];
  --cycles;
  ++programCounter;
  return currentData;
}

/**
 * Fetches a 16-bit word from memory at the current Program Counter.
 * Assumes Little-Endian architecture (lower byte first).
 */
Word CPU::fetchWord(int &cycles, const Memory &mem) {
  Word currentData = mem[programCounter];
  ++programCounter;
  currentData |= (mem[programCounter] << 8);
  ++programCounter;
  cycles -= 2;
  return currentData;
}

// ============================================================================
// Instruction Set Implementations
// ============================================================================

/**
 * Loads the Accumulator (Register A) with data from a 16-bit memory address.
 */
void CPU::loadAccumulator(int &cycles, const Memory &mem) {
  auto address = fetchWord(cycles, mem);
  A = mem[address];
}

/**
 * Stores the contents of the Accumulator into a specified 16-bit memory address.
 */
void CPU::storeAccumulator(int &cycles, Memory &mem) {
  Word address = fetchWord(cycles, mem);
  mem[address] = A;
}

/**
 * Transfers data from the source register to the destination register.
 */
void CPU::moveRegister(Byte &destination, Byte &source) {
  destination = source;
}

/**
 * Moves memory contents pointed to by the H-L register pair into the destination.
 */
void CPU::moveIndirectTo(Byte &destination, const Memory &mem, int &cycles) {
  auto address = readRegisterPair(H, L);
  cycles--;
  destination = mem[address];
}

/**
 * Moves the contents of the source register into memory pointed to by the H-L pair.
 */
void CPU::moveIndirectFrom(const Byte &source, Memory &mem, int &cycles) {
  auto address = readRegisterPair(H, L);
  cycles--;
  mem[address] = source;
}

/**
 * Loads 8-bit immediate data into the destination register.
 */
void CPU::moveImmediate(Byte &destination, int &cycles, const Memory &mem) {
  auto data = fetchByte(cycles, mem);
  destination = data;
}

/**
 * Loads a 16-bit immediate value into an 8-bit register pair.
 */
void CPU::loadImmediate(Byte &reg1, Byte &reg2, int &cycles, const Memory &mem) {
  auto data = fetchWord(cycles, mem);
  writeRegisterPair(reg1, reg2, data);
}

/**
 * Loads a 16-bit immediate value into a 16-bit register (e.g., Stack Pointer).
 */
void CPU::loadImmediate(Word &reg, int &cycles, const Memory &mem) {
  auto data = fetchWord(cycles, mem);
  reg = data;
}

/**
 * Unconditionally branches program control to a specified memory address.
 */
void CPU::unconditionalJump(int &cycles, const Memory &mem) {
  auto destination = fetchWord(cycles, mem);
  programCounter = destination; // Corrected: Jump to the address, not the data at the address
}

/**
 * Branches program control if the Zero flag is not set.
 */
void CPU::jumpIfNotZero(int &cycles, const Memory &mem) {
  if (!statusFlags.z) {
    unconditionalJump(cycles, mem);
  } else {
    fetchWord(cycles, mem); // Consume operands and cycles
  }
}

/**
 * Branches program control if the Zero flag is set.
 */
void CPU::jumpIfZero(int &cycles, const Memory &mem) {
  if (statusFlags.z) {
    unconditionalJump(cycles, mem);
  } else {
    fetchWord(cycles, mem);
  }
}

/**
 * Branches program control if the Carry flag is not set.
 */
void CPU::jumpIfNotCarry(int &cycles, const Memory &mem) {
  if (!statusFlags.c) {
    unconditionalJump(cycles, mem);
  } else {
    fetchWord(cycles, mem);
  }
}

/**
 * Branches program control if the Carry flag is set.
 */
void CPU::jumpIfCarry(int &cycles, const Memory &mem) {
  if (statusFlags.c) {
    unconditionalJump(cycles, mem);
  } else {
    fetchWord(cycles, mem);
  }
}

/**
 * Branches program control if the Parity flag is Odd (not set).
 */
void CPU::jumpIfOddParity(int &cycles, const Memory &mem) {
  if (!statusFlags.p) {
    unconditionalJump(cycles, mem);
  } else {
    fetchWord(cycles, mem);
  }
}

/**
 * Branches program control if the Parity flag is Even (set).
 */
void CPU::jumpIfEvenParity(int &cycles, const Memory &mem) {
  if (statusFlags.p) {
    unconditionalJump(cycles, mem);
  } else {
    fetchWord(cycles, mem);
  }
}

/**
 * Branches program control if the Sign flag is Positive (not set).
 */
void CPU::jumpIfPositive(int &cycles, const Memory &mem) {
  if (!statusFlags.s) {
    unconditionalJump(cycles, mem);
  } else {
    fetchWord(cycles, mem);
  }
}

/**
 * Branches program control if the Sign flag is Minus (set).
 */
void CPU::jumpIfMinus(int &cycles, const Memory &mem) {
  if (statusFlags.s) {
    unconditionalJump(cycles, mem);
  } else {
    fetchWord(cycles, mem);
  }
}

/**
 * Unconditionally branches to a subroutine, pushing the return address to the stack.
 */
void CPU::unconditionalCall(int &cycles, Memory &mem) {
  auto destination = fetchWord(cycles, mem);
  auto returnAddress = mem[programCounter];
  stackPush(cycles, mem, returnAddress);
  programCounter = destination; // Corrected: Jump to the address
}

/**
 * Branches to a subroutine if the Zero flag is not set.
 */
void CPU::callIfNotZero(int &cycles, Memory &mem) {
  if (!statusFlags.z) {
    unconditionalCall(cycles, mem);
  } else {
    fetchWord(cycles, mem);
  }
}

/**
 * Branches to a subroutine if the Zero flag is set.
 */
void CPU::callIfZero(int &cycles, Memory &mem) {
  if (statusFlags.z) {
    unconditionalCall(cycles, mem);
  } else {
    fetchWord(cycles, mem);
  }
}

/**
 * Branches to a subroutine if the Carry flag is not set.
 */
void CPU::callIfNotCarry(int &cycles, Memory &mem) {
  if (!statusFlags.c) {
    unconditionalCall(cycles, mem);
  } else {
    fetchWord(cycles, mem);
  }
}

/**
 * Branches to a subroutine if the Carry flag is set.
 */
void CPU::callIfCarry(int &cycles, Memory &mem) {
  if (statusFlags.c) {
    unconditionalCall(cycles, mem);
  } else {
    fetchWord(cycles, mem);
  }
}

/**
 * Branches to a subroutine if the Parity flag is Odd (not set).
 */
void CPU::callIfOddParity(int &cycles, Memory &mem) {
  if (!statusFlags.p) {
    unconditionalCall(cycles, mem);
  } else {
    fetchWord(cycles, mem);
  }
}

/**
 * Branches to a subroutine if the Parity flag is Even (set).
 */
void CPU::callIfEvenParity(int &cycles, Memory &mem) {
  if (statusFlags.p) {
    unconditionalCall(cycles, mem);
  } else {
    fetchWord(cycles, mem);
  }
}

/**
 * Branches to a subroutine if the Sign flag is Positive (not set).
 */
void CPU::callIfPositive(int &cycles, Memory &mem) {
  if (!statusFlags.s) {
    unconditionalCall(cycles, mem);
  } else {
    fetchWord(cycles, mem);
  }
}

/**
 * Branches to a subroutine if the Sign flag is Minus (set).
 */
void CPU::callIfMinus(int &cycles, Memory &mem) {
  if (statusFlags.s) {
    unconditionalCall(cycles, mem);
  } else {
    fetchWord(cycles, mem);
  }
}

/**
 * Pushes a byte onto the stack and decrements the Stack Pointer.
 */
void CPU::stackPush(int &cycles, Memory &mem, const Byte &data) {
  cycles++;
  mem[stackPointer] = data;
  stackPointer--;
}

/**
 * Pops a byte from the stack and increments the Stack Pointer.
 */
Byte CPU::stackPop(int &cycles, const Memory &mem) {
  cycles++;
  auto data = mem[stackPointer];
  stackPointer++;
  return data;
}

}
