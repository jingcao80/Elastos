#include "Elastos.Droid.Net.h"
//TODO #include "elastos/net/InetUnixAddress.h"
#include "elastos/droid/server/am/NativeCrashListener.h"
#include "elastos/droid/system/Os.h"
#include "elastos/droid/system/OsConstants.h"
#include "elastos/core/AutoLock.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::CApplicationErrorReportCrashInfo;
using Elastos::Droid::App::IApplicationErrorReportCrashInfo;
//TODO using Elastos::Droid::System::CStructTimevalHelper;
using Elastos::Droid::System::IStructTimeval;
using Elastos::Droid::System::IStructUcred;
using Elastos::Droid::System::IStructTimevalHelper;
using Elastos::Droid::System::Os;
using Elastos::Droid::System::OsConstants;

using Elastos::Core::AutoLock;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::CFile;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::IFile;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IOutputStream;
//TODO using Elastos::Net::CInetSocketAddress;
using Elastos::Net::IInetSocketAddress;
//TODO using Elastos::Net::InetUnixAddress;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

//=====================================================================
//               NativeCrashListener::NativeCrashReporter
//=====================================================================
NativeCrashListener::NativeCrashReporter::NativeCrashReporter(
    /* [in] */ ProcessRecord* app,
    /* [in] */ Int32 signal,
    /* [in] */ const String& report,
    /* [in] */ NativeCrashListener* owner)
{
    Thread::constructor(String("NativeCrashReport"));
    mApp = app;
    mSignal = signal;
    mCrashReport = report;
    mOwner = owner;
}

ECode NativeCrashListener::NativeCrashReporter::Run()
{
    //try {
        AutoPtr<IApplicationErrorReportCrashInfo> ci;
        CApplicationErrorReportCrashInfo::New((IApplicationErrorReportCrashInfo**)&ci);
        ci->SetExceptionClassName(String("Native crash"));
        ci->SetExceptionMessage(String(""))/*TODO Os::Strsignal(mSignal)*/;
        ci->SetThrowFileName(String("unknown"));
        ci->SetThrowClassName(String("unknown"));
        ci->SetThrowMethodName(String("unknown"));
        ci->SetStackTrace(mCrashReport);

        if (DEBUG) Slogger::V(TAG, "Calling handleApplicationCrash()");
        mOwner->mAm->HandleApplicationCrashInner(String("native_crash"), mApp, mApp->mProcessName, ci);
        if (DEBUG) Slogger::V(TAG, "<-- handleApplicationCrash() returned");
    //} catch (Exception e) {
    //    Slog.e(TAG, "Unable to report native crash", e);
    //}
    return NOERROR;
}

//=====================================================================
//                         NativeCrashListener
//=====================================================================
const String NativeCrashListener::TAG("NativeCrashListener");
const Boolean NativeCrashListener::DEBUG = FALSE;
const Boolean NativeCrashListener::MORE_DEBUG = DEBUG && FALSE;
const String NativeCrashListener::DEBUGGERD_SOCKET_PATH("/data/system/ndebugsocket");
const Int64 NativeCrashListener::SOCKET_TIMEOUT_MILLIS;

NativeCrashListener::NativeCrashListener(
    /* [in] */ CActivityManagerService* am)
{
    mAm = am;
}

ECode NativeCrashListener::Run()
{
    AutoPtr<ArrayOf<Byte> > ackSignal = ArrayOf<Byte>::Alloc(1);

    if (DEBUG) Slogger::I(TAG, "Starting up");

    // The file system entity for this socket is created with 0700 perms, owned
    // by system:system.  debuggerd runs as root, so is capable of connecting to
    // it, but 3rd party apps cannot.
    {
        AutoPtr<IFile> socketFile;
        CFile::New(DEBUGGERD_SOCKET_PATH, (IFile**)&socketFile);
        Boolean exists;
        if (socketFile->Exists(&exists), exists) {
            socketFile->Delete();
        }
    }

    //try {
        AutoPtr<IFileDescriptor> serverFd;
        System::Os::Socket(OsConstants::_AF_UNIX, OsConstants::_SOCK_STREAM, 0, (IFileDescriptor**)&serverFd);
        //TODO AutoPtr<InetUnixAddress> sockAddr = new InetUnixAddress(DEBUGGERD_SOCKET_PATH);
        //TODO System::Os::Bind(serverFd, sockAddr, 0);
        System::Os::Listen(serverFd, 1);

        while (TRUE) {
            //TODO AutoPtr<IInetSocketAddress> peer;//= new InetSocketAddress();
            //TODO CInetSocketAddress::New((IInetSocketAddress**)&peer);
            AutoPtr<IFileDescriptor> peerFd;
            //try {
                if (MORE_DEBUG) Slogger::V(TAG, "Waiting for debuggerd connection");
                //TODO System::Os::Accept(serverFd, peer, (IFileDescriptor**)&peerFd);
                if (MORE_DEBUG) Slogger::V(TAG, "Got debuggerd socket ");//TODO + peerFd);
                if (peerFd != NULL) {
                    // Only the superuser is allowed to talk to us over this socket
                    AutoPtr<IStructUcred> credentials;
                    System::Os::GetsockoptUcred(peerFd, OsConstants::_SOL_SOCKET,
                            OsConstants::_SO_PEERCRED, (IStructUcred**)&credentials);
                    Int32 uid;
                    credentials->GetUid(&uid);
                    if (uid == 0) {
                        // the reporting thread may take responsibility for
                        // acking the debugger; make sure we play along.
                        ConsumeNativeCrashData(peerFd);
                    }
                }
            //} catch (Exception e) {
            //    Slog.w(TAG, "Error handling connection", e);
            //} finally {
                // Always ack debuggerd's connection to us.  The actual
                // byte written is irrelevant.
                if (peerFd != NULL) {
                    //try {
                        Int32 num;
                        ECode ec = System::Os::Write(peerFd, ackSignal, 0, 1, &num);
                    //} catch (Exception e) {
                        /* we don't care about failures here */
                        if (FAILED(ec) && MORE_DEBUG) {
                            Slogger::D(TAG, "Exception writing ack: ");// + e.getMessage());
                        }
                    //}
                    //try {
                        ec = System::Os::Close(peerFd);
                    //} catch (ErrnoException e) {
                        if (FAILED(ec) && MORE_DEBUG) {
                            Slogger::D(TAG, "Exception closing socket: ");// + e.getMessage());
                        }
                    //}
                }
            //}
        }
    //} catch (Exception e) {
    //    Slog.e(TAG, "Unable to init native debug socket!", e);
    //}
    return NOERROR;
}

Int32 NativeCrashListener::UnpackInt(
    /* [in] */ ArrayOf<Byte>* buf,
    /* [in] */ Int32 offset)
{
    Int32 b0, b1, b2, b3;

    b0 = ((Int32) (*buf)[offset]) & 0xFF; // mask against sign extension
    b1 = ((Int32) (*buf)[offset+1]) & 0xFF;
    b2 = ((Int32) (*buf)[offset+2]) & 0xFF;
    b3 = ((Int32) (*buf)[offset+3]) & 0xFF;
    return (b0 << 24) | (b1 << 16) | (b2 << 8) | b3;
}

Int32 NativeCrashListener::ReadExactly(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 numBytes)
{
    Int32 totalRead = 0;
    while (numBytes > 0) {
        Int32 n;
        System::Os::Read(fd, buffer, offset + totalRead, numBytes, &n);
        if (n <= 0) {
            if (DEBUG) {
                //Slogger::W(TAG, "Needed " + numBytes + " but saw " + n);
            }
            return -1;  // premature EOF or timeout
        }
        numBytes -= n;
        totalRead += n;
    }
    return totalRead;
}

ECode NativeCrashListener::ConsumeNativeCrashData(
    /* [in] */ IFileDescriptor* fd)
{
    if (MORE_DEBUG) Slogger::I(TAG, "debuggerd connected");
    AutoPtr<ArrayOf<Byte> > buf = ArrayOf<Byte>::Alloc(4096);
    AutoPtr<IByteArrayOutputStream> os;
    CByteArrayOutputStream::New(4096, (IByteArrayOutputStream**)&os);

    //try {
        AutoPtr<IStructTimevalHelper> stHelper;
        //TODO CStructTimevalHelper::AcquireSingleton((IStructTimevalHelper**)&stHelper);
        AutoPtr<IStructTimeval> timeout;
        stHelper->FromMillis(SOCKET_TIMEOUT_MILLIS, (IStructTimeval**)&timeout);
        System::Os::SetsockoptTimeval(fd, OsConstants::_SOL_SOCKET, OsConstants::_SO_RCVTIMEO, timeout);
        System::Os::SetsockoptTimeval(fd, OsConstants::_SOL_SOCKET, OsConstants::_SO_SNDTIMEO, timeout);

        // first, the pid and signal number
        Int32 headerBytes = ReadExactly(fd, buf, 0, 8);
        if (headerBytes != 8) {
            // protocol failure; give up
            Slogger::E(TAG, "Unable to read from debuggerd");
            return NOERROR;
        }

        Int32 pid = UnpackInt(buf, 0);
        Int32 signal = UnpackInt(buf, 4);
        if (DEBUG) {
            //Slogger::V(TAG, "Read pid=" + pid + " signal=" + signal);
        }

        // now the text of the dump
        if (pid > 0) {
            AutoPtr<ProcessRecord> pr;
            {
                AutoLock lock(mAm->mPidsSelfLockedLock);
                pr = mAm->mPidsSelfLocked[pid];
            }
            if (pr != NULL) {
                // Don't attempt crash reporting for persistent apps
                if (pr->mPersistent) {
                    if (DEBUG) {
                        Slogger::V(TAG, "Skipping report for persistent app ");// + pr);
                    }
                    return NOERROR;
                }

                Int32 bytes;
                do {
                    // get some data
                    System::Os::Read(fd, buf, 0, buf->GetLength(), &bytes);
                    if (bytes > 0) {
                        if (MORE_DEBUG) {
                            //String s = new String(buf, 0, bytes, "UTF-8");
                            //Slog.v(TAG, "READ=" + bytes + "> " + s);
                        }
                        // did we just get the EOD null byte?
                        if ((*buf)[bytes-1] == 0) {
                            IOutputStream::Probe(os)->Write(buf, 0, bytes-1);  // exclude the EOD token
                            break;
                        }
                        // no EOD, so collect it and read more
                        IOutputStream::Probe(os)->Write(buf, 0, bytes);
                    }
                } while (bytes > 0);

                // Okay, we've got the report.
                if (DEBUG) Slogger::V(TAG, "processing");

                // Mark the process record as being a native crash so that the
                // cleanup mechanism knows we're still submitting the report
                // even though the process will vanish as soon as we let
                // debuggerd proceed.
                {
                    AutoLock lock(mAm);
                    pr->mCrashing = TRUE;
                    pr->mForceCrashReport = TRUE;
                }

                // Crash reporting is synchronous but we want to let debuggerd
                // go about it business right away, so we spin off the actual
                // reporting logic on a thread and let it take it's time.
                AutoPtr<ArrayOf<Byte> > osbytes;
                os->ToByteArray((ArrayOf<Byte>**)&osbytes);
                //String reportString = new String(os.toByteArray(), "UTF-8");
                String reportString(*osbytes);
                AutoPtr<NativeCrashReporter> ncr = new NativeCrashReporter(pr, signal, reportString, this);
                ncr->Start();
            } else {
                Slogger::W(TAG, "Couldn't find ProcessRecord for pid %d", pid);
            }
        } else {
            Slogger::E(TAG, "Bogus pid!");
        }
    //} catch (Exception e) {
    //    Slog.e(TAG, "Exception dealing with report", e);
        // ugh, fail.
    //}
    return NOERROR;
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
