
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
    IByteArrayOutputStream *ibaos;
    IByteArrayOutputStream *ibaos2;

    //test default constructor
    ec = CByteArrayOutputStream::New(&ibaos);
    FAIL_RETURN(ec,"CByteArrayOutputStream default constructor");
    ec = ibaos->GetSize(&size); 
    FAIL_RETURN(ec,"ibaos->GetSize");
    if(size != 0){PRINTF_ERROR("default constructor"); return;}

    //test the constructor with parameters
    size = 100;
    ec = CByteArrayOutputStream::New(size, &ibaos2);
    FAIL_RETURN(ec,"CByteArrayOutputStream constructor with parameters");
    ec = ibaos2->GetSize(&size);
    FAIL_RETURN(ec,"ibaos2->GetSize");
    if(size != 0){PRINTF_ERROR("constructor with parameters"); return;}

    PRINTF_OK("test_constructor");
}

void test_Close()
{
    IByteArrayOutputStream *ibaos;
    ECode ec = 0;

    ec = CByteArrayOutputStream::New(&ibaos);
    FAIL_RETURN(ec,"CByteArrayOutputStream default constructor");
    ec = ibaos->Close();
    FAIL_RETURN(ec,"ibaos->Close");

    PRINTF_OK("test_Close");
}

void test_Reset()
{
    IByteArrayOutputStream *ibaos;
    ECode ec = 0;
    Int32 value = 'a';
    Int32 size =0;

    ec = CByteArrayOutputStream::New(&ibaos);
    FAIL_RETURN(ec,"CByteArrayOutputStream default constructor");
    ec = ibaos->Write(value);
    FAIL_RETURN(ec,"ibaos->Write");
    ec = ibaos->GetSize(&size);
    FAIL_RETURN(ec,"ibaos->GetSize");
    if(size != 1) {PRINTF_ERROR("ibaos->GetSize"); return;}
    ec = ibaos->Reset();
    FAIL_RETURN(ec,"ibais->Reset");
    ec = ibaos->GetSize(&size);
    FAIL_RETURN(ec,"ibaos->GetSize");
    if(size != 0) {PRINTF_ERROR("ibaos->GetSize"); return;}

    PRINTF_OK("test_Reset");
}

void test_GetSize()
{
    IByteArrayOutputStream *ibaos;
    ECode ec = 0;
    Int32 value = 'a';
    Int32 size =0;

    ec = CByteArrayOutputStream::New(&ibaos);
    FAIL_RETURN(ec,"CByteArrayOutputStream default constructor");
    ec = ibaos->Write(value);
    FAIL_RETURN(ec,"ibaos->Write");
    ec = ibaos->Write(value);
    FAIL_RETURN(ec,"ibaos->Write");
    ec = ibaos->GetSize(&size);
    FAIL_RETURN(ec,"ibaos->GetSize");
    if(size != 2) {PRINTF_ERROR("ibaos->GetSize"); return;}
    ec = ibaos->Reset();
    FAIL_RETURN(ec,"ibais->Reset");
    ec = ibaos->GetSize(&size);
    FAIL_RETURN(ec,"ibaos->GetSize");
    if(size != 0) {PRINTF_ERROR("ibaos->GetSize"); return;}

    PRINTF_OK("test_GetSize");
}

void test_ToByteArray()
{
    Byte acarray[7]={'a','b','c','d','e','f','g'};
    const BufferOf<Byte> myArray2(acarray,7);
    IByteArrayOutputStream *ibaos;
    ECode ec = 0;
    BufferOf<Byte>* byteArray;

    ec = CByteArrayOutputStream::New(&ibaos);
    FAIL_RETURN(ec,"CByteArrayOutputStream default constructor");
    ec = ibaos->WriteBufferEx(0, 7, myArray2);
    FAIL_RETURN(ec,"ibaos->WriteBufferEx");
    ec = ibaos->ToByteArray(&byteArray);
    FAIL_RETURN(ec,"ibaos->ToByteArray");
    
    for (Int32 i = 0; i < 7; i++) {
        if(myArray2.GetPayload()[i] != (*byteArray).GetPayload()[i]) {PRINTF_ERROR("test_ToByteArray"); return;}
    }

    PRINTF_OK("test_ToByteArray");
}

void test_Write()
{
    IByteArrayOutputStream *ibaos;
    ECode ec = 0;
    Int32 value = 'a';
    BufferOf<Byte>* byteArray;

    ec = CByteArrayOutputStream::New(&ibaos);
    FAIL_RETURN(ec,"CByteArrayOutputStream default constructor");
    ec = ibaos->Write(value);
    FAIL_RETURN(ec,"ibaos->Write");
    ec = ibaos->ToByteArray(&byteArray);
    FAIL_RETURN(ec,"ibaos->ToByteArray");
    if((*byteArray).GetPayload()[0] != 'a') {PRINTF_ERROR("test_Write"); return;}

    PRINTF_OK("test_Write");
}

void test_WriteBufferEx()
{
    Byte acarray[7]={'a','b','c','d','e','f','g'};
    const BufferOf<Byte> myArray2(acarray,7);
    IByteArrayOutputStream *ibaos;
    ECode ec = 0;
    BufferOf<Byte>* byteArray;

    ec = CByteArrayOutputStream::New(&ibaos);
    FAIL_RETURN(ec,"CByteArrayOutputStream default constructor");
    ec = ibaos->WriteBufferEx(0, 7, myArray2);
    FAIL_RETURN(ec,"ibaos->WriteBufferEx");
    ec = ibaos->ToByteArray(&byteArray);
    FAIL_RETURN(ec,"ibaos->ToByteArray");
    
    for (Int32 i = 0; i < 7; i++) {
        if(myArray2[i] != (*byteArray).GetPayload()[i]) {PRINTF_ERROR("test_WriteBufferEx"); return;}
    }

    PRINTF_OK("test_WriteBufferEx");
}

int main() {
    printf("\n");
    printf("=======testing ByteArrayOutputStream=========\n");
    printf("============================================\n");
    test_constructor();
    test_Close();
    test_Reset();
    test_GetSize();
    test_ToByteArray();
    test_Write();
    test_WriteBufferEx();
    printf("============================================\n");
    printf("============end=============================\n");
}