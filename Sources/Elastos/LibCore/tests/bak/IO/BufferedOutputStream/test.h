
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
    int TestConstructor2(int argc,char *argv[]);
    int TestClose(int argc,char *argv[]);
    int TestFlush(int argc,char *argv[]);
    int TestWrite(int argc,char *argv[]);
    int TestWriteBuffer(int argc,char *argv[]);
    int TestWriteBufferEx(int argc,char *argv[]);
    int TestCheckError(int argc,char *argv[]);

    CTest();
    ~CTest();

private:
    IByteArrayOutputStream* m_pByteAos;
    IByteArrayInputStream* m_pByteAis;
//    IOutputStream* os;
    IBufferedOutputStream* m_pBufOs;
    BufferOf<Byte>* m_pFileString;

};
#endif