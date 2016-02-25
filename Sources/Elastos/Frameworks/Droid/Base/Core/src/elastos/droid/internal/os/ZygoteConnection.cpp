
#include "elastos/droid/ext/frameworkdef.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Libcore.h>
#include "Elastos.Droid.Net.h"
#include "elastos/droid/internal/os/ZygoteConnection.h"
#include "elastos/droid/internal/os/CZygoteInit.h"
#include "elastos/droid/internal/os/RuntimeInit.h"
#include "elastos/droid/internal/os/Zygote.h"
#include "elastos/droid/os/SELinux.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/SystemProperties.h"
#include <Elastos.CoreLibrary.h>
#include <elastos/core/StringUtils.h>
#include <elastos/droid/system/Os.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::SELinux;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::SystemProperties;
using Elastos::IO::ICloseable;
using Elastos::IO::IReader;
using Elastos::IO::IOutputStream;
using Elastos::IO::IInputStream;
using Elastos::IO::IInputStreamReader;
using Elastos::IO::CInputStreamReader;
using Elastos::IO::CDataOutputStream;
using Elastos::IO::CBufferedReader;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::CDataInputStream;
using Elastos::IO::IDataInput;
using Elastos::IO::IDataOutput;
using Elastos::IO::CPrintStream;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;
using Libcore::IO::IIoUtils;
using Libcore::IO::CIoUtils;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

const String ZygoteConnection::TAG("ZygoteConnection");
const Int32 ZygoteConnection::CONNECTION_TIMEOUT_MILLIS;
const Int32 ZygoteConnection::MAX_ZYGOTE_ARGC;
AutoPtr<ILocalSocket> ZygoteConnection::sPeerWaitSocket;

ZygoteConnection::Arguments::Arguments(
    /* [in] */ const ArrayOf<String>& args)
    : mUid(0)
    , mUidSpecified(FALSE)
    , mGid(0)
    , mGidSpecified(FALSE)
    , mDebugFlags(0)
    , mMountExternal(IZygote::MOUNT_EXTERNAL_NONE)
    , mTargetSdkVersion(0)
    , mTargetSdkVersionSpecified(FALSE)
    , mRuntimeInit(FALSE)
    , mCapabilitiesSpecified(FALSE)
    , mPermittedCapabilities(0)
    , mEffectiveCapabilities(0)
    , mSeInfoSpecified(FALSE)
    , mAbiListQuery(FALSE)
{
    ASSERT_SUCCEEDED(ParseArgs(args));
}

ECode ZygoteConnection::Arguments::ParseArgs(
    /* [in] */ const ArrayOf<String>& args)
{
    Int32 curArg = 0;

    for ( /* curArg */ ; curArg < args.GetLength(); curArg++) {
        String arg = args[curArg];

        if (arg.Equals("--")) {
            curArg++;
            break;
        }
        else if (arg.StartWith("--setuid=")) {
            if (mUidSpecified) {
                Logger::E(TAG, "Duplicate arg specified");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            mUidSpecified = TRUE;
            mUid = StringUtils::ParseInt32(arg.Substring(arg.IndexOf('=') + 1));
        }
        else if (arg.StartWith("--setgid=")) {
            if (mGidSpecified) {
                Logger::E(TAG, "Duplicate arg specified");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            mGidSpecified = TRUE;
            mGid = StringUtils::ParseInt32(arg.Substring(arg.IndexOf('=') + 1));
        }
        else if (arg.StartWith("--target-sdk-version=")) {
            if (mTargetSdkVersionSpecified) {
                // throw new IllegalArgumentException(
                //         "Duplicate target-sdk-version specified");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            mTargetSdkVersionSpecified = TRUE;
            mTargetSdkVersion = StringUtils::ParseInt32(arg.Substring(arg.IndexOf('=') + 1));
        }
        else if (arg.Equals("--enable-debugger")) {
            mDebugFlags |= IZygote::DEBUG_ENABLE_DEBUGGER;
        }
        else if (arg.Equals("--enable-safemode")) {
            mDebugFlags |= IZygote::DEBUG_ENABLE_SAFEMODE;
        }
        else if (arg.Equals("--enable-checkjni")) {
            mDebugFlags |= IZygote::DEBUG_ENABLE_CHECKJNI;
        }
        else if (arg.Equals("--enable-jni-logging")) {
            mDebugFlags |= IZygote::DEBUG_ENABLE_JNI_LOGGING;
        }
        else if (arg.Equals("--enable-assert")) {
            mDebugFlags |= IZygote::DEBUG_ENABLE_ASSERT;
        }
        else if (arg.Equals("--runtime-init")) {
            mRuntimeInit = TRUE;
        }
        else if (arg.StartWith("--seinfo=")) {
            if (mSeInfoSpecified) {
                Logger::E(TAG, "Duplicate arg specified");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            mSeInfoSpecified = TRUE;
            mSeInfo = arg.Substring(arg.IndexOf('=') + 1);
        }
        else if (arg.StartWith("--capabilities=")) {
            if (mCapabilitiesSpecified) {
                Logger::E(TAG, "Duplicate arg specified");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            mCapabilitiesSpecified = TRUE;
            String capString = arg.Substring(arg.IndexOf('=') + 1);

            AutoPtr< ArrayOf<String> > capStrings;
            StringUtils::Split(capString, String(","), 2, (ArrayOf<String>**)&capStrings);

            if (capStrings->GetLength() == 1) {
                mEffectiveCapabilities = StringUtils::ParseInt64((*capStrings)[0]);
                mPermittedCapabilities = mEffectiveCapabilities;
            }
            else {
                mPermittedCapabilities = StringUtils::ParseInt64((*capStrings)[0]);
                mEffectiveCapabilities = StringUtils::ParseInt64((*capStrings)[1]);
            }
        }
        else if (arg.StartWith("--rlimit=")) {
            // Duplicate --rlimit arguments are specifically allowed.
            AutoPtr<ArrayOf<String> > limitStrings;
            StringUtils::Split(arg.Substring(arg.IndexOf('=') + 1), String(","),
                (ArrayOf<String>**)&limitStrings);

            Int32 len = limitStrings->GetLength();
            if (len != 3) {
                Logger::E(TAG, "--rlimit= should have 3 comma-delimited ints");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            AutoPtr<ArrayOf<Int32> > rlimitTuple = ArrayOf<Int32>::Alloc(len);
            for (Int32 i = 0; i < len; i++) {
                (*rlimitTuple)[i] = StringUtils::ParseInt32((*limitStrings)[i]);
            }

            if (mRlimits == NULL) {
                mRlimits = new List< Int32Array >();
            }

            mRlimits->PushBack(rlimitTuple);
        }
        else if (arg.Equals("-classpath")) {
            if (mClasspath != NULL) {
                Logger::E(TAG, "Duplicate arg specified");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            if (curArg + 1 == args.GetLength()) {
                Logger::E(TAG, "-classpath requires argument");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            mClasspath = args[++curArg];
        }
        else if (arg.StartWith("--setgroups=")) {
            if (mGids != NULL) {
                Logger::E(TAG, "Duplicate arg specified");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            AutoPtr< ArrayOf<String> > params;
            String gids = arg.Substring(arg.IndexOf('=') + 1);
            StringUtils::Split(gids, String(","), (ArrayOf<String>**)&params);

            mGids = ArrayOf<Int32>::Alloc(params->GetLength());

            for (Int32 i = params->GetLength() - 1; i >= 0 ; i--) {
                (*mGids)[i] = StringUtils::ParseInt32((*params)[i]);
            }
        }
        else if (arg.Equals("--invoke-with")) {
            if (!mInvokeWith.IsNull()) {
                Logger::E(TAG, "Duplicate arg specified");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            // try {
            mInvokeWith = args[++curArg];
            // } catch (IndexOutOfBoundsException ex) {
            //     throw new IllegalArgumentException(
            //             "--invoke-with requires argument");
            // }
        }
        else if (arg.StartWith("--nice-name=")) {
            if (!mNiceName.IsNull()) {
                Logger::E(TAG, "Duplicate arg specified");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            mNiceName = arg.Substring(arg.IndexOf('=') + 1);
        }
        else if (arg.Equals("--mount-external-multiuser")) {
            mMountExternal = IZygote::MOUNT_EXTERNAL_MULTIUSER;
        }
        else if (arg.Equals("--mount-external-multiuser-all")) {
            mMountExternal = IZygote::MOUNT_EXTERNAL_MULTIUSER_ALL;
        }
        else if (arg.Equals("--query-abi-list")) {
            mAbiListQuery = TRUE;
        }
        else if (arg.StartWith("--instruction-set=")) {
            mInstructionSet = arg.Substring(arg.IndexOf('=') + 1);
        }
        else if (arg.StartWith("--app-data-dir=")) {
            mAppDataDir = arg.Substring(arg.IndexOf('=') + 1);
        }
        else {
            break;
        }
    }

    // if (runtimeInit && classpath != null) {
    //     throw new IllegalArgumentException(
    //             "--runtime-init and -classpath are incompatible");
    // }

    mRemainingArgs = ArrayOf<String>::Alloc(args.GetLength() - curArg);

    for (Int32 i = 0; i < mRemainingArgs->GetLength(); ++i) {
        (*mRemainingArgs)[i] = args[curArg + i];
    }
    return NOERROR;
}

ZygoteConnection::ZygoteConnection(
    /* [in] */ ILocalSocket* socket,
    /* [in] */ const String& abiList)
    : mSocket(socket)
    , mAbiList(abiList)
{
    AutoPtr<IOutputStream> out;
    socket->GetOutputStream((IOutputStream**)&out);
    CDataOutputStream::New(out, (IDataOutputStream**)&mSocketOutStream);

    AutoPtr<IInputStream> in;
    socket->GetInputStream((IInputStream**)&in);
    AutoPtr<IInputStreamReader> reader;
    CInputStreamReader::New(in, (IInputStreamReader**)&reader);
    CBufferedReader::New(IReader::Probe(reader), 256, (IBufferedReader**)&mSocketReader);

    mSocket->SetSoTimeout(CONNECTION_TIMEOUT_MILLIS);

    // try {
    ECode ec = mSocket->GetPeerCredentials((ICredentials**)&mPeer);
    if (ec == (ECode)E_IO_EXCEPTION) {
        Logger::E(TAG, "Cannot read peer credentials");
    }
    // } catch (IOException ex) {
    //     Log.e(TAG, "Cannot read peer credentials", ex);
    //     throw ex;
    // }

    AutoPtr<IFileDescriptor> fd;
    mSocket->GetFileDescriptor((IFileDescriptor**)&fd);
    mPeerSecurityContext = SELinux::GetPeerContext(fd);
}

void ZygoteConnection::CheckTime(
    /* [in] */ Int64 startTime,
    /* [in] */ const String& where)
{
    Int64 now = SystemClock::GetElapsedRealtime();
    if ((now - startTime) > 1000) {
        // If we are taking more than a second, log about it.
        Logger::W(TAG, "Slow operation: %lldms so far, now at %s", (now - startTime), where.string());
    }
}

ECode ZygoteConnection::GetFileDescriptor(
    /* [out] */ IFileDescriptor** fd)
{
    return mSocket->GetFileDescriptor(fd);
}

Boolean ZygoteConnection::RunOnce(
    /* [out] */ IRunnable** task)
{
    *task = NULL;
    AutoPtr< ArrayOf<String> > args;
    AutoPtr<Arguments> parsedArgs;
    AutoPtr< ArrayOf<IFileDescriptor*> > descriptors;

    Int64 startTime = SystemClock::GetElapsedRealtime();

//    try {
    ECode ec = NOERROR;
    do {
        ec = ReadArgumentList((ArrayOf<String>**)&args);
        if (FAILED(ec))
            break;
        ec = mSocket->GetAncillaryFileDescriptors((ArrayOf<IFileDescriptor*>**)&descriptors);
    } while (0);

    if (ec == (ECode)E_IO_EXCEPTION) {
        Logger::W(TAG, "IOException on command socket ");
        CloseSocket();
        return TRUE;
    }

    if (args == NULL) {
        // EOF reached.
        CloseSocket();
        return TRUE;
    }

    /** the stderr of the most recent request, if avail */
    AutoPtr<IPrintStream> newStderr;

    if (descriptors != NULL && descriptors->GetLength() >= 3) {
        AutoPtr<IFileOutputStream> fops;
        CFileOutputStream::New((*descriptors)[2], (IFileOutputStream**)&fops);
        CPrintStream::New(IOutputStream::Probe(fops), (IPrintStream**)&newStderr);
    }

    Int32 pid = -1;
    AutoPtr<IFileDescriptor> childPipeFd;
    AutoPtr<IFileDescriptor> serverPipeFd;

    ec = NOERROR;
    do {
        parsedArgs = new Arguments(*args);
        if (parsedArgs->mAbiListQuery) {
            return HandleAbiListQuery();
        }

        if (parsedArgs->mPermittedCapabilities != 0 || parsedArgs->mEffectiveCapabilities != 0) {
            Logger::E(TAG, "Client may not specify capabilities: permitted=0x%x, effective=0x%x",
                parsedArgs->mPermittedCapabilities, parsedArgs->mEffectiveCapabilities);
            ec = E_ZYGOTE_SECURITY_EXCEPTION;
            break;
        }

        ec = ApplyUidSecurityPolicy(parsedArgs, mPeer, mPeerSecurityContext);
        if (FAILED(ec))
            break;
        ec = ApplyRlimitSecurityPolicy(parsedArgs, mPeer, mPeerSecurityContext);
        if (FAILED(ec))
            break;
        ec = ApplyInvokeWithSecurityPolicy(parsedArgs, mPeer, mPeerSecurityContext);
        if (FAILED(ec))
            break;
        ec = ApplyseInfoSecurityPolicy(parsedArgs, mPeer, mPeerSecurityContext);
        if (FAILED(ec))
            break;

        CheckTime(startTime, String("zygoteConnection.runOnce: apply security policies"));

        ApplyDebuggerSystemProperty(parsedArgs);
        ApplyInvokeWithSystemProperty(parsedArgs);

        CheckTime(startTime, String("zygoteConnection.runOnce: apply security policies"));

        AutoPtr<ArrayOf< Int32Array > > rlimits;
       if (parsedArgs->mRlimits != NULL) {
            Int32 size = parsedArgs->mRlimits->GetSize();
            rlimits = ArrayOf< Int32Array >::Alloc(size);
            List< Int32Array >::Iterator it;
            Int32 i = 0;
            for (it = parsedArgs->mRlimits->Begin(); it != parsedArgs->mRlimits->End(); ++it) {
                Int32Array array = *it;
                rlimits->Set(i++, array);
            }
       }

        if (parsedArgs->mRuntimeInit && parsedArgs->mInvokeWith != NULL) {
            AutoPtr<ArrayOf<IFileDescriptor*> > pipeFds;
            ec = Elastos::Droid::System::Os::Pipe((ArrayOf<IFileDescriptor*>**)&pipeFds);
            if (FAILED(ec))
                break;
            childPipeFd = (*pipeFds)[1];
            serverPipeFd = (*pipeFds)[0];
            ec = CZygoteInit::SetCloseOnExec(serverPipeFd, TRUE);
            if (FAILED(ec))
                break;
        }

        /**
         * In order to avoid leaking descriptors to the Zygote child,
         * the native code must close the two Zygote socket descriptors
         * in the child process before it switches from Zygote-root to
         * the UID and privileges of the application being launched.
         *
         * In order to avoid "bad file descriptor" errors when the
         * two LocalSocket objects are closed, the Posix file
         * descriptors are released via a dup2() call which closes
         * the socket and substitutes an open descriptor to /dev/null.
         */
        AutoPtr<ArrayOf<Int32> > fdsToClose = ArrayOf<Int32>::Alloc(2);
        (*fdsToClose)[0] = -1;
        (*fdsToClose)[1] = -1;

        AutoPtr<IFileDescriptor> fd;
        mSocket->GetFileDescriptor((IFileDescriptor**)&fd);

        if (fd != NULL) {
            Int32 ifd;
            fd->GetDescriptor(&ifd);
            (*fdsToClose)[0] = ifd;
        }

        fd = CZygoteInit::GetServerSocketFileDescriptor();
        if (fd != NULL) {
            Int32 ifd;
            fd->GetDescriptor(&ifd);
            (*fdsToClose)[1] = ifd;
        }

        fd = NULL;

        CheckTime(startTime, String("zygoteConnection.runOnce: preForkAndSpecialize"));

        pid = Zygote::ForkAndSpecialize(parsedArgs->mUid, parsedArgs->mGid, parsedArgs->mGids,
                parsedArgs->mDebugFlags, rlimits, parsedArgs->mMountExternal, parsedArgs->mSeInfo,
                parsedArgs->mNiceName, fdsToClose, parsedArgs->mInstructionSet, parsedArgs->mAppDataDir);
    } while (0);

    if (ec == (ECode)E_IO_EXCEPTION)
        LogAndPrintError(newStderr, String("Exception creating pipe"));
    else if (ec == (ECode)E_ERRNO_EXCEPTION)
        LogAndPrintError(newStderr, String("Exception creating pipe"));
    else if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION)
        LogAndPrintError(newStderr, String("Invalid zygote arguments"));
    else if (ec == (ECode)E_ZYGOTE_SECURITY_EXCEPTION)
        LogAndPrintError(newStderr, String("Zygote security policy prevents request: "));

    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);

    if (pid == 0) {
        // in child
        ioUtils->CloseQuietly(serverPipeFd);
        serverPipeFd = NULL;
        HandleChildProc(parsedArgs, descriptors, childPipeFd, newStderr, task);
        // should never get here, the child is expected to either
        // throw ZygoteInit.MethodAndArgsCaller or exec().
        ioUtils->CloseQuietly(childPipeFd);
        return TRUE;
    }
    else {
        // in parent...pid of < 0 means failure
        ioUtils->CloseQuietly(childPipeFd);
        childPipeFd = NULL;
        Boolean ret = HandleParentProc(pid, descriptors, serverPipeFd, parsedArgs);
        ioUtils->CloseQuietly(serverPipeFd);
        return ret;
    }
}

Boolean ZygoteConnection::HandleAbiListQuery()
{
    ECode ec = NOERROR;
    do {
        // final byte[] abiListBytes = abiList.getBytes(StandardCharsets.US_ASCII);
        AutoPtr<ArrayOf<Byte> > abiListBytes = mAbiList.GetBytes();
        ec = IDataOutput::Probe(mSocketOutStream)->WriteInt32(abiListBytes->GetLength());
        if (FAILED(ec))
            break;
        ec = IOutputStream::Probe(mSocketOutStream)->Write(abiListBytes);
    } while (0);

    if (ec == (ECode)E_IO_EXCEPTION) {
        Logger::E(TAG, "Error writing to command socket");
        return TRUE;
    }
    return FALSE;
}


void ZygoteConnection::CloseSocket()
{
    // try {
    ICloseable::Probe(mSocket)->Close();
    // } catch (IOException ex) {
    //     Log.e(TAG, "Exception while closing command "
    //             + "socket in parent", ex);
    // }
}

ECode ZygoteConnection::ReadArgumentList(
    /* [out] */ ArrayOf<String>** args)
{
    VALIDATE_NOT_NULL(args);
    *args = NULL;

    /**
     * See android.os.Process.zygoteSendArgsAndGetPid()
     * Presently the wire format to the zygote process is:
     * a) a count of arguments (argc, in essence)
     * b) a number of newline-separated argument strings equal to count
     *
     * After the zygote process reads these it will write the pid of
     * the child or -1 on failure.
     */

    Int32 argc;

//    try {
    String s;
    mSocketReader->ReadLine(&s);

    if (s.IsNull()) {
        // EOF reached.
        return NOERROR;
    }
    argc = StringUtils::ParseInt32(s);
//    } catch (NumberFormatException ex) {
//        Log.e(TAG, "invalid Zygote wire format: non-int at argc");
//        throw new IOException("invalid wire format");
//    }

    // See bug 1092107: large argc can be used for a DOS attack
    if (argc > MAX_ZYGOTE_ARGC) {
        // throw new IOException("max arg count exceeded");
        return E_IO_EXCEPTION;
    }

    AutoPtr< ArrayOf<String> > result = ArrayOf<String>::Alloc(argc);
    for (Int32 i = 0; i < argc; i++) {
        String arg;
        mSocketReader->ReadLine(&arg);
        if (arg.IsNull()) {
            // We got an unexpected EOF.
            // throw new IOException("truncated request");
            return E_IO_EXCEPTION;
        }
        (*result)[i] = arg;
    }

    *args = result;
    REFCOUNT_ADD(*args);
    return NOERROR;
}

ECode ZygoteConnection::ApplyUidSecurityPolicy(
    /* [in] */ Arguments* args,
    /* [in] */ ICredentials* peer,
    /* [in] */ const String& peerSecurityContext)
{
//    int peerUid = peer.getUid();
//
//    if (peerUid == 0) {
//        // Root can do what it wants
//    } else if (peerUid == Process.SYSTEM_UID ) {
//        // System UID is restricted, except in factory test mode
//        String factoryTest = SystemProperties.get("ro.factorytest");
//        boolean uidRestricted;
//
//        /* In normal operation, SYSTEM_UID can only specify a restricted
//         * set of UIDs. In factory test mode, SYSTEM_UID may specify any uid.
//         */
//        uidRestricted
//             = !(factoryTest.equals("1") || factoryTest.equals("2"));
//
//        if (uidRestricted
//                && args.uidSpecified && (args.uid < Process.SYSTEM_UID)) {
//            throw new ZygoteSecurityException(
//                    "System UID may not launch process with UID < "
//                            + Process.SYSTEM_UID);
//        }
//    } else {
//        // Everything else
//        if (args.uidSpecified || args.gidSpecified
//            || args.gids != null) {
//            throw new ZygoteSecurityException(
//                    "App UIDs may not specify uid's or gid's");
//        }
//    }
//
//    if (args.uidSpecified || args.gidSpecified || args.gids != null) {
//        boolean allowed = SELinux.checkSELinuxAccess(peerSecurityContext,
//                                                     peerSecurityContext,
//                                                     "zygote",
//                                                     "specifyids");
//        if (!allowed) {
//            throw new ZygoteSecurityException(
//                    "Peer may not specify uid's or gid's");
//        }
//    }
//
//    // If not otherwise specified, uid and gid are inherited from peer
//    if (!args.uidSpecified) {
//        args.uid = peer.getUid();
//        args.uidSpecified = true;
//    }
//    if (!args.gidSpecified) {
//        args.gid = peer.getGid();
//        args.gidSpecified = true;
//    }
    return NOERROR;
}

void ZygoteConnection::ApplyDebuggerSystemProperty(
    /* [in] */ Arguments* args)
{
    String value;
    SystemProperties::Get(String("ro.debuggable"), &value);
   if (value.Equals("1")) {
       args->mDebugFlags |= IZygote::DEBUG_ENABLE_DEBUGGER;
   }
}

ECode ZygoteConnection::ApplyRlimitSecurityPolicy(
    /* [in] */ Arguments* args,
    /* [in] */ ICredentials* peer,
    /* [in] */ const String& peerSecurityContext)
{
//    int peerUid = peer.getUid();
//
//    if (!(peerUid == 0 || peerUid == Process.SYSTEM_UID)) {
//        // All peers with UID other than root or SYSTEM_UID
//        if (args.rlimits != null) {
//            throw new ZygoteSecurityException(
//                    "This UID may not specify rlimits.");
//        }
//    }
//
//    if (args.rlimits != null) {
//        boolean allowed = SELinux.checkSELinuxAccess(peerSecurityContext,
//                                                     peerSecurityContext,
//                                                     "zygote",
//                                                     "specifyrlimits");
//        if (!allowed) {
//            throw new ZygoteSecurityException(
//                    "Peer may not specify rlimits");
//        }
//     }
    return NOERROR;
}

ECode ZygoteConnection::ApplyInvokeWithSecurityPolicy(
    /* [in] */ Arguments* args,
    /* [in] */ ICredentials* peer,
    /* [in] */ const String& peerSecurityContext)
{
//    int peerUid = peer.getUid();
//
//    if (args.invokeWith != null && peerUid != 0) {
//        throw new ZygoteSecurityException("Peer is not permitted to specify "
//                + "an explicit invoke-with wrapper command");
//    }
//
//    if (args.invokeWith != null) {
//        boolean allowed = SELinux.checkSELinuxAccess(peerSecurityContext,
//                                                     peerSecurityContext,
//                                                     "zygote",
//                                                     "specifyinvokewith");
//        if (!allowed) {
//            throw new ZygoteSecurityException("Peer is not permitted to specify "
//                + "an explicit invoke-with wrapper command");
//        }
//    }
    return NOERROR;
}

ECode ZygoteConnection::ApplyseInfoSecurityPolicy(
    /* [in] */ Arguments* args,
    /* [in] */ ICredentials* peer,
    /* [in] */ const String& peerSecurityContext)
{
//    int peerUid = peer.getUid();
//
//    if (args.seInfo == null) {
//        // nothing to check
//        return;
//    }
//
//    if (!(peerUid == 0 || peerUid == Process.SYSTEM_UID)) {
//        // All peers with UID other than root or SYSTEM_UID
//        throw new ZygoteSecurityException(
//                "This UID may not specify SELinux info.");
//    }
//
//    boolean allowed = SELinux.checkSELinuxAccess(peerSecurityContext,
//                                                 peerSecurityContext,
//                                                 "zygote",
//                                                 "specifyseinfo");
//    if (!allowed) {
//        throw new ZygoteSecurityException(
//                "Peer may not specify SELinux info");
//    }
//
//    return;
    return NOERROR;
}

void ZygoteConnection::ApplyInvokeWithSystemProperty(
    /* [in] */ Arguments* args)
{
    if (args->mInvokeWith == NULL && args->mNiceName != NULL) {
        if (args->mNiceName != NULL) {
            String property("wrap.");
            property += args->mNiceName;
            if (property.GetLength() > 31) {
                property = property.Substring(0, 31);
            }
            SystemProperties::Get(property, &args->mInvokeWith);
            if (args->mInvokeWith != NULL && args->mInvokeWith.GetLength() == 0) {
                args->mInvokeWith = NULL;
            }
        }
    }
}

ECode ZygoteConnection::HandleChildProc(
    /* [in] */ Arguments* parsedArgs,
    /* [in] */ ArrayOf<IFileDescriptor*>* descriptors,
    /* [in] */ IFileDescriptor* pipeFd,
    /* [in] */ IPrintStream* newStderr,
    /* [out] */ IRunnable** task)
{
    /**
     * By the time we get here, the native code has closed the two actual Zygote
     * socket connections, and substituted /dev/null in their place.  The LocalSocket
     * objects still need to be closed properly.
     */
    CloseSocket();
    CZygoteInit::CloseServerSocket();

    if (descriptors != NULL) {
        // try {
        CZygoteInit::ReopenStdio((*descriptors)[0],
                (*descriptors)[1], (*descriptors)[2]);

        AutoPtr<IIoUtils> ioUtils;
        CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
        for (Int32 i = 0; i < descriptors->GetLength(); ++i) {
            AutoPtr<IFileDescriptor> fd = (*descriptors)[i];
            ioUtils->CloseQuietly(fd);
        }
//        newStderr = System.err;
        // } catch (IOException ex) {
        //     Log.e(TAG, "Error reopening stdio", ex);
        // }
    }

    if (!parsedArgs->mNiceName.IsNull()) {
        Process::SetArgV0(parsedArgs->mNiceName);
    }

    if (parsedArgs->mRuntimeInit) {
        if (!parsedArgs->mInvokeWith.IsNull()) {
            assert(0);
//            WrapperInit.execApplication(parsedArgs.invokeWith,
//                    parsedArgs.niceName, parsedArgs.targetSdkVersion,
//                    pipeFd, parsedArgs.remainingArgs);
        }
        else {
            RuntimeInit::ZygoteInit(parsedArgs->mTargetSdkVersion,
                    parsedArgs->mRemainingArgs, task);
        }
    }
    else {
        assert(0);
//        String className;
//        // try {
//        className = parsedArgs.remainingArgs[0];
//        // } catch (ArrayIndexOutOfBoundsException ex) {
//        //     logAndPrintError(newStderr,
//        //             "Missing required class name argument", null);
//        //     return;
//        // }
//
//        String[] mainArgs = new String[parsedArgs.remainingArgs.length - 1];
//        System.arraycopy(parsedArgs.remainingArgs, 1,
//                mainArgs, 0, mainArgs.length);
//
//        if (parsedArgs.invokeWith != null) {
//            WrapperInit.execStandalone(parsedArgs.invokeWith,
//                    parsedArgs.classpath, className, mainArgs);
//        }
//        else {
//            ClassLoader cloader;
//            if (parsedArgs.classpath != null) {
//                cloader = new PathClassLoader(parsedArgs.classpath,
//                        ClassLoader.getSystemClassLoader());
//            }
//            else {
//                cloader = ClassLoader.getSystemClassLoader();
//            }
//
//            try {
//                ZygoteInit.invokeStaticMain(cloader, className, mainArgs);
//            } catch (RuntimeException ex) {
//                logAndPrintError(newStderr, "Error starting.", ex);
//            }
//        }
    }
    return NOERROR;
}

Boolean ZygoteConnection::HandleParentProc(
    /* [in] */ Int32 pid,
    /* [in] */ ArrayOf<IFileDescriptor*>* descriptors,
    /* [in] */ IFileDescriptor* pipeFd,
    /* [in] */ Arguments* parsedArgs)
{
    if (pid > 0) {
        SetChildPgid(pid);
    }

    if (descriptors != NULL) {
        AutoPtr<IIoUtils> ioUtils;
        CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
        for (Int32 i = 0; i < descriptors->GetLength(); ++i) {
            AutoPtr<IFileDescriptor> fd = (*descriptors)[i];
            ioUtils->CloseQuietly(fd);
        }
    }

    Boolean usingWrapper = FALSE;
    if (pipeFd != NULL && pid > 0) {
        AutoPtr<IFileInputStream> fis;
        CFileInputStream::New(pipeFd, (IFileInputStream**)&fis);
        AutoPtr<IDataInputStream> is;
        CDataInputStream::New((IInputStream*)fis.Get(), (IDataInputStream**)&is);
        Int32 innerPid = -1;
        // try {
        IDataInput::Probe(is)->ReadInt32(&innerPid);
        // } catch (IOException ex) {
        //     Log.w(TAG, "Error reading pid from wrapped process, child may have died", ex);
        // } finally {
        //     try {
        ICloseable::Probe(is)->Close();
        //     } catch (IOException ex) {
        //     }
        // }

        // Ensure that the pid reported by the wrapped process is either the
        // child process that we forked, or a descendant of it.
        if (innerPid > 0) {
            Int32 parentPid = innerPid;
            while (parentPid > 0 && parentPid != pid) {
                parentPid = Process::GetParentPid(parentPid);
            }
            if (parentPid > 0) {
                // Log.i(TAG, "Wrapped process has pid " + innerPid);
                pid = innerPid;
                usingWrapper = TRUE;
            }
            else {
                // Log.w(TAG, "Wrapped process reported a pid that is not a child of "
                //         + "the process that we forked: childPid=" + pid
                //         + " innerPid=" + innerPid);
            }
        }
    }

    // try {
    IDataOutput::Probe(mSocketOutStream)->WriteInt32(pid);
    IDataOutput::Probe(mSocketOutStream)->WriteBoolean(usingWrapper);
    // } catch (IOException ex) {
    //     Log.e(TAG, "Error reading from command socket", ex);
    //     return true;
    // }

    return FALSE;
}

void ZygoteConnection::SetChildPgid(
    /* [in] */ Int32 pid)
{
    // Try to move the new child into the peer's process group.
//    try {
    Int32 ppid;
    mPeer->GetPid(&ppid);
    Int32 pgid;
    CZygoteInit::Getpgid(ppid, &pgid);
    CZygoteInit::Setpgid(pid, pgid);
//    } catch (IOException ex) {
//        // This exception is expected in the case where
//        // the peer is not in our session
//        // TODO get rid of this log message in the case where
//        // getsid(0) != getsid(peer.getPid())
//        Log.i(TAG, "Zygote: setpgid failed. This is "
//            + "normal if peer is not in our session");
//    }
}

/**
 * Logs an error message and prints it to the specified stream, if
 * provided
 *
 * @param newStderr null-ok; a standard error stream
 * @param message non-null; error message
 * @param ex null-ok an exception
 */
void ZygoteConnection::LogAndPrintError (
    /* [in] */ IPrintStream* newStderr,
    /* [in] */ const String& message)
{
    Logger::E(TAG, message);
    if (newStderr != NULL) {
        newStderr->Println(message);
    }
}

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos
