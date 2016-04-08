
#include <elastos.h>
#include <stdio.h>
#include <elautoptr.h>
#include "Elastos.CoreLibrary.h"

using namespace Elastos;

void testConstructor()
{
    AutoPtr<IOutputStream> os;
	AutoPtr<IFilterOutputStream> fos;
	CFilterOutputStream::New(os,(IFilterOutputStream **)&fos);
}
void testWrite()
{
    Int32 oneByte = 97;
    String fp = String("/data/data/com.elastos.runtime/elastos/iotest.txt");
    AutoPtr<IFileOutputStream> fos;
    CFileOutputStream::New(fp, (IFileOutputStream**)&fos);
	AutoPtr<IFilterOutputStream> ftos;
	CFilterOutputStream::New(fos,(IFilterOutputStream **)&ftos);
	ftos->Write(oneByte);
	oneByte = 98;
	ftos->Write(oneByte);
}
void testWriteBuffer()
{
    ArrayOf_<Byte,8> bytes;
	bytes[0] = 97;
	bytes[1] = 98;
	bytes[2] = 99;
	bytes[3] = 100;
	bytes[4] = 101;
	bytes[5] = 102;
	bytes[6] = 103;
	bytes[7] = 104;
    String fp = String("/data/data/com.elastos.runtime/elastos/iotest.txt");
    AutoPtr<IFileOutputStream> fos;
    CFileOutputStream::New(fp, (IFileOutputStream**)&fos);
	AutoPtr<IFilterOutputStream> ftos;
	CFilterOutputStream::New(fos,(IFilterOutputStream **)&ftos);
	ftos->WriteBuffer(bytes);
	ftos->Flush();
    ftos->Close();
}
int main(int argc, char *argv[])
{
    printf("output\n");
	testWriteBuffer();
	//testWrite();
	//testConstructor();
    printf("output2\n");
}
