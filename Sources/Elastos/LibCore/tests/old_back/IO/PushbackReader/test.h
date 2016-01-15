
#ifndef __TEST_H__
#define __TEST_H__

//#include "tstprintf.h"
#include <stdlib.h>
#include <stdio.h>
#include "Elastos.CoreLibrary.h"
#include "elastos.h"

using namespace Elastos;

class CTest{

public:
    int TestConstructor(int argc,char *argv[]);
    int TestConstructor2(int argc,char *argv[]);
    int TestClose(int argc,char *argv[]);
    int TestMark(int argc,char *argv[]);
    int TestIsMarkSupported(int argc,char *argv[]);
    int TestRead(int argc,char *argv[]);
    int TestReadBuffer(int argc,char *argv[]);
    int TestReadBufferEx(int argc,char *argv[]);
    int TestIsReady(int argc,char *argv[]);
    int TestReset(int argc,char *argv[]);
    int TestSkip(int argc,char *argv[]);
    int TestUnRead(int argc,char *argv[]);
    int TestUnReadBuffer(int argc,char *argv[]);
    int TestUnReadBufferEx(int argc,char *argv[]);


    CTest();
    ~CTest();

private:
    IPushbackReader* m_pr;

    static const String m_text;
//    String m_text;
    String m_dosText;


};
#endif