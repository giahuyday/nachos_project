// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
#include "synchconsole.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions
//	is in machine.h.
//----------------------------------------------------------------------

/*enum ExceptionType { NoException,           // Everything ok!
             SyscallException,      // A program executed a system call.
             PageFaultException,    // No valid translation found
             ReadOnlyException,     // Write attempted to page marked
                        // "read-only"
             BusErrorException,     // Translation resulted in an
                        // invalid physical address
             AddressErrorException, // Unaligned reference or one that
                        // was beyond the end of the
                        // address space
             OverflowException,     // Integer overflow in add or sub.
             IllegalInstrException, // Unimplemented or reserved instr.

             NumExceptionTypes
};*/

#define Create_Name_Length 20
void IncreasePC()
{
    /* set previous programm counter (debugging only)*/
    kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

    /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
    kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

    /* set next programm counter for brach execution */
    kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
}

void ExceptionHandler(ExceptionType which)
{
    int type = kernel->machine->ReadRegister(2);

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which)
    {
    case NoException:
        return;
        break;
    case ReadOnlyException:
        cout << "Read only";
        SysHalt();
        break;
    case PageFaultException:
        cout << "Page fault";
        SysHalt();
        break;
    case BusErrorException:
        cout << "Bus Error";
        SysHalt();
        break;
    case AddressErrorException:
        cout << "Address error";
        SysHalt();
        break;
    case OverflowException:
        cout << "Over flow";
        SysHalt();
        break;
    case IllegalInstrException:
        cout << "Illegal instruction";
        SysHalt();
        break;
    case NumExceptionTypes:
        cout << "Number exception type";
        SysHalt();
        break;
    case SyscallException:
        switch (type)
        {
        case SC_Halt:
        {
            DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

            SysHalt();

            ASSERTNOTREACHED();
            break;
        }
        case SC_Add:
        {
            DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");
            /* Process SysAdd Systemcall*/
            int result;
            result = SysAdd(/* int op1 */ (int)kernel->machine->ReadRegister(4),
                            /* int op2 */ (int)kernel->machine->ReadRegister(5));
            DEBUG(dbgSys, "Add returning with " << result << "\n");
            IncreasePC();
            return;
            ASSERTNOTREACHED();
            break;
        }
        case SC_ReadNum:
        {
            DEBUG(dbgSys, "Read number\n");
            int result;
            result = SysReadNum();
            DEBUG(dbgSys, "SysReadNum returning with " << result << "\n");
            IncreasePC();
            return;
            ASSERTNOTREACHED();
            break;
        }

        case SC_PrintNum:
            DEBUG(dbgSys, "PrintNum " << kernel->machine->ReadRegister(4) << "\n");
            SysPrintNum((int)kernel->machine->ReadRegister(4));
            IncreasePC();
            return;
            ASSERTNOTREACHED();
            break;

        case SC_ReadChar:
        {
            DEBUG(dbgSys, "Read character\n");
            char result;
            result = SysReadChar();
            DEBUG(dbgSys, "SysReadChar returning with " << result << "\n");
            /* Prepare Result */
            kernel->machine->WriteRegister(2, (int)result);
            IncreasePC();
            ASSERTNOTREACHED();
            break;
        }

        case SC_PrintChar:
            DEBUG(dbgSys, "Print character: " << kernel->machine->ReadRegister(4) << "\n");
            SysPrintChar(kernel->machine->ReadRegister(4));
            IncreasePC();
            return;
            ASSERTNOTREACHED();
            break;

        case SC_RandomNumber:
        {
            DEBUG(dbgSys, "Random number\n");
            int result;
            result = SysRandomNum();
            DEBUG(dbgSys, "SysRandomNum returning with " << result << "\n");
            /* Prepare Result */
            kernel->machine->WriteRegister(2, (int)result);
            IncreasePC();
            return;
            ASSERTNOTREACHED();
            break;
        }

        case SC_ReadString:
        {
            DEBUG(dbgSys, "Read string\n");
            SysReadString((char *)kernel->machine->ReadRegister(4), (int)kernel->machine->ReadRegister(5));
            IncreasePC();
            return;
            ASSERTNOTREACHED();
            break;
        }

        case SC_PrintString:
            DEBUG(dbgSys, "PrintString " << kernel->machine->ReadRegister(4) << "\n");
            SysPrintString((char *)kernel->machine->ReadRegister(4));
            IncreasePC();
            return;
            ASSERTNOTREACHED();
            break;

        default:
            cerr << "Unexpected system call " << type << "\n";
            break;
        }
    default:
        cerr << "Unexpected user mode exception" << (int)which << "\n";
        break;
    }

    ASSERTNOTREACHED();
}
