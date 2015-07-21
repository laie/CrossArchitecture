# CrossArchitecture
allows to call X64 apis in wow64 emulation process in C++ style. designed to be lightning fast, easy to use

# Examples
## getting 64bit version of library
	auto ntdll32 = GetModuleHandle(L"ntdll.dll");   // original version of getmodulehandle
	auto ntdll64 = GetModuleHandle64(L"ntdll.dll"); // in wow64 emulation process, there are both ntdll32/64 
	
## loading 64bit version of library
	auto kernelbase = LoadLibrary64(L"kernelbase.dll"); // kernelbase.dll is not loaded in wow64 processs. so manualy load it
	std::wcout << L"LoadLibrary(\"kernelbase.dll\")  : " << LoadLibrary(L"kernelbase.dll") << L", x64version: " << kernelbase << std::endl;

## function call
	Function64<HANDLE64()> currentprocess = GetProcAddress64(kernelbase, "GetCurrentProcess");
	std::wcout << L"GetCurrentProcess   of platform: " << GetCurrentProcess() << L", x64version: " << currentprocess() << std::endl;
	Function64< DWORD()> getTickCount = GetProcAddress64(ntdll64, "NtGetTickCount");
	std::wcout << L"NtGetTickCount      of platform: " << GetTickCount() << L", x64version: " << getTickCount() << std::endl;
