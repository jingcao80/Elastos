
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_PROXYCONTROLLER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_PROXYCONTROLLER_H__

#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::DataConnection::IDctController;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

class ProxyController
    : public Object
    , public IProxyController
{
public:
    CAR_INTERFACE_DECL()

    //***** Class Methods
    static CARAPI GetInstance(
        /* [in] */ IContext* context,
        /* [in] */ ArrayOf<IPhone*>* phoneProxy,
        /* [in] */ IUiccController* uiccController,
        /* [in] */ ArrayOf<ICommandsInterface*>* ci,
        /* [out] */ IProxyController** result);

    static CARAPI GetInstance(
        /* [out] */ IProxyController** result);

    CARAPI UpdateDataConnectionTracker(
        /* [in] */ Int32 sub);

    CARAPI EnableDataConnectivity(
        /* [in] */ Int32 sub);

    CARAPI DisableDataConnectivity(
        /* [in] */ Int32 sub,
        /* [in] */ IMessage* dataCleanedUpMsg);

    CARAPI UpdateCurrentCarrierInProvider(
        /* [in] */ Int32 sub);

    CARAPI RegisterForAllDataDisconnected(
        /* [in] */ Int64 subId,
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI UnregisterForAllDataDisconnected(
        /* [in] */ Int64 subId,
        /* [in] */ IHandler* h);

    CARAPI IsDataDisconnected(
        /* [in] */ Int64 subId,
        /* [out] */ Boolean* result);

private:
    ProxyController(
        /* [in] */ IContext* context,
        /* [in] */ ArrayOf<IPhone*>* phoneProxy,
        /* [in] */ IUiccController* uiccController,
        /* [in] */ ArrayOf<ICommandsInterface*>* ci);

    CARAPI_(void) Logd(
        /* [in] */ String string);

public:
    static const String LOGTAG;

private:
    //***** Class Variables
    static AutoPtr<IProxyController> sProxyController;

    AutoPtr<ArrayOf<IPhone*> > mProxyPhones;

    AutoPtr<IUiccController> mUiccController;

    AutoPtr<ArrayOf<ICommandsInterface*> > mCi;

    AutoPtr<IContext> mContext;

    static AutoPtr<IDctController> mDctController;

    //UiccPhoneBookController to use proper IccPhoneBookInterfaceManagerProxy object
    AutoPtr<IUiccPhoneBookController> mUiccPhoneBookController;

    //PhoneSubInfoController to use proper PhoneSubInfoProxy object
    AutoPtr<IPhoneSubInfoController> mPhoneSubInfoController;

    //UiccSmsController to use proper IccSmsInterfaceManager object
    AutoPtr<IUiccSmsController> mUiccSmsController;

  //  private SubscriptionManager mSubscriptionManager;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_PROXYCONTROLLER_H__
