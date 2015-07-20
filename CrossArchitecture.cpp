#include "CrossArchitecture.h"
#include <vector>
#include <intrin.h>

using namespace CrossArchitecture;

TEB64 CrossArchitecture::GetTEB64()
{
#if _WIN64
	return *(TEB64*)__readgsqword(0x30);
#else
	TEB64 teb64;
	Ptr64 teb64Ptr=0;

	BeginX64Stub;
	DB_(0x41) DB_(0x54);			//push r12	; R12 register should always contain pointer to TEB64 in WoW64 processes
	__asm pop dword ptr[teb64Ptr];	//			; this will pop QWORD from stack, as we're in x64 mode now
	EndX64Stub;

	memcpy64((DWORD64)&teb64, teb64Ptr, sizeof(teb64));
	return teb64;
#endif
}

PEB64 CrossArchitecture::GetPEB64()
{
	auto teb = GetTEB64();
	PEB64 peb64;
	memcpy64((DWORD64)&peb64, teb.ProcessEnvironmentBlock, sizeof(PEB64));
	return peb64;
}

HMODULE64 CrossArchitecture::GetModuleHandle64(const std::wstring& moduleName)
{
	auto peb64 = GetPEB64();
	PEB_LDR_DATA64 ldr;
	memcpy64((DWORD64)&ldr, peb64.Ldr, sizeof(ldr));

	// traversing loader module list, find matched module handle
	// NOTE: we should acquire the loader lock here, but I found none of elegant ways to find ntdll!LdrLockLoaderLock without manually implemented GetModuleHandle64
	// TODO: resolve this.... sometime

	auto head = (peb64.Ldr + (DWORD64)(&((PEB_LDR_DATA64*)nullptr)->InLoadOrderModuleList));
	DWORD64 node;
	memcpy64((DWORD64)&node, head, sizeof(node));
	for (; node != head; memcpy64((DWORD64)&node, node, sizeof(node)))
	{
		LDR_DATA_TABLE_ENTRY64 tableEntry = {};
		memcpy64((DWORD64)&tableEntry, node, sizeof(tableEntry));

		std::wstring dllName;
		dllName.resize(tableEntry.BaseDllName.Length / 2);

		memcpy64((DWORD64)dllName.data(), tableEntry.BaseDllName.Buffer, (DWORD64)(dllName.size() * 2));

		if (_wcsnicmp(moduleName.c_str(), dllName.c_str(), max(moduleName.size(), dllName.size())) == 0) return tableEntry.DllBase;
	}
	return 0;
}
Ptr64 CrossArchitecture::GetProcAddress64(HMODULE64 module, const char* procName)
{
	IMAGE_DOS_HEADER dos;
	IMAGE_NT_HEADERS64 nt64;

	memcpy64(&dos, module, sizeof(dos));
	memcpy64(&nt64, module + dos.e_lfanew, sizeof(nt64));

	auto exportDirEntry = nt64.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
	if (exportDirEntry.VirtualAddress == 0 || exportDirEntry.Size == 0) return 0;

	std::vector<unsigned char> dirBuffer;
	dirBuffer.resize(exportDirEntry.Size);
	memcpy64(dirBuffer.data(), module + exportDirEntry.VirtualAddress, exportDirEntry.Size);

	DWORD64 bufferVA = -(int)exportDirEntry.VirtualAddress;

	IMAGE_EXPORT_DIRECTORY& dir = *(IMAGE_EXPORT_DIRECTORY*)dirBuffer.data();
	DWORD* namevasArray = (DWORD*)(dirBuffer.data() + bufferVA + dir.AddressOfNames);
	WORD* nameordArray = (WORD*)(dirBuffer.data() + bufferVA + dir.AddressOfNameOrdinals);
	DWORD* addrArray = (DWORD*)(dirBuffer.data() + bufferVA + dir.AddressOfFunctions);

	int nameIndex = -1;
	for (int ni = 0; ni < (int)dir.NumberOfNames; ni++)
	{
		const char* name = (const char*)(namevasArray[ni] + dirBuffer.data() + bufferVA);
		if (strcmp(name, procName) == 0)
		{
			nameIndex = ni;
			break;
		}
	}

	if (nameIndex == -1) return 0;
	return (DWORD)addrArray[nameordArray[nameIndex]] + module;
}
HMODULE64 CrossArchitecture::LoadLibrary64(const std::wstring& moduleName)
{
	static HMODULE64 ntdll64 = GetModuleHandle64(L"ntdll.dll");
	static Function64< NTSTATUS(const wchar_t* pathToFileWide, ULONG flags, _UNICODE_STRING_T<DWORD64>* moduleFileName, HMODULE64* out)> ldrLoadDll = GetProcAddress64(ntdll64, "LdrLoadDll");
	_UNICODE_STRING_T<DWORD64> us;
	us.Buffer = (DWORD64)moduleName.c_str();
	us.Length = (WORD)(moduleName.size() * 2);
	us.MaximumLength = (WORD)(moduleName.size() * 2);
	HMODULE64 module = 0;

	NTSTATUS ret = ldrLoadDll(nullptr, 0, &us, &module);
	if (0 == ret) return module;
	SetLastError(ret);
	return 0;
}

#ifdef _WIN64
void CrossArchitecture::memcpy64(Ptr64 dst, Ptr64 src, DWORD64 size)
{
	// it's equivalent in x64
	memcpy((void*)dst.content_, (void*)src.content_, size);
}
#else
__declspec(noinline) void CrossArchitecture::memcpy64(Ptr64 dest, Ptr64 source, DWORD64 byteCount)
{
	// it causes crash when inlined?
	DWORD originalEsp;
	__asm pushfd;
	__asm mov[originalEsp], esp
	//__asm and esp, 0xFFFFFFF0
	__asm cld
	BeginX64Stub;
	__asm push esi;
	__asm push edi;
	__asm push ecx;
	
	DB_(0x48) __asm mov edi, dword ptr[dest];	//mov rdi, QWORD PTR [rbp + 0x8]
	DB_(0x48) __asm mov esi, dword ptr[source];	//mov rsi, QWORD PTR [rbp + 0x10]
	DB_(0x48) __asm mov ecx, dword ptr[byteCount];	//mov rcx, QWORD PTR [rbp + 0x18]
	__asm repe movsb;

	__asm pop ecx;
	__asm pop edi;
	__asm pop esi;
	EndX64Stub;
	__asm mov esp, [originalEsp]
	__asm popfd;
}
#endif
inline void CrossArchitecture::memcpy64(void* dst, DWORD64 src, DWORD size)
{
	return memcpy64((DWORD64)dst, src, size);
}
