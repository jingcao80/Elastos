
#include "CActivityController.h"
#include "elastos/droid/os/Build.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::IStrictMode;
using Elastos::Droid::Os::CStrictMode;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::CFileDescriptor;
using Elastos::IO::CFileDescriptorHelper;
using Elastos::IO::CPrintStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IFileDescriptorHelper;
using Elastos::IO::IPrintStream;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

static const Int32 DEBUG_ALLOW_ANY_STARTS = 0;
static const Int32 DEBUG_ALLOW_ANY_RESTARTS = 0;

ECode CActivityController::ActivityStarting(
    /* [in] */ IIntent *intent,
    /* [in] */ const String& pkg,
    /* [out] */ Boolean *res)
{
    VALIDATE_NOT_NULL(res)
    Boolean allow = mHost->CheckEnteringPackage(pkg) || (DEBUG_ALLOW_ANY_STARTS != 0);
    if (mHost->GetVerbose() > 0) {
        // StrictMode's disk checks end up catching this on
        // userdebug/eng builds due to PrintStream going to a
        // FileOutputStream in the end (perhaps only when
        // redirected to a file?)  So we allow disk writes
        // around this region for the monkey to minimize
        // harmless dropbox uploads from monkeys.
        Handle32 sp;
        AutoPtr<IStrictMode> sm;
        CStrictMode::AcquireSingleton((IStrictMode**)&sm);
        sm->AllowThreadDiskWrites(&sp);
        String IntentInString;
        intent->ToString(&IntentInString);
        AutoPtr<IFileOutputStream> fos;
        AutoPtr<IPrintStream> ps;
        AutoPtr<IFileDescriptor> OUT;
        AutoPtr<IFileDescriptorHelper> helper;
        CFileDescriptorHelper::AcquireSingleton((IFileDescriptorHelper**)&helper);
        helper->GetOutFile((IFileDescriptor**)&OUT);
        CFileOutputStream::New(OUT, (IFileOutputStream**)&fos);
        CPrintStream::New(fos, (IPrintStream**)&ps);
        String str("    // ");
        if (allow) {
            str += "Allowing";
        }
        else {
            str += "Rejecting";
        }
        str += String(" start of ") + IntentInString;
        str += String(" in package ") + pkg;
        ps->PrintStringln(str);
        sm->SetThreadPolicy(sp);
    }
    mHost->currentPackage = pkg;
    mHost->currentIntent = intent;
    *res = allow;
    return NOERROR;
}

ECode CActivityController::ActivityResuming(
    /* [in] */ const String& pkg,
    /* [out] */ Boolean * res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IStrictMode> sm;
    Handle32 sp;
    CStrictMode::AcquireSingleton((IStrictMode**)&sm);
    sm->AllowThreadDiskWrites(&sp);
    AutoPtr<IFileOutputStream> fos;
    AutoPtr<IPrintStream> ps;
    AutoPtr<IFileDescriptor> OUT;
    AutoPtr<IFileDescriptorHelper> helper;
    CFileDescriptorHelper::AcquireSingleton((IFileDescriptorHelper**)&helper);
    helper->GetOutFile((IFileDescriptor**)&OUT);
    CFileOutputStream::New(OUT, (IFileOutputStream**)&fos);
    CPrintStream::New(fos, (IPrintStream**)&ps);
    String str("    // activityResuming(");
    str += pkg + ")";
    ps->PrintStringln(str);
    Boolean allow = mHost->CheckEnteringPackage(pkg) || (DEBUG_ALLOW_ANY_RESTARTS != 0);
    if (!allow) {
        if (mHost->GetVerbose() > 0) {
            str = "    // ";
            if (allow) {
                str += "Allowing";
            }
            else {
                str += "Rejecting";
            }
            str += " resume of package ";
            str += pkg;
            ps->PrintStringln(str);
        }
    }
    mHost->currentPackage = pkg;
    sm->SetThreadPolicy(sp);
    *res = allow;
    return NOERROR;
}

ECode CActivityController::AppCrashed(
    /* [in] */ const String& processName,
    /* [in] */ Int32 pid,
    /* [in] */ const String& shortMsg,
    /* [in] */ const String& longMsg,
    /* [in] */ Int64 timeMillis,
    /* [in] */ const String& stackTrace,
    /* [out] */ Boolean * res)
{
    VALIDATE_NOT_NULL(res)
    AutoPtr<IStrictMode> sm;
    Handle32 sp;
    CStrictMode::AcquireSingleton((IStrictMode**)&sm);
    sm->AllowThreadDiskWrites(&sp);
    AutoPtr<IFileOutputStream> fos;
    AutoPtr<IPrintStream> ps;
    AutoPtr<IFileDescriptor> ERR;
    AutoPtr<IFileDescriptorHelper> helper;
    CFileDescriptorHelper::AcquireSingleton((IFileDescriptorHelper**)&helper);
    helper->GetErrFile((IFileDescriptor**)&ERR);
    CFileOutputStream::New(ERR, (IFileOutputStream**)&fos);
    CPrintStream::New(fos, (IPrintStream**)&ps);
    String str("// CRASH: ");
    str += processName;
    str.AppendFormat(" (pid %d)", pid);
    ps->PrintStringln(str);
    str = String("// Short Msg: ") + shortMsg;
    ps->PrintStringln(str);
    str = String("// Long Msg: ") + longMsg;
    ps->PrintStringln(str);
    str = String("// Build Label: ") + Build::FINGERPRINT;
    ps->PrintStringln(str);
    str = String("// Build Changelist: ") + Build::VERSION::INCREMENTAL;
    ps->PrintStringln(str);
    str = "// Build Time: ";
    str.AppendFormat("%lld", Build::TIME);
    ps->PrintStringln(str);
    StringBuilder sb(stackTrace.GetByteLength() * 4);
    Int32 searchStart = 0;
    Int32 matchStart = 0;
    while((matchStart = stackTrace.IndexOf("\n", searchStart)) != -1) {
        sb.Append(stackTrace.Substring(searchStart, matchStart));
        sb.Append("\n// ");
        searchStart = matchStart + String("\n// ").GetLength();
    }
    sb.Append(stackTrace.Substring(searchStart));
    sb.ToString(&str);
    str = String("// ") + str;
    ps->PrintStringln(str);
    sm->SetThreadPolicy(sp);
    if (!mHost->mIgnoreCrashes || mHost->mRequestBugreport) {
        {
            AutoLock lock(mHost->mLockLock);
            if (!mHost->mIgnoreCrashes) {
                mHost->mAbort = TRUE;
            }
            if (mHost->mRequestBugreport){
                mHost->mRequestAppCrashBugreport = TRUE;
                mHost->mReportProcessName = processName;
            }
        }
        *res = !mHost->mKillProcessAfterError;
        return NOERROR;
    }
    *res = FALSE;
    return NOERROR;
}

ECode CActivityController::AppEarlyNotResponding(
    /* [in] */ const String& processName,
    /* [in] */ Int32 pid,
    /* [in] */ const String& annotation,
    /* [out] */ Int32 * res)
{
    VALIDATE_NOT_NULL(res);
    *res = 0;
    return NOERROR;
}

ECode CActivityController::AppNotResponding(
    /* [in] */ const String& processName,
    /* [in] */ Int32 pid,
    /* [in] */ const String& processStats,
    /* [out] */ Int32 * res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IStrictMode> sm;
    Handle32 sp;
    CStrictMode::AcquireSingleton((IStrictMode**)&sm);
    sm->AllowThreadDiskWrites(&sp);
    AutoPtr<IFileOutputStream> fos;
    AutoPtr<IPrintStream> ps;
    AutoPtr<IFileDescriptor> err;
    AutoPtr<IFileDescriptorHelper> helper;
    CFileDescriptorHelper::AcquireSingleton((IFileDescriptorHelper**)&helper);
    helper->GetErrFile((IFileDescriptor**)&err);
    CFileOutputStream::New(err, (IFileOutputStream**)&fos);
    CPrintStream::New(fos, (IPrintStream**)&ps);
    String str;
    str.AppendFormat("// NOT RESPONDING: %s (pid %d)", processName.string(), pid);
    ps->PrintStringln(str);
    ps->PrintStringln(processStats);
    sm->SetThreadPolicy(sp);
    {
        AutoLock lock(mHost->mLockLock);
        mHost->mRequestAnrTraces = TRUE;
        mHost->mRequestDumpsysMemInfo = TRUE;
        mHost->mRequestProcRank = TRUE;
        if (mHost->mRequestBugreport){
          mHost->mRequestAnrBugreport = TRUE;
          mHost->mReportProcessName = processName;
        }
    }
    if (!mHost->mIgnoreTimeouts) {
        AutoLock lock(mHost->mLockLock);
        mHost->mAbort = TRUE;
    }
    *res = (mHost->mKillProcessAfterError) ? -1 : 1;
    return NOERROR;
}

ECode CActivityController::constructor(
    /* [in] */ Handle32 monkey)
{
    mHost = (Monkey*)monkey;
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

