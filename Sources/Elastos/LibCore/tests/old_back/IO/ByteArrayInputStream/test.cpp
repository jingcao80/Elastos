
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
    Int32 number = 0;
    ECode ec = 0;
    IByteArrayInputStream *ibais;
    IByteArrayInputStream *ibais2;

    //test default constructor
    ec = CByteArrayInputStream::New(myArray2, &ibais);
    FAIL_RETURN(ec,"CByteArrayInputStream default constructor");
    ec = ibais->Available(&number); 
    FAIL_RETURN(ec,"ibais->Available");
    if(number != myArray2.GetCapacity()){PRINTF_ERROR("default constructor"); return;}

    //test the constructor with parameters
    ec = CByteArrayInputStream::New(myArray2, 2, 3, &ibais2);
    FAIL_RETURN(ec,"CByteArrayInputStream constructor with parameters");
    ec = ibais2->Available(&number);
    FAIL_RETURN(ec,"ibais2->Available");
    if(number != 3){PRINTF_ERROR("constructor with parameters"); return;}

    PRINTF_OK("test_constructor");
}

void test_Available()
{
    Byte acarray[7]={'a','b','c','d','e','f','g'};
    const BufferOf<Byte> myArray2(acarray,7);
    IByteArrayInputStream *ibais;
    ECode ec = 0;
    Int32 number = 0;

    ec = CByteArrayInputStream::New(myArray2, &ibais);
    FAIL_RETURN(ec,"CByteArrayInputStream default constructor");
    ec = ibais->Available(&number);
    FAIL_RETURN(ec,"ibais->Available");
    if(number != myArray2.GetCapacity()){PRINTF_ERROR("test_Available"); return;}

    PRINTF_OK("test_constructor");
}

void test_Close()
{
    Byte acarray[7]={'a','b','c','d','e','f','g'};
    const BufferOf<Byte> myArray2(acarray,7);
    IByteArrayInputStream *ibais;
    ECode ec = 0;

    ec = CByteArrayInputStream::New(myArray2, &ibais);
    FAIL_RETURN(ec,"CByteArrayInputStream default constructor");
    ec = ibais->Close();
    FAIL_RETURN(ec,"ibais->Close");

    PRINTF_OK("test_Close");
}

void test_Mark()
{
    Byte acarray[7]={'a','b','c','d','e','f','g'};
    const BufferOf<Byte> myArray2(acarray,7);
    IByteArrayInputStream *ibais;
    Int32 readLimit = 2;
    ECode ec = 0;

    ec = CByteArrayInputStream::New(myArray2, &ibais);
    FAIL_RETURN(ec,"CByteArrayInputStream default constructor");
    ec = ibais->Mark(readLimit);
    FAIL_RETURN(ec,"ibais->Mark");

    PRINTF_OK("test_Mark");
}

void test_IsMarkedSupported()
{
    Byte acarray[7]={'a','b','c','d','e','f','g'};
    const BufferOf<Byte> myArray2(acarray,7);
    IByteArrayInputStream *ibais;
    Boolean supported = FALSE;
    ECode ec = 0;

    ec = CByteArrayInputStream::New(myArray2, &ibais);
    FAIL_RETURN(ec,"CByteArrayInputStream default constructor");
    ec = ibais->IsMarkSupported(&supported);
    FAIL_RETURN(ec,"ibais->IsMarkSupported");
    if(supported != TRUE) {PRINTF_ERROR("test_IsMarkedSupported"); return;}

    PRINTF_OK("test_IsMarkedSupported");
}

void test_Read() 
{
    Byte acarray[7]={'a','b','c','d','e','f','g'};
    const BufferOf<Byte> myArray2(acarray,7);
    IByteArrayInputStream *ibais;
    ECode ec = 0;
    Int32 value = 0;

    ec = CByteArrayInputStream::New(myArray2, &ibais);
    FAIL_RETURN(ec,"CByteArrayInputStream default constructor");
    ec = ibais->Read(&value);
    FAIL_RETURN(ec,"ibais->Read");
    if(value != 'a') {PRINTF_ERROR("test_Read"); return;}

    PRINTF_OK("test_Read");
}

void test_ReadBufferEx()
{
    Byte acarray[7]={'a','b','c','d','e','f','g'};
    const BufferOf<Byte> myArray2(acarray,7);
    IByteArrayInputStream *ibais;
    BufferOf<Byte> *buffer = BufferOf<Byte>::Alloc(8);
    ECode ec = 0;
    Int32 num = 0;

    ec = CByteArrayInputStream::New(myArray2, &ibais);
    FAIL_RETURN(ec,"CByteArrayInputStream default constructor");
    ec = ibais->ReadBufferEx(1,2,buffer,&num);
    FAIL_RETURN(ec,"ibais->ReadBufferEx");
    Byte *bt=(*buffer).GetPayload();
    if(num!=2 || bt[1]!='a' || bt[2]!='b') {PRINTF_ERROR("test_ReadBufferEx"); return;}
    BufferOf<Byte>::Free(buffer);

    PRINTF_OK("test_ReadBufferEx");
}

void test_Reset()
{
    Byte acarray[7]={'a','b','c','d','e','f','g'};
    const BufferOf<Byte> myArray2(acarray,7);
    IByteArrayInputStream *ibais;
    ECode ec = 0;
    Int32 value1 = 0,value2 = 0;

    ec = CByteArrayInputStream::New(myArray2, &ibais);
    FAIL_RETURN(ec,"CByteArrayInputStream default constructor");
    ec = ibais->Mark(200);      FAIL_RETURN(ec,"ibais->Mark");
    ec = ibais->Read(&value1);  FAIL_RETURN(ec,"ibais->Read");
    ec = ibais->Reset();        FAIL_RETURN(ec,"ibais->Reset");
    ec = ibais->Read(&value2);  FAIL_RETURN(ec,"ibais->Read");
    ec = ibais->Reset();        FAIL_RETURN(ec,"ibais->Reset");
    if(value1 != value2) {PRINTF_ERROR("test_Reset"); return;}

    PRINTF_OK("test_Reset");
}

void test_Skip()
{
    Byte acarray[7]={'a','b','c','d','e','f','g'};
    const BufferOf<Byte> myArray2(acarray,7);
    IByteArrayInputStream *ibais;
    ECode ec = 0;
    Int64 count = 3;
    Int64 number = 0;
    Int32 value = 0;

    ec = CByteArrayInputStream::New(myArray2, &ibais);
    FAIL_RETURN(ec,"CByteArrayInputStream default constructor");
    ec = ibais->Skip(count,&number);
    FAIL_RETURN(ec,"ibais->Skip");
    ec = ibais->Read(&value);
    FAIL_RETURN(ec,"ibais->Read");
    if(value != 'd') {PRINTF_ERROR("test_Skip"); return;}

    PRINTF_OK("test_Skip");
}

int main() {
    printf("\n");
    printf("=======testing ByteArrayInputStream=========\n");
    printf("============================================\n");
    test_constructor();
    test_Available();
    test_Close();
    test_Mark();
    test_IsMarkedSupported();
    test_Read();
    test_ReadBufferEx();
    test_Reset();
    test_Skip();
    printf("============================================\n");
    printf("============end=============================\n");
}