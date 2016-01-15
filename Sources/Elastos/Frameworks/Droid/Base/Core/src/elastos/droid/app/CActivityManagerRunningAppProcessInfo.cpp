
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/CActivityManagerRunningAppProcessInfo.h"
#include "elastos/droid/content/CComponentName.h"

using Elastos::Droid::Content::CComponentName;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL_2(CActivityManagerRunningAppProcessInfo, Object, IActivityManagerRunningAppProcessInfo, IParcelable)

CAR_OBJECT_IMPL(CActivityManagerRunningAppProcessInfo)

CActivityManagerRunningAppProcessInfo::CActivityManagerRunningAppProcessInfo()
    : mPid(0)
    , mUid(0)
    , mFlags(0)
    , mLastTrimLevel(0)
    , mImportance(0)
    , mLru(0)
    , mImportanceReasonCode(0)
    , mImportanceReasonPid(0)
    , mProcessState(IActivityManager::PROCESS_STATE_IMPORTANT_FOREGROUND)
{
}

ECode CActivityManagerRunningAppProcessInfo::constructor()
{
    mImportance = IActivityManagerRunningAppProcessInfo::IMPORTANCE_FOREGROUND;
    mImportanceReasonCode = IActivityManagerRunningAppProcessInfo::REASON_UNKNOWN;
    return NOERROR;
}

ECode CActivityManagerRunningAppProcessInfo::constructor(
    /* [in] */ const String& pProcessName,
    /* [in] */ Int32 pPid,
    /* [in] */ ArrayOf<String>* pArr)
{
    mProcessName = pProcessName;
    mPid = pPid;
    mPkgList = pArr;
    return NOERROR;
}

Int32 CActivityManagerRunningAppProcessInfo::ProcStateToImportance(
    /* [in] */ Int32 procState)
{
    if (procState >= IActivityManager::PROCESS_STATE_HOME) {
        return IActivityManagerRunningAppProcessInfo::IMPORTANCE_BACKGROUND;
    } else if (procState >= IActivityManager::PROCESS_STATE_SERVICE) {
        return IActivityManagerRunningAppProcessInfo::IMPORTANCE_SERVICE;
    } else if (procState > IActivityManager::PROCESS_STATE_HEAVY_WEIGHT) {
        return IActivityManagerRunningAppProcessInfo::IMPORTANCE_CANT_SAVE_STATE;
    } else if (procState >= IActivityManager::PROCESS_STATE_IMPORTANT_BACKGROUND) {
        return IActivityManagerRunningAppProcessInfo::IMPORTANCE_PERCEPTIBLE;
    } else if (procState >= IActivityManager::PROCESS_STATE_IMPORTANT_FOREGROUND) {
        return IActivityManagerRunningAppProcessInfo::IMPORTANCE_VISIBLE;
    } else {
        return IActivityManagerRunningAppProcessInfo::IMPORTANCE_FOREGROUND;
    }
}

ECode CActivityManagerRunningAppProcessInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    FAIL_RETURN(dest->WriteString(mProcessName));
    FAIL_RETURN(dest->WriteInt32(mPid));
    FAIL_RETURN(dest->WriteInt32(mUid));
    FAIL_RETURN(dest->WriteArrayOfString(mPkgList));
    FAIL_RETURN(dest->WriteInt32(mFlags));
    FAIL_RETURN(dest->WriteInt32(mLastTrimLevel));
    FAIL_RETURN(dest->WriteInt32(mImportance));
    FAIL_RETURN(dest->WriteInt32(mLru));
    FAIL_RETURN(dest->WriteInt32(mImportanceReasonCode));
    FAIL_RETURN(dest->WriteInt32(mImportanceReasonPid));
    FAIL_RETURN(CComponentName::WriteToParcel(mImportanceReasonComponent, dest));
    FAIL_RETURN(dest->WriteInt32(mImportanceReasonImportance));
    FAIL_RETURN(dest->WriteInt32(mProcessState));

    return NOERROR;
}

ECode CActivityManagerRunningAppProcessInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);

    FAIL_RETURN(source->ReadString(&mProcessName));
    FAIL_RETURN(source->ReadInt32(&mPid));
    FAIL_RETURN(source->ReadInt32(&mUid));
    FAIL_RETURN(source->ReadArrayOfString((ArrayOf<String>**)&mPkgList));
    FAIL_RETURN(source->ReadInt32(&mFlags));
    FAIL_RETURN(source->ReadInt32(&mLastTrimLevel));
    FAIL_RETURN(source->ReadInt32(&mImportance));
    FAIL_RETURN(source->ReadInt32(&mLru));
    FAIL_RETURN(source->ReadInt32(&mImportanceReasonCode));
    FAIL_RETURN(source->ReadInt32(&mImportanceReasonPid));
    FAIL_RETURN(CComponentName::ReadFromParcel(source, (IComponentName**)&mImportanceReasonComponent));
    FAIL_RETURN(source->ReadInt32(&mImportanceReasonImportance));
    FAIL_RETURN(source->ReadInt32(&mProcessState));

    return NOERROR;
}

/**
 * The name of the process that this object is associated with
 */
ECode CActivityManagerRunningAppProcessInfo::GetProcessName(
    /* [out] */ String* processName)
{
    VALIDATE_NOT_NULL(processName);
    *processName = mProcessName;
    return NOERROR;
}

ECode CActivityManagerRunningAppProcessInfo::SetProcessName(
    /* [in] */ const String& processName)
{
    mProcessName = processName;
    return NOERROR;
}

/**
 * The pid of this process; 0 if none
 */
ECode CActivityManagerRunningAppProcessInfo::GetPid(
    /* [out] */ Int32* pid)
{
    VALIDATE_NOT_NULL(pid);
    *pid = mPid;
    return NOERROR;
}

ECode CActivityManagerRunningAppProcessInfo::SetPid(
    /* [in] */ Int32 pid)
{
    mPid = pid;
    return NOERROR;
}

/**
 * The user id of this process.
 */
ECode CActivityManagerRunningAppProcessInfo::GetUid(
    /* [out] */ Int32* uid)
{
    VALIDATE_NOT_NULL(uid);
    *uid = mUid;
    return NOERROR;
}

ECode CActivityManagerRunningAppProcessInfo::SetUid(
    /* [in] */ Int32 uid)
{
    mUid = uid;
    return NOERROR;
}

/**
 * All packages that have been loaded into the process.
 */
ECode CActivityManagerRunningAppProcessInfo::GetPkgList(
    /* [out, callee] */ ArrayOf<String>** pkgList)
{
    VALIDATE_NOT_NULL(pkgList);
    *pkgList = mPkgList;
    REFCOUNT_ADD(*pkgList);
    return NOERROR;
}

ECode CActivityManagerRunningAppProcessInfo::SetPkgList(
    /* [in] */ ArrayOf<String>* pkgList)
{
    mPkgList = pkgList;
    return NOERROR;
}

/**
 * Flags of information.  May be any of
 * {@link #FLAG_CANT_SAVE_STATE}.
 * @hide
 */
ECode CActivityManagerRunningAppProcessInfo::GetFlags(
    /* [out] */ Int32* flags)
{
    VALIDATE_NOT_NULL(flags);
    *flags = mFlags;
    return NOERROR;
}

ECode CActivityManagerRunningAppProcessInfo::SetFlags(
    /* [in] */ Int32 flags)
{
    mFlags = flags;
    return NOERROR;
}

/**
 * Last memory trim level reported to the process: corresponds to
 * the values supplied to {@link android.content.ComponentCallbacks2#onTrimMemory(int)
 * ComponentCallbacks2.onTrimMemory(int)}.
 */
ECode CActivityManagerRunningAppProcessInfo::GetLastTrimLevel(
    /* [out] */ Int32* level)
{
    VALIDATE_NOT_NULL(level);
    *level = mLastTrimLevel;
    return NOERROR;
}

ECode CActivityManagerRunningAppProcessInfo::SetLastTrimLevel(
    /* [in] */ Int32 level)
{
    mLastTrimLevel = level;
    return NOERROR;
}

/**
 * The relative importance level that the system places on this
 * process.  May be one of {@link #IMPORTANCE_FOREGROUND},
 * {@link #IMPORTANCE_VISIBLE}, {@link #IMPORTANCE_SERVICE},
 * {@link #IMPORTANCE_BACKGROUND}, or {@link #IMPORTANCE_EMPTY}.  These
 * constants are numbered so that "more important" values are always
 * smaller than "less important" values.
 */
ECode CActivityManagerRunningAppProcessInfo::GetImportance(
    /* [out] */ Int32* importance)
{
    VALIDATE_NOT_NULL(importance);
    *importance = mImportance;
    return NOERROR;
}

ECode CActivityManagerRunningAppProcessInfo::SetImportance(
    /* [in] */ Int32 importance)
{
    mImportance = importance;
    return NOERROR;
}

/**
 * An additional ordering within a particular {@link #importance}
 * category, providing finer-grained information about the relative
 * utility of processes within a category.  This number means nothing
 * except that a smaller values are more recently used (and thus
 * more important).  Currently an LRU value is only maintained for
 * the {@link #IMPORTANCE_BACKGROUND} category, though others may
 * be maintained in the future.
 */
ECode CActivityManagerRunningAppProcessInfo::GetLru(
    /* [out] */ Int32* lru)
{
    VALIDATE_NOT_NULL(lru);
    *lru = mLru;
    return NOERROR;
}

ECode CActivityManagerRunningAppProcessInfo::SetLru(
    /* [in] */ Int32 lru)
{
    mLru = lru;
    return NOERROR;
}

/**
 * The reason for {@link #importance}, if any.
 */
ECode CActivityManagerRunningAppProcessInfo::GetImportanceReasonCode(
    /* [out] */ Int32* code)
{
    VALIDATE_NOT_NULL(code);
    *code = mImportanceReasonCode;
    return NOERROR;
}

ECode CActivityManagerRunningAppProcessInfo::SetImportanceReasonCode(
    /* [in] */ Int32 code)
{
    mImportanceReasonCode = code;
    return NOERROR;
}

/**
 * For the specified values of {@link #importanceReasonCode}, this
 * is the process ID of the other process that is a client of this
 * process.  This will be 0 if no other process is using this one.
 */
ECode CActivityManagerRunningAppProcessInfo::GetImportanceReasonPid(
    /* [out] */ Int32* pid)
{
    VALIDATE_NOT_NULL(pid);
    *pid = mImportanceReasonPid;
    return NOERROR;
}

ECode CActivityManagerRunningAppProcessInfo::SetImportanceReasonPid(
    /* [in] */ Int32 pid)
{
    mImportanceReasonPid = pid;
    return NOERROR;
}

/**
 * For the specified values of {@link #importanceReasonCode}, this
 * is the name of the component that is being used in this process.
 */
ECode CActivityManagerRunningAppProcessInfo::GetImportanceReasonComponent(
    /* [out] */ IComponentName** component)
{
    VALIDATE_NOT_NULL(component);
    *component = mImportanceReasonComponent;
    REFCOUNT_ADD(*component);
    return NOERROR;
}

ECode CActivityManagerRunningAppProcessInfo::SetImportanceReasonComponent(
    /* [in] */ IComponentName* component)
{
    mImportanceReasonComponent = component;
    return NOERROR;
}

/**
 * When {@link importanceReasonPid} is non-0, this is the importance
 * of the other pid. @hide
 */
ECode CActivityManagerRunningAppProcessInfo::GetImportanceReasonImportance(
    /* [out] */ Int32* importance)
{
    VALIDATE_NOT_NULL(importance);
    *importance = mImportanceReasonImportance;
    return NOERROR;
}

ECode CActivityManagerRunningAppProcessInfo::SetImportanceReasonImportance(
    /* [in] */ Int32 importance)
{
    mImportanceReasonImportance = importance;
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
