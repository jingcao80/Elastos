
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/internal/telephony/CPhoneFactory.h"
// #include "elastos/droid/internal/telephony/CDefaultPhoneNotifier.h"
#include "elastos/droid/internal/telephony/gsm/CGSMPhone.h"
#include "elastos/droid/internal/telephony/CRIL.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/provider/CSettingsGlobal.h"
#include "elastos/droid/internal/telephony/CPhoneProxy.h"
#include "elastos/droid/net/CLocalServerSocket.h"
#include "elastos/droid/telephony/CTelephonyManagerHelper.h"
#include "elastos/droid/telephony/CSubscriptionManager.h"
#include "elastos/droid/os/CSystemProperties.h"
#include "elastos/droid/os/CUserHandleHelper.h"
#include "elastos/droid/R.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Thread.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::R;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Net::CLocalServerSocket;
using Elastos::Droid::Net::ILocalServerSocket;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::ITelephonyManagerHelper;
using Elastos::Droid::Telephony::CTelephonyManagerHelper;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::CSubscriptionManager;
// using Elastos::Droid::Internal::Telephony::CDefaultPhoneNotifier;
using Elastos::Droid::Internal::Telephony::CPhoneProxy;
using Elastos::Droid::Internal::Telephony::Gsm::CGSMPhone;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;
using Elastos::Droid::Internal::Telephony::Cdma::ICdmaSubscriptionSourceManager;
using Elastos::Droid::Internal::Telephony::Cdma::ICdmaSubscriptionSourceManagerHelper;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccControllerHelper;
using Elastos::Droid::Internal::Telephony::Gsm::IGSMPhone;
using Elastos::Core::AutoLock;
using Elastos::Core::Thread;
using Elastos::Core::CInteger32;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

//==============================================================
//  CPhoneFactory::
//==============================================================

const String CPhoneFactory::LOGTAG("CPhoneFactory");
const Int32 CPhoneFactory::SOCKET_OPEN_RETRY_MILLIS = 2 * 1000;
const Int32 CPhoneFactory::SOCKET_OPEN_MAX_RETRY = 3;

Object CPhoneFactory::sLockProxyPhones;
AutoPtr<ArrayOf<IPhoneProxy*> > CPhoneFactory::sProxyPhones = NULL;
AutoPtr<IPhoneProxy> CPhoneFactory::sProxyPhone;

AutoPtr<ArrayOf<ICommandsInterface*> > CPhoneFactory::sCommandsInterfaces = NULL;

AutoPtr<IProxyController> CPhoneFactory::mProxyController;
AutoPtr<IUiccController> CPhoneFactory::mUiccController;

AutoPtr<ICommandsInterface> CPhoneFactory::sCommandsInterface;
AutoPtr<ISubInfoRecordUpdater> CPhoneFactory::sSubInfoRecordUpdater;
AutoPtr<IModemBindingPolicyHandler> CPhoneFactory::sModemBindingPolicyHandler;
AutoPtr<IModemStackController> CPhoneFactory::sModemStackController;

Boolean CPhoneFactory::sMadeDefaults = FALSE;
AutoPtr<IPhoneNotifier> CPhoneFactory::sPhoneNotifier;
AutoPtr<IContext> CPhoneFactory::sContext;

CAR_INTERFACE_IMPL(CPhoneFactory, Singleton, IPhoneFactory)

CAR_SINGLETON_IMPL(CPhoneFactory)

ECode CPhoneFactory::MakeDefaultPhones(
    /* [in] */ IContext* context)
{
    return MakeDefaultPhone(context);
}

ECode CPhoneFactory::MakeDefaultPhone(
    /* [in] */ IContext* context)
{
    Logger::D("CPhoneFactory", "TODO: MakeDefaultPhone");
//     {
//         AutoLock syncLock(sLockProxyPhones);
//         if (!sMadeDefaults) {
//             sContext = context;

//             // create the telephony device controller.
//             AutoPtr<ITelephonyDevControllerHelper> hlpTDC;
//             assert(0 && "TODO");
//             // CTelephonyDevControllerHelper::AcquireSingleton((ITelephonyDevControllerHelper**)&hlpTDC);
//             AutoPtr<ITelephonyDevController> dc;
//             hlpTDC->Create((ITelephonyDevController**)&dc);

//             Int32 retryCount = 0;
//             for(;;) {
//                 Boolean hasException = FALSE;
//                 retryCount++;

//                 // try {
//                     // use UNIX domain socket to
//                     // prevent subsequent initialization
//                     AutoPtr<ILocalServerSocket> p;
//                     ECode ec = CLocalServerSocket::New(String("com.android.internal.telephony"), (ILocalServerSocket**)&p);
//                     if (FAILED(ec)) {
//                         hasException = TRUE;
//                     }
//                 // } Catch (java.io.IOException ex) {
//                 //     hasException = TRUE;
//                 // }

//                 if (!hasException) {
//                     break;
//                 }
//                 else if (retryCount > SOCKET_OPEN_MAX_RETRY) {
//                     // throw new RuntimeException("PhoneFactory probably already running");
//                     return E_RUNTIME_EXCEPTION;
//                 }
//                 else {
//                     // try {
//                         Thread::Sleep(SOCKET_OPEN_RETRY_MILLIS);
//                     // } Catch (InterruptedException er) {
//                     // }
//                 }
//             }

//             assert(0 && "TODO");
//             // CDefaultPhoneNotifier::New((IDefaultPhoneNotifier**)&sPhoneNotifier);

//             // Get preferred network mode
//             Int32 preferredNetworkMode = IRILConstants::PREFERRED_NETWORK_MODE;
//             AutoPtr<ITelephonyManagerHelper> tmhlp;
//             CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&tmhlp);
//             Int32 lteoncdmamode = 0;
//             tmhlp->GetLteOnCdmaModeStatic(&lteoncdmamode);
//             if (lteoncdmamode == IPhoneConstants::LTE_ON_CDMA_TRUE) {
//                 preferredNetworkMode = IPhone::NT_MODE_GLOBAL;
//             }
//             Int32 lteongsmmode = 0;
//             assert(0 && "TODO");
//             // tmhlp->GetLteOnGsmModeStatic(&lteongsmmode);
//             if (lteongsmmode != 0) {
//                 preferredNetworkMode = IPhone::NT_MODE_LTE_GSM_WCDMA;
//             }

//             AutoPtr<ICdmaSubscriptionSourceManagerHelper> scmhlp;
//             assert(0 && "TODO");
//             // CCdmaSubscriptionSourceManagerHelper::AcquireSingleton((ICdmaSubscriptionSourceManagerHelper**)&scmhlp);
//             Int32 cdmaSubscription = 0;
//             scmhlp->GetDefault(context, &cdmaSubscription);
//             // Rlog::I(LOGTAG, "Cdma Subscription set to %d", cdmaSubscription);

//             /* In case of multi SIM mode two instances of PhoneProxy, RIL are created,
//                where as in single SIM mode only instance. IsMultiSimEnabled() function checks
//                whether it is single SIM or multi SIM mode */
//             AutoPtr<ITelephonyManagerHelper> hlp;
//             CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&hlp);
//             AutoPtr<ITelephonyManager> tm;
//             hlp->GetDefault((ITelephonyManager**)&tm);
//             Int32 numPhones = 0;
//             tm->GetPhoneCount(&numPhones);
//             AutoPtr<ArrayOf<Int32> > networkModes = ArrayOf<Int32>::Alloc(numPhones);
//             sProxyPhones = ArrayOf<IPhoneProxy*>::Alloc(numPhones);
//             sCommandsInterfaces = ArrayOf<ICommandsInterface*>::Alloc(numPhones);
//             AutoPtr<ISystemProperties> sysprop;
//             CSystemProperties::AcquireSingleton((ISystemProperties**)&sysprop);
//             String sRILClassname;
//             sysprop->Get(String("ro.telephony.ril_class"), String("RIL"), &sRILClassname);
//             sRILClassname.Trim();
//             // Rlog::I(LOGTAG, "RILClassname is %s", sRILClassname);

//             for (Int32 i = 0; i < numPhones; i++) {
//                 //reads the system properties and makes commandsinterface
//                 // try {
// //                        // Get preferred network type.
// //                        TODO: Sishir added this code to but we need a new technique for MSim
// //                        Int32 networkType = CalculatePreferredNetworkType(context);
// //                        Rlog->I(LOGTAG, "Network Type set to " + Integer->ToString(networkType));

//                     AutoPtr<IContentResolver> cr;
//                     context->GetContentResolver((IContentResolver**)&cr);
//                     hlp->GetInt32AtIndex(
//                             cr,
//                             ISettingsGlobal::PREFERRED_NETWORK_MODE,
//                             i,
//                             &((*networkModes)[i]));
//                 // } Catch (SettingNotFoundException snfe) {
//                 //     Rlog->E(LOGTAG, "Settings Exception Reading Value At Index for"+
//                 //             " ISettingsGlobal::PREFERRED_NETWORK_MODE");
//                 //     networkModes[i] = preferredNetworkMode;
//                 // }

//                 AutoPtr<IResources> res;
//                 sContext->GetResources((IResources**)&res);
//                 Boolean bPhoneEnabled = FALSE;
//                 res->GetBoolean(
//                         R::bool_::config_global_phone_enabled, &bPhoneEnabled);
//                 if (bPhoneEnabled &&
//                         i == IPhoneConstants::PHONE_ID1) {
//                     (*networkModes)[i] = IPhone::NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA;
//                     Boolean res = FALSE;
//                     hlp->PutInt32AtIndex(cr,
//                             ISettingsGlobal::PREFERRED_NETWORK_MODE, i, (*networkModes)[i], &res);
//                 }

//                 // Rlog::I(LOGTAG, "Network Mode set to %s", StringUtils::ToString((*networkModes)[i]));
//                 // Use reflection to construct the RIL Class (defaults to RIL)
//                 // try {
//                     AutoPtr<IInteger32> pI;
//                     CInteger32::New(i, (IInteger32**)&pI);
//                     AutoPtr<IInterface> pRIL = InstantiateCustomRIL(
//                                                 sRILClassname, context, (*networkModes)[i], cdmaSubscription, pI);
//                     (*sCommandsInterfaces)[i] = ICommandsInterface::Probe(pRIL);
//                 // } Catch (Exception e) {
//                 //     // 6 different types of exceptions are thrown here that it's
//                 //     // easier to just catch Exception as our "error handling" is the same.
//                 //     // Yes, we're blocking the whole thing and making the radio unusable. That's by design.
//                 //     // The log message should make it clear why the radio is broken
//                 //     While (TRUE) {
//                 //         Rlog->E(LOGTAG, "Unable to construct custom RIL class", e);
//                 //         try {Thread->Sleep(10000);} Catch (InterruptedException ie) {}
//                 //     }
//                 // }
//             }
//             // Rlog::I(LOGTAG, "Creating SubscriptionController");
//             AutoPtr<ISubscriptionControllerHelper> schlp;
//             assert(0 && "TODO");
//             // CSubscriptionControllerHelper::AcquireSingleton(&schlp);
//             AutoPtr<ISubscriptionController> sc;
//             schlp->Init(context, sCommandsInterfaces, (ISubscriptionController**)&sc);

//             // Instantiate UiccController so that all other classes can just
//             // call GetInstance()
//             AutoPtr<IUiccControllerHelper> uchlp;
//             assert(0 && "TODO");
//             // CUiccControllerHelper::AcquireSingleton((IUiccControllerHelper**)&uchlp);
//             uchlp->Make(context, sCommandsInterfaces, (IUiccController**)&mUiccController);
//             AutoPtr<IModemStackControllerHelper> mchlp;
//             assert(0 && "TODO");
//             // CModemStackControllerHelper::AcquireSingleton((IModemStackControllerHelper**)&mchlp);
//             // mchlp->Make(context,
//             //         mUiccController,
//             //         sCommandsInterfaces,
//             //         (IModemStackController**)&sModemStackController);
//             AutoPtr<IModemBindingPolicyHandlerHelper> mphlp;
//             assert(0 && "TODO");
//             // CModemBindingPolicyHandlerHelper::AcquireSingleton((IModemBindingPolicyHandlerHelper**)&mphlp);
//             // mphlp->Make(context,
//             //         mUiccController,
//             //         sCommandsInterfaces,
//             //         (IModemBindingPolicyHandler**)&sModemBindingPolicyHandler);

//             for (Int32 i = 0; i < numPhones; i++) {
//                 AutoPtr<IPhoneBase> phone;
//                 Int32 phoneType = 0;
//                 hlp->GetPhoneType((*networkModes)[i], &phoneType);
//                 if (phoneType == IPhoneConstants::PHONE_TYPE_GSM) {
//                     assert(0 && "TODO");
//                     // CGSMPhone::New(context,
//                     //         (*sCommandsInterfaces)[i], sPhoneNotifier, i, (IGSMPhone**)&Phone);
//                 }
//                 else if (phoneType == IPhoneConstants::PHONE_TYPE_CDMA) {
//                     assert(0 && "TODO");
//                     // CCDMALTEPhone::New(context,
//                     //         (*sCommandsInterfaces)[i], sPhoneNotifier, i, (ICDMALTEPhone**)&Phone);
//                 }
//                 // Rlog::I(LOGTAG, "Creating Phone with type = %d sub = %d", phoneType, i);

//                 CPhoneProxy::New(phone, (IPhoneProxy**)&((*sProxyPhones)[i]));
//             }
//             AutoPtr<IProxyControllerHelper> pchlp;
//             assert(0 && "TODO");
//             // CProxyControllerHelper::AcquireSingleton((IProxyControllerHelper**)&pchlp);
//             // pchlp->GetInstance(context, sProxyPhones,
//             //         mUiccController, sCommandsInterfaces, (IProxyController**)&mProxyController);

//             // Set the default phone in base class.
//             // FIXME: This is a first best guess at what the defaults will be. It
//             // FIXME: needs to be done in a more controlled manner in the future.
//             sProxyPhone = (*sProxyPhones)[0];
//             sCommandsInterface = (*sCommandsInterfaces)[0];

//             // Ensure that we have a default SMS app. Requesting the app with
//             // updateIfNeeded set to TRUE is enough to configure a default SMS app.
//             AutoPtr<ISmsApplication> application;
//             assert(0 && "TODO");
//             // CSmsApplication::AcquireSingleton((ISmsApplication**)&application);
//             AutoPtr<IComponentName> componentName;
//             application->GetDefaultSmsApplication(context, TRUE /* updateIfNeeded */, (IComponentName**)&componentName);
//             String packageName("NONE");
//             if (componentName != NULL) {
//                 componentName->GetPackageName(&packageName);
//             }
//             // Rlog::I(LOGTAG, "defaultSmsApplication: %s", packageName);

//             // Set up monitor to watch for changes to SMS packages
//             application->InitSmsPackageMonitor(context);

//             sMadeDefaults = TRUE;

//             // Rlog::I(LOGTAG, "Creating SubInfoRecordUpdater ");
//             assert(0 && "TODO");
//             // sSubInfoRecordUpdater = new SubInfoRecordUpdater(context,
//             //         sProxyPhones, sCommandsInterfaces);
//             AutoPtr<ISubscriptionControllerHelper> hlpSC;
//             assert(0 && "TODO");
//             // CSubscriptionControllerHelper::AcquireSingleton((ISubscriptionControllerHelper**)&hlpSC);
//             AutoPtr<ISubscriptionController> instance;
//             hlpSC->GetInstance((ISubscriptionController**)&instance);
//             instance->UpdatePhonesAvailability(sProxyPhones);
//         }
//     }
    return NOERROR;
}

ECode CPhoneFactory::GetCdmaPhone(
    /* [in] */ Int32 phoneId,
    /* [out] */ IPhone** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IPhone> phone;
    {
        AutoLock syncLock(PhoneProxy::lockForRadioTechnologyChange);
        assert(0 && "TODO");
        // phone = new CDMALTEPhone(sContext, (*sCommandsInterfaces)[phoneId],
        //         sPhoneNotifier, phoneId);
    }
    *result = phone;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CPhoneFactory::GetGsmPhone(
    /* [in] */ Int32 phoneId,
    /* [out] */ IPhone** result)
{
    VALIDATE_NOT_NULL(result)
    {
        AutoLock syncLock(PhoneProxy::lockForRadioTechnologyChange);
        AutoPtr<IPhone> phone;
        CGSMPhone::New(sContext, (*sCommandsInterfaces)[phoneId], sPhoneNotifier, phoneId, (IPhone**)&phone);
        *result = phone;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
}

AutoPtr<IInterface> CPhoneFactory::InstantiateCustomRIL(
    /* [in] */ String sRILClassname,
    /* [in] */ IContext* context,
    /* [in] */ Int32 networkMode,
    /* [in] */ Int32 cdmaSubscription,
    /* [in] */ IInteger32* instanceId)
{
    //assert(0 && "TODO");
    // Class<?> clazz = Class->ForName("com.android.internal.telephony." + sRILClassname);
    // Constructor<?> constructor = clazz->GetConstructor(Context.class, Int32.class, Int32.class, Integer.class);
    // return (T) clazz->Cast(constructor->NewInstance(context, networkMode, cdmaSubscription, instanceId));
    AutoPtr<ICommandsInterface> ci;
    CRIL::New(context, networkMode, cdmaSubscription, instanceId, (ICommandsInterface**)&ci);
    return ci;
}

ECode CPhoneFactory::GetDefaultPhone(
    /* [out] */ IPhone** result)
{
    VALIDATE_NOT_NULL(result)
    {
        AutoLock syncLock(sLockProxyPhones);
        if (!sMadeDefaults) {
            // throw new IllegalStateException("Default phones haven't been made yet!");
            *result = NULL;
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        *result = IPhone::Probe(sProxyPhone);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
}

ECode CPhoneFactory::GetPhone(
    /* [in] */ Int32 phoneId,
    /* [out] */ IPhone** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IPhone> phone;
    {
        AutoLock syncLock(sLockProxyPhones);
        if (!sMadeDefaults) {
            // throw new IllegalStateException("Default phones haven't been made yet!");
            *result = NULL;
            return NOERROR;
            // CAF_MSIM FIXME need to introduce default phone id ?
        }
        else if (phoneId == ISubscriptionManager::DEFAULT_PHONE_ID) {
            // Rlog::D(LOGTAG, "getPhone: phoneId == DEFAULT_PHONE_ID");
            phone = IPhone::Probe(sProxyPhone);
        }
        else {
            // Rlog::D(LOGTAG, "getPhone: phoneId != DEFAULT_PHONE_ID");
            AutoPtr<ITelephonyManagerHelper> hlp;
            CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&hlp);
            AutoPtr<ITelephonyManager> tm;
            hlp->GetDefault((ITelephonyManager**)&tm);
            Int32 phoneCount = 0;
            tm->GetPhoneCount(&phoneCount);
            phone = (((phoneId >= 0)
                            && (phoneId < phoneCount))
                    ? IPhone::Probe((*sProxyPhones)[phoneId]) : NULL);
        }
        // Rlog::D(LOGTAG, "getPhone:- phone=%p", phone.Get());
        *result = phone;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
}

ECode CPhoneFactory::GetPhones(
    /* [out] */ ArrayOf<IPhone*>** result)
{
    VALIDATE_NOT_NULL(result)
    {
        AutoLock syncLock(sLockProxyPhones);
        if (!sMadeDefaults) {
            // throw new IllegalStateException("Default phones haven't been made yet!");
            *result = NULL;
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        assert(0 && "TODO");
        // *result = (ArrayOf<IPhone*>*)sProxyPhones;
        return NOERROR;
    }
}

ECode CPhoneFactory::GetCdmaPhone(
    /* [out] */ IPhone** result)
{
    VALIDATE_NOT_NULL(result)
    if (!sMadeDefaults) {
        // throw new IllegalStateException("Default phones haven't been made yet!");
        *result = NULL;
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    AutoPtr<IPhone> phone;
    {
        AutoLock syncLock(PhoneProxy::lockForRadioTechnologyChange);
        AutoPtr<ITelephonyManagerHelper> hlp;
        CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&hlp);
        Int32 cdmamodestatic = 0;
        hlp->GetLteOnCdmaModeStatic(&cdmamodestatic);
        switch (cdmamodestatic) {
            case IPhoneConstants::LTE_ON_CDMA_TRUE: {
                assert(0 && "TODO");
                // phone = new CDMALTEPhone(sContext, sCommandsInterface, sPhoneNotifier);
                break;
            }
            case IPhoneConstants::LTE_ON_CDMA_FALSE:
            case IPhoneConstants::LTE_ON_CDMA_UNKNOWN:
            default: {
                assert(0 && "TODO");
                // phone = new CDMAPhone(sContext, sCommandsInterface, sPhoneNotifier);
                break;
            }
        }
    }
    *result = phone;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CPhoneFactory::GetGsmPhone(
    /* [out] */ IPhone** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ISubscriptionControllerHelper> hlpSC;
    assert(0 && "TODO");
    // CSubscriptionControllerHelper::AcquireSingleton((ISubscriptionControllerHelper**)&hlpSC);
    AutoPtr<ISubscriptionController> sc;
    hlpSC->GetInstance((ISubscriptionController**)&sc);
    Int64 subscription = 0;
    GetDefaultSubscription(&subscription);
    Int32 phoneId = 0;
    IISub::Probe(sc)->GetPhoneId(subscription, &phoneId);
    AutoPtr<ITelephonyManagerHelper> hlp;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&hlp);
    AutoPtr<ITelephonyManager> tm;
    hlp->GetDefault((ITelephonyManager**)&tm);
    Int32 phoneCount = 0;
    tm->GetPhoneCount(&phoneCount);
    if (phoneId < 0 || phoneId >= phoneCount) {
        phoneId = 0;
    }
    return GetGsmPhone(phoneId, result);
}

ECode CPhoneFactory::GetContext(
    /* [out] */ IContext** result)
{
    VALIDATE_NOT_NULL(result)
    *result = sContext;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CPhoneFactory::MakeSipPhone(
    /* [in] */ const String& sipUri,
    /* [out] */ ISipPhone** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ISipPhone> sp;
    assert(0 && "TODO");
    // SipPhoneFactory::MakePhone(sipUri, sContext, sPhoneNotifier, (ISipPhone**)&sp);
    *result = sp;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CPhoneFactory::SetDefaultSubscription(
    /* [in] */ Int32 subId)
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    sp->Set(ITelephonyProperties::PROPERTY_DEFAULT_SUBSCRIPTION, StringUtils::ToString(subId));
    AutoPtr<ISubscriptionControllerHelper> hlpSC;
    assert(0 && "TODO");
    // CSubscriptionControllerHelper::AcquireSingleton((ISubscriptionControllerHelper**)&hlpSC);
    AutoPtr<ISubscriptionController> sc;
    hlpSC->GetInstance((ISubscriptionController**)&sc);
    Int32 phoneId = 0;
    IISub::Probe(sc)->GetPhoneId(subId, &phoneId);

    {
        AutoLock syncLock(sLockProxyPhones);
        // Set the default phone in base class
        if (phoneId >= 0 && phoneId < sProxyPhones->GetLength()) {
            sProxyPhone = (*sProxyPhones)[phoneId];
            sCommandsInterface = (*sCommandsInterfaces)[phoneId];
            sMadeDefaults = TRUE;
        }
    }

    // Update MCC MNC device configuration information
    AutoPtr<ITelephonyManagerHelper> hlp;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&hlp);
    AutoPtr<ITelephonyManager> tm;
    hlp->GetDefault((ITelephonyManager**)&tm);
    String defaultMccMnc;
    tm->GetSimOperator(phoneId, &defaultMccMnc);
    // Rlog::D(LOGTAG, "update mccmnc=%s", defaultMccMnc);
    assert(0 && "TODO");
    // MccTable::UpdateMccMncConfiguration(sContext, defaultMccMnc, FALSE);

    // Broadcast an Intent for default sub change
    AutoPtr<IIntent> intent;
    CIntent::New(ITelephonyIntents::ACTION_DEFAULT_SUBSCRIPTION_CHANGED, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
    AutoPtr<ISubscriptionManager> sm;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
    sm->PutPhoneIdAndSubIdExtra(intent, phoneId);
    // Rlog::D(LOGTAG, "setDefaultSubscription : %d Broadcasting Default Subscription Changed...", subId);
    AutoPtr<IUserHandleHelper> uhhlp;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhhlp);
    AutoPtr<IUserHandle> uh;
    uhhlp->GetALL((IUserHandle**)&uh);
    sContext->SendStickyBroadcastAsUser(intent, uh);
    return NOERROR;
}

ECode CPhoneFactory::CalculatePreferredNetworkType(
    /* [in] */ IContext* context,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return CalculatePreferredNetworkType(context, GetDefaultPhoneId(), result);
}

ECode CPhoneFactory::CalculatePreferredNetworkType(
    /* [in] */ IContext* context,
    /* [in] */ Int32 phoneId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 networkType = 0;
    Int32 preferredNetworkType = IRILConstants::PREFERRED_NETWORK_MODE;
    AutoPtr<ITelephonyManagerHelper> hlp;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&hlp);
    Int32 cdmamodestatic = 0;
    assert(0 && "TODO");
    // hlp->GetLteOnCdmaModeStatic(phoneId, &cdmamodestatic);
    if (cdmamodestatic == IPhoneConstants::LTE_ON_CDMA_TRUE) {
        preferredNetworkType = IPhone::NT_MODE_GLOBAL;
    }
    // try {
        AutoPtr<IContentResolver> cr;
        context->GetContentResolver((IContentResolver**)&cr);
        hlp->GetInt32AtIndex(cr,
                ISettingsGlobal::PREFERRED_NETWORK_MODE, phoneId, &networkType);
    // } Catch (SettingNotFoundException snfe) {
    //     Rlog->E(LOGTAG, "Settings Exception Reading Value At Index " + phoneId +
    //             " for ISettingsGlobal::PREFERRED_NETWORK_MODE");
    //     networkType = preferredNetworkType;
    // }
    *result = networkType;
    return NOERROR;
}

ECode CPhoneFactory::GetDefaultSubscription(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ISubscriptionControllerHelper> hlpSC;
    assert(0 && "TODO");
    // CSubscriptionControllerHelper::AcquireSingleton((ISubscriptionControllerHelper**)&hlpSC);
    AutoPtr<ISubscriptionController> sc;
    hlpSC->GetInstance((ISubscriptionController**)&sc);
    Int64 subId = 0;
    IISub::Probe(sc)->GetDefaultSubId(&subId);
    *result = subId;
    return NOERROR;
}

ECode CPhoneFactory::GetVoiceSubscription(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 subId = 0;

    // try {
        AutoPtr<IContentResolver> cr;
        sContext->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<ISettingsGlobal> sg;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
        sg->GetInt32(cr,
                ISettingsGlobal::MULTI_SIM_VOICE_CALL_SUBSCRIPTION, &subId);
    // } Catch (SettingNotFoundException snfe) {
    //     Rlog->E(LOGTAG, "Settings Exception Reading Dual Sim Voice Call Values");
    // }

    AutoPtr<ISubscriptionControllerHelper> hlpSC;
    // CSubscriptionControllerHelper::AcquireSingleton((ISubscriptionControllerHelper**)&hlpSC);
    AutoPtr<ISubscriptionController> sc;
    hlpSC->GetInstance((ISubscriptionController**)&sc);
    Int32 phoneId = 0;
    IISub::Probe(sc)->GetPhoneId(subId, &phoneId);
    // Set subscription to 0 if current subscription is invalid.
    // Ex: multisim.config property is TSTS and subscription is 2.
    // If user is trying to set multisim.config to DSDS and reboots
    // in this case index 2 is invalid so need to set to 0.
    AutoPtr<ITelephonyManagerHelper> hlp;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&hlp);
    AutoPtr<ITelephonyManager> tm;
    hlp->GetDefault((ITelephonyManager**)&tm);
    Int32 phoneCount = 0;
    tm->GetPhoneCount(&phoneCount);
    if (phoneId < 0 || phoneId >= phoneCount) {
        // Rlog::I(LOGTAG, "Subscription is invalid...%d Set to 0", subId);
        subId = 0;
        SetVoiceSubscription(subId);
    }

    *result = subId;
    return NOERROR;
}

ECode CPhoneFactory::IsPromptEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean prompt = FALSE;
    Int32 value = 0;
    // try {
        AutoPtr<IContentResolver> cr;
        sContext->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<ISettingsGlobal> sg;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
        sg->GetInt32(cr,
                ISettingsGlobal::MULTI_SIM_VOICE_PROMPT, &value);
    // } Catch (SettingNotFoundException snfe) {
    //     Rlog->E(LOGTAG, "Settings Exception Reading Dual Sim Voice Prompt Values");
    // }
    prompt = (value == 0) ? FALSE : TRUE ;
    // Rlog::D(LOGTAG, "Prompt option:%d", prompt);

   *result = prompt;
   return NOERROR;
}

ECode CPhoneFactory::SetPromptEnabled(
    /* [in] */ Boolean enabled)
{
    Int32 value = (enabled == FALSE) ? 0 : 1;
    AutoPtr<IContentResolver> cr;
    sContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsGlobal> sg;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
    Boolean bRes = FALSE;
    sg->PutInt32(cr,
                ISettingsGlobal::MULTI_SIM_VOICE_PROMPT,
                value,
                &bRes);
    // Rlog::D(LOGTAG, "setVoicePromptOption to %d", enabled);
    return NOERROR;
}

ECode CPhoneFactory::IsSMSPromptEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean prompt = FALSE;
    Int32 value = 0;
    // try {
        AutoPtr<IContentResolver> cr;
        sContext->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<ISettingsGlobal> sg;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
        sg->GetInt32(cr,
                ISettingsGlobal::MULTI_SIM_SMS_PROMPT, &value);
    // } Catch (SettingNotFoundException snfe) {
    //     Rlog->E(LOGTAG, "Settings Exception Reading Dual Sim SMS Prompt Values");
    // }
    prompt = (value == 0) ? FALSE : TRUE ;
    // Rlog::D(LOGTAG, "SMS Prompt option:%d", prompt);

   *result = prompt;
   return NOERROR;
}

ECode CPhoneFactory::SetSMSPromptEnabled(
    /* [in] */ Boolean enabled)
{
    Int32 value = (enabled == FALSE) ? 0 : 1;
    AutoPtr<IContentResolver> cr;
    sContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsGlobal> sg;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
    Boolean bRes = FALSE;
    sg->PutInt32(cr,
                ISettingsGlobal::MULTI_SIM_SMS_PROMPT,
                value,
                &bRes);
    // Rlog::D(LOGTAG, "setSMSPromptOption to %d", enabled);
    return NOERROR;
}

ECode CPhoneFactory::GetDataSubscription(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    Int64 subId = 1;

    // try {
        AutoPtr<IContentResolver> cr;
        sContext->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<ISettingsGlobal> sg;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
        sg->GetInt64(cr,
                ISettingsGlobal::MULTI_SIM_DATA_CALL_SUBSCRIPTION, &subId);
    // } Catch (SettingNotFoundException snfe) {
    //     Rlog->E(LOGTAG, "Settings Exception Reading Dual Sim Data Call Values");
    // }

    AutoPtr<ISubscriptionControllerHelper> hlpSC;
    assert(0 && "TODO");
    // CSubscriptionControllerHelper::AcquireSingleton((ISubscriptionControllerHelper**)&hlpSC);
    AutoPtr<ISubscriptionController> sc;
    hlpSC->GetInstance((ISubscriptionController**)&sc);
    Int32 phoneId = 0;
    IISub::Probe(sc)->GetPhoneId(subId, &phoneId);
    AutoPtr<ITelephonyManagerHelper> hlp;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&hlp);
    AutoPtr<ITelephonyManager> tm;
    hlp->GetDefault((ITelephonyManager**)&tm);
    Int32 phoneCount = 0;
    tm->GetPhoneCount(&phoneCount);
    if (phoneId < 0 || phoneId >= phoneCount) {
        subId = 1;
        // Rlog::I(LOGTAG, "Subscription is invalid...%ld Set to 0", subId);
        SetDataSubscription(subId);
    }

    *result = subId;
    return NOERROR;
}

ECode CPhoneFactory::GetSMSSubscription(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 subId = 0;
    // try {
        AutoPtr<IContentResolver> cr;
        sContext->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<ISettingsGlobal> sg;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
        sg->GetInt32(cr,
                ISettingsGlobal::MULTI_SIM_SMS_SUBSCRIPTION, &subId);
    // } Catch (SettingNotFoundException snfe) {
    //     Rlog->E(LOGTAG, "Settings Exception Reading Dual Sim SMS Values");
    // }

    AutoPtr<ISubscriptionControllerHelper> hlpSC;
    assert(0 && "TODO");
    // CSubscriptionControllerHelper::AcquireSingleton((ISubscriptionControllerHelper**)&hlpSC);
    AutoPtr<ISubscriptionController> sc;
    hlpSC->GetInstance((ISubscriptionController**)&sc);
    Int32 phoneId = 0;
    IISub::Probe(sc)->GetPhoneId(subId, &phoneId);
    AutoPtr<ITelephonyManagerHelper> hlp;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&hlp);
    AutoPtr<ITelephonyManager> tm;
    hlp->GetDefault((ITelephonyManager**)&tm);
    Int32 phoneCount = 0;
    tm->GetPhoneCount(&phoneCount);
    if (phoneId < 0 || phoneId >= phoneCount) {
        // Rlog::I(LOGTAG, "Subscription is invalid..." + subId + " Set to 0");
        subId = 0;
        SetSMSSubscription(subId);
    }

    *result = subId;
    return NOERROR;
}

ECode CPhoneFactory::SetVoiceSubscription(
    /* [in] */ Int32 subId)
{
    AutoPtr<IContentResolver> cr;
    sContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsGlobal> sg;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
    Boolean bRes = FALSE;
    sg->PutInt32(cr,
                ISettingsGlobal::MULTI_SIM_VOICE_CALL_SUBSCRIPTION,
                subId,
                &bRes);
    // Rlog::D(LOGTAG, "setVoiceSubscription : %d", subId);
    return NOERROR;
}

ECode CPhoneFactory::SetDataSubscription(
    /* [in] */ Int64 subId)
{
    Boolean enabled;

    AutoPtr<IContentResolver> cr;
    sContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsGlobal> sg;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
    Boolean bRes = FALSE;
    sg->PutInt64(cr,
                ISettingsGlobal::MULTI_SIM_DATA_CALL_SUBSCRIPTION,
                subId,
                &bRes);
    // Rlog::D(LOGTAG, "setDataSubscription: %ld", subId);

    // Update the current mobile data flag
    Int32 num = 0;
    sg->GetInt32(cr,
            ISettingsGlobal::MOBILE_DATA + subId, 0, &num);
    enabled = num != 0;
    sg->PutInt32(cr,
                ISettingsGlobal::MOBILE_DATA,
                enabled ? 1 : 0,
                &bRes);
    // Rlog::D(LOGTAG, "set mobile_data: %d", enabled);

    // Update the current data roaming flag
    sg->GetInt32(cr,
            ISettingsGlobal::DATA_ROAMING + subId, 0, &num);
    enabled = num != 0;
    sg->PutInt32(cr,
                ISettingsGlobal::DATA_ROAMING,
                enabled ? 1 : 0,
                &bRes);
    // Rlog::D(LOGTAG, "set data_roaming: %d", enabled);
    return NOERROR;
}

ECode CPhoneFactory::SetSMSSubscription(
    /* [in] */ Int32 subId)
{
    AutoPtr<IContentResolver> cr;
    sContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsGlobal> sg;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
    Boolean bRes = FALSE;
    sg->PutInt32(cr,
                ISettingsGlobal::MULTI_SIM_SMS_SUBSCRIPTION,
                subId,
                &bRes);

    AutoPtr<IIntent> intent;
    CIntent::New(String("com.android.mms.transaction.SEND_MESSAGE"), (IIntent**)&intent);
    sContext->SendBroadcast(intent);

    // Change occured in SMS preferred sub, update the default
    // SMS interface Manager object with the new SMS preferred subscription.
    // Rlog::D(LOGTAG, "setSMSSubscription : %d", subId);
    return NOERROR;
}

ECode CPhoneFactory::MakeImsPhone(
    /* [in] */ IPhoneNotifier* phoneNotifier,
    /* [in] */ IPhone* defaultPhone,
    /* [out] */ IImsPhone** result)
{
    VALIDATE_NOT_NULL(result)
    assert(0 && "TODO");
    // return ImsPhoneFactory::MakePhone(sContext, phoneNotifier, defaultPhone);
    *result = NULL;
    return NOERROR;
}

Boolean CPhoneFactory::IsValidphoneId(
    /* [in] */ Int32 phoneId)
{
    AutoPtr<ITelephonyManagerHelper> hlp;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&hlp);
    AutoPtr<ITelephonyManager> tm;
    hlp->GetDefault((ITelephonyManager**)&tm);
    Int32 pCount = 0;
    tm->GetPhoneCount(&pCount);
    return (phoneId >= 0) && (phoneId < pCount);
}

Int32 CPhoneFactory::GetDefaultPhoneId()
{
    AutoPtr<ISubscriptionControllerHelper> hlpSC;
    assert(0 && "TODO");
    // CSubscriptionControllerHelper::AcquireSingleton((ISubscriptionControllerHelper**)&hlpSC);
    AutoPtr<ISubscriptionController> sc;
    hlpSC->GetInstance((ISubscriptionController**)&sc);
    Int64 subscription = 0;
    assert(0 && "TODO");
    // GetDefaultSubscription(&subscription);
    Int32 phoneId = 0;
    IISub::Probe(sc)->GetPhoneId(subscription, &phoneId);
    if (!IsValidphoneId(phoneId)) {
        phoneId = 0;
    }
    return phoneId;
}

ECode CPhoneFactory::GetSubInfoRecordUpdater(
    /* [out] */ ISubInfoRecordUpdater** result)
{
    VALIDATE_NOT_NULL(result)
    *result = sSubInfoRecordUpdater;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
