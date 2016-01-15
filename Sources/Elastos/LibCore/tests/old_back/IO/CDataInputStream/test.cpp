
#include "Elastos.CoreLibrary.h"
#include "stdio.h"
#include "elastos.h"
#include <elautoptr.h>

using namespace Elastos;

void test_constructor()
{
    String str("abcdefghijk");
    AutoPtr<IStringBufferInputStream> isbis;
	ECode ec = CStringBufferInputStream::New(str,(IStringBufferInputStream**)&isbis);
    AutoPtr<IDataInputStream> idis;
    ec = CDataInputStream::New(isbis,(IDataInputStream**)&idis);

}

void test_ReadBuffer()
{
    String str("godblessluckypeople");
    AutoPtr<IStringBufferInputStream> isbis;
	CStringBufferInputStream::New(str, (IStringBufferInputStream**)&isbis);
    AutoPtr<IDataInputStream> idis;
    CDataInputStream::New(isbis,(IDataInputStream**)&idis);
	ArrayOf_<Byte,38> bytes;
	Int32 number = 0;
	idis->ReadBuffer(&bytes, &number);
	for(Int32 i = 0; i < number; i++)
	{
	    printf("%c",bytes[i]);
	}

	printf("\nnumber=%d\n",number);//19

}

void test_ReadBufferEx()
{
    String str("godblessluckypeople");
    AutoPtr<IStringBufferInputStream> isbis;
	ECode ec = CStringBufferInputStream::New(str, (IStringBufferInputStream**)&isbis);
    AutoPtr<IDataInputStream> idis;
    ec = CDataInputStream::New(isbis,(IDataInputStream**)&idis);

	Int32 offset = 3;
	Int32 length = 16;
	ArrayOf_<Byte,38> bytes;
	Int32 number = 0;
	idis->ReadBufferEx(offset, length, &bytes, &number);
	for(Int32 i = offset; i < offset + length; i++)
	{
	    printf("%c",bytes[i]);
	}

	printf("\nnumber=%d\n",number);
}
void test_ReadBoolean()
{
    String str("godblessluckypeople");
    AutoPtr<IStringBufferInputStream> isbis;
	ECode ec = CStringBufferInputStream::New(str, (IStringBufferInputStream**)&isbis);
    AutoPtr<IDataInputStream> idis;
    ec = CDataInputStream::New(isbis,(IDataInputStream**)&idis);

	AutoPtr<IDataInput> di;
	di = IDataInput::Probe(idis);
	Boolean value;
	ec = di->ReadBoolean(&value);
    printf("value=%d\n",value);

}
void test_ReadByte()
{
    String str("godblessluckypeople");
    AutoPtr<IStringBufferInputStream> isbis;
	ECode ec = CStringBufferInputStream::New(str, (IStringBufferInputStream**)&isbis);
    AutoPtr<IDataInputStream> idis;
    ec = CDataInputStream::New(isbis,(IDataInputStream**)&idis);

	AutoPtr<IDataInput> di;
	di = IDataInput::Probe(idis);
	Byte b = 20;
    ec = di->ReadByte(&b);
    printf("value=%d\n",b);
}

void test_ReadChar()
{
    String str("godblessluckypeople");
    AutoPtr<IStringBufferInputStream> isbis;
	ECode ec = CStringBufferInputStream::New(str, (IStringBufferInputStream**)&isbis);
    AutoPtr<IDataInputStream> idis;
    ec = CDataInputStream::New(isbis,(IDataInputStream**)&idis);

	AutoPtr<IDataInput> di;
	di = IDataInput::Probe(idis);

	Char32 c = 'a';
	ec = di->ReadChar(&c);
	printf("value=%d\n",c);

}

void test_ReadDouble()
{
    String str("godblessluckypeople");
    AutoPtr<IStringBufferInputStream> isbis;
	ECode ec = CStringBufferInputStream::New(str, (IStringBufferInputStream**)&isbis);
    AutoPtr<IDataInputStream> idis;
    ec = CDataInputStream::New(isbis,(IDataInputStream**)&idis);

	AutoPtr<IDataInput> di;
	di = IDataInput::Probe(idis);

    Double d = 0.0;
	ec = di->ReadDouble(&d);
	printf("value=%g\n",d);


}

void test_ReadFloat()
{
    String str("godblessluckypeople");
    AutoPtr<IStringBufferInputStream> isbis;
	ECode ec = CStringBufferInputStream::New(str, (IStringBufferInputStream**)&isbis);
    AutoPtr<IDataInputStream> idis;
    ec = CDataInputStream::New(isbis,(IDataInputStream**)&idis);

	AutoPtr<IDataInput> di;
	di = IDataInput::Probe(idis);

	Float f = 1.0f;
	ec = di->ReadFloat(&f);
	printf("value=%f\n",f);
}

void test_ReadFully()
{
    String str("godblessluckypeople");
    AutoPtr<IStringBufferInputStream> isbis;
	CStringBufferInputStream::New(str, (IStringBufferInputStream**)&isbis);
    AutoPtr<IDataInputStream> idis;
    CDataInputStream::New(isbis,(IDataInputStream**)&idis);

	AutoPtr<IDataInput> di;
	di = IDataInput::Probe(idis);

	ArrayOf_<Byte,30> bytes;
	di->ReadFully(&bytes);
	for(Int32 i=0; i<bytes.GetLength(); i++) {
	    printf("%c", bytes[i]);
	}
	printf("\n");
}


void test_ReadFullyEx()
{
    String str("godblessluckypeople");
    AutoPtr<IStringBufferInputStream> isbis;
	ECode ec = CStringBufferInputStream::New(str, (IStringBufferInputStream**)&isbis);
    AutoPtr<IDataInputStream> idis;
    ec = CDataInputStream::New(isbis,(IDataInputStream**)&idis);

	AutoPtr<IDataInput> di;
	di = IDataInput::Probe(idis);

	Int32 offset = 2;
	Int32 length = 8;
	ArrayOf_<Byte, 10> buffer;

	ec = di->ReadFullyEx(offset,length,&buffer);
	for(Int32 i = offset; i < offset + length; i++) {
	    printf("%c", buffer[i]);//godbless
	}
	printf("\n");
}

void test_ReadInt32()
{
    String str("godblessluckypeople");
    AutoPtr<IStringBufferInputStream> isbis;
	ECode ec = CStringBufferInputStream::New(str, (IStringBufferInputStream**)&isbis);
    AutoPtr<IDataInputStream> idis;
    ec = CDataInputStream::New(isbis,(IDataInputStream**)&idis);

	AutoPtr<IDataInput> di;
	di = IDataInput::Probe(idis);

	Int32 i = 0;
	ec = di->ReadInt32(&i);
	printf("value=%d\n",i);
}

void test_ReadLine()
{
    String str("godblessluc\nkypeople");
    AutoPtr<IStringBufferInputStream> isbis;
	ECode ec = CStringBufferInputStream::New(str, (IStringBufferInputStream**)&isbis);
    AutoPtr<IDataInputStream> idis;
    ec = CDataInputStream::New(isbis,(IDataInputStream**)&idis);

	AutoPtr<IDataInput> di;
	di = IDataInput::Probe(idis);

    String s("Best!");
    ec = di->ReadLine(&s);
	Int32 len = s.GetLength();
	printf("length=%d\n",len);
	assert(len==11);
}
void test_ReadInt64()
{
    String str("godblessluckypeople");
    AutoPtr<IStringBufferInputStream> isbis;
	ECode ec = CStringBufferInputStream::New(str, (IStringBufferInputStream**)&isbis);
    AutoPtr<IDataInputStream> idis;
    ec = CDataInputStream::New(isbis,(IDataInputStream**)&idis);

	AutoPtr<IDataInput> di;
	di = IDataInput::Probe(idis);

	Int64 i = 32767LL;
	ec = di->ReadInt64(&i);
	printf("i=%d\n",i);
}

void test_ReadInt16()
{
    String str("godblessluckypeople");
    AutoPtr<IStringBufferInputStream> isbis;
	ECode ec = CStringBufferInputStream::New(str, (IStringBufferInputStream**)&isbis);
    AutoPtr<IDataInputStream> idis;
    ec = CDataInputStream::New(isbis,(IDataInputStream**)&idis);

	AutoPtr<IDataInput> di;
	di = IDataInput::Probe(idis);

	Int16 i = 9;
	ec = di->ReadInt16(&i);
	printf("i=%d\n",i);
}

void test_ReadUnsignedByte()
{
    String str("godblessluckypeople");
    AutoPtr<IStringBufferInputStream> isbis;
	ECode ec = CStringBufferInputStream::New(str, (IStringBufferInputStream**)&isbis);
    AutoPtr<IDataInputStream> idis;
    ec = CDataInputStream::New(isbis,(IDataInputStream**)&idis);

	AutoPtr<IDataInput> di;
	di = IDataInput::Probe(idis);

	Int32 i = 125;
	ec = di->ReadUnsignedByte(&i);
	printf("i=%d\n",i);
}

void test_SkipBytes()
{
    String str("godblessluckypeople");
    AutoPtr<IStringBufferInputStream> isbis;
	ECode ec = CStringBufferInputStream::New(str, (IStringBufferInputStream**)&isbis);
    AutoPtr<IDataInputStream> idis;
    ec = CDataInputStream::New(isbis,(IDataInputStream**)&idis);

	AutoPtr<IDataInput> di;
	di = IDataInput::Probe(idis);

	Int32 count = 12;
	Int32 number = 0;
	ec = di->SkipBytes(count,&number);
	printf("number=%d\n",number);
	assert(number==12);
}

int main()
{
    printf("=====test begin=====\n");
    test_ReadFullyEx();
    printf("=====test end=======\n");
	return 0;
}
