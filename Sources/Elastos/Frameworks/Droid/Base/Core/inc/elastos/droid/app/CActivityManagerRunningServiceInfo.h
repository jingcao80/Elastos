
#ifndef __ELASTOS_DROID_APP_CACTIVITYMANAGERRUNNINGSERVICEINFO_H__
#define __ELASTOS_DROID_APP_CACTIVITYMANAGERRUNNINGSERVICEINFO_H__

#include "Elastos.Droid.Content.h"
#include "_Elastos_Droid_App_CActivityManagerRunningServiceInfo.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IComponentName;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * Information you can retrieve about a particular Service that is
 * currently running in the system.
 */
CarClass(CActivityManagerRunningServiceInfo)
    , public Object
    , public IActivityManagerRunningServiceInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CActivityManagerRunningServiceInfo();

    CARAPI constructor();

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /**
     * The service component.
     */
    CARAPI GetService(
        /* [out] */ IComponentName** service);

    CARAPI SetService(
        /* [in] */ IComponentName* service);

    /**
     * If non-zero, this is the process the service is running in.
     */
    CARAPI GetPid(
        /* [out] */ Int32* pid);

    CARAPI SetPid(
        /* [in] */ Int32 pid);

    /**
     * The UID that owns this service.
     */
    CARAPI GetUid(
        /* [out] */ Int32* uid);

    CARAPI SetUid(
        /* [in] */ Int32 uid);

    /**
     * The name of the process this service runs in.
     */
    CARAPI GetProcess(
        /* [out] */ String* process);

    CARAPI SetProcess(
        /* [in] */ const String& process);

    /**
     * Set to true if the service has asked to run as a foreground process.
     */
    CARAPI GetForeground(
        /* [out] */ Boolean* foreground);

    CARAPI SetForeground(
        /* [in] */ Boolean foreground);

    /**
     * The time when the service was first made active, either by someone
     * starting or binding to it.  This
     * is in units of {@link android.os.SystemClock#elapsedRealtime()}.
     */
    CARAPI GetActiveSince(
        /* [out] */ Int64* activeSince);

    CARAPI SetActiveSince(
        /* [in] */ Int64 activeSince);

    /**
     * Set to true if this service has been explicitly started.
     */
    CARAPI IsStarted(
        /* [out] */ Boolean* started);

    CARAPI SetStarted(
        /* [in] */ Boolean started);

    /**
     * Number of clients connected to the service.
     */
    CARAPI GetClientCount(
        /* [out] */ Int32* clientCount);

    CARAPI SetClientCount(
        /* [in] */ Int32 clientCount);

    /**
     * Number of times the service's process has crashed while the service
     * is running.
     */
    CARAPI GetCrashCount(
        /* [out] */ Int32* crashCount);

    CARAPI SetCrashCount(
        /* [in] */ Int32 crashCount);

    /**
     * The time when there was last activity in the service (either
     * explicit requests to start it or clients binding to it).  This
     * is in units of {@link android.os.SystemClock#uptimeMillis()}.
     */
    CARAPI GetLastActivityTime(
        /* [out] */ Int64* time);

    CARAPI SetLastActivityTime(
        /* [in] */ Int64 time);

    /**
     * If non-zero, this service is not currently running, but scheduled to
     * restart at the given time.
     */
    CARAPI GetRestarting(
        /* [out] */ Int64* restarting);

    CARAPI SetRestarting(
        /* [in] */ Int64 restarting);

    /**
     * Running flags.
     */
    CARAPI GetFlags(
        /* [out] */ Int32* flags);

    CARAPI SetFlags(
        /* [in] */ Int32 flags);

    /**
     * For special services that are bound to by system code, this is
     * the package that holds the binding.
     */
    CARAPI GetClientPackage(
        /* [out] */ String* clientPackage);

    CARAPI SetClientPackage(
        /* [in] */ const String& clientPackage);

    /**
     * For special services that are bound to by system code, this is
     * a string resource providing a user-visible label for who the
     * client is.
     */
    CARAPI GetClientLabel(
        /* [out] */ Int32* clientLabel);

    CARAPI SetClientLabel(
        /* [in] */ Int32 clientLabel);

private:
    AutoPtr<IComponentName> mService;

    Int32 mPid;

    Int32 mUid;

    String mProcess;

    Boolean mForeground;

    Int64 mActiveSince;

    Boolean mStarted;

    Int32 mClientCount;

    Int32 mCrashCount;

    Int64 mLastActivityTime;

    Int64 mRestarting;
    Int32 mFlags;

    String mClientPackage;

    Int32 mClientLabel;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CACTIVITYMANAGERRUNNINGSERVICEINFO_H__
