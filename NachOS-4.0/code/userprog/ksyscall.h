/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__
#define __USERPROG_KSYSCALL_H__

#include "kernel.h"
#include "synchconsole.h"

#define BUFFER_MAX_LENGTH 255
#define FILE_MAX_LENGTH 32

void SysHalt()
{
  kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2)
{
  return op1 + op2;
}

char *User2System(int virtAddr, int limit)
{
  int i; // index
  int oneChar;
  char *kernelBuf = NULL;
  kernelBuf = new char[limit + 1]; // need for terminal string

  if (kernelBuf == NULL)
    return kernelBuf;

  memset(kernelBuf, 0, limit + 1);
  // printf("\n Filename u2s:");

  for (i = 0; i < limit; i++)
  {
    kernel->machine->ReadMem(virtAddr + i, 1, &oneChar);
    kernelBuf[i] = (char)oneChar;
    // printf("%c",kernelBuf[i]);
    if (oneChar == 0)
      break;
  }
  return kernelBuf;
}

int System2User(int virtAddr, int len, char *buffer)
{
  if (len < 0)
    return -1;
  if (len == 0)
    return len;
  int i = 0;
  int oneChar = 0;
  do
  {
    oneChar = (int)buffer[i];
    kernel->machine->WriteMem(virtAddr + i, 1, oneChar);
    i++;
  } while (i < len && oneChar != 0);
  return i;
}

int SysReadNum()
{
	char c;
	int res = 0;
	int sign = 1;
	bool isValid = true;

	// bỏ qua khoảng trống và xuống dòng
	c = kernel->synchConsoleIn->GetChar();
	while (c == ' ' || c == '\n')
		c = kernel->synchConsoleIn->GetChar();

	if (c == '-') // kiểm tra số âm
	{
		sign = -1;
		c = kernel->synchConsoleIn->GetChar();
	}

	while (c != ' ' && c != '\n')
	{
		if (isValid) // số hợp lệ
		{
			if (c >= '0' && c <= '9')
			{
				res *= 10;
				res += sign * (c - '0');
				if ((sign > 0 && res < 0) || (sign < 0 && res > 0)) // overflow
					isValid = false;
			}
			else
				isValid = false;
		}
		c = kernel->synchConsoleIn->GetChar();
	}

	if (isValid)
		return res;
}

void SysPrintNum(int number)
{
	//trường hợp đầu vào < 0 thêm - phía trước
	if (number < 0)
	{
		kernel->synchConsoleOut->PutChar('-');
	}

	int digitCount = 0; // số chữ số
	int digits[10];		// mảng lưu trữ từng số
						// max 2^-31 -> 2^31-1

	//lưu các số từ phải sang trái, số âm có dấu - ở trước
	int numberCopy = number;
	while (numberCopy != 0)
	{
		digits[digitCount] = numberCopy % 10;
		numberCopy /= 10;
		digitCount++;
	}

	// in mảng chữ số
	if (number > 0)
	{
		for (int i = 0; i < digitCount; i++)
		{
			kernel->synchConsoleOut->PutChar(digits[digitCount - 1 - i] + '0');
		}
	}
	else
	{
		for (int i = 0; i < digitCount; i++)
		{
			// -(0 -> -9) + '0' = ('0' -> '9')
			kernel->synchConsoleOut->PutChar(-digits[digitCount - 1 - i] + '0');
		}
	}
}

//đọc kí tự từ bàn phím
char SysReadChar()
{
	return kernel->synchConsoleIn->GetChar();
}

//in ra kí tự
void SysPrintChar(char c)
{
	kernel->synchConsoleOut->PutChar(c);
}

//số ngẫu nhiên
int SysRandomNum()
{
	RandomInit((unsigned int)time(NULL));
	return RandomNumber();
}

//đọc chuỗi từ bàn phím
int SysReadString(char *virtAddr, int length)
{
	for(int i=0;i<length;i++)
	{
		kernel->synchConsoleIn->GetChar();
	}
}

//in ra chuỗi 
void SysPrintString(char* buff)
{
	for(int i=0;i<strlen(buff);i++)
	{
		kernel->synchConsoleOut->PutChar(buff[i]);
	}
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
