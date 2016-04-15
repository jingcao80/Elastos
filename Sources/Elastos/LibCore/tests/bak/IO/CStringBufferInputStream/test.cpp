
#include <elastos.h>
#include <stdio.h>
#include <elautoptr.h>
#include "Elastos.CoreLibrary.h"

using namespace Elastos;
void testConstructor()
{
    String str("abcdefgh");
	AutoPtr<IStringBufferInputStream> sbis;
	CStringBufferInputStream::New(str, (IStringBufferInputStream**)&sbis);

}
void testAvailable()
{
    Int32 number = 0;
    String str("abcdefgh");
	AutoPtr<IStringBufferInputStream> sbis;
	CStringBufferInputStream::New(str, (IStringBufferInputStream**)&sbis);
    sbis->Available(&number);
    printf("number=%d\n",number);//8
}
void testRead()
{
    Int32 value = 0;
	String str("abcdefgh");
	AutoPtr<IStringBufferInputStream> sbis;
	CStringBufferInputStream::New(str, (IStringBufferInputStream**)&sbis);
    sbis->Read(&value);
	printf("value=%c\n", value);//a
    sbis->Read(&value);
	printf("value=%c\n", value);//b
	sbis->Reset();
	sbis->Read(&value);
	printf("value=%c\n", value);//a

	Int64 count = 4LL;
	Int64 number = 0LL;
	sbis->Skip(count, &number);
	assert(number == 4);
    sbis->Read(&value);
	printf("value=%c\n", value);//f
}
void testReadBufferEx()
{
    Int32 offset = 3;
	Int32 length = 3;
	ArrayOf_<Byte, 6> characters;
	Int32 number = 0;
	String str("abcdefgh");
	AutoPtr<IStringBufferInputStream> sbis;
	CStringBufferInputStream::New(str, (IStringBufferInputStream**)&sbis);
    sbis->ReadBufferEx(offset, length, &characters, &number);
    for(Int32 i = offset; i < offset+length; i++)
    {
        printf("%c ", characters[i]);
    }
    assert(number==3);
	printf("\n");
}
int main(int argc, char *argv[])
{
    printf("output\n");
	testReadBufferEx();
	//testRead();
	//testAvailable();
	//testConstructor();
	printf("output2\n");
}
