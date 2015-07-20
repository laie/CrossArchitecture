#include "targetver.h"
#include "CrossArchitecture.h"
#include <iostream>

inline bool debuggerPresent()
{
	// this detection routine becomes very powerful on Wow64.
	// note that on Wow64, there will be both PEB32 and PEB64
	// since it checks x64 version of BeingDebugged flag, 99.9% of debugger with detection prevention plugins will be detected.
	return CrossArchitecture::GetPEB64().BeingDebugged != 0;
}
void TestAntidebug()
{
	std::wcout << L"am I now being debugged: " << IsDebuggerPresent() << L", x64answer: " << debuggerPresent() << std::endl;
}
void TestCrossPlatform()
{
	// this will work both on platform Win32 and platform x64
	using namespace CrossArchitecture;

	auto ntdll32 = GetModuleHandle(L"ntdll.dll");
	auto ntdll64 = GetModuleHandle64(L"ntdll.dll");
	std::wcout << std::hex;
	std::wcout << L"ntdll of platform              : " << ntdll32 << L", x64ntdll: " << ntdll64 << std::endl;

	auto kernelbase = LoadLibrary64(L"kernelbase.dll");
	std::wcout << L"LoadLibrary(\"kernelbase.dll\")  : " << LoadLibrary(L"kernelbase.dll") << L", x64version: " << kernelbase << std::endl;

	Function64< DWORD()> getTickCount = GetProcAddress64(ntdll64, "NtGetTickCount");
	std::wcout << L"NtGetTickCount      of platform: " << GetTickCount() << L", x64version: " << getTickCount() << std::endl;

	Function64<DWORD()> currentprocessId = GetProcAddress64(kernelbase, "GetCurrentProcessId");
	std::wcout << L"GetCurrentProcessId of platform: " << GetCurrentProcessId() << L", x64version: " << currentprocessId() << std::endl;
	Function64<HANDLE64()> currentprocess = GetProcAddress64(kernelbase, "GetCurrentProcess");
	std::wcout << L"GetCurrentProcess   of platform: " << GetCurrentProcess() << L", x64version: " << currentprocess() << std::endl;

	Function64<
		NTSTATUS(
		_In_    HANDLE64	ProcessHandle,
		_Inout_ Ptr64*		BaseAddress,
		_In_    DWORD64		ZeroBits,
		_Inout_ DWORD64*	RegionSize,
		_In_    ULONG		AllocationType,
		_In_    ULONG		Protect
		)>
		ntAllocateVirtualMemory(GetProcAddress64(ntdll64, "NtAllocateVirtualMemory"));

	Ptr64 base = 0;
	DWORD64 size = 0x1000;
	std::wcout << L"NtAllocateVirtualMemory x64 returns: " << (void*)ntAllocateVirtualMemory(currentprocess(),
		&base,
		0,
		&size,
		MEM_COMMIT,
		PAGE_EXECUTE_READWRITE
		);
	std::wcout << L" memory allocated: " << base << L"~" << base + size << std::endl;

	Function64<LONG(HANDLE64* threadHandle, ACCESS_MASK access, Ptr64 attributes, HANDLE64 processHandle, Ptr64 startRoutine, Ptr64 argument, ULONG createFlags, DWORD64 zeroBits, SIZE_T stackSize, SIZE_T maximumStackSize, Ptr64 attributeList)>
		ntCreateThreadEx = GetProcAddress64(ntdll64, "NtCreateThreadEx");

	//DWORD64 threadHandle = 0;
	//ATHROW(0 == ntCreateThreadEx(&threadHandle, 0x1FFFFF, 0, processHandle, startRoutine, nullptr, 0, 0, 0, 0, 0));
	//ATHROW(threadHandle);
	std::wcout << L"suspending myself.. " << std::endl;
	Function64 < NTSTATUS(HANDLE64 process)> ntSuspendProcess = GetProcAddress64(ntdll64, "NtSuspendProcess");
	std::wcout << L"resumed! and returned " << (void*)ntSuspendProcess(HANDLE64(0xFFFFFFFFFFFFFFFF)) << std::endl;
	std::wcout << L"you might not be able to get into here unless you resume manually with third party apps. I'm quiting!" << std::endl;

	//X64Function < NTSTATUS(DWORD64 path, ULONG flags, _UNICODE_STRING_T<DWORD64>* moduleFileName, DWORD64 handleOut)> f;
}

int main()
{
	TestAntidebug();
	TestCrossPlatform();
	return 0;
}

