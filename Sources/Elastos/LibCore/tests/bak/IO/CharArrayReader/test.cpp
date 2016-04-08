
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
    Byte acarray[7]={'a','b','c','d','e','f','g'};
    const BufferOf<Byte> myArray2(acarray,7);
    Boolean success = FALSE;
    ECode ec = 0;
    ICharArrayReader *icar;
    ICharArrayReader *icar2;
    Int32 character;

    //test default constructor
    ec = CCharArrayReader::New(myArray2, &icar);
    FAIL_RETURN(ec,"CCharArrayReader default constructor");
    ec = icar->IsReady(&success); 
    FAIL_RETURN(ec,"icar->IsReady");
    if(success != TRUE){PRINTF_ERROR("default constructor"); return;}

    //test the constructor with parameters
    ec = CCharArrayReader::New(myArray2, 5, 2, &icar2);
    FAIL_RETURN(ec,"CCharArrayReader constructor with parameters");
    ec = icar2->IsReady(&success);
    FAIL_RETURN(ec,"icar2->IsReady");
    if(success != TRUE){PRINTF_ERROR("constructor with parameters"); return;}
    ec = icar2->Read(&character);
    FAIL_RETURN(ec,"icar2->Read");
    if(character != 'f'){PRINTF_ERROR("constructor with parameters"); return;}

    PRINTF_OK("test_constructor");
}

void test_Close()
{
    Byte acarray[7]={'a','b','c','d','e','f','g'};
    const BufferOf<Byte> myArray2(acarray,7);
    ICharArrayReader *icar;
    ECode ec = 0;
    Int32 character;

    ec = CCharArrayReader::New(myArray2, &icar);
    FAIL_RETURN(ec,"CCharArrayReader default constructor");
    ec = icar->Close();
    FAIL_RETURN(ec,"icar->Close");
    ec = icar->Read(&character);
    FAIL_RETURN(!ec,"icar->Read");

    PRINTF_OK("test_Close");
}


void test_Mark()
{
    Byte acarray[7]={'a','b','c','d','e','f','g'};
    const BufferOf<Byte> myArray2(acarray,7);
    ICharArrayReader *icar;
    ECode ec = 0;
    Int32 character;
    Int64 number;

    ec = CCharArrayReader::New(myArray2, &icar);
    FAIL_RETURN(ec,"CCharArrayReader default constructor");
    ec = icar->Skip(5L, &number);
    FAIL_RETURN(ec,"icar->Skip");
    ec = icar->Mark(100);
    FAIL_RETURN(ec,"icar->Mark");
    ec = icar->Read(&character);
    FAIL_RETURN(ec,"icar->Read");
    ec = icar->Reset();
    FAIL_RETURN(ec,"icar->Reset");
    ec = icar->Read(&character);
    FAIL_RETURN(ec,"icar->Read");
    if(character != 'f'){PRINTF_ERROR("constructor with parameters"); return;}

    ec = icar->Close();
    FAIL_RETURN(ec,"icar->Close");
    ec = icar->Mark(100);
    FAIL_RETURN(!ec,"icar->Mark");

    PRINTF_OK("test_Mark");
}

void test_IsMarkedSupported()
{
    Byte acarray[7]={'a','b','c','d','e','f','g'};
    const BufferOf<Byte> myArray2(acarray,7);
    ICharArrayReader *icar;
    Boolean supported = FALSE;
    ECode ec = 0;

    ec = CCharArrayReader::New(myArray2, &icar);
    FAIL_RETURN(ec,"CCharArrayReader default constructor");
    ec = icar->IsMarkSupported(&supported);
    FAIL_RETURN(ec,"icar->IsMarkSupported");
    if(supported != TRUE) {PRINTF_ERROR("test_IsMarkedSupported"); return;}

    PRINTF_OK("test_IsMarkedSupported");
}

void test_Read() 
{
    Byte acarray[7]={'a','b','c','d','e','f','g'};
    const BufferOf<Byte> myArray2(acarray,7);
    ICharArrayReader *icar;
    ECode ec = 0;
    Int32 value = 0;

    ec = CCharArrayReader::New(myArray2, &icar);
    FAIL_RETURN(ec,"CCharArrayReader default constructor");
    ec = icar->Read(&value);
    FAIL_RETURN(ec,"icar->Read");
    if(value != 'a') {PRINTF_ERROR("test_Read"); return;}

    PRINTF_OK("test_Read");
}

void test_ReadBufferEx()
{
    Byte acarray[7]={'a','b','c','d','e','f','g'};
    const BufferOf<Byte> myArray2(acarray,7);
    ICharArrayReader *icar;
    BufferOf<Byte> *buffer = BufferOf<Byte>::Alloc(8);
    ECode ec = 0;
    Int32 num = 0;

    ec = CCharArrayReader::New(myArray2, &icar);
    FAIL_RETURN(ec,"CCharArrayReader default constructor");
    ec = icar->ReadBufferEx(1,2,buffer,&num);
    FAIL_RETURN(ec,"icar->ReadBufferEx");
    Byte *bt=(*buffer).GetPayload();
    if(num!=2 || bt[1]!='a' || bt[2]!='b') {PRINTF_ERROR("test_ReadBufferEx"); return;}
    BufferOf<Byte>::Free(buffer);

    PRINTF_OK("test_ReadBufferEx");
}

void test_IsReady()
{
    Byte acarray[7]={'a','b','c','d','e','f','g'};
    const BufferOf<Byte> myArray2(acarray,7);
    ICharArrayReader *icar;
    ECode ec = 0;
    Boolean success = FALSE;
    

    ec = CCharArrayReader::New(myArray2, &icar);
    FAIL_RETURN(ec,"CCharArrayReader default constructor");
    ec = icar->IsReady(&success);
    FAIL_RETURN(ec,"icar->IsReady");

    PRINTF_OK("test_IsReady");
}

void test_Reset()
{
    Byte acarray[7]={'a','b','c','d','e','f','g'};
    const BufferOf<Byte> myArray2(acarray,7);
    ICharArrayReader *icar;
    ECode ec = 0;
    Int32 value1 = 0;
    Int64 number = 0;

    ec = CCharArrayReader::New(myArray2, &icar);
    FAIL_RETURN(ec,"CCharArrayReader default constructor");
    ec = icar->Skip(5L,&number); FAIL_RETURN(ec,"icar->Mark");
    if(number != 5) {PRINTF_ERROR("icar->Skip"); return;}
    ec = icar->Mark(100);        FAIL_RETURN(ec,"icar->Mark");
    ec = icar->Read(&value1);    FAIL_RETURN(ec,"icar->Read");
    ec = icar->Reset();          FAIL_RETURN(ec,"icar->Reset");
    if(value1 != 'f') {PRINTF_ERROR("test_Reset"); return;}

    PRINTF_OK("test_Reset");
}

void test_Skip()
{
    Byte acarray[7]={'a','b','c','d','e','f','g'};
    const BufferOf<Byte> myArray2(acarray,7);
    ICharArrayReader *icar;
    ECode ec = 0;
    Int64 count = 5;
    Int64 number = 0;
    Int32 value = 0;

    ec = CCharArrayReader::New(myArray2, &icar);
    FAIL_RETURN(ec,"CCharArrayReader default constructor");
    ec = icar->Skip(count,&number);
    FAIL_RETURN(ec,"icar->Skip");
    if(number != count) {PRINTF_ERROR("icar->Skip"); return;}
    ec = icar->Read(&value);
    FAIL_RETURN(ec,"icar->Read");
    if(value != 'f') {PRINTF_ERROR("test_Skip"); return;}

    PRINTF_OK("test_Skip");
}

int main() {
    printf("\n");
    printf("=======testing CharArrayReader=========\n");
    printf("============================================\n");
    test_constructor();
    test_Close();
    test_Mark();
    test_IsMarkedSupported();
    test_Read();
    test_ReadBufferEx();
    test_IsReady();
    test_Reset();
    test_Skip();
    printf("============================================\n");
    printf("============end=============================\n");
}