#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#ifdef _linux
#include <sys/io.h>
#else
#include <io.h>
#endif
#include <sys/stat.h>
#include <stdio.h>

#ifdef _linux
#define _stricmp  strcasecmp
#endif

#define ALIGNMENT    0x8000
#define NAMELEN  260

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

char *gbuf = NULL;

typedef struct _CarcodeStruct{
    unsigned int    uMagic;
    unsigned int    bDate;
    unsigned int    cCode;
    unsigned int    uSize;
    const char      puRn[0];
}CarcodeStruct;

#define INFO_NULL 0
#define INFO_GET_BUILDDATE 1
#define INFO_GET_CARCODE 2
#define INFO_GET_URN 3
#define INFO_ALL 4
#define INFO_DEPENDENCE 5

#define RESOURCE_MATIC 0x80402010
#define RETFAIL -1

void ShowUsage(void)
{
    printf(" getDllInfo.exe: get resource infomation from dll.\n");
    printf(" Usage:          getDllInfo.exe [-b | -c | -u| -h] xxx.dll\n");
    printf(" -b              output build-date\n");
    printf(" -c              output carcode\n");
    printf(" -u              output urn\n");
    printf(" -d              output dependence\n");
    printf(" -h              output help infomation\n");
}

int CheckFileType(char *psName)
{
    int uLen;

    uLen = strlen(psName);
    if (!_stricmp(psName+uLen-4, ".eco") || !_stricmp(psName+uLen-4, ".ecx")
        || !_stricmp(psName+uLen-4, ".dll") || !_stricmp(psName+uLen-4, ".exe")) {
        return 0;
    }

    ShowUsage();
    return RETFAIL;
}

int CheckParam(char *pType)
{
    int utype;

    if (*pType != '-') {
        ShowUsage();
        return INFO_NULL;
    }

    switch (*(pType+1)) {
        case 'b':
            utype = INFO_GET_BUILDDATE;
            break;

        case 'c':
            utype = INFO_GET_CARCODE;
            break;

        case 'u':
            utype = INFO_GET_URN;
            break;

        case 'd':
            utype = INFO_DEPENDENCE;
            break;

        default:
            utype = INFO_NULL;
            ShowUsage();
            break;
    }

    return utype;
}

int CheckFileFormat(int fd, Elf32_Ehdr *ehdr)
{
    int cnt;

   if (lseek(fd, 0, SEEK_SET) < 0) {
        printf("lseek() failed! <%s,line:%d>\n", __FUNCTION__, __LINE__);
        return RETFAIL;
    }

    if ((cnt = read(fd, (char *)ehdr, sizeof(Elf32_Ehdr))) < 0) {
        printf("read() %s failed! <%s,line:%d>\n", __FUNCTION__, __LINE__);
        return RETFAIL;
    }

    if ((ehdr->e_ident[EI_MAG0] != ELFMAG0) || (ehdr->e_ident[EI_MAG1] != ELFMAG1) ||
          (ehdr->e_ident[EI_MAG2] != ELFMAG2) || (ehdr->e_ident[EI_MAG3] != ELFMAG3)) {
        printf("elf format mismatch!!\n");
        return RETFAIL;
    }

    return 0;
}

void* ReadResourceSection(int fd, Elf32_Ehdr *ehdr)
{
    int cnt;
    Elf32_Shdr *shdr;
    char *pStringTable = NULL;
    char *pName;
    Elf32_Shdr *pTemShdr;
    char *ptp = NULL;

   if (lseek(fd, ehdr->e_shoff, SEEK_SET) < 0) {
        printf("lseek() failed! <%s,line:%d>\n", __FUNCTION__, __LINE__);
        goto reterr;
    }

    gbuf = (char *)malloc(sizeof(Elf32_Shdr) * ehdr->e_shnum);
    if(NULL == gbuf){
        printf("malloc() failed! <%s,line:%d>\n", __FUNCTION__, __LINE__);
        goto reterr;
    }

    if ((cnt = read(fd, gbuf, ehdr->e_shnum * sizeof(Elf32_Shdr))) < 0) {
        printf("read() failed! <%s,line:%d>\n", __FUNCTION__, __LINE__);
        goto reterr;
    }
    shdr = (Elf32_Shdr *)gbuf;
    pTemShdr = shdr + ehdr->e_shstrndx;
    pStringTable = (char *)malloc(pTemShdr->sh_size);

   if (lseek(fd, pTemShdr->sh_offset, SEEK_SET) < 0) {
        printf("lseek() failed! <%s,line:%d>\n", __FUNCTION__, __LINE__);
        goto reterr;
    }
    if ((cnt = read(fd, pStringTable, pTemShdr->sh_size)) < 0) {
        printf("read() failed! <%s,line:%d>\n", __FUNCTION__, __LINE__);
        goto reterr;
    }

    pTemShdr = shdr;
    for (cnt=0; cnt<ehdr->e_shnum; cnt++){
        pName = pStringTable + pTemShdr->sh_name;
        if (!strncmp(pName, ".resource", 9)) {
            break;
        }
        pTemShdr ++;
    }
    if (cnt == ehdr->e_shnum) {
        printf("resource section does not exist!\n");
        goto reterr;
    }

    ptp = (char *)malloc(pTemShdr->sh_size);
    if(NULL == ptp){
        printf("malloc() failed! <%s,line:%d>\n", __FUNCTION__, __LINE__);
        goto reterr;
    }
    if (lseek(fd, pTemShdr->sh_offset, SEEK_SET) < 0) {
        printf("lseek() failed! <%s,line:%d>\n", __FUNCTION__, __LINE__);
        goto reterr;
    }
    if ((cnt = read(fd, ptp, pTemShdr->sh_size)) < 0) {
        printf("read() failed! <%s,line:%d>\n", __FUNCTION__, __LINE__);
        goto reterr;
    }

    return (void*)ptp;

reterr:
    if (ptp)
        free(ptp);
    if(pStringTable)
        free(pStringTable);
    return NULL;
}

void getdependence(const char * pDependence)
{
    const char *p;
    p = pDependence;
    printf("dependence:\n");
    while (0 != strcmp(p, "#BAB")) {
        printf("%s\n", p);
        p = p + strlen(p) + 1;
    }
}

int main(int argc, char *argv[])
{
    int fd;
    char *pFile;
    int ret;
    int iType;
    Elf32_Ehdr ehdr;
    const CarcodeStruct *pCarCode = NULL;

    if (argc == 2) {
        pFile = argv[1];
        ret = CheckFileType(pFile);
        iType = INFO_ALL;
    } else if (argc == 3) {
        pFile = argv[2];
        ret = CheckFileType(pFile);
        iType = CheckParam(argv[1]);
    } else {
        ShowUsage();
        goto err;
    }

    if (ret || iType == INFO_NULL) {
        goto err;
    }

    fd = open(pFile, O_RDONLY, 0);
    if(fd == -1) {
        printf("open %s failed! <%s,line:%d>\n", pFile, __FUNCTION__, __LINE__);
        goto err;
    }

    ret = CheckFileFormat(fd, &ehdr);
    if (ret) {
        goto err;
    }

    pCarCode = (CarcodeStruct *)ReadResourceSection(fd, &ehdr);
    if (NULL == pCarCode) {
        goto err;
    }

    if (RESOURCE_MATIC != pCarCode->uMagic) {
        printf("resource section magic mismatch!!\n");
        goto err;
    }

    switch (iType) {
        case INFO_GET_BUILDDATE:
            printf("bd:0x%x\n", pCarCode->bDate);
            break;

        case INFO_GET_CARCODE:
            printf("fp:0x%x\n", pCarCode->cCode);
            break;

        case INFO_GET_URN:
            printf("urn:%s\n", pCarCode->puRn);
            break;

        case INFO_DEPENDENCE:
            getdependence(pCarCode->puRn + pCarCode->uSize);
            break;

        case INFO_ALL:
            printf("%s resource infomation:\n", pFile);
            printf("build date: 0x%x\n", pCarCode->bDate);
            printf("carcode:    0x%x\n", pCarCode->cCode);
            printf("uRn:        %s\n", pCarCode->puRn);
            getdependence(pCarCode->puRn + pCarCode->uSize);
            break;

        default:
            printf("Error!!\n");
            break;
    }

err:
    if (gbuf)
        free((void *)gbuf);
    if (pCarCode)
        free((void *)pCarCode);
    return 0;
}



