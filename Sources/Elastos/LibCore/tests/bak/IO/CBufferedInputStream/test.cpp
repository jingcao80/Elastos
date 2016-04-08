
#include "stdio.h"
#include "Elastos.CoreLibrary.h"
#include "elastos.h"
#include <elautoptr.h>
using namespace Elastos;

void test_constructor()
{
//constructs an String input stream
    String s("Onewouldwin");
    AutoPtr<IStringBufferInputStream> isbis;
    ECode ec = CStringBufferInputStream::New(s, (IStringBufferInputStream**)&isbis);

//test default constructor
	AutoPtr<IBufferedInputStream> ibis,ibis2;
	ec = CBufferedInputStream::New(isbis, (IBufferedInputStream**)&ibis);

//test the constructor with parameters
	ec = CBufferedInputStream::New(isbis, 4096, (IBufferedInputStream**)&ibis2);
}

void test_Available()
{
    String s("Onewouldwinin");
    AutoPtr<IStringBufferInputStream> isbis;
    CStringBufferInputStream::New(s, (IStringBufferInputStream**)&isbis);
	AutoPtr<IBufferedInputStream> ibis;
    CBufferedInputStream::New(isbis, 4096, (IBufferedInputStream**)&ibis);
    Int32 number = 0;
    ibis->Available(&number);
	printf("available number = %d\n",number);//13
	ibis->Close();
	ibis->Available(&number);
	printf("available number = %d\n",number);//13  Close()ľ����
}


void test_IsMarkedSupported()
{
    String s("hisenemy's.");
    AutoPtr<IStringBufferInputStream> isbis;
    CStringBufferInputStream::New(s, (IStringBufferInputStream**)&isbis);
	AutoPtr<IBufferedInputStream> ibis;
    CBufferedInputStream::New(isbis, 4096, (IBufferedInputStream**)&ibis);
	Boolean supported = FALSE;
	ibis->IsMarkSupported(&supported);
	assert(supported == TRUE);
}

void test_Read()
{
    String s("and his enemy's.");
    AutoPtr<IStringBufferInputStream> isbis;
    ECode ec = CStringBufferInputStream::New(s, (IStringBufferInputStream**)&isbis);
	AutoPtr<IBufferedInputStream> ibis;
    ec = CBufferedInputStream::New(isbis, 4096, (IBufferedInputStream**)&ibis);
    Int32 value = 0;
    Int32 num = 0;
	ibis->Available(&num);
	for(Int32 i = 0; i < num; i++)
	{
	    ibis->Read(&value);
	    printf("%c",value);
	}//and his enemy's.
	printf("\n");
}

void test_ReadBufferEx()
{
    String s("of his and his enemy's.");
    AutoPtr<IStringBufferInputStream> isbis;
    CStringBufferInputStream::New(s, (IStringBufferInputStream**)&isbis);
	AutoPtr<IBufferedInputStream> ibis;
    CBufferedInputStream::New(isbis, 4096, (IBufferedInputStream**)&ibis);
	Int32 offset = 4;
	Int32 length = 8;
	ArrayOf_<Byte,20> bytes;
	Int32 number = 0;
	ibis->ReadBufferEx(offset, length, &bytes, &number);
	for(Int32 i = offset; i < offset + length; i++)
	{
	    printf("%c",bytes[i]);
	}//of his a
	printf("\n");


}

void test_Reset()
{
    String s("abcd,efg,hij.");
    AutoPtr<IStringBufferInputStream> isbis;
    CStringBufferInputStream::New(s, (IStringBufferInputStream**)&isbis);
	AutoPtr<IBufferedInputStream> ibis;
    CBufferedInputStream::New(isbis, 4096, (IBufferedInputStream**)&ibis);
    Int32 value = 0;
    ibis->Read(&value);
    printf("the readout value is:%c\n",value);//a
	ibis->Reset();
	ibis->Read(&value);
    printf("the readout value is:%c\n",value);//b,���⣺reset��Ӧ����a�Ŷ�
	Boolean supported = FALSE;
	ibis->IsMarkSupported(&supported);
    ibis->Mark(4);
	ibis->Read(&value);
	printf("the readout value is:%c\n",value);//b   //none  ����
	ibis->Read(&value);
	printf("the readout value is:%c\n",value);//b   //none  ����

	ibis->Read(&value);
	printf("the readout value is:%c\n",value);//b   //none  ����

}

void test_Skip()
{
    String s("One would win in all wars if he has knowledges of his and his enemy's.");
    AutoPtr<IStringBufferInputStream> isbis;
    CStringBufferInputStream::New(s, (IStringBufferInputStream**)&isbis);
	AutoPtr<IBufferedInputStream> ibis;
    CBufferedInputStream::New(isbis, 4096, (IBufferedInputStream**)&ibis);
    Int64 count = 4;
	Int64 number;
	ibis->Skip(count,&number);
	assert(number == 4);
	Int32 value = 0;
	ibis->Read(&value);
	printf("the readout value is:%c\n",value);//w
}

int main() {

	printf("============begin==========\n");
    //test_constructor();
    //test_Available();
	//test_IsMarkedSupported();
    //test_Read();
    //test_ReadBufferEx();
	test_Reset();
	//test_Skip();
	printf("============end============\n");

}
