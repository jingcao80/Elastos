
#include "stdio.h"
#include "Elastos.IO2.h"
#include "elastos.h"

#define   PRINTF_OK(a)            printf("%s-------OK!\n",a);
#define   PRINTF_ERROR(a)         printf("%s-------ERROR,LINE=%d!\n",a,__LINE__);
#define   FAIL_RETURN(ec,reason)  if(ec != NOERROR)          \
                                  {                          \
                                      PRINTF_ERROR(reason);  \
                                      return;                \
                                  }

using namespace Elastos;

void test_constructor()
{
    Int32 size = 0;
    ECode ec = 0;
    ICharArrayWriter *icaw;
    ICharArrayWriter *icaw2;

    //test default constructor
    ec = CCharArrayWriter::New(&icaw);
    FAIL_RETURN(ec,"CCharArrayWriter default constructor");
    ec = icaw->GetSize(&size);
    FAIL_RETURN(ec,"icaw->GetSize");
    if(size != 0){PRINTF_ERROR("default constructor"); return;}

    //test the constructor with parameters
    size = 90;
    ec = CCharArrayWriter::New(size, &icaw2);
    FAIL_RETURN(ec,"CCharArrayWriter constructor with parameters");
    ec = icaw2->GetSize(&size);
    FAIL_RETURN(ec,"icaw2->GetSize");
    if(size != 0){PRINTF_ERROR("constructor with parameters"); return;}

    PRINTF_OK("test_constructor");
}

void test_Close()
{
    ICharArrayWriter *icaw;
    ECode ec = 0;

    ec = CCharArrayWriter::New(&icaw);
    FAIL_RETURN(ec,"CCharArrayWriter default constructor");
    ec = icaw->Close();
    FAIL_RETURN(ec,"icaw->Close");

    PRINTF_OK("test_Close");
}

void test_Flush()
{
    ICharArrayWriter *icaw;
    ECode ec = 0;

    ec = CCharArrayWriter::New(&icaw);
    FAIL_RETURN(ec,"CCharArrayWriter default constructor");
    ec = icaw->Flush();
    FAIL_RETURN(ec,"icaw->Flush");

    PRINTF_OK("test_Flush");
}

void test_Reset()
{
    ICharArrayWriter *icaw;
    ECode ec = 0;
    Int32 value = 'a';
    Int32 size =0;

    ec = CCharArrayWriter::New(&icaw);
    FAIL_RETURN(ec,"CCharArrayWriter default constructor");
    ec = icaw->Write(value);
    FAIL_RETURN(ec,"icaw->Write");
    ec = icaw->GetSize(&size);
    FAIL_RETURN(ec,"icaw->GetSize");
    if(size != 1) {PRINTF_ERROR("icaw->GetSize"); return;}
    ec = icaw->Reset();
    FAIL_RETURN(ec,"ibais->Reset");
    ec = icaw->GetSize(&size);
    FAIL_RETURN(ec,"icaw->GetSize");
    if(size != 0) {PRINTF_ERROR("icaw->GetSize"); return;}

    PRINTF_OK("test_Reset");
}

void test_GetSize()
{
    ICharArrayWriter *icaw;
    ECode ec = 0;
    Int32 value = 'a';
    Int32 size =0;

    ec = CCharArrayWriter::New(&icaw);
    FAIL_RETURN(ec,"CCharArrayWriter default constructor");
    ec = icaw->GetSize(&size);
    FAIL_RETURN(ec,"icaw->GetSize");
    if(size != 0) {PRINTF_ERROR("icaw->GetSize"); return;}
    ec = icaw->Write(value);
    FAIL_RETURN(ec,"icaw->Write");
    ec = icaw->Write(value);
    FAIL_RETURN(ec,"icaw->Write");
    ec = icaw->GetSize(&size);
    FAIL_RETURN(ec,"icaw->GetSize");
    if(size != 2) {PRINTF_ERROR("icaw->GetSize"); return;}

    PRINTF_OK("test_GetSize");
}

void test_ToByteArray()
{
    Byte acarray[7]={'a','b','c','d','e','f','g'};
    const BufferOf<Byte> myArray2(acarray,7);
    ICharArrayWriter *icaw;
    ECode ec = 0;
    BufferOf<Byte>* byteArray;

    ec = CCharArrayWriter::New(&icaw);
    FAIL_RETURN(ec,"CCharArrayWriter default constructor");
    ec = icaw->WriteBufferEx(0, 7, myArray2);
    FAIL_RETURN(ec,"icaw->WriteBufferEx");
    ec = icaw->ToCharArray(&byteArray);
    FAIL_RETURN(ec,"icaw->ToCharArray");

    for (Int32 i = 0; i < 7; i++) {
        if(myArray2.GetPayload()[i] != (*byteArray).GetPayload()[i]) {PRINTF_ERROR("test_ToByteArray"); return;}
    }

    PRINTF_OK("test_ToByteArray");
}

void test_Write()
{
    ICharArrayWriter *icaw;
    ICharArrayReader *icar;
    ECode ec = 0;
    Int32 value = 0;
    BufferOf<Byte>* byteArray;

    ec = CCharArrayWriter::New(&icaw);
    FAIL_RETURN(ec,"CByteArrayOutputStream default constructor");
    ec = icaw->Write('T');
    FAIL_RETURN(ec,"icaw->Write");
    ec = icaw->ToCharArray(&byteArray);
    FAIL_RETURN(ec,"icaw->ToCharArray");
    ec = CCharArrayReader::New(byteArray, &icar);
    ec = icar->Read(&value);
    FAIL_RETURN(ec,"icar->Read");
    if(value != 'T') {PRINTF_ERROR("test_Read"); return;}

    PRINTF_OK("test_Write");
}

void test_WriteBufferEx()
{
    Byte acarray[7]={'a','b','c','d','e','f','g'};
    const BufferOf<Byte> myArray2(acarray,7);
    ICharArrayWriter *icaw;
    ECode ec = 0;
    BufferOf<Byte>* byteArray;

    ec = CCharArrayWriter::New(&icaw);
    FAIL_RETURN(ec,"CCharArrayWriter default constructor");
    ec = icaw->WriteBufferEx(0, 7, myArray2);
    FAIL_RETURN(ec,"icaw->WriteBufferEx");
    ec = icaw->ToCharArray(&byteArray);
    FAIL_RETURN(ec,"icaw->ToCharArray");

    for (Int32 i = 0; i < 7; i++) {
        if(myArray2[i] != (*byteArray).GetPayload()[i]) {PRINTF_ERROR("test_WriteBufferEx"); return;}
    }

    PRINTF_OK("test_WriteBufferEx");
}

int main() {
    printf("\n");
    printf("=======testing CCharArrayWriter=========\n");
    printf("============================================\n");
    test_constructor();
    test_Close();
    test_Flush();
    test_Reset();
    test_GetSize();
    test_ToByteArray();
    test_Write();
    test_WriteBufferEx();
    printf("============================================\n");
    printf("============end=============================\n");
}
