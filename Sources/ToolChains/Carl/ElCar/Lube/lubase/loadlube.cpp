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

#include <stdlib.h>
#ifdef _linux
#include <sys/io.h>
#else
#include <io.h>
#include <windows.h>
#endif
#include <malloc.h>
#include <lube.h>

#ifdef _linux
#define _MAX_PATH 256
#endif

static const char *s_pszLubePath = "";
const char *g_pszOutputPath = "";
static int s_nOffset;

static const CLSID CLSID_XOR_CallbackSink = \
/* e724df56-e16a-4599-8edd-a97ab245d583 */
{0xe724df56,0xe16a,0x4599,{0x8e,0xdd,0xa9,0x7a,0xb2,0x45,0xd5,0x83}};

extern int AddCLSLibrary(CLSModule *);
extern int RetrieveClass(const char *, CLSModule *, BOOL);
extern int RetrieveInterface(const char *, CLSModule *, BOOL);
extern int RetrieveStruct(const char *, CLSModule *, BOOL);
extern int RetrieveEnum(const char *, const char *, CLSModule *, BOOL);
extern int RetrieveAlias(const char *, CLSModule *, BOOL);
extern int MergeCLS(const CLSModule *, CLSModule *);
extern int RetrieveIdentifyType(
                const char *, const char *, CLSModule *, TypeDescriptor *, BOOL);
extern void DestroyAllLibraries();

void RelocateState(PSTATEDESC pDesc)
{
    if (pDesc->pBlockette) {
        pDesc->pBlockette = (PSTATEDESC)((int)pDesc->pBlockette + s_nOffset);
        RelocateState(pDesc->pBlockette);
    }
    if (pDesc->pNext) {
        pDesc->pNext = (PSTATEDESC)((int)pDesc->pNext + s_nOffset);
        RelocateState(pDesc->pNext);
    }
    if (pDesc->pvData) {
        pDesc->pvData = (PVOID)((int)pDesc->pvData + s_nOffset);
    }
}

void RelocateTemplate(LubeTemplate *pTemplate)
{
    pTemplate->mName = (char *)((int)pTemplate->mName + s_nOffset);
    if (pTemplate->tRoot.pBlockette) {
        pTemplate->tRoot.pBlockette = (PSTATEDESC)
                    ((int)pTemplate->tRoot.pBlockette + s_nOffset);
        RelocateState(pTemplate->tRoot.pBlockette);
    }
}

void RelocateLube(PLUBEHEADER pLube)
{
    int n;

    s_nOffset = (int)pLube;

    pLube->ppTemplates = (LubeTemplate **)
                ((int)pLube->ppTemplates + s_nOffset);
    for (n = 0; n < pLube->cTemplates; n++) {
        pLube->ppTemplates[n] = (LubeTemplate *)
                    ((int)pLube->ppTemplates[n] + s_nOffset);
        RelocateTemplate(pLube->ppTemplates[n]);
    }
}

int RelocFlattedLube(PLUBEHEADER pSrc, int nSize, PLUBEHEADER *ppDest)
{
    PLUBEHEADER pDest;

    if (pSrc->dwMagic != c_dwLubeMagic) {
        fprintf(stderr, "[ERROR] lube (0x0301) : Illegal format of resource.\n");
        return LUBE_FAIL;
    }
    pDest = (PLUBEHEADER)new char[nSize];
    if (!pDest) {
        fprintf(stderr, "[ERROR] lube (0x0302) : Out of memory.\n");
        return LUBE_FAIL;
    }
    memcpy(pDest, pSrc, nSize);

    RelocateLube(pDest);

    *ppDest = pDest;
    return LUBE_OK;
}

void SetLubePath(const char *pszPath)
{
    s_pszLubePath = pszPath;
}

void SetOutputPath(const char *pszPath)
{
    g_pszOutputPath = pszPath;
}

#ifdef _win32
int LoadLubeFromDll(const char *pszName, PLUBEHEADER *ppLube)
{
    int nSize;
    HRSRC hResInfo;
    HGLOBAL hRes;
    LPVOID lpLockRes;
    HMODULE handle;

    if (pszName) {
        handle = LoadLibraryExA(pszName, NULL, LOAD_LIBRARY_AS_DATAFILE);
        if (!handle) goto ErrorExit;
    }
    else {
        handle = NULL;
    }

    hResInfo = FindResourceA(handle, "#1", "Lube");
    if (!hResInfo) goto ErrorExit;

    nSize = SizeofResource(handle, hResInfo);
    if (0 == nSize) goto ErrorExit;

    hRes = LoadResource(handle, hResInfo);
    if (!hRes) goto ErrorExit;

    lpLockRes = LockResource(hRes);
    if (!lpLockRes) goto ErrorExit;

    return RelocFlattedLube((PLUBEHEADER)lpLockRes, nSize, ppLube);

ErrorExit:
    fprintf(stderr, "[ERROR] lube (0x0303 : Can't load lube resource from file.\n");
    return LUBE_FAIL;
}
#endif

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

int LoadLubeFromELF(const char *pszName, PLUBEHEADER *ppLube)
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
    // int             rRet          = 0;
    int             nRet          = CLS_NoError;

    if (pszName == NULL) {
        char path[256];
        strcpy(path, getenv("CAR_TOOLS"));
        strcat(path, "/lube");
        pFile = fopen(path, "rb");
    }
    else {
        pFile = fopen(pszName, "rb");
    }

    if (!pFile) {
        return LUBE_FAIL;
    }

    if (fseek(pFile, 0, SEEK_SET) < 0) {
        return LUBE_FAIL;
    }

    if ((cnt = fread((char *)&ehdr, 1, sizeof(Elf32_Ehdr), pFile)) < 0) {
        return LUBE_FAIL;
    }

    if (fseek(pFile, ehdr.e_shoff, SEEK_SET) < 0) {
        return LUBE_FAIL;
    }

    secHeader = (char *)malloc(sizeof(Elf32_Shdr) * ehdr.e_shnum);

    if(NULL == secHeader){
        nRet = LUBE_FAIL;
        goto reterr;
    }

    if ((cnt = fread(secHeader, 1, ehdr.e_shnum * sizeof(Elf32_Shdr), pFile)) < 0) {
        nRet = LUBE_FAIL;
        goto reterr;
    }

    shdr         = (Elf32_Shdr *)secHeader;
    pTemShdr     = shdr + ehdr.e_shstrndx;
    pStringTable = (char *)malloc(pTemShdr->sh_size);

    if (fseek(pFile, pTemShdr->sh_offset, SEEK_SET) < 0) {
        nRet = LUBE_FAIL;
        goto reterr;
    }

    if ((cnt = fread(pStringTable, 1, pTemShdr->sh_size, pFile)) < 0) {
        nRet = LUBE_FAIL;
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
        nRet = LUBE_FAIL;
        goto reterr;
    }

    pGetSec = (char *)malloc(pTemShdr->sh_size);

    if(NULL == pGetSec){
        nRet = LUBE_FAIL;
        goto reterr;
    }

    if (fseek(pFile, pTemShdr->sh_offset, SEEK_SET) < 0) {
        nRet = LUBE_FAIL;
        goto reterr;
    }

    if ((cnt = fread(pGetSec, 1, pTemShdr->sh_size, pFile)) < 0) {
        nRet = LUBE_FAIL;
        goto reterr;
    }

    fclose(pFile);
    pRsc = (ModuleRscStruct *)pGetSec;
//    rRet    = IsValidCLS((CLSModule *)&(pRsc->uClsinfo[0]), pRsc->uSize, pszName);
//    nRet = (rRet < 0) ? rRet : RelocFlattedCLS((CLSModule *)&(pRsc->uClsinfo[0]), pRsc->uSize, ppDest);

    nRet = RelocFlattedLube((PLUBEHEADER)&(pRsc->uClsinfo[0]), pRsc->uSize, ppLube);

reterr:
    if (secHeader)
        free((void *)secHeader);
    if (pGetSec)
        free(pGetSec);
    if(pStringTable)
        free(pStringTable);
    return nRet;
}

int LoadLubeFromFile(const char *pszName, PLUBEHEADER *ppLube)
{
    int nSize, nRet = LUBE_FAIL;
    FILE *pFile;
    PLUBEHEADER pLube;

    pFile = fopen(pszName, "rb");
    if (!pFile) {
        fprintf(stderr, "[ERROR] lube (0x0304 : Can't open file %s.\n", pszName);
        return LUBE_FAIL;
    }

#ifdef _linux
    fseek(pFile, 0, SEEK_END);
    nSize = ftell(pFile);
#else
    nSize = _filelength(pFile->_file);
#endif
    pLube = (PLUBEHEADER)new char[nSize];
    if (!pLube) {
        fprintf(stderr, "[ERROR] lube (0x0305 : Out of memory.\n");
        goto ErrorExit;
    }
    fread(pLube, nSize, 1, pFile);

    nRet = RelocFlattedLube(pLube, nSize, ppLube);
    delete [] (char *)pLube;

ErrorExit:
    fclose(pFile);
    return nRet;
}

int LoadLube(const char *pszName, PLUBEHEADER *ppLube)
{
    int n;
    char szResult[_MAX_PATH];

    if (!pszName) {
#ifdef _linux
    	return LoadLubeFromELF(NULL, ppLube);
#else
    	return LoadLubeFromDll(NULL, ppLube);
#endif
    }

    n = SearchFileFromPath(s_pszLubePath, pszName, szResult);
    if (n < 0) return n;

    n = strlen(pszName);

    if (!_stricmp(pszName + n - 4, ".lbo")) {
        return LoadLubeFromFile(szResult, ppLube);
    }
    else if (!_stricmp(pszName + n - 4, ".eco")) {
#ifdef _linux
        return LoadLubeFromELF(szResult, ppLube);
#else
        return LoadLubeFromDll(szResult, ppLube);
#endif
    }
    return LUBE_FAIL;
}

