
#include "CActivityOne.h"
#include "gen/fwLocalSocket/R.h"
//#include "Uri.h"
#include <tstcom.h>
#include <elastos/core/Math.h>
#include <unistd.h>
#include <utils/Timers.h>
//using namespace Elastos::Core;
TPDECL;

typedef ECode (CActivityOne::*PTestEntry)();
PTestEntry TestEntry[] = {
    &CActivityOne::Test1,
    &CActivityOne::Test2,
};

static int s_RunCount = 0;
Boolean s_beQuit = true;
sem_t g_sem;
typedef struct
{
    CActivityOne* mActivity;
    int mIndex;
} ActivityParams;

const String CActivityOne::mSockAddr("com.android.net.LocalSocketTest");

void* CActivityOne::EntryRoutine(void *arg)
{
    ActivityParams* paramsArg = reinterpret_cast<ActivityParams*>(arg);
    CActivityOne* activity = paramsArg->mActivity;
    int index = paramsArg->mIndex;
    do {
        activity->PostcppCallback(index);
        sem_wait(&g_sem);
    } while (!s_beQuit);
#ifdef AutoExit
    TPOK;
    KillElastosProcess();
#endif
    return reinterpret_cast<void*>(0);
}

ECode CActivityOne::HandleMessage(int index)
{
    (this->*TestEntry[index-1])();
    sem_post(&g_sem);
    return NOERROR;
}

ECode CActivityOne::PostcppCallback(int index)
{
    ECode (STDCALL CActivityOne::*pHandlerFunc)(int index);
    pHandlerFunc = &CActivityOne::HandleMessage;

    AutoPtr<IParcel> params;
    CCallbackParcel::New((IParcel**)&params);
    params->WriteInt32Ptr(index);
    Boolean result;
    return mDefaultHandler->PostCppCallback((Handle32)this, *(Handle32*)&pHandlerFunc, params, 0, FALSE, &result);
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(Elastos::R::layout::main);

    sem_init(&g_sem, 0, 0);

    CHandler::New((IHandler**)&mDefaultHandler);

    String strPackageName;
    char szIndex[10];
    Int32 index;
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    intent->GetInt32Extra(String("ARG"), -1, &index);
    intent->GetPackage(&strPackageName);

    int nLength = sizeof(TestEntry) / sizeof(TestEntry[0]);
    if (index <= 0 || index > nLength) {
        printf("*ERROR* Invalid testing case number\n");
        return -1;
    }

    sprintf(szIndex , "%d" , index);
    char *argv[] = {(char*)(const char *)strPackageName, szIndex};
    int argc = 2;

    TPINIT;

    ActivityParams* pArg = new ActivityParams;
    pArg->mActivity = this;
    pArg->mIndex = index;

    pthread_t   threadMessage;
    pthread_create(&threadMessage, NULL, EntryRoutine, (void*)pArg);

    return NOERROR;
}

/*
 * 测试简述: testLocalConnections
 * 测试类型: 功能测试
 * 优先级别: P1
 * 结果检查: Dailybuild
 */
ECode CActivityOne::Test1()
{
    // create client and server socket
    AutoPtr<ILocalServerSocket> localServerSocket;
    CLocalServerSocket::New(mSockAddr, (ILocalServerSocket**)&localServerSocket);
    AutoPtr<ILocalSocket> clientSocket;
    CLocalSocket::New((ILocalSocket**)&clientSocket);

    // establish connection between client and server
    AutoPtr<ILocalSocketAddress> locSockAddr;
    CLocalSocketAddress::New(mSockAddr, (ILocalSocketAddress**)&locSockAddr);
    Boolean bConnected;

    clientSocket->IsConnected(&bConnected);
    TPAssertFalse("clientSocket IsConnected ", bConnected);
    clientSocket->Connect(locSockAddr);
    clientSocket->IsConnected(&bConnected);
    TPAssertTrue("clientSocket IsConnected ", bConnected);
    AutoPtr<ILocalSocket> serverSocket;
    localServerSocket->Accept((ILocalSocket**)&serverSocket);

    AutoPtr<ICredentials> credent;
    clientSocket->GetPeerCredentials((ICredentials**)&credent);
    Int32 pid;
    credent->GetPid(&pid);
    TPAssertTrue("credent.pid is not 0", (0 != pid));

    // send data from client to server
    AutoPtr<IOutputStream> clientOutStream;
    clientSocket->GetOutputStream((IOutputStream**)&clientOutStream);
    clientOutStream->Write(12);
    AutoPtr<IInputStream> serverInStream;
    serverSocket->GetInputStream((IInputStream**)&serverInStream);
    Int32 inStream;
    serverInStream->Read(&inStream);
    TPAssertEquals(12, inStream);

    //send data from server to client
    AutoPtr<IOutputStream> serverOutStream;
    serverSocket->GetOutputStream((IOutputStream**)&serverOutStream);
    serverOutStream->Write(3);
    AutoPtr<IInputStream> clientInStream;
    clientSocket->GetInputStream((IInputStream**)&clientInStream);
    Int32 cinStream;
    clientInStream->Read(&cinStream);
    TPAssertEquals(3, cinStream);

    // Test sending and receiving file descriptors
    //new FileDescriptor[]{FileDescriptor.in}
    AutoPtr<IFileDescriptorHelper> fdHelper;
    CFileDescriptorHelper::AcquireSingleton((IFileDescriptorHelper**)&fdHelper);
    AutoPtr<IFileDescriptor> fd_IN;
    fdHelper->GetInFile((IFileDescriptor**)&fd_IN);
    AutoPtr< ArrayOf<IFileDescriptor*> > fdArray = ArrayOf<IFileDescriptor*>::Alloc(1);
    fdArray->Set(0, fd_IN.Get());

    Int32 num;
    clientSocket->SetFileDescriptorsForSend(fdArray);
    clientOutStream->Write(32);
    serverInStream->Read(&num);
    TPAssertEquals(32, num);

    AutoPtr< ArrayOf<IFileDescriptor*> > out;
    serverSocket->GetAncillaryFileDescriptors((ArrayOf<IFileDescriptor*>**)&out);
    TPAssertEquals(1, out->GetLength());
    AutoPtr<IFileDescriptor> fd;
    clientSocket->GetFileDescriptor((IFileDescriptor**)&fd);
    Boolean bValid;
    fd->Valid(&bValid);
    TPAssertTrue("fd is valid", bValid);

    //shutdown input stream of client
    clientSocket->ShutdownInput();
    clientInStream->Read(&num);
    TPAssertEquals(-1, num);

    //shutdown output stream of client
    clientSocket->ShutdownOutput();


    ECode ec = clientOutStream->Write(10);
    if (SUCCEEDED(ec)) {
        TPERR("testLocalSocket shouldn't come to here1 failed.ec = 0x%x.\n", ec);
        goto Exit;
    }

    //shutdown input stream of server
    serverSocket->ShutdownInput();
    serverInStream->Read(&num);
    TPAssertEquals(-1, num);

    //shutdown output stream of server
    serverSocket->ShutdownOutput();

    ec = serverOutStream->Write(10);
    if (SUCCEEDED(ec)) {
        TPERR("testLocalSocket shouldn't come to here2 failed.ec = 0x%x.\n", ec);
        goto Exit;
    }

    //close client socket
    clientSocket->Close();
    ec = clientInStream->Read(&num);
    if (SUCCEEDED(ec)) {
        TPERR("testLocalSocket shouldn't come to here3 failed.ec = 0x%x.\n", ec);
        goto Exit;
    }

    //close server socket
    serverSocket->Close();

    ec = serverInStream->Read(&num);
    if (SUCCEEDED(ec)) {
        TPERR("testLocalSocket shouldn't come to here4 failed.ec = 0x%x.\n", ec);
        goto Exit;
    }

Exit:
    return NOERROR;
}

/*
 * 测试简述: testAccessors
 * 测试类型: 功能测试
 * 优先级别: P1
 * 结果检查: Dailybuild
 */
ECode CActivityOne::Test2()
{
    AutoPtr<ILocalSocket> socket;
    CLocalSocket::New((ILocalSocket**)&socket);
    AutoPtr<ILocalSocketAddress> addr;
    CLocalSocketAddress::New(String("secondary"), (ILocalSocketAddress**)&addr);

    Boolean bol;
    socket->IsBound(&bol);
    TPAssertFalse("socket IsBound ", bol);
    socket->Bind(addr);
    socket->IsBound(&bol);
    TPAssertTrue("socket IsBound ", bol);
    AutoPtr<ILocalSocketAddress> localAddr;
    socket->GetLocalSocketAddress((ILocalSocketAddress**)&localAddr);
    TPAssertEquals(addr.Get(), localAddr.Get());

//    String str;
//    socket->ToString(&str);
//    bol = str.Contains("impl:android.net.LocalSocketImpl");
//    TPAssertTrue("socket contains impl:android.net.LocalSocketImpl", bol);

    socket->SetReceiveBufferSize(1999);
    Int32 bufSize;
    socket->GetReceiveBufferSize(&bufSize);
    TPAssertEquals(1999 << 1, bufSize);

    socket->SetSendBufferSize(1998);
    socket->GetSendBufferSize(&bufSize);
    TPAssertEquals(1998 << 1, bufSize);

    // Timeout is not support at present, so set is ignored
//    socket->SetSoTimeout(1996);
//    Int32 timeout;
//    socket->GetSoTimeout(&timeout);
//    TPAssertEquals(0, timeout);

    ECode ec;
    AutoPtr<ILocalSocketAddress> remotAddr;
    ec = socket->GetRemoteSocketAddress((ILocalSocketAddress**)&remotAddr);
    if (SUCCEEDED(ec)) {
        TPERR("testLocalSocketSecondary shouldn't come to here1 failed.ec = 0x%x.\n", ec);
        goto Exit;
    }

    ec = socket->IsClosed(&bol);
    if (SUCCEEDED(ec)) {
        TPERR("testLocalSocketSecondary shouldn't come to here2 failed.ec = 0x%x.\n", ec);
        goto Exit;
    }

    ec = socket->IsInputShutdown(&bol);
    if (SUCCEEDED(ec)) {
        TPERR("testLocalSocketSecondary shouldn't come to here3 failed.ec = 0x%x.\n", ec);
        goto Exit;
    }

    ec = socket->IsOutputShutdown(&bol);
    if (SUCCEEDED(ec)) {
        TPERR("testLocalSocketSecondary shouldn't come to here4 failed.ec = 0x%x.\n", ec);
        goto Exit;
    }

    ec = socket->Connect(addr, 2005);
    if (SUCCEEDED(ec)) {
        TPERR("testLocalSocketSecondary shouldn't come to here5 failed.ec = 0x%x.\n", ec);
        goto Exit;
    }

Exit:
    return NOERROR;
}

