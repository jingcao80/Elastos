
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Libcore.h>
#include <elastos/core/Runnable.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/droid/system/OsConstants.h>

using Elastos::Droid::Net::ILocalSocket;
using Elastos::Droid::Net::ILocalServerSocket;
using Elastos::Droid::Net::CLocalServerSocket;
using Elastos::Droid::Os::IDebug;
using Elastos::Droid::Os::CDebug;
using Elastos::Droid::System::OsConstants;
using Elastos::Core::IThread;
using Elastos::Core::CThread;
using Elastos::Core::Runnable;
using Elastos::IO::IBufferedReader;
using Elastos::IO::CBufferedReader;
using Elastos::IO::ICloseable;
using Elastos::IO::IInputStream;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::CFileDescriptor;
using Elastos::IO::IReader;
using Elastos::IO::CInputStreamReader;
using Elastos::Utility::Logging::Logger;
using Libcore::IO::IIoBridge;
using Libcore::IO::CIoBridge;

class _Runnable
    : public Runnable
{
public:
    CARAPI Run();

private:
    CARAPI_(void) Dump();
};

ECode _Runnable::Run()
{
    AutoPtr<ILocalServerSocket> ssocket;
    ECode ec = CLocalServerSocket::New(String("elastos:heapdump"), (ILocalServerSocket**)&ssocket);
    if (FAILED(ec)) {
        Logger::W("HeapDump", "Create heapdump socket failed.");
        return NOERROR;
    }

    while (TRUE) {
        AutoPtr<ILocalSocket> socket;
        ssocket->Accept((ILocalSocket**)&socket);

        AutoPtr<IInputStream> input;
        socket->GetInputStream((IInputStream**)&input);
        AutoPtr<IReader> reader;
        CInputStreamReader::New(input, (IReader**)&reader);
        AutoPtr<IBufferedReader> socketReader;
        CBufferedReader::New(reader, 256, (IBufferedReader**)&socketReader);

        String cmd;
        socketReader->ReadLine(&cmd);
        if (cmd.Equals("dump")) {
            Dump();
        }
    }

    return NOERROR;
}

void _Runnable::Dump()
{
    AutoPtr<IFile> file;
    CFile::New(String("/data/debug"), (IFile**)&file);
    Boolean bval;
    file->Exists(&bval);
    if (!bval) {
        file->Mkdirs(&bval);
    }

    AutoPtr<IIoBridge> ioBridge;
    CIoBridge::AcquireSingleton((IIoBridge**)&ioBridge);
    AutoPtr<IFileDescriptor> ifd;
    ioBridge->Open(String("/data/debug/memo_dump.txt"),
            OsConstants::_O_RDWR | OsConstants::_O_CREAT | OsConstants::_O_TRUNC,
            (IFileDescriptor**)&ifd);

    AutoPtr<IDebug> dbg;
    CDebug::AcquireSingleton((IDebug**)&dbg);
    dbg->DumpHeap(ifd);
}

void __attribute__((constructor)) StartDumpHeapServer()
{
    AutoPtr<IRunnable> r = new _Runnable();
    AutoPtr<IThread> thread;
    CThread::New(r, (IThread**)&thread);
    thread->Start();
}
