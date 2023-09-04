
#include <windows.h>
#include <stdio.h>
#include <memory>

#include "BoomDef.h"


void readMemory(ULONG64 pid, ULONG64 address, UCHAR *buf, ULONG size)
{
	BOOM_PROCESS_OPERA	input = { 0 };

	input.tag = TAG_READ;
	input.pid = pid;
	input.address = address;
	input.buf = (UCHAR*)buf;
	input.size = size;

	RegSetValueExA(HKEY_LOCAL_MACHINE,
				   "",
				   NULL,
				   REG_BINARY,
				   (byte*)&input,
				   48);
}

void writeMemory(ULONG64 pid, ULONG64 address, UCHAR *buf, ULONG size)
{
	BOOM_PROCESS_OPERA	input = { 0 };

	input.tag = TAG_WRITE;
	input.pid = pid;
	input.address = address;
	input.buf = (UCHAR*)buf;
	input.size = size;

	RegSetValueExA(HKEY_LOCAL_MACHINE,
				   "",
				   NULL,
				   REG_BINARY,
				   (byte*)&input,
				   48);

}

void probeRead(ULONG64 pid, ULONG64 address)
{
	BOOM_PROCESS_OPERA	input = { 0 };

	input.tag = TAG_PROBEREAD;
	input.pid = pid;
	input.address = address;
	input.buf = NULL;
	input.size = NULL;

	RegSetValueExA(HKEY_LOCAL_MACHINE,
				   "",
				   NULL,
				   REG_BINARY,
				   (byte*)&input,
				   48);
}

int main()
{


	ULONG64 pid = 1448;

	HANDLE hprocess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	ULONG64 address = NULL;

	if (!address)
	{
		address = (ULONG64)VirtualAllocEx(hprocess, 0, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READ);
	}

	UCHAR buf[16] = { 0 };

	probeRead(pid, address);

	//
	//∂¡ƒ⁄¥Ê≤‚ ‘
	//

	readMemory(pid, address, buf, sizeof(buf));

	for (auto b:buf)
	{
		printf("%02X", b);
	}
	printf("\n");

	//
	//–¥ƒ⁄¥Ê≤‚ ‘
	//

	memset(buf, 0x90, sizeof(buf));
	writeMemory(pid, address, buf, sizeof(buf));
	

	//
	//‘Ÿ¥Œ∂¡ƒ⁄¥Ê≤‚ ‘
	//
	memset(buf, 0, sizeof(buf));
	readMemory(pid, address, buf, sizeof(buf));

	for (auto b : buf)
	{
		printf("%02X", b);
	}
	printf("\n");

	CloseHandle(hprocess);


	system("pause");
	return 0;
}