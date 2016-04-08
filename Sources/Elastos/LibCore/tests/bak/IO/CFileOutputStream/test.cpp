
#include <elastos.h>
#include <stdio.h>
#include <elautoptr.h>
#include "Elastos.IO2.h"

using namespace Elastos;

int main(int argc, char *argv[])
{
    printf("==== File: %s, Line: %d ====\n", __FILE__, __LINE__);
    String fp = String("/data/data/com.elastos.runtime/elastos/iotest.txt");
    AutoPtr<IFileOutputStream> fos;
    CFileOutputStream::New(fp, (IFileOutputStream**)&fos);
    printf("==== File: %s, Line: %d ====\n", __FILE__, __LINE__);
    fos->Write(65);
    fos->Write(66);
    fos->Write(67);
    printf("==== File: %s, Line: %d ====\n", __FILE__, __LINE__);
    ArrayOf_<Byte, 3> bytes;
    bytes[0] = 68;
    bytes[1] = 69;
    bytes[2] = 70;
    fos->WriteBufferEx(0, 3, bytes);
    printf("==== File: %s, Line: %d ====\n", __FILE__, __LINE__);
    return 0;
}
