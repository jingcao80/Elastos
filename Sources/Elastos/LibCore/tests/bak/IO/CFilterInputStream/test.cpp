
#include <elastos.h>
#include <stdio.h>
#include <elautoptr.h>
#include "Elastos.CoreLibrary.h"

using namespace Elastos;
void testConstructor()
{
    String str("abcd");
    AutoPtr<IStringBufferInputStream> is;
	CStringBufferInputStream::New(str,(IStringBufferInputStream**)&is);
	AutoPtr<IFilterInputStream> fis;
	CFilterInputStream::New(is,(IFilterInputStream**)&fis);
}
void testAvailable()
{
    Int32 num = 0;
	String str("abcd");
    AutoPtr<IStringBufferInputStream> is;
	CStringBufferInputStream::New(str,(IStringBufferInputStream**)&is);
	AutoPtr<IFilterInputStream> fis;
	CFilterInputStream::New(is,(IFilterInputStream**)&fis);
	fis->Available(&num);
	assert(num == 4);
}
void testClose()
{
    Int32 num = 0;
	String str("abcd");
    AutoPtr<IStringBufferInputStream> is;
	CStringBufferInputStream::New(str,(IStringBufferInputStream**)&is);
	AutoPtr<IFilterInputStream> fis;
	CFilterInputStream::New(is,(IFilterInputStream**)&fis);
	fis->Close();
	fis->Available(&num);
	printf("num=%d\n",num);
}
void testMark()
{
    Int32 readLimit = 2;
	String str("abcd");
    AutoPtr<IStringBufferInputStream> is;
	CStringBufferInputStream::New(str,(IStringBufferInputStream**)&is);
	AutoPtr<IFilterInputStream> fis;
	CFilterInputStream::New(is,(IFilterInputStream**)&fis);
	fis->Mark(readLimit);
}
void testIsMarkSupported()
{
    Boolean supported = TRUE;
	String str("abcd");
    AutoPtr<IStringBufferInputStream> is;
	CStringBufferInputStream::New(str,(IStringBufferInputStream**)&is);
	AutoPtr<IFilterInputStream> fis;
	CFilterInputStream::New(is,(IFilterInputStream**)&fis);
    fis->IsMarkSupported(&supported);
    assert(supported==FALSE);
}
void testRead()
{
    Int32 value = 0;
	String str("abcde");
    AutoPtr<IStringBufferInputStream> is;
	CStringBufferInputStream::New(str,(IStringBufferInputStream**)&is);
	AutoPtr<IFilterInputStream> fis;
	CFilterInputStream::New(is,(IFilterInputStream**)&fis);
    fis->Read(&value);
    printf("value=%c\n",value);	//a
    fis->Read(&value);
    printf("value=%c\n",value);//b
	fis->Mark(1);
    fis->Read(&value);
    printf("value=%c\n",value);	//c
    fis->Read(&value);
    printf("value=%c\n",value);//d  //���⣺after Mark(1),it limit 1 character can be read,but it still read the next character "d";
    fis->Read(&value);
    printf("value=%d\n",value);//-1
	fis->Reset();//���⣺after Reset().did not reposition to the last Marked position
    fis->Read(&value);
    printf("value=%c\n",value);//a
}
void testReadBuffer()
{
    Int32 number = 0;
	ArrayOf_<Byte, 8> bytes;
	String str("abcd");
    AutoPtr<IStringBufferInputStream> is;
	CStringBufferInputStream::New(str,(IStringBufferInputStream**)&is);
	AutoPtr<IFilterInputStream> fis;
	CFilterInputStream::New(is,(IFilterInputStream**)&fis);
    fis->ReadBuffer(&bytes, &number);
    for(Int32 i = 0; i < number; i++)
	{
	    printf("%c ",bytes[i]);
	}
	printf("\n");

}
void testReadBufferEx()
{
	Int32 offset = 1;
	Int32 length = 4;
	Int32 number = 0;
	ArrayOf_<Byte, 8> bytes;
	String str("abcdefg");
    AutoPtr<IStringBufferInputStream> is;
	CStringBufferInputStream::New(str,(IStringBufferInputStream**)&is);
	AutoPtr<IFilterInputStream> fis;
	CFilterInputStream::New(is,(IFilterInputStream**)&fis);
	fis->ReadBufferEx(offset,length,&bytes,&number);
	for(Int32 i = offset; i < offset+length; i++)
	{
	    printf("%c ",bytes[i]);
	}
	printf("\n");
}
void testSkip()
{
    Int64 count = 2LL;
	Int64 number = 0LL;
    String str("abcdefg");
    AutoPtr<IStringBufferInputStream> is;
	CStringBufferInputStream::New(str,(IStringBufferInputStream**)&is);
	AutoPtr<IFilterInputStream> fis;
	CFilterInputStream::New(is,(IFilterInputStream**)&fis);
    Int32 value = 0;
    fis->Read(&value);
    printf("value=%c\n",value);	//a
	fis->Skip(count, &number);
	fis->Read(&value);
    printf("value=%c\n",value);	//d
	assert(number==2);
	count = 4LL;
	fis->Skip(count, &number);
	fis->Read(&value);
    printf("value=%d\n",value);	//-1
	assert(number==3);

}
int main(int argc, char *argv[])
{
    printf("output\n");
	//testSkip();
//	testReadBufferEx();
	//testReadBuffer();
	testRead();
	//testIsMarkSupported();
	//testMark();
	//testClose();
	//testAvailable();
	//testConstructor();
	printf("output2\n");
}
