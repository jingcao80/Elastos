#include <elastos.h>
#include <stdio.h>
#include <new>
#include <Elastos.Utility.Zip.h>
#include <Elastos.CoreLibrary.h>
#include <stdlib.h>
using namespace Elastos;

int main(int argc, char *argv[])
{
    ECode ec = NOERROR;
    ArrayOf<Byte> *buffer = ArrayOf<Byte>::Alloc(1024);
    Int32 len;

    IFileOutputStream *pfo = NULL;
    ec = CFileOutputStream::New(String("/data/data/com.elastos.runtime/elastos/tstbuffer.txt"), &pfo);
    if (FAILED(ec)) {
    	printf("Cann't Create CFileOutputStream, ec = %x\n", ec);
        return -1;
    }


    IBufferedOutputStream *pbos = NULL;
    ec = CBufferedOutputStream::New((IFileOutputStream *)pfo, &pbos);
    if (FAILED(ec)) {
    	printf("Cann't Create CZipOutputStream, ec = %x\n", ec);
        return -1;
    }

    Int32 i = 0;

    buffer->Copy((Byte *)"random", 6);
    while(i < 100) {
        ec = pbos->WriteBufferEx(0, 6, *buffer);
        if (FAILED(ec)) {
            printf("WriteBufferEx Failed! ec = %x\n", ec);
            return -1;
        }
        pbos->Flush();
        //buffer->Copy((Byte *)&i, 4);
        i++;
    }
    printf("==============================================\n");
    pbos->Close();
    return 0;

}
