
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/CActivityManagerProcessErrorStateInfo.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL_2(CActivityManagerProcessErrorStateInfo, Object, IActivityManagerProcessErrorStateInfo, IParcelable)

CAR_OBJECT_IMPL(CActivityManagerProcessErrorStateInfo)

CActivityManagerProcessErrorStateInfo::CActivityManagerProcessErrorStateInfo()
    : mCondition(0)
    , mPid(0)
    , mUid(0)
{
}

ECode CActivityManagerProcessErrorStateInfo::constructor()
{
    return NOERROR;
}

ECode CActivityManagerProcessErrorStateInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    FAIL_RETURN(dest->WriteInt32(mCondition));
    FAIL_RETURN(dest->WriteString(mProcessName));
    FAIL_RETURN(dest->WriteInt32(mPid));
    FAIL_RETURN(dest->WriteInt32(mUid));
    FAIL_RETURN(dest->WriteString(mTag));
    FAIL_RETURN(dest->WriteString(mShortMsg));
    FAIL_RETURN(dest->WriteString(mLongMsg));
    FAIL_RETURN(dest->WriteString(mStackTrace));

    return NOERROR;
}

ECode CActivityManagerProcessErrorStateInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);

    FAIL_RETURN(source->ReadInt32(&mCondition));
    FAIL_RETURN(source->ReadString(&mProcessName));
    FAIL_RETURN(source->ReadInt32(&mPid));
    FAIL_RETURN(source->ReadInt32(&mUid));
    FAIL_RETURN(source->ReadString(&mTag));
    FAIL_RETURN(source->ReadString(&mShortMsg));
    FAIL_RETURN(source->ReadString(&mLongMsg));
    FAIL_RETURN(source->ReadString(&mStackTrace));

    return NOERROR;
}

/**
 * The condition that the process is in.
 */
ECode CActivityManagerProcessErrorStateInfo::GetCondition(
    /* [out] */ Int32* condition)
{
    VALIDATE_NOT_NULL(condition);
    *condition = mCondition;
    return NOERROR;
}

ECode CActivityManagerProcessErrorStateInfo::SetCondition(
    /* [in] */ Int32 condition)
{
    mCondition = condition;
    return NOERROR;
}

/**
 * The process name in which the crash or error occurred.
 */
ECode CActivityManagerProcessErrorStateInfo::GetProcessName(
    /* [out] */ String* processName)
{
    VALIDATE_NOT_NULL(processName);
    *processName = mProcessName;
    return NOERROR;
}

ECode CActivityManagerProcessErrorStateInfo::SetProcessName(
    /* [in] */ const String& processName)
{
    mProcessName = processName;
    return NOERROR;
}

/**
 * The pid of this process; 0 if none
 */
ECode CActivityManagerProcessErrorStateInfo::GetPid(
    /* [out] */ Int32* pid)
{
    VALIDATE_NOT_NULL(pid);
    *pid = mPid;
    return NOERROR;
}

ECode CActivityManagerProcessErrorStateInfo::SetPid(
    /* [in] */ Int32 pid)
{
    mPid = pid;
    return NOERROR;
}

/**
 * The kernel user-ID that has been assigned to this process;
 * currently this is not a unique ID (multiple applications can have
 * the same uid).
 */
ECode CActivityManagerProcessErrorStateInfo::GetUid(
    /* [out] */ Int32* uid)
{
    VALIDATE_NOT_NULL(uid);
    *uid = mUid;
    return NOERROR;
}

ECode CActivityManagerProcessErrorStateInfo::SetUid(
    /* [in] */ Int32 uid)
{
    mUid = uid;
    return NOERROR;
}

/**
 * The activity name associated with the error, if known.  May be null.
 */
ECode CActivityManagerProcessErrorStateInfo::GetTag(
    /* [out] */ String* tag)
{
    VALIDATE_NOT_NULL(tag);
    *tag = mTag;
    return NOERROR;
}

ECode CActivityManagerProcessErrorStateInfo::SetTag(
    /* [in] */ const String& tag)
{
    mTag = tag;
    return NOERROR;
}

/**
 * A short message describing the error condition.
 */
ECode CActivityManagerProcessErrorStateInfo::GetShortMsg(
    /* [out] */ String* shortMsg)
{
    VALIDATE_NOT_NULL(shortMsg);
    *shortMsg = mShortMsg;
    return NOERROR;
}

ECode CActivityManagerProcessErrorStateInfo::SetShortMsg(
    /* [in] */ const String& shortMsg)
{
    mShortMsg = shortMsg;
    return NOERROR;
}

/**
 * A long message describing the error condition.
 */
ECode CActivityManagerProcessErrorStateInfo::GetLongMsg(
    /* [out] */ String* longMsg)
{
    VALIDATE_NOT_NULL(longMsg);
    *longMsg = mLongMsg;
    return NOERROR;
}

ECode CActivityManagerProcessErrorStateInfo::SetLongMsg(
    /* [in] */ const String& longMsg)
{
    mLongMsg = longMsg;
    return NOERROR;
}

/**
 * The stack trace where the error originated.  May be null.
 */
ECode CActivityManagerProcessErrorStateInfo::GetStackTrace(
    /* [out] */ String* stackTrace)
{
    VALIDATE_NOT_NULL(stackTrace);
    *stackTrace = mStackTrace;
    return NOERROR;
}

ECode CActivityManagerProcessErrorStateInfo::SetStackTrace(
    /* [in] */ const String& stackTrace)
{
    mStackTrace = stackTrace;
    return NOERROR;
}

/**
 * to be deprecated: This value will always be null.
 */
ECode CActivityManagerProcessErrorStateInfo::GetCrashData(
    /* [out, callee] */ ArrayOf<Byte>** crashData)
{
    VALIDATE_NOT_NULL(crashData);
    *crashData = mCrashData;
    REFCOUNT_ADD(*crashData);
    return NOERROR;
}

ECode CActivityManagerProcessErrorStateInfo::SetCrashData(
    /* [in] */ ArrayOf<Byte>* crashData)
{
    mCrashData = crashData;
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
