#include <elastos.h>
#include <stdio.h>
#include <new>
#include <Elastos.Utility.Zip.h>
#include <Elastos.CoreLibrary.h>
using namespace Elastos;

int main(int argc, char *argv[])
{
    ECode ec;

    IFileInputStream *fis = NULL;
    ec = CFileInputStream::New(String("/data/data/com.elastos.runtime/elastos/android.zip"), &fis);
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

    String name;
    Int64 size;
    pentry->GetCompressedSize(&size);
    pentry->GetName(&name);

    Int64 crc;
    pentry->GetCrc(&crc);

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
    Int32 len = 0;
    ec = zis->ReadBufferEx(0, 1024, buf, &len);
    while( ec == NOERROR && len > 0) {
        pos->WriteBufferEx(0, len, *buf);
        zis->ReadBufferEx(0,1024,buf, &len);
    }

    zis->Close();
    pos->Close();
    //dest->Flush();
    printf("%s, %d\n", __FILE__, __LINE__);
    //dest->Close();
    return 0;

}


