#include <elastos.h>
#include <stdio.h>
#include <stdlib.h>
#include <new>
#include <Elastos.CoreLibrary.h>
#include <elautoptr.h>

using namespace Elastos;

AutoPtr<IFile> gFile;
AutoPtr<IDataInput> gInput;
AutoPtr<IDataOutput> gOutput;
AutoPtr<IRandomAccessFile> gRFile;


int funcReadLine()
{
   String str;
   gInput->ReadLine(&str);
   printf("the read line is %s\n", (const char *)str);
   printf("the function %s test end\n", __FUNCTION__);
   return 0;
}

int funcReadChar()
{
   Char32 pc;
   gInput->ReadChar(&pc);
   printf("\n ******** \n    the read char %c \n", pc);
   printf("the function %s test end\n", __FUNCTION__);
   return 0;
}

int funcReadByte()
{
   Byte bvalue;
   gInput->ReadByte(&bvalue);
   printf("\n ******** \n    the byte %x %c \n", bvalue, bvalue);
   printf("the function %s test end\n", __FUNCTION__);
   return 0;
}

int funcReadBoolean()
{
   Boolean bvalue;
   gInput->ReadBoolean(&bvalue);
   printf("\n ******* \n the boolean is %d\n", bvalue);
   printf("the function %s test end\n", __FUNCTION__);
   return 0;
}

int funcReadFullyEx()
{
   ArrayOf_<Byte, 7> buffer;
   gInput->ReadFullyEx(0, 7, &buffer);
   printf("the Byte is %s \n", (const char *)buffer.GetPayload());
   return 0;
}

int funcWriteBoolean()
{
    Boolean bvalue = TRUE;
    gOutput->WriteBoolean((Int32)bvalue);
    return 0;
}

int funcWriteBytes()
{
    ArrayOf_<Byte, 7> buffer;
    buffer.Copy((Byte*)"bcdefgh", 7);
    gOutput->WriteBytes(buffer);
    return 0;
}

int funcWriteByte()
{
    Byte by = 'm';
    gOutput->WriteByte((Int32)by);
    return 0;
}

int initRandomAccessFile(String str)
{
    AutoPtr<IFile> file;
    ECode ec = CFile::New(str, (IFile**)&file);
    if (FAILED(ec)) {
        printf("Cann't Open the file\n");
 	      return -1;
    }

    ec = CRandomAccessFile::New(file, "rw", (IRandomAccessFile**)&gRFile);
    if (FAILED(ec)) {
        printf("Cann't construct Random file\n");
 	      return -1;
    }

    gInput = IDataInput::Probe(gRFile);
    gOutput = IDataOutput::Probe(gRFile);
    return 0;
}


int main(int argc, char *argv[])
{
    if (argc < 2) {
         printf("the arg number is error!\n");
         printf("please input test case number such as\n");
         printf("    ./testRandomAccessFile 1 \n");
         return -1;
    }

    Int32 caseno = atoi(argv[1]);
    printf("the case no is %d\n", caseno);

    String str("/data/data/com.elastos.runtime/elastos/random.txt");
    initRandomAccessFile(str);

    switch(caseno) {
        case 1:
             funcReadLine();
             break;
        case 2:
	           funcReadChar();
             break;
        case 3:
	           funcReadByte();
             break;
        case 4:
             funcWriteBoolean();
             funcWriteBytes();
             funcWriteByte();
             gRFile->Close();
             initRandomAccessFile(str);
             funcReadBoolean();
             funcReadFullyEx();
             funcReadByte();
             gRFile->Close();
    }
    return 0;
}
