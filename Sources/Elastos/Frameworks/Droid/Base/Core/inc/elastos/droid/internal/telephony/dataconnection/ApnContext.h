
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_APNCONTEXT_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_APNCONTEXT_H__

#include "_Elastos.Droid.Internal.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::IDctConstants;
using Elastos::Droid::Net::INetworkConfig;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

/**
 * Maintain the Apn context
 */
class ApnContext
    : public Object
    , public IApnContext
{
public:
    CAR_INTERFACE_DECL()

    ApnContext();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ const String& apnType,
        /* [in] */ const String& logTag,
        /* [in] */ INetworkConfig* config,
        /* [in] */ IDcTrackerBase* tracker);

    CARAPI GetApnType(
        /* [out] */ String* result);

    CARAPI GetDcAc(
        /* [out] */ IDcAsyncChannel** result);

    CARAPI SetDataConnectionAc(
        /* [in] */ IDcAsyncChannel* dcac);

    CARAPI GetReconnectIntent(
        /* [out] */ IPendingIntent** result);

    CARAPI SetReconnectIntent(
        /* [in] */ IPendingIntent* intent);

    CARAPI GetApnSetting(
        /* [out] */ IApnSetting** result);

    CARAPI SetApnSetting(
        /* [in] */ IApnSetting* apnSetting);

    CARAPI SetWaitingApns(
        /* [in] */ IArrayList* waitingApns);

    CARAPI GetWaitingApnsPermFailCount(
        /* [out] */ Int32* result);

    CARAPI DecWaitingApnsPermFailCount();

    CARAPI GetNextWaitingApn(
        /* [out] */ IApnSetting** result);

    CARAPI RemoveWaitingApn(
        /* [in] */ IApnSetting* apn);

    CARAPI GetWaitingApns(
        /* [out] */ IArrayList** result);

    CARAPI SetState(
        /* [in] */ DctConstantsState s);

    CARAPI GetState(
        /* [out] */ DctConstantsState* result);

    CARAPI IsDisconnected(
        /* [out] */ Boolean* result);

    CARAPI SetReason(
        /* [in] */ const String& reason);

    CARAPI GetReason(
        /* [out] */ String* result);

    CARAPI IsReady(
        /* [out] */ Boolean* result);

    CARAPI IsConnectable(
        /* [out] */ Boolean* result);

    CARAPI IsConnectedOrConnecting(
        /* [out] */ Boolean* result);

    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

    CARAPI IsEnabled(
        /* [out] */ Boolean* result);

    CARAPI SetDependencyMet(
        /* [in] */ Boolean met);

    CARAPI GetDependencyMet(
        /* [out] */ Boolean* result);

    CARAPI IsProvisioningApn(
        /* [out] */ Boolean* result);

    CARAPI IncRefCount();

    CARAPI DecRefCount();

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI Log(
        /* [in] */ const char *fmt, ...);

    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    CARAPI GetPriority(
        /* [out] */ Int32* result);

public:
    static const Boolean DBG;

    AutoPtr<IDcAsyncChannel> mDcAc;

    String mReason;

    AutoPtr<IPendingIntent> mReconnectAlarmIntent;

    /**
     * user/app requested connection on this APN
     */
    AutoPtr<IAtomicBoolean> mDataEnabled;

    /**
     * carrier requirements met
     */
    AutoPtr<IAtomicBoolean> mDependencyMet;

private:
    String LOG__TAG;

    AutoPtr<IContext> mContext;

    String mApnType;

    DctConstantsState mState;

    AutoPtr<IArrayList> mWaitingApns;

    Int32 mPriority;

    /** A zero indicates that all waiting APNs had a permanent error */
    AutoPtr<IAtomicInteger32> mWaitingApnsPermanentFailureCountDown;

    AutoPtr<IApnSetting> mApnSetting;

    AutoPtr<IObject> mRefCountLock;

    Int32 mRefCount;

    AutoPtr<IDcTrackerBase> mDcTracker;
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_APNCONTEXT_H__
