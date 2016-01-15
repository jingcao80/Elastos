
#ifndef __ELASTOS_DROID_APP_CACTIVITYMANAGERPROCESSERRORSTATEINFO_H__
#define __ELASTOS_DROID_APP_CACTIVITYMANAGERPROCESSERRORSTATEINFO_H__

#include "_Elastos_Droid_App_CActivityManagerProcessErrorStateInfo.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace App {

/**
 * Information you can retrieve about any processes that are in an error condition.
 */
CarClass(CActivityManagerProcessErrorStateInfo)
    , public Object
    , public IActivityManagerProcessErrorStateInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CActivityManagerProcessErrorStateInfo();

    CARAPI constructor();

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /**
     * The condition that the process is in.
     */
    CARAPI GetCondition(
        /* [out] */ Int32* condition);

    CARAPI SetCondition(
        /* [in] */ Int32 condition);

    /**
     * The process name in which the crash or error occurred.
     */
    CARAPI GetProcessName(
        /* [out] */ String* processName);

    CARAPI SetProcessName(
        /* [in] */ const String& processName);

    /**
     * The pid of this process; 0 if none
     */
    CARAPI GetPid(
        /* [out] */ Int32* pid);

    CARAPI SetPid(
        /* [in] */ Int32 pid);

    /**
     * The kernel user-ID that has been assigned to this process;
     * currently this is not a unique ID (multiple applications can have
     * the same uid).
     */
    CARAPI GetUid(
        /* [out] */ Int32* uid);

    CARAPI SetUid(
        /* [in] */ Int32 uid);

    /**
     * The activity name associated with the error, if known.  May be null.
     */
    CARAPI GetTag(
        /* [out] */ String* tag);

    CARAPI SetTag(
        /* [in] */ const String& tag);

    /**
     * A short message describing the error condition.
     */
    CARAPI GetShortMsg(
        /* [out] */ String* shortMsg);

    CARAPI SetShortMsg(
        /* [in] */ const String& shortMsg);

    /**
     * A long message describing the error condition.
     */
    CARAPI GetLongMsg(
        /* [out] */ String* longMsg);

    CARAPI SetLongMsg(
        /* [in] */ const String& longMsg);

    /**
     * The stack trace where the error originated.  May be null.
     */
    CARAPI GetStackTrace(
        /* [out] */ String* stackTrace);

    CARAPI SetStackTrace(
        /* [in] */ const String& stackTrace);

    /**
     * to be deprecated: This value will always be null.
     */
    CARAPI GetCrashData(
        /* [out, callee] */ ArrayOf<Byte>** crashData);

    CARAPI SetCrashData(
        /* [in] */ ArrayOf<Byte>* crashData);

private:
    Int32 mCondition;

    String mProcessName;

    Int32 mPid;

    Int32 mUid;

    String mTag;

    String mShortMsg;

    String mLongMsg;

    String mStackTrace;

    AutoPtr< ArrayOf<Byte> > mCrashData;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CACTIVITYMANAGERPROCESSERRORSTATEINFO_H__
