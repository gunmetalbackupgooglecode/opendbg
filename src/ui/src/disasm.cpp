/*
    *    
    * Copyright (c) 2008 
    * d1mk4 <d1mk4@bk.ru> 
    *

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "precomp.h"

#include  "cadt.h"
#include  "disasm.h"
//#include  "ngtracerapi.h"

ULONG Disasm(PVOID addr,PVOID trueAddr,PMnemonic pMnem)
{
	ULONG			result=0;
	ULONG			Len;
	TDisCommand		Command;
	TInstruction	Instr;
	TMnemonicOptios Options;
	PMnemonic	mnem=pMnem;

	static char addrConv[50];
	static std::string str_hex;

	memset(&Instr,0,sizeof(Instr));
	memset(&Options,0,sizeof(Options));
	memset(&Command,0,sizeof(Command));

	if(mnem)
	{
		Options.RealtiveOffsets = TRUE;
		Options.AddAddresPart   = FALSE;
		Options.AlternativeAddres = 0;
		Options.AddHexDump = TRUE;
		Options.MnemonicAlign = 18;
		Len = InstrDecode(addr, &Instr, FALSE);
		mnem->InstLen=Len;
		memcpy(&mnem->HexDump,addr,Len);
		InstrDasm(&Instr, &Command, FALSE);

		Options.AlternativeAddres=(ULONG)trueAddr;
		MakeMnemonic(mnem->AsmCode, &Command, &Options);

		sprintf(addrConv, "%0.8X", trueAddr);

		strncpy(mnem->AddrStr, addrConv, 50);

		size_t asmCodeLen = strlen(mnem->AsmCode);

		bool end_hex = false;
		for (size_t i = 0; i < asmCodeLen; ++i)
		{
			str_hex += mnem->AsmCode[i];

			if(!end_hex)
				if(mnem->AsmCode[i] == ' ')
			{
					strncpy(mnem->HexDump, str_hex.c_str(), 50);
					str_hex.clear();
					end_hex = true;
			}
		
			if(end_hex)
				if(i == asmCodeLen-1)
			{
				memset(mnem->AsmCode, 0, 50*sizeof(mnem->AsmCode[0]));
				strncpy(mnem->AsmCode, str_hex.c_str(), 50);
				str_hex.clear();
			}
		}

		result=1;
	}

	return result;
}

ULONG	GetCmdCount(PVOID addr,ULONG size){
	ULONG count=0;
	ULONG i=0;
	ULONG origSize=size-16;
	ULONG Len=0;
	TDisCommand		Command;
	TInstruction	Instr;
	TMnemonicOptios Options;
	PBYTE p=(PBYTE)addr;
	
	memset(&Options,0,sizeof(Options));
	memset(&Command,0,sizeof(Command));
	memset(&Instr,0,sizeof(Instr));

	while((Len=InstrDecode(p,&Instr,FALSE)) ){
		memset(&Instr,0,sizeof(Instr));
		p+=Len;
		i+=Len;
		count++;
		if(i>=origSize) break;
	}
return count;
}

DISASMAPI	PMnemonics DasmDisasmCmd(ULONG sesId,PVOID addr,ULONG size)
{
	PBYTE		buffer,b;
	PMnemonics	mnem=0;
	//PBYTE		p=0;
	BOOLEAN		oneCmd=FALSE;
	ULONG		trueAddr=(ULONG)addr;
	if(!size) {oneCmd=TRUE;}
	size+=16;
		buffer=(PBYTE)malloc(size);
		memset(buffer,0,size);
	
	ULONG instCount=0,i=0;

	if(trc_read_memory(sesId,addr,buffer,size))
	{
		if(oneCmd)
			instCount=1;
		else
			instCount=GetCmdCount(buffer,size);

		if(instCount)
		{
			mnem=(PMnemonics)malloc(instCount*sizeof(TMnemonicRow)+4);
			if(mnem)
			{
				b=buffer;
				mnem->count=instCount;
				//p=(PBYTE)(mnem+4);
				while(instCount--)
				{
					if(Disasm(b,(PVOID)trueAddr,&mnem->row[i].mnem))
					{
						mnem->row[i].size=sizeof(TMnemonic);
						b+=mnem->row[i].mnem.InstLen;
						trueAddr+=mnem->row[i].mnem.InstLen;
						i++;
					}
				}
			}
		}
		
	}

return mnem;	
}

DISASMAPI	VOID DasmFreeBuffer(PVOID buffer)
{
	if(buffer)
	{
		free(buffer);
		buffer = NULL;
	}
}
