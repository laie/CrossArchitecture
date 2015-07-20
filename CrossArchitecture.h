#pragma once
#include <Windows.h>
#include <string>

// defines from reference: http://blog.rewolf.pl/blog/?p=102
// great work, ReWolf :)

#define DB_(c) __asm __emit (c)
#define BeginX64Stub \
								{ \
		DB_(0x6A) DB_(0x33)							/*  push   _cs             */ \
		DB_(0xE8) DB_(0) DB_(0) DB_(0) DB_(0)		/*  call   $+5             */ \
		DB_(0x83) DB_(4) DB_(0x24) DB_(5)			/*  add    dword [esp], 5  */ \
		DB_(0xCB)									/*  retf                   */ \
								}


#define EndX64Stub \
								{ \
		DB_(0xE8) DB_(0) DB_(0) DB_(0) DB_(0)						            /*  call   $+5                   */ \
		DB_(0xC7) DB_(0x44) DB_(0x24) DB_(4) DB_(0x23) DB_(0) DB_(0) DB_(0)		/*  mov    dword [rsp + 4], _cs  */ \
		DB_(0x83) DB_(4) DB_(0x24) DB_(0xD)								        /*  add    dword [rsp], 0xD      */ \
		DB_(0xCB)																/*  retf                         */ \
								}
namespace CrossArchitecture
{
#pragma pack(push, 1)
	// some structures extracted from https://github.com/DarthTon/wow64dm/blob/4291e749e3e1b22e9d578b240721d99aeb8a7121/Native.h
	template <class T> struct _LIST_ENTRY_T
	{
		T Flink;
		T Blink;
	};
	template <class T> struct _UNICODE_STRING_T
	{
		union
		{
			struct
			{
				WORD Length;
				WORD MaximumLength;
			};
			T dummy;
		};
		T Buffer;
	};

	template <class T> struct _NT_TIB_T
	{
		T ExceptionList;
		T StackBase;
		T StackLimit;
		T SubSystemTib;
		T FiberData;
		T ArbitraryUserPointer;
		T Self;
	};

	template <class T> struct _CLIENT_ID
	{
		T UniqueProcess;
		T UniqueThread;
	};

	template <class T> struct _LDR_DATA_TABLE_ENTRY_T
	{
		_LIST_ENTRY_T<T> InLoadOrderLinks;
		_LIST_ENTRY_T<T> InMemoryOrderLinks;
		_LIST_ENTRY_T<T> InInitializationOrderLinks;
		T DllBase;
		T EntryPoint;
		union
		{
			DWORD SizeOfImage;
			T dummy01;
		};
		_UNICODE_STRING_T<T> FullDllName;
		_UNICODE_STRING_T<T> BaseDllName;
		DWORD Flags;
		WORD LoadCount;
		WORD TlsIndex;
		union
		{
			_LIST_ENTRY_T<T> HashLinks;
			struct
			{
				T SectionPointer;
				T CheckSum;
			};
		};
		union
		{
			T LoadedImports;
			DWORD TimeDateStamp;
		};
		T EntryPointActivationContext;
		T PatchInformation;
		_LIST_ENTRY_T<T> ForwarderLinks;
		_LIST_ENTRY_T<T> ServiceTagLinks;
		_LIST_ENTRY_T<T> StaticLinks;
		T ContextInformation;
		T OriginalBase;
		_LARGE_INTEGER LoadTime;
	};

	template <class T> struct _PEB_LDR_DATA_T
	{
		DWORD Length;
		DWORD Initialized;
		T SsHandle;
		_LIST_ENTRY_T<T> InLoadOrderModuleList;
		_LIST_ENTRY_T<T> InMemoryOrderModuleList;
		_LIST_ENTRY_T<T> InInitializationOrderModuleList;
		T EntryInProgress;
		DWORD ShutdownInProgress;
		T ShutdownThreadId;

	};

	template <class T, class NGF, int A> struct _PEB_T
	{
		union
		{
			struct
			{
				BYTE InheritedAddressSpace;
				BYTE ReadImageFileExecOptions;
				BYTE BeingDebugged;
				BYTE BitField;
			};
			T dummy01;
		};
		T Mutant;
		T ImageBaseAddress;
		T Ldr;
		T ProcessParameters;
		T SubSystemData;
		T ProcessHeap;
		T FastPebLock;
		T AtlThunkSListPtr;
		T IFEOKey;
		T CrossProcessFlags;
		T UserSharedInfoPtr;
		DWORD SystemReserved;
		DWORD AtlThunkSListPtr32;
		T ApiSetMap;
		T TlsExpansionCounter;
		T TlsBitmap;
		DWORD TlsBitmapBits[2];
		T ReadOnlySharedMemoryBase;
		T HotpatchInformation;
		T ReadOnlyStaticServerData;
		T AnsiCodePageData;
		T OemCodePageData;
		T UnicodeCaseTableData;
		DWORD NumberOfProcessors;
		union
		{
			DWORD NtGlobalFlag;
			NGF dummy02;
		};
		LARGE_INTEGER CriticalSectionTimeout;
		T HeapSegmentReserve;
		T HeapSegmentCommit;
		T HeapDeCommitTotalFreeThreshold;
		T HeapDeCommitFreeBlockThreshold;
		DWORD NumberOfHeaps;
		DWORD MaximumNumberOfHeaps;
		T ProcessHeaps;
		T GdiSharedHandleTable;
		T ProcessStarterHelper;
		T GdiDCAttributeList;
		T LoaderLock;
		DWORD OSMajorVersion;
		DWORD OSMinorVersion;
		WORD OSBuildNumber;
		WORD OSCSDVersion;
		DWORD OSPlatformId;
		DWORD ImageSubsystem;
		DWORD ImageSubsystemMajorVersion;
		T ImageSubsystemMinorVersion;
		T ActiveProcessAffinityMask;
		T GdiHandleBuffer[A];
		T PostProcessInitRoutine;
		T TlsExpansionBitmap;
		DWORD TlsExpansionBitmapBits[32];
		T SessionId;
		ULARGE_INTEGER AppCompatFlags;
		ULARGE_INTEGER AppCompatFlagsUser;
		T pShimData;
		T AppCompatInfo;
		_UNICODE_STRING_T<T> CSDVersion;
		T ActivationContextData;
		T ProcessAssemblyStorageMap;
		T SystemDefaultActivationContextData;
		T SystemAssemblyStorageMap;
		T MinimumStackCommit;
		T FlsCallback;
		_LIST_ENTRY_T<T> FlsListHead;
		T FlsBitmap;
		DWORD FlsBitmapBits[4];
		T FlsHighIndex;
		T WerRegistrationData;
		T WerShipAssertPtr;
		T pContextData;
		T pImageHeaderHash;
		T TracingFlags;
	};

	template <class T> struct _TEB_T_
	{
		_NT_TIB_T<T> NtTib;
		T EnvironmentPointer;
		_CLIENT_ID<T> ClientId;
		T ActiveRpcHandle;
		T ThreadLocalStoragePointer;
		T ProcessEnvironmentBlock;
		DWORD LastErrorValue;
		DWORD CountOfOwnedCriticalSections;
		T CsrClientThread;
		T Win32ThreadInfo;
		DWORD User32Reserved[26];
		//rest of the structure is not defined for now, as it is not needed 
	};

	typedef _LDR_DATA_TABLE_ENTRY_T<DWORD> LDR_DATA_TABLE_ENTRY32;
	typedef _LDR_DATA_TABLE_ENTRY_T<DWORD64> LDR_DATA_TABLE_ENTRY64;

	typedef _TEB_T_<DWORD> TEB32;
	typedef _TEB_T_<DWORD64> TEB64;

	typedef _PEB_LDR_DATA_T<DWORD> PEB_LDR_DATA32;
	typedef _PEB_LDR_DATA_T<DWORD64> PEB_LDR_DATA64;

	typedef _PEB_T<DWORD, DWORD64, 34> PEB32;
	typedef _PEB_T<DWORD64, DWORD, 30> PEB64;
#pragma pack(pop)

	struct X64Literal
	{
		DWORD64 content_;
		inline X64Literal(const DWORD64& src) : content_(src){ }
		inline X64Literal& operator=(DWORD64 a)
		{	
			content_ = a;
			return *this;
		}
		//inline operator const DWORD64&() const
		//{
		//	return content_;
		//}
		inline operator DWORD64() const
		{
			return content_;
		}
	};

	struct HANDLE64 : public X64Literal{ inline HANDLE64(const DWORD64&  src) : X64Literal(src){ } };
	struct HMODULE64 : public X64Literal{ inline HMODULE64(const DWORD64&  src) : X64Literal(src){ } };
	struct Ptr64 : public X64Literal{ inline Ptr64(const DWORD64&  src) : X64Literal(src){ } };

	struct expand_parameter_pack{ template<typename ...T> expand_parameter_pack(const T&...) {} };
	template<typename TPrototype>
	class Function64;
	template<typename TRet, typename... TArgs>
	class Function64 < TRet(TArgs...) >
	{
		static const int ArgCount = sizeof...(TArgs);

		static const int StackParams = ArgCount * 8;
		static const int StackReserved = max(0x20, ArgCount * 8);
		static const int StackPad = StackReserved-StackParams;
		static const int AlignMask = (StackReserved & 0x0000000F) == 0 ? 0xFFFFFFF0 : 0xFFFFFFF8;
		static_assert(((AlignMask - StackReserved) & 0x0F) == 0, "stack not aligned 16 bytes");

		Ptr64 address;

#if _WIN64
		inline static TRet Call(Ptr64 function, const TArgs&... args)
		{
			return (TRet)((DWORD64(*)(TArgs...))function.content_)(args...);
		}
#else
		inline static TRet Call(Ptr64 function, const TArgs&... args)
		{
			DWORD64 retv;
			DWORD originalEsp;
			DWORD64* params;
			__asm mov[originalEsp], esp;
			__asm and esp, AlignMask;			// 0x10 bytes alignment
												// this allows it to be properly inlined
			__asm sub esp, StackPad;			// reserved by callee
			__asm mov dword ptr[params], esp;	// recording the begining of parameter space
			__asm sub esp, StackParams;			// allocating parameter space
			expand_parameter_pack{ (*(--params) = (const DWORD64)args)... }; // translates all x86 params to x64 params

			BeginX64Stub;
			DB_(0x48) __asm mov ecx, [esp];					// mov rcx,[rsp]
			DB_(0x48) __asm mov edx, [esp + 8];				// mov rdx,[rsp+8]
			DB_(0x4C) __asm mov eax, [esp + 0x10];			// mov r8, [rsp+0x10]
			DB_(0x4C) __asm mov ecx, [esp + 0x18];			// mov r9, [rsp+0x18]
			__asm call dword ptr[function];					// call [rbp+08]
			DB_(0x48) __asm mov dword ptr[retv], eax;		// mov [ebp-??],rax
			EndX64Stub;

			__asm mov esp, [originalEsp];					// restore masked stack
			return (TRet)retv;
		}
#endif

	public:
		Function64(Ptr64 address) : address(address)
		{ }
		inline TRet operator()(const TArgs&... args){ return Call(address, args...); }
	};

	TEB64 GetTEB64();
	PEB64 GetPEB64();

	HMODULE64		GetModuleHandle64(const std::wstring& moduleName);
	Ptr64			GetProcAddress64(HMODULE64 module, const char* procName);
	HMODULE64		LoadLibrary64(const std::wstring& moduleName);

	void			memcpy64(Ptr64 /*dst*/, Ptr64 /*src*/, DWORD64 /*size*/);
	inline void		memcpy64(void* dst, DWORD64 src, DWORD size);
}