#include <elastos.h>
#include <stdio.h>
#include <new>
#include <Elastos.Utility.Zip.h>
#include <Elastos.CoreLibrary.h>
#include <elautoptr.h>
#include <sys/stat.h>
#include <sys/types.h>
using namespace Elastos;

int main(int argc, char *argv[])
{
    printf("\n\n\n\n\n");
    printf("\n\n\n\n\n");
    printf("\n\n\n\n\n");
    printf("\n\n\n\n\n");
    printf("\n\n\n\n\n");
    printf("\n\n\n\n\n");
    IBufferedOutputStream *dest = NULL;
    ECode ec;
    char abpath[256] = {0};
    strcpy(abpath, "/data/data/com.elastos.runtime/");
    char path[256] = {0};


    IFileInputStream *fis = NULL;
    ec = CFileInputStream::New(String("/data/data/com.elastos.runtime/elastos.zip"), &fis);
    if (FAILED(ec)) {
        printf("ec is %x\n", ec);
        return -1;
    }

    IZipInputStream *zis = NULL;
    ec =CZipInputStream::New(fis, &zis);
    if (FAILED(ec)) {
        printf("ec is %x\n", ec);
        return -1;
    }

    IZipEntry *pentry = NULL;
    ec = zis->GetNextEntry(&pentry);
    if (FAILED(ec)) {
        printf("ec is %x\n", ec);
        return -1;
    }

    Int32 dflag;
    ArrayOf<Byte> *buf = ArrayOf<Byte>::Alloc(256);

    while(ec == NOERROR && pentry != NULL) {
        String name;
        pentry->GetName(&name);
        //printf("======%s========\n", (const char *)name);
        AutoPtr<IFile> outfile;

        AutoPtr<IFileOutputStream> pos;
        strcpy(path, abpath);
        strcat(path, (const char *)name);

        if (path[strlen(path) - 1] == '/') {
             path[strlen(path)-1] = '\0';
             if(mkdir(path, S_IRWXU) == 0) {
                 dflag = 1;
                 goto NEXT;
             }
             else {
                 printf("create dir failed\n");
             }
        } else {
            dflag = 0;
        }
        ec = CFile::New(String(path), (IFile **)&outfile);
        if (FAILED(ec)) {
            printf("Cann't Open the File the name is %s\n", path);
            return -1;
        }

        ec = CFileOutputStream::New((IFile *)outfile, (IFileOutputStream **)&pos);
        if (FAILED(ec)) {
            return -1;
        }

        Int32 len;

NEXT:
        ec = zis->ReadBufferEx(0, 256, buf, &len);
        printf("ec is %x, %d\n", ec, len);
        while(ec == NOERROR && len > 0) {
             printf("%s, %d\n", __FILE__, __LINE__);
             pos->WriteBufferEx(0, len , *buf);
             ec = zis->ReadBufferEx(0, 256, buf, &len);
        }
        if (!dflag)
            pos->Close();
        //ArrayOf<Byte>::Free(buf);
        //if (pentry->IsDirectory()) {
        //    printf("this is a directory \n");
        //}
        ec = zis->GetNextEntry(&pentry);
    }
/*
    IFileOutputStream *pos = NULL;
    IFile *outfile = NULL;
    ec = CFile::New(String("/data/data/com.elastos.runtime/elastos/a.pdf"),&outfile);
    if (FAILED(ec)) {
        printf("Cann't Create CFile a.pdf\n");
        return -1;
    }
    ec = CFileOutputStream::New(outfile, &pos);
    if (FAILED(ec)) {
        printf("create output stream ec is %x\n", ec);
        return -1;
    }

    ArrayOf<Byte> *buf = ArrayOf<Byte>::Alloc(1024);
    while(ec == NOERROR && pentry != NULL) {
    	Int32 len = 0;
    	ec = zis->ReadBufferEx(0, 1024, buf, &len);
    	while( ec == NOERROR && len > 0) {
            pos->WriteBufferEx(0, len, *buf);
            zis->ReadBufferEx(0,1024,buf, &len);
    	}
    }
*/

    zis->Close();
    return 0;

}


