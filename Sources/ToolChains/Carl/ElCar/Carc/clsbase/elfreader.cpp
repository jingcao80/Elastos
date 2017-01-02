//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#ifdef _linux
#include <sys/io.h>
#else
#include <io.h>
#endif
#include <stdio.h>

#include "clsbase.h"

typedef	unsigned short		__uint16_t;
typedef	unsigned int		__uint32_t;

typedef __uint16_t	Elf32_Half;	/* Unsigned medium integer */
typedef __uint32_t	Elf32_Word;	/* Unsigned large integer */
typedef __uint32_t	Elf32_Addr;	/* Unsigned program address */
typedef __uint32_t	Elf32_Off;	/* Unsigned file offset */

#define EI_MAG0		0		/* file ID */
#define EI_MAG1		1		/* file ID */
#define EI_MAG2		2		/* file ID */
#define EI_MAG3		3		/* file ID */
#define EI_NIDENT   16

#define	ELFMAG0		0x7f		/* e_ident[EI_MAG0] */
#define	ELFMAG1		'E'		/* e_ident[EI_MAG1] */
#define	ELFMAG2		'L'		/* e_ident[EI_MAG2] */
#define	ELFMAG3		'F'		/* e_ident[EI_MAG3] */
#define	ELFMAG		"\177ELF"	/* magic */

typedef struct elfhdr {
	unsigned char	e_ident[EI_NIDENT]; /* ELF Identification */
	Elf32_Half	e_type;		/* object file type */
	Elf32_Half	e_machine;	/* machine */
	Elf32_Word	e_version;	/* object file version */
	Elf32_Addr	e_entry;	/* virtual entry point */
	Elf32_Off	e_phoff;	/* program header table offset */
	Elf32_Off	e_shoff;	/* section header table offset */
	Elf32_Word	e_flags;	/* processor-specific flags */
	Elf32_Half	e_ehsize;	/* ELF header size */
	Elf32_Half	e_phentsize;	/* program header entry size */
	Elf32_Half	e_phnum;	/* number of program header entries */
	Elf32_Half	e_shentsize;	/* section header entry size */
	Elf32_Half	e_shnum;	/* number of section header entries */
	Elf32_Half	e_shstrndx;	/* section header table's "section
					   header string table" entry offset */
} Elf32_Ehdr;

typedef struct {
	Elf32_Word	sh_name;	/* name - index into section header
					   string table section */
	Elf32_Word	sh_type;	/* type */
	Elf32_Word	sh_flags;	/* flags */
	Elf32_Addr	sh_addr;	/* address */
	Elf32_Off	sh_offset;	/* file offset */
	Elf32_Word	sh_size;	/* section size */
	Elf32_Word	sh_link;	/* section header table index link */
	Elf32_Word	sh_info;	/* extra information */
	Elf32_Word	sh_addralign;	/* address alignment */
	Elf32_Word	sh_entsize;	/* section entry size */
} Elf32_Shdr;



typedef struct ModuleRscStruct {
    unsigned int    uSize;
	const char      uClsinfo[0];
} ModuleRscStruct;


int CheckFileFormat(FILE *pFile)
{
    int         cnt  = 0;
    Elf32_Ehdr  ehdr;

    if(fseek(pFile, 0, SEEK_SET) < 0) {
        ExtraMessage("fseek() failed");
        _ReturnError (CLSError_OpenFile);
    }

    if ((cnt = fread((char *)&ehdr, 1,sizeof(Elf32_Ehdr),pFile)) < 0) {
        ExtraMessage("fread() failed");
        _ReturnError (CLSError_OpenFile);
    }

    if ((ehdr.e_ident[EI_MAG0] != ELFMAG0) || (ehdr.e_ident[EI_MAG1] != ELFMAG1) ||
        (ehdr.e_ident[EI_MAG2] != ELFMAG2) || (ehdr.e_ident[EI_MAG3] != ELFMAG3)) {
        ExtraMessage("elf format mismatch!!\n");
        _ReturnError (CLSError_OpenFile);
    }

    _ReturnOK (CLS_NoError);;
}

int LoadResourceFromELF(const char *pszName, CLSModule **ppDest)
{
    Elf32_Ehdr      ehdr;
    Elf32_Shdr      *shdr         = NULL;
    Elf32_Shdr      *pTemShdr     = NULL;
    ModuleRscStruct *pRsc         = NULL;
    FILE            *pFile        = NULL;
    char            *pStringTable = NULL;
    char            *pSecName     = NULL;
    char            *pGetSec      = NULL;
    char            *secHeader    = NULL;
    int             cnt           = 0;
    int             rRet          = 0;
    int             nRet          = CLS_NoError;

    if (pszName == NULL) {
        char path[256];
        strcpy(path, getenv("XDK_TOOLS"));
        strcat(path, "/carc");
        pFile = fopen(path, "rb");
    }
    else {
        pFile = fopen(pszName, "rb");
    }

    if (!pFile) {
        ExtraMessage("fopen() failed!");
        _ReturnError (CLSError_OpenFile);
    }

    if (fseek(pFile, 0, SEEK_SET) < 0) {
        ExtraMessage("fseek() failed!");
        _ReturnError (CLSError_OpenFile);
    }

    if ((cnt = fread((char *)&ehdr, 1, sizeof(Elf32_Ehdr), pFile)) < 0) {
        ExtraMessage("fread() failed!");
        _ReturnError (CLSError_OpenFile);
    }

    if (fseek(pFile, ehdr.e_shoff, SEEK_SET) < 0) {
        ExtraMessage("fopen() failed!");
        _ReturnError (CLSError_OpenFile);
    }

    secHeader = (char *)malloc(sizeof(Elf32_Shdr) * ehdr.e_shnum);

    if(NULL == secHeader){
        ExtraMessage("malloc() failed!");
        nRet = CLSError_OpenFile;
        goto reterr;
    }

    if ((cnt = fread(secHeader, 1, ehdr.e_shnum * sizeof(Elf32_Shdr), pFile)) < 0) {
        ExtraMessage("fread() failed!");
        nRet = CLSError_OpenFile;
        goto reterr;
    }

    shdr         = (Elf32_Shdr *)secHeader;
    pTemShdr     = shdr + ehdr.e_shstrndx;
    pStringTable = (char *)malloc(pTemShdr->sh_size);

    if (fseek(pFile, pTemShdr->sh_offset, SEEK_SET) < 0) {
        ExtraMessage("fseek() failed!");
        nRet = CLSError_OpenFile;
        goto reterr;
    }

    if ((cnt = fread(pStringTable, 1, pTemShdr->sh_size, pFile)) < 0) {
        ExtraMessage("fread() failed!");
        nRet = CLSError_OpenFile;
        goto reterr;
    }

    pTemShdr = shdr;

    for (cnt=0; cnt<ehdr.e_shnum; cnt++){
        pSecName = pStringTable + pTemShdr->sh_name;

        if (!strncmp(pSecName, ".clsresource", 12)) {
            break;
        }

        pTemShdr ++;
    }

    if (cnt == ehdr.e_shnum) {
        ExtraMessage("resource section does not exist!");
        nRet = CLSError_OpenFile;
        goto reterr;
    }

    pGetSec = (char *)malloc(pTemShdr->sh_size);

    if(NULL == pGetSec){
        ExtraMessage("malloc() failed!");
        nRet = CLSError_OpenFile;
        goto reterr;
    }

    if (fseek(pFile, pTemShdr->sh_offset, SEEK_SET) < 0) {
        ExtraMessage("fseek() failed!");
        nRet = CLSError_OpenFile;
        goto reterr;
    }

    if ((cnt = fread(pGetSec, 1, pTemShdr->sh_size, pFile)) < 0) {
        ExtraMessage("fread() failed!");
        nRet = CLSError_OpenFile;
        goto reterr;
    }

    fclose(pFile);
    pRsc = (ModuleRscStruct *)pGetSec;
    rRet    = IsValidCLS((CLSModule *)&(pRsc->uClsinfo[0]), pRsc->uSize, pszName);
    nRet = (rRet < 0) ? rRet : RelocFlattedCLS((CLSModule *)&(pRsc->uClsinfo[0]), pRsc->uSize, ppDest);

reterr:
    if (secHeader)
        free((void *)secHeader);
    if (pGetSec)
        free(pGetSec);
    if(pStringTable)
        free(pStringTable);
    _ReturnError(nRet);
}



