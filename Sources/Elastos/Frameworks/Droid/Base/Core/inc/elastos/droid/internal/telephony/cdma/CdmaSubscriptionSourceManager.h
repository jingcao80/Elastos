#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMASUBSCRIPTIONSOURCEMANAGER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMASUBSCRIPTIONSOURCEMANAGER_H__

#include "_Elastos.Droid.Internal.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/RegistrantList.h"
#include "elastos/droid/os/Handler.h"

// import android.os.Registrant;
// import android.provider.Settings;
// import android.telephony.Rlog;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Os::RegistrantList;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

/**
  * Class that handles the CDMA subscription source changed events from RIL
  */
class CdmaSubscriptionSourceManager
    : public Handler
    , public ICdmaSubscriptionSourceManager
{
public:
    CAR_INTERFACE_DECL();

    /**
      * This function creates a single instance of this class
      *
      * @return object of type CdmaSubscriptionSourceManager
      */
    static CARAPI_(AutoPtr<ICdmaSubscriptionSourceManager>) GetInstance(
        /* [in] */ IContext* context,
        /* [in] */ ICommandsInterface* ci,
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    /**
      * Unregisters for the registered event with RIL
      */
    virtual CARAPI Dispose(
        /* [in] */ IHandler* h);

    /*
      * (non-Javadoc)
      * @see android.os.Handler#handleMessage(android.os.Message)
      */
    // @Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    /**
      * Returns the current CDMA subscription source value
      * @return CDMA subscription source value
      */
    virtual CARAPI GetCdmaSubscriptionSource(
        /* [out] */ Int32* result);

    /**
      * Gets the default CDMA subscription source
      *
      * @return Default CDMA subscription source from Settings DB if present.
      */
    static CARAPI_(Int32) GetDefault(
        /* [in] */ IContext* context);

private:
    // Constructor
    CdmaSubscriptionSourceManager(
        /* [in] */ IContext* context,
        /* [in] */ ICommandsInterface* ci);

    /**
      * Clients automatically register for CDMA subscription source changed event
      * when they get an instance of this object.
      */
    CARAPI_(void) RegisterForCdmaSubscriptionSourceChanged(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    /**
      * Handles the call to get the subscription source
      *
      * @param ar AsyncResult object that contains the result of get CDMA
      *            subscription source call
      */
    CARAPI_(void) HandleGetCdmaSubscriptionSource(
        /* [in] */ AsyncResult* ar);

    CARAPI_(void) Log(
        /* [in] */ const String& s);

    CARAPI_(void) Logw(
        /* [in] */ const String& s);

public:
    static const String LOGTAG;

private:
    static const Int32 EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED = 1;
    static const Int32 EVENT_GET_CDMA_SUBSCRIPTION_SOURCE = 2;
    static const Int32 EVENT_RADIO_ON = 3;
    static const Int32 EVENT_SUBSCRIPTION_STATUS_CHANGED = 4;
    // To know subscription is activated
    static const Int32 SUBSCRIPTION_ACTIVATED = 1;
    static AutoPtr<ICdmaSubscriptionSourceManager> sInstance;
    static AutoPtr<IObject> sReferenceCountMonitor;
    static Int32 sReferenceCount;
    // ***** Instance Variables
    AutoPtr<ICommandsInterface> mCi;
    AutoPtr<IContext> mContext;
    AutoPtr<RegistrantList> mCdmaSubscriptionSourceChangedRegistrants;
    // Type of CDMA subscription source
    AutoPtr<IAtomicInteger32> mCdmaSubscriptionSource;
};

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMASUBSCRIPTIONSOURCEMANAGER_H__
