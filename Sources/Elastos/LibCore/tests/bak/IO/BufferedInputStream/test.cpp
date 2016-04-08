
#include "stdio.h"
#include "Elastos.IO2.h"
#include "elastos.h"

using namespace Elastos;

void test_constructor()
{
//constructs an String input stream
    printf("\n");
    printf("Start test_constructor() at Line:%d\n", __LINE__);
    String s("One would win in all wars if he has knowledges of his and his enemy's.");
    IStringBufferInputStream *isbis;
    ECode ec = CStringBufferInputStream::New(s, &isbis);
    assert(!ec);
//test default constructor
    IBufferedInputStream *ibis, *ibis2;
    ec = CBufferedInputStream::New(isbis, &ibis);
    assert(!ec);

//test the constructor with parameters	
    ec = CBufferedInputStream::New(isbis, 4096, &ibis2);
    assert(!ec);
}

void test_Available()
{
    printf("\n");
    printf("Start test_Available() at Line:%d\n", __LINE__);
    String s("One would win in all wars if he has knowledges of his and his enemy's.");
    IStringBufferInputStream *isbis;
    ECode ec = CStringBufferInputStream::New(s, &isbis);
    IBufferedInputStream *ibis;
    ec = CBufferedInputStream::New(isbis, 4096, &ibis);
    Int32 number = 0;
    ec = ibis->Available(&number);
    assert(!ec);
    printf("available number = %d\n",number);
    assert(number==s.GetLength());
}

void test_Close()
{
    printf("\n");
    printf("Start test_Close() at Line:%d\n", __LINE__);
    String s("One would win in all wars if he has knowledges of his and his enemy's.");
    IStringBufferInputStream *isbis;
    ECode ec = CStringBufferInputStream::New(s, &isbis);
    IBufferedInputStream *ibis;
    ec = CBufferedInputStream::New(isbis, 4096, &ibis);
    ec = ibis->Close();
    assert(!ec);
    Int32 number = 0;
    ec = ibis->Available(&number);
    printf("available number = %d\n",number);
    assert(number == 0);
}

void test_Mark()
{
    printf("\n");
    printf("Start test_Mark() at Line:%d\n", __LINE__);
    String s("One would win in all wars if he has knowledges of his and his enemy's.");
    IStringBufferInputStream *isbis;
    ECode ec = CStringBufferInputStream::New(s, &isbis);
    IBufferedInputStream *ibis;
    ec = CBufferedInputStream::New(isbis, 4096, &ibis);
    Int32 readLimit = 5;
    ec = ibis->Mark(readLimit);
    assert(!ec);
}

void test_IsMarkedSupported()
{
    printf("\n");
    printf("Start test_IsMarkedSupported() at Line:%d\n", __LINE__);
    String s("One would win in all wars if he has knowledges of his and his enemy's.");
    IStringBufferInputStream *isbis;
    ECode ec = CStringBufferInputStream::New(s, &isbis);
    IBufferedInputStream *ibis;
    ec = CBufferedInputStream::New(isbis, 4096, &ibis);
    Boolean supported = FALSE;
    ec = ibis->IsMarkSupported(&supported);
    assert(supported == TRUE);

    
}

void test_Read() 
{
    printf("\n");
    printf("Start test_Read() at Line:%d\n", __LINE__);
    String s("One would win in all wars if he has knowledges of his and his enemy's.");
    IStringBufferInputStream *isbis;
    ECode ec = CStringBufferInputStream::New(s, &isbis);
    IBufferedInputStream *ibis;
    ec = CBufferedInputStream::New(isbis, 4096, &ibis);
    Int32 value = 0;
    ec = ibis->Read(&value);
    printf("the readout value is:%d\n",value);
    assert(value == 79);
    ec = ibis->Read(&value);
    printf("the readout value is:%d\n",value);

    ec = ibis->Mark(3);
    assert(!ec);
    ec = ibis->Read(&value);
    printf("the readout value is:%d\n",value);

}

void test_ReadBufferEx()
{
    printf("\n");
    printf("Start test_ReadBufferEx() at Line:%d\n", __LINE__);
    String s("One would win in all wars if he has knowledges of his and his enemy's.");
    IStringBufferInputStream *isbis;
    ECode ec = CStringBufferInputStream::New(s, &isbis);
    IBufferedInputStream *ibis;
    ec = CBufferedInputStream::New(isbis, 4096, &ibis);

    BufferOf<Byte>* buffer = BufferOf<Byte>::Alloc(11);
    Int32 num = 0;
    ibis->ReadBufferEx(2,7,buffer,&num);

    printf("%d\n",num);
    assert(num==7);
}

void test_Reset()
{
    printf("\n");
    printf("Start test_Reset() at Line:%d\n", __LINE__);
    String s("One would win in all wars if he has knowledges of his and his enemy's.");
    IStringBufferInputStream *isbis;
    ECode ec = CStringBufferInputStream::New(s, &isbis);
    IBufferedInputStream *ibis;
    ec = CBufferedInputStream::New(isbis, 4096, &ibis);
    Int32 value = 0;
    ibis->Mark(10);
    for(Int32 i = 0;i < 4;i++)
    {
        ec = ibis->Read(&value);
        printf("the readout value is:%d\n",value);
    }
    ec = ibis->Reset();
    ec = ibis->Read(&value);
    printf("the readout value is:%d\n",value);
    assert(value==79);

}

void test_Skip()
{
    printf("\n");
    printf("Start test_Skip() at Line:%d\n", __LINE__);
    String s("One would win in all wars if he has knowledges of his and his enemy's.");
    IStringBufferInputStream *isbis;
    ECode ec = CStringBufferInputStream::New(s, &isbis);
    IBufferedInputStream *ibis;
    ec = CBufferedInputStream::New(isbis, 4096, &ibis);
    Int64 count = 3;
    Int64 number = 0;
    ec = ibis->Skip(count,&number);
    printf("the skip number is:%d\n",number);
    assert(number == 3);
}

int main() {
    
    printf("============begin==========\n");
    //test_constructor();
    //test_Available();
    //test_Close();
    //test_Mark();
    //test_IsMarkedSupported();
    //test_Read();
    //test_ReadBufferEx();
    //test_Reset();
   // test_Skip();
    printf("============end============\n");

}