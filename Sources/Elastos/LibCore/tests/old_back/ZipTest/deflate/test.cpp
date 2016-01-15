#include <elastos.h>
#include <stdio.h>
#include <new>
#include <Elastos.Utility.Zip.h>
#include <Elastos.CoreLibrary.h>
using namespace Elastos;

int main(int argc, char *argv[])
{
    IFile *file = NULL;
    ECode ec = NOERROR;

    ec = CFile::New(String("/data/data/com.elastos.runtime/elastos/android.pdf"), &file);
    if (FAILED(ec)) {
        printf("Cann't Create File\n");
        return -1;
    }


    IFileInputStream *pfs = NULL;
    ec = CFileInputStream::New((IFile *)file, &pfs);
    if (FAILED(ec)) {
    	printf("Cann't open CFileInputStream, ec = %x\n", ec);
        return -1;
    }

    IBufferedInputStream *pbis = NULL;
    ec = CBufferedInputStream::New((IFileInputStream *) pfs, &pbis);
    if (FAILED(ec)) {
    	printf("Cann't open CFileInputStream, ec = %x\n", ec);
        return -1;
    }


    ArrayOf<Byte> *buffer = ArrayOf<Byte>::Alloc(1024);
    Int32 len;

    IFileOutputStream *pfo = NULL;
    ec = CFileOutputStream::New(String("/data/data/com.elastos.runtime/elastos/android.zip"), &pfo);
    if (FAILED(ec)) {
    	printf("Cann't Create CFileOutputStream, ec = %x\n", ec);
        return -1;
    }


    /*IBufferedOutputStream *pbos = NULL;
    ec = CBufferedOutputStream::New((IFileOutputStream *)pfo, &pbos);
    if (FAILED(ec)) {
    	printf("Cann't Create CZipOutputStream, ec = %x\n", ec);
        return -1;
    }
    */
    IZipOutputStream *pzos = NULL;
    ec = CZipOutputStream::New(pfo, &pzos);
    if (FAILED(ec)) {
    	printf("Cann't Create CZipOutputStream, ec = %x\n", ec);
        return -1;
    }


    printf("%s, %d\n", __FILE__, __LINE__);
        printf("------------------------------");
        IZipEntry *entry = NULL;
        ec = CZipEntry::New(String("android.pdf"), &entry);
        if (FAILED(ec)) {
            printf("%s, %d\n", __FILE__, __LINE__);
            return -1;
        } else {
            printf("%s, %d\n", __FILE__, __LINE__);
        }
        ec = pzos->PutNextEntry(entry);
        if (FAILED(ec)) {
            printf("%s, %d\n", __FILE__, __LINE__);
            return -1;
        } else {
            printf("PutEntry Successed\n");
        }
    ec = pbis->ReadBufferEx(0, 256, buffer,  &len);
    printf("%s, %d, %d\n", __FILE__, __LINE__, len);
    while(len != -1 && ec == NOERROR) {
        ec = pzos->WriteBufferEx(0, len, *buffer);
        if (FAILED(ec)) {
            printf("WriteBufferEx Failed! ec = %x\n", ec);
            return -1;
        }
        pzos->Flush();
        //pzos->Finish();
        len = 0;
        ec = pbis->ReadBufferEx(0, 256, buffer,  &len);
    }


    printf("&&&&&&&&%s, %d\n", __FILE__, __LINE__);
    //pzos->Finish();
    printf("&&&&&&&&%s, %d\n", __FILE__, __LINE__);
    pbis->Close();
    printf("&&&&&&&&%s, %d\n", __FILE__, __LINE__);
    pzos->Close();
}
