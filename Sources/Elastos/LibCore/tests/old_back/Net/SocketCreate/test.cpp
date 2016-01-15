#include "test.h"
#include <stdio.h>
#include <Elastos.Net.h>
#include <elautoptr.h>
#include <malloc.h>
using namespace Elastos;
using namespace Elastos::Core;
//using namespace Elastos::Core;
//#define DINFO() printf("%s, %d\n", __FILE__, __LINE__)
#define DINFO()

int CTest::test1(int argc, char* argv[])
{
    AutoPtr<IServerSocket> pServerSocket;
    AutoPtr<ISocket> pSock;
    AutoPtr<IBufferedReader> pBufReader;
    AutoPtr<IInputStreamReader> pInReader;
    AutoPtr<IInputStream> pIn;
    AutoPtr<INetGlobalInit> init;

    ECode ec = NOERROR;
    ec = CNetGlobalInit::New((INetGlobalInit**)&init);
    if (FAILED(ec)) {
        printf("Global Init Failed! %x\n", ec);
        return ec;
    }

    ec = CServerSocket::New(10000, (IServerSocket**)&pServerSocket);
    DINFO();
    if (FAILED(ec)) {
        printf("Cann't Create the server Socket! ec = %x\n", ec);
        return ec;
    }

    while(1) {
        DINFO();
        ec = pServerSocket->Accept((ISocket**)&pSock);
        if (FAILED(ec)) {
            printf("Cann't Accept from the server Socket! ec = %x\n", ec);
            return ec;
        }
        DINFO();
        ec = pSock->GetInputStream((IInputStream**) &pIn);
        if (FAILED(ec)) {
            printf("Cann't GetInputStream from the Socket! ec = %x\n", ec);
            return ec;
        }
        DINFO();

        IDataInputStream* pDataIn;
        ec = CDataInputStream::New(pIn, (IDataInputStream**)&pDataIn);
        if (FAILED(ec)) {
            printf("New DataInputStream Failed!\n");
            return ec;
        }

        ArrayOf<Byte>* buf = ArrayOf<Byte>::Alloc(1000);
        Int32 number = 0;
        Int32 totalnumber = 0;
        pDataIn->ReadBufferEx(0, 29, buf, &number);
        while(number > 0) {
            totalnumber += number;
            ec = pDataIn->ReadBufferEx(0, 29, buf, &number);
            if (FAILED(ec)) {
                printf("Continue Read FAILED!\n");
            }
        }
        printf("the totalnumber is %d\n", totalnumber);
//        ec = CInputStreamReader::New(pIn, (IInputStreamReader**)&pInReader);
//        DINFO();
//        if (FAILED(ec)) {
//            printf("New IInputStreamReader FAILED! ec = %x\n", ec);
//            return ec;
//        }
//        DINFO();
//        ec = CBufferedReader::New(pInReader, (IBufferedReader**) &pBufReader);
//        DINFO();
//        if (FAILED(ec)) {
//            printf("New CBufferedReader FAILED! ec = %x\n", ec);
//            return ec;
//        }
//        DINFO();
//        String str;
//        DINFO();
//        pBufReader->ReadLine(&str);
//        DINFO();
        pSock->Close();
    }

    pServerSocket->Close();
    return ec;
}

int CTest::test2(int argc, char* argv[])
{
    AutoPtr<ISocket> pSock;
    AutoPtr<IOutputStream> pOut;
    AutoPtr<IDataOutputStream> pDataOutput;
    AutoPtr<INetGlobalInit> init;

    ECode ec = NOERROR;
    ec = CNetGlobalInit::New((INetGlobalInit**)&init);
    if (FAILED(ec)) {
        printf("Global Init Failed! %x\n", ec);
        return ec;
    }

    ec = CSocket::New(String("127.0.0.1"), 10000, (ISocket**) &pSock);
    if (FAILED(ec)) {
        printf("Cann't Create the socket!ec = %x, line = %d\n", ec, __LINE__);
        return ec;
    }

    DINFO();
    ec = pSock->GetOutputStream((IOutputStream**) &pOut);
    if (FAILED(ec)) {
        printf("Cann't GetOutputStream!ec = %x, line = %d\n", ec, __LINE__);
        return ec;
    }

    DINFO();
    ec = CDataOutputStream::New(pOut, (IDataOutputStream**) &pDataOutput);
    if (FAILED(ec)) {
        printf("Cann't CDataOutputStream = %x, line = %d\n", ec, __LINE__);
        return ec;
    }

    DINFO();
    ArrayOf<Byte> *buf;
    Int32 count = 0;
    buf = ArrayOf<Byte>::Alloc(30);
    while(count < 10) {
        count++;
        buf->Copy((Byte*)"12345jianfeng simple testdfgop", 30);
        pDataOutput->WriteBuffer(*buf);
    }
    DINFO();
    pSock->Close();
    return 0;
}



int CTest::test3(int argc, char* argv[])
{
    return 0;
}

int CTest::test4(int argc, char* argv[])
{
    return 0;
}


int CTest::test5(int argc, char* argv[])
{
    return 0;
}

int CTest::test6(int argc, char* argv[])
{
    return 0;
}

int CTest::test7(int argc, char* argv[])
{
    return 0;
}

int CTest::test8(int argc, char* argv[])
{
    return 0;
}

