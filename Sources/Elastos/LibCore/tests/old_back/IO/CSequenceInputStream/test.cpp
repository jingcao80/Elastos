
#include <elastos.h>
#include <stdio.h>
#include <elautoptr.h>
#include "Elastos.CoreLibrary.h"

using namespace Elastos;

void testContructor1()
{
    String str1("abcdefg");
	AutoPtr<IStringBufferInputStream> sbis1;
	CStringBufferInputStream::New(str1, (IStringBufferInputStream **)&sbis1);
    String str2("abcdefg");
	AutoPtr<IStringBufferInputStream> sbis2;
	CStringBufferInputStream::New(str2, (IStringBufferInputStream **)&sbis2);
	AutoPtr<IInputStream> is;
	CSequenceInputStream::New(sbis1, sbis2, (IInputStream **)&is);
}
void testConstructor2()
{
    //ObjectNode pHeader
}

void testAvailable()
{
    Int32 number = 0;
    String str1("abcdefg");
	AutoPtr<IStringBufferInputStream> sbis1;
	CStringBufferInputStream::New(str1, (IStringBufferInputStream **)&sbis1);
    String str2("jhhj");
	AutoPtr<IStringBufferInputStream> sbis2;
	CStringBufferInputStream::New(str2, (IStringBufferInputStream **)&sbis2);
	AutoPtr<IInputStream> is;
	CSequenceInputStream::New(sbis1, sbis2, (IInputStream **)&is);
    is->Available(&number);
	printf("number=%d\n",number);//7 ���⣺it should be 11,currently it only includes the first inputstream,not consisting of the second stream .
	is->Close();
    is->Available(&number);
	printf("number=%d\n",number);//0
}

void testRead()
{
    Int32 value = 0;
    String str1("abcdefg");
	AutoPtr<IStringBufferInputStream> sbis1;
	CStringBufferInputStream::New(str1, (IStringBufferInputStream **)&sbis1);
    String str2("jhhj");
	AutoPtr<IStringBufferInputStream> sbis2;
	CStringBufferInputStream::New(str2, (IStringBufferInputStream **)&sbis2);
	AutoPtr<IInputStream> is;
	CSequenceInputStream::New(sbis1, sbis2, (IInputStream **)&is);
    Int32 number = 0;
	is->Available(&number);
	for(Int32 i = 0; i < number; i++)
	{
	    is->Read(&value);
		printf("value=%c ",value);//value=a value=b value=c value=d value=e value=f value=g
		                          //���⣺ֻ����˵�һ�������ַ�
	}
	printf("\n");
}

void testReadBufferEx()
{
    ArrayOf_<Byte,20> bytes;
	Int32 offset = 4;
	Int32 count = 10;
	Int32 number = 0;
	String str1("abcdefg");
	AutoPtr<IStringBufferInputStream> sbis1;
	CStringBufferInputStream::New(str1, (IStringBufferInputStream **)&sbis1);
    String str2("jhhj");
	AutoPtr<IStringBufferInputStream> sbis2;
	CStringBufferInputStream::New(str2, (IStringBufferInputStream **)&sbis2);
	AutoPtr<IInputStream> is;
	CSequenceInputStream::New(sbis1, sbis2, (IInputStream **)&is);
	is->ReadBufferEx(offset, count, &bytes, &number);
	for(Int32 i = offset; i < offset + count; i++)
	{
	    printf("%c ",bytes[i]);
	}//a b c d e f g //���⣺ӦΪ��a b c d e f g j h h
     //number=7  //���⣺Ӧ��Ϊ10�Ŷ�
	printf("\n");
	printf("number=%d\n",number);
}

int main(int argc, char *argv[])
{
    printf("output\n");
	testReadBufferEx();
	//testRead();
    //testAvailable();
	//testContructor1();

	printf("output2\n");
}
