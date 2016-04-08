
#ifndef __TEST_H__
#define __TEST_H__

//#include "tstprintf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elatypes.h>
#include <stdarg.h>

#include "stdio.h"
#include "Elastos.CoreLibrary.h"
#include "elastos.h"

using namespace Elastos;

class CTest{

public:
    int TestConstructor(int argc,char *argv[]);
    int TestClose(int argc,char *argv[]);
    int TestFlush(int argc,char *argv[]);
    int TestWrite(int argc,char *argv[]);
    int TestWriteBuffer(int argc,char *argv[]);
    int TestWriteBufferEx(int argc,char *argv[]);
    int TestCheckError(int argc,char *argv[]);
    int TestWriteBoolean(int argc,char *argv[]);
    int TestWriteByte(int argc,char *argv[]);
    int TestWriteBytes(int argc,char *argv[]);
    int TestWriteBytesEx(int argc,char *argv[]);
    int TestWriteChar(int argc,char *argv[]);
    int TestWriteDouble(int argc,char *argv[]);
    int TestWriteFloat(int argc,char *argv[]);
    int TestWriteInt32(int argc,char *argv[]);
    int TestWriteInt64(int argc,char *argv[]);
    int TestWriteInt16(int argc,char *argv[]);


    CTest();
    ~CTest();

private:
    IDataOutputStream* m_pDos;
    IDataInputStream* m_pDis;
    IByteArrayOutputStream* m_pBaos;
    String testString;
    String mUnihw;
    Int32 testLength;

};
#endif