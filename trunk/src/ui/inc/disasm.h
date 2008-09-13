#define DISASMAPI	 extern "C" __declspec(dllexport) 

typedef struct _ref{
	DWORD	CA_Flag;
	DWORD	CA_Address;
} TReference,*PReference;

typedef struct _mnem{
	DWORD		Address;
	DWORD		InstLen;
	CHAR		AddrStr[50];
	CHAR		HexDump[50];
	CHAR		AsmCode	[50];
	CHAR		Comment[200];
	BOOLEAN		CA_Execute;
	DWORD		RefCount;
	TReference	References[1];
} TMnemonic,*PMnemonic;

typedef struct _mnemrow{
	ULONG		size;
	TMnemonic	mnem;
} TMnemonicRow,*PMnemonicRow;

typedef struct _mnemmass{
	ULONG			count;
	TMnemonicRow	row[1];
} TMnemonics,*PMnemonics;


DISASMAPI	PMnemonics DasmDisasmCmd(ULONG sesId,PVOID addr,ULONG size);
DISASMAPI	VOID DasmFreeBuffer(PVOID buffer);