#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/R.h"
#include "elastos/droid/telephony/CellLocation.h"
#include "elastos/droid/telephony/PhoneStateListener.h"
#include "elastos/droid/telephony/CTelephonyManager.h"
#include "elastos/droid/telephony/SubscriptionManager.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::IRILConstants;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::R;
using Elastos::Core::StringUtils;
using Elastos::IO::CFileInputStream;
using Elastos::IO::ICloseable;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IInputStream;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Regex::CPatternHelper;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::IMatchResult;
using Elastos::Utility::Regex::IPatternHelper;

namespace Elastos {
namespace Droid {
namespace Telephony {

static String InitMultiSimConfig()
{
    String str;
    SystemProperties::Get(ITelephonyProperties::PROPERTY_MULTI_SIM_CONFIG, &str);
    return str;
}

static String InitLteOnCdmaProductType()
{
    String str;
    SystemProperties::Get(ITelephonyProperties::PROPERTY_LTE_ON_CDMA_PRODUCT_TYPE, String(""), &str);
    return str;
}

static AutoPtr<IPattern> InitProductTypePattern()
{
    AutoPtr<IPatternHelper> helper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
    AutoPtr<IPattern> pattern;
    helper->Compile(String("\\sproduct_type\\s*=\\s*(\\w+)"), (IPattern**)&pattern);

    return pattern;
}

static AutoPtr<ITelephonyManager> InitInstance()
{
    AutoPtr<ITelephonyManager> tm;
    CTelephonyManager::New((ITelephonyManager**)&tm);
    return tm;
}

CAR_INTERFACE_IMPL(CTelephonyManager, Object, ITelephonyManager)

CAR_OBJECT_IMPL(CTelephonyManager)

const String CTelephonyManager::TAG("CTelephonyManager");

AutoPtr<IITelephonyRegistry> CTelephonyManager::sRegistry;

String CTelephonyManager::sMultiSimConfig = InitMultiSimConfig();

INIT_PROI_3 AutoPtr<ITelephonyManager> CTelephonyManager::sInstance = InitInstance();

/** Kernel command line */
String CTelephonyManager::sKernelCmdLine = GetProcCmdLine();

/** Pattern for selecting the product type from the kernel command line */
AutoPtr<IPattern> CTelephonyManager::sProductTypePattern = InitProductTypePattern();

/** The ProductType used for LTE on CDMA devices */
String CTelephonyManager::sLteOnCdmaProductType = InitLteOnCdmaProductType();

CTelephonyManager::CTelephonyManager()
{
}

CTelephonyManager::~CTelephonyManager()
{
}

ECode CTelephonyManager::constructor()
{
    mContext = NULL;
    return NOERROR;
}

ECode CTelephonyManager::constructor(
    /* [in] */ IContext* context)
{
    AutoPtr<IContext> appContext;
    context->GetApplicationContext((IContext**)&appContext);

    if (appContext != NULL) {
        mContext = appContext;
    }
    else {
        mContext = context;
    }

    if (sRegistry == NULL) {
        AutoPtr<IInterface> b = ServiceManager::GetService(String("telephony.registry"));
        sRegistry = IITelephonyRegistry::Probe(b);
    }
    return NOERROR;
}

ECode CTelephonyManager::GetMultiSimConfiguration(
    /* [out] */ MultiSimVariants* res)
{
    VALIDATE_NOT_NULL(res)

    String mSimConfig;
    SystemProperties::Get(ITelephonyProperties::PROPERTY_MULTI_SIM_CONFIG, &mSimConfig);

    if (mSimConfig.Equals("dsds")) {
        *res = MultiSimVariants_DSDS;
    } else if (mSimConfig.Equals("dsda")) {
        *res = MultiSimVariants_DSDA;
    } else if (mSimConfig.Equals("tsts")) {
        *res = MultiSimVariants_TSTS;
    } else {
        *res = MultiSimVariants_UNKNOWN;
    }
    return NOERROR;
}

ECode CTelephonyManager::GetPhoneCount(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    MultiSimVariants val;
    GetMultiSimConfiguration(&val);

    Int32 phoneCount = 1;
    switch (val) {
        case MultiSimVariants_DSDS:
        case MultiSimVariants_DSDA:
            phoneCount = IPhoneConstants::MAX_PHONE_COUNT_DUAL_SIM;
            break;
        case MultiSimVariants_TSTS:
            phoneCount = IPhoneConstants::MAX_PHONE_COUNT_TRI_SIM;
            break;
    }
    *res = phoneCount;
    return NOERROR;
}

ECode CTelephonyManager::IsMultiSimEnabled(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    *res = (sMultiSimConfig.Equals("dsds") || sMultiSimConfig.Equals("dsda") ||
        sMultiSimConfig.Equals("tsts"));
    return NOERROR;
}

ECode CTelephonyManager::GetDeviceSoftwareVersion(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetSubscriberInfo()->GetDeviceSvn(res);
    // } catch (RemoteException ex) {
    //     return null;
    // } catch (NullPointerException ex) {
    //     return null;
    // }
}

ECode CTelephonyManager::GetDeviceId(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    Int32 val;
    GetDefaultSim(&val);
    return GetDeviceId(val, res);
}

ECode CTelephonyManager::GetDeviceId(
    /* [in] */ Int32 slotId,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    AutoPtr<ArrayOf<Int64> > subId;
    SubscriptionManager::GetSubId(slotId, (ArrayOf<Int64>**)&subId);

    // try {
    return GetSubscriberInfo()->GetDeviceIdForSubscriber((*subId)[0], res);
    // } catch (RemoteException ex) {
    //     return null;
    // } catch (NullPointerException ex) {
    //     return null;
    // }
}

ECode CTelephonyManager::GetImei(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    Int32 val;
    GetDefaultSim(&val);
    return GetImei(val, res);
}

ECode CTelephonyManager::GetImei(
    /* [in] */ Int32 slotId,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    AutoPtr<ArrayOf<Int64> > subId;
    SubscriptionManager::GetSubId(slotId, (ArrayOf<Int64>**)&subId);

    // try {
    return GetSubscriberInfo()->GetImeiForSubscriber((*subId)[0], res);
    // } catch (RemoteException ex) {
    //     return null;
    // } catch (NullPointerException ex) {
    //     return null;
    // }
}

ECode CTelephonyManager::GetCellLocation(
    /* [out] */ ICellLocation** res)
{
    VALIDATE_NOT_NULL(res)
    *res = NULL;

    // try {
    AutoPtr<IBundle> bundle;
    GetITelephony()->GetCellLocation((IBundle**)&bundle);

    Boolean b;
    if (bundle->IsEmpty(&b), b) return NOERROR;

    AutoPtr<ICellLocation> cl;
    CellLocation::NewFromBundle(bundle, (ICellLocation**)&cl);

    if (cl->IsEmpty(&b), b)
        return NOERROR;

    *res = cl;
    REFCOUNT_ADD(*res)
    return NOERROR;
    // } catch (RemoteException ex) {
    //     return null;
    // } catch (NullPointerException ex) {
    //     return null;
    // }
}

ECode CTelephonyManager::EnableLocationUpdates()
{
    return EnableLocationUpdates(GetDefaultSubscription());
}

ECode CTelephonyManager::EnableLocationUpdates(
    /* [in] */ Int64 subId)
{
    // try {
    return GetITelephony()->EnableLocationUpdatesForSubscriber(subId);
    // } catch (RemoteException ex) {
    // } catch (NullPointerException ex) {
    // }
}

ECode CTelephonyManager::DisableLocationUpdates()
{
    return DisableLocationUpdates(GetDefaultSubscription());
}

ECode CTelephonyManager::DisableLocationUpdates(
    /* [in] */ Int64 subId)
{
    // try {
    return GetITelephony()->DisableLocationUpdatesForSubscriber(subId);
    // } catch (RemoteException ex) {
    // } catch (NullPointerException ex) {
    // }
}

ECode CTelephonyManager::GetNeighboringCellInfo(
    /* [out] */ IList** res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    String name;
    mContext->GetOpPackageName(&name);
    return GetITelephony()->GetNeighboringCellInfo(name, res);
    // } catch (RemoteException ex) {
    //     return null;
    // } catch (NullPointerException ex) {
    //     return null;
    // }
}

ECode CTelephonyManager::GetCurrentPhoneType(
    /* [out] */ Int32* val)
{
    return GetCurrentPhoneType(GetDefaultSubscription(), val);
}

ECode CTelephonyManager::GetCurrentPhoneType(
    /* [in] */ Int64 subId,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    // try{
    AutoPtr<IITelephony> telephony = GetITelephony();
    if (telephony != NULL) {
        return telephony->GetActivePhoneTypeForSubscriber(subId, res);
    }
    else {
        // This can happen when the ITelephony interface is not up yet.
        *res = GetPhoneTypeFromProperty(subId);
    }
    // } catch (RemoteException ex) {
    //     // This shouldn't happen in the normal case, as a backup we
    //     // read from the system property.
    //     return getPhoneTypeFromProperty(subId);
    // } catch (NullPointerException ex) {
    //     // This shouldn't happen in the normal case, as a backup we
    //     // read from the system property.
    //     return getPhoneTypeFromProperty(subId);
    // }
    return NOERROR;
}

ECode CTelephonyManager::GetPhoneType(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    Boolean b;
    if (IsVoiceCapable(&b), !b) {
        *res = PHONE_TYPE_NONE;
    }
    return GetCurrentPhoneType(res);
}

Int32 CTelephonyManager::GetLteOnGsmModeStatic()
{
    Int32 v = 0;
    SystemProperties::GetInt32(ITelephonyProperties::PROPERTY_LTE_ON_GSM_DEVICE, 0, &v);
    return v;
}

ECode CTelephonyManager::GetNetworkOperatorName(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)
    return GetNetworkOperatorName(GetDefaultSubscription(), res);
}

ECode CTelephonyManager::GetNetworkOperatorName(
    /* [in] */ Int64 subId,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)
    return GetTelephonyProperty(ITelephonyProperties::PROPERTY_OPERATOR_ALPHA,
            subId, String(""), res);
}

ECode CTelephonyManager::GetNetworkOperator(
    /* [out] */ String* res)
{
    return GetNetworkOperator(GetDefaultSubscription(), res);
}

ECode CTelephonyManager::GetNetworkOperator(
    /* [in] */ Int64 subId,
    /* [out] */ String* res)
{
    return GetTelephonyProperty(ITelephonyProperties::PROPERTY_OPERATOR_NUMERIC,
            subId, String(""), res);
}

ECode CTelephonyManager::IsNetworkRoaming(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    return IsNetworkRoaming(GetDefaultSubscription(), res);
}

ECode CTelephonyManager::IsNetworkRoaming(
    /* [in] */ Int64 subId,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    String str;
    GetTelephonyProperty(ITelephonyProperties::PROPERTY_OPERATOR_ISROAMING,
            subId, String(NULL), &str);
    *res = str.Equals("TRUE");
    return NOERROR;
}

ECode CTelephonyManager::GetNetworkCountryIso(
    /* [out] */ String* res)
{
    return GetNetworkCountryIso(GetDefaultSubscription(), res);
}

ECode CTelephonyManager::GetNetworkCountryIso(
    /* [in] */ Int64 subId,
    /* [out] */ String* res)
{
    return GetTelephonyProperty(ITelephonyProperties::PROPERTY_OPERATOR_ISO_COUNTRY,
            subId, String(""), res);
}

ECode CTelephonyManager::GetNetworkType(
    /* [out] */ Int32* res)
{
    return GetDataNetworkType(res);
}

ECode CTelephonyManager::GetNetworkType(
    /* [in] */ Int64 subId,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

   // try {
   AutoPtr<IITelephony> telephony = GetITelephony();
   if (telephony != NULL) {
       return telephony->GetNetworkTypeForSubscriber(subId, res);
   }
   else {
       // This can happen when the ITelephony interface is not up yet.
       *res = NETWORK_TYPE_UNKNOWN;
   }
   // } catch(RemoteException ex) {
   //     // This shouldn't happen in the normal case
   //     return NETWORK_TYPE_UNKNOWN;
   // } catch (NullPointerException ex) {
   //     // This could happen before phone restarts due to crashing
   //     return NETWORK_TYPE_UNKNOWN;
   // }
    return NOERROR;
}

ECode CTelephonyManager::GetDataNetworkType(
    /* [out] */ Int32* res)
{
    return GetDataNetworkType(GetDefaultSubscription(), res);
}

ECode CTelephonyManager::GetDataNetworkType(
    /* [in] */ Int64 subId,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    // try{
   AutoPtr<IITelephony> telephony = GetITelephony();
    if (telephony != NULL) {
        return telephony->GetDataNetworkTypeForSubscriber(subId, res);
    }
    else {
        // This can happen when the ITelephony interface is not up yet.
        *res = NETWORK_TYPE_UNKNOWN;
    }
    // } catch(RemoteException ex) {
    //     // This shouldn't happen in the normal case
    //     return NETWORK_TYPE_UNKNOWN;
    // } catch (NullPointerException ex) {
    //     // This could happen before phone restarts due to crashing
    //     return NETWORK_TYPE_UNKNOWN;
    // }
    return NOERROR;
}

ECode CTelephonyManager::GetVoiceNetworkType(
    /* [out] */ Int32* res)
{
    return GetVoiceNetworkType(GetDefaultSubscription(), res);
}

ECode CTelephonyManager::GetVoiceNetworkType(
    /* [in] */ Int64 subId,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    // try{
   AutoPtr<IITelephony> telephony = GetITelephony();
    if (telephony != NULL) {
        return telephony->GetVoiceNetworkTypeForSubscriber(subId, res);
    }
    else {
        // This can happen when the ITelephony interface is not up yet.
        *res = NETWORK_TYPE_UNKNOWN;
    }
    // } catch(RemoteException ex) {
    //     // This shouldn't happen in the normal case
    //     return NETWORK_TYPE_UNKNOWN;
    // } catch (NullPointerException ex) {
    //     // This could happen before phone restarts due to crashing
    //     return NETWORK_TYPE_UNKNOWN;
    // }
    return NOERROR;
}

ECode CTelephonyManager::GetNetworkTypeName(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    Int32 val;
    GetNetworkType(&val);
    return GetNetworkTypeName(val, res);
}

ECode CTelephonyManager::HasIccCard(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    Int32 val;
    GetDefaultSim(&val);
    return HasIccCard(val, res);
}

ECode CTelephonyManager::HasIccCard(
    /* [in] */ Int64 slotId,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->HasIccCardUsingSlotId(slotId, res);
    // } catch (RemoteException ex) {
    //     // Assume no ICC card if remote exception which shouldn't happen
    //     return false;
    // } catch (NullPointerException ex) {
    //     // This could happen before phone restarts due to crashing
    //     return false;
    // }
}

ECode CTelephonyManager::GetSimState(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    Int32 val;
    GetDefaultSim(&val);
    return GetSimState(val, res);
}

ECode CTelephonyManager::GetSimState(
    /* [in] */ Int32 slotId,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    AutoPtr<ArrayOf<Int64> > subId;
    SubscriptionManager::GetSubId(slotId, (ArrayOf<Int64>**)&subId);
    if (subId == NULL) {
        *res = SIM_STATE_ABSENT;
        return NOERROR;
    }
    // FIXME Do not use a property to determine SIM_STATE, call
    // appropriate method on some object.
    String prop;
    GetTelephonyProperty(ITelephonyProperties::PROPERTY_SIM_STATE,
            (*subId)[0], String(""), &prop);

    if (prop.Equals("ABSENT")) {
        *res = SIM_STATE_ABSENT;
    }
    else if (prop.Equals("PIN_REQUIRED")) {
        *res = SIM_STATE_PIN_REQUIRED;
    }
    else if (prop.Equals("PUK_REQUIRED")) {
        *res = SIM_STATE_PUK_REQUIRED;
    }
    else if (prop.Equals("NETWORK_LOCKED")) {
        *res = SIM_STATE_NETWORK_LOCKED;
    }
    else if (prop.Equals("READY")) {
        *res = SIM_STATE_READY;
    }
    else if (prop.Equals("CARD_IO_ERROR")) {
        *res = SIM_STATE_CARD_IO_ERROR;
    }
    else {
        *res = SIM_STATE_UNKNOWN;
    }
    return NOERROR;
}

ECode CTelephonyManager::GetSimOperator(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)
    Int64 subId = GetDefaultSubscription();
    Logger::D(TAG, "getSimOperator(): default subId=%ld", subId);
    return GetSimOperator(subId, res);
}

ECode CTelephonyManager::GetSimOperator(
    /* [in] */ Int64 subId,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)
    String oper;
    GetTelephonyProperty(ITelephonyProperties::PROPERTY_ICC_OPERATOR_NUMERIC,
            subId, String(""), &oper);
    Logger::D(TAG, "getSimOperator: subId=%ld operator=%s", subId, oper.string());
    *res = oper;
    return NOERROR;
}

ECode CTelephonyManager::GetSimOperatorName(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)
    return GetSimOperatorName(GetDefaultSubscription(), res);
}

ECode CTelephonyManager::GetSimOperatorName(
    /* [in] */ Int64 subId,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)
    return GetTelephonyProperty(ITelephonyProperties::PROPERTY_ICC_OPERATOR_ALPHA,
            subId, String(""), res);
}

ECode CTelephonyManager::GetSimCountryIso(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)
    return GetSimCountryIso(GetDefaultSubscription(), res);
}

ECode CTelephonyManager::GetSimCountryIso(
    /* [in] */ Int64 subId,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)
    return GetTelephonyProperty(ITelephonyProperties::PROPERTY_ICC_OPERATOR_ISO_COUNTRY,
            subId, String(""), res);
}

ECode CTelephonyManager::GetSimSerialNumber(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)
    return GetSimSerialNumber(GetDefaultSubscription(), res);
}

ECode CTelephonyManager::GetSimSerialNumber(
    /* [in] */ Int64 subId,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetSubscriberInfo()->GetIccSerialNumberForSubscriber(subId, res);
    // } catch (RemoteException ex) {
    //     return null;
    // } catch (NullPointerException ex) {
    //     // This could happen before phone restarts due to crashing
    //     return null;
    // }
}

ECode CTelephonyManager::GetLteOnCdmaMode(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    return GetLteOnCdmaMode(GetDefaultSubscription(), res);
}

ECode CTelephonyManager::GetLteOnCdmaMode(
    /* [in] */ Int64 subId,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->GetLteOnCdmaModeForSubscriber(subId, res);
    // } catch (RemoteException ex) {
    //     // Assume no ICC card if remote exception which shouldn't happen
    //     return PhoneConstants.LTE_ON_CDMA_UNKNOWN;
    // } catch (NullPointerException ex) {
    //     // This could happen before phone restarts due to crashing
    //     return PhoneConstants.LTE_ON_CDMA_UNKNOWN;
    // }
}

ECode CTelephonyManager::GetSubscriberId(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)
    return GetSubscriberId(GetDefaultSubscription(), res);
}

ECode CTelephonyManager::GetSubscriberId(
    /* [in] */ Int64 subId,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetSubscriberInfo()->GetSubscriberIdForSubscriber(subId, res);
    // } catch (RemoteException ex) {
    //     return null;
    // } catch (NullPointerException ex) {
    //     // This could happen before phone restarts due to crashing
    //     return null;
    // }
}

ECode CTelephonyManager::GetGroupIdLevel1(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetSubscriberInfo()->GetGroupIdLevel1(res);
    // } catch (RemoteException ex) {
    //     return null;
    // } catch (NullPointerException ex) {
    //     // This could happen before phone restarts due to crashing
    //     return null;
    // }
}

ECode CTelephonyManager::GetGroupIdLevel1(
    /* [in] */ Int64 subId,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetSubscriberInfo()->GetGroupIdLevel1ForSubscriber(subId, res);
    // } catch (RemoteException ex) {
    //     return null;
    // } catch (NullPointerException ex) {
    //     // This could happen before phone restarts due to crashing
    //     return null;
    // }
}

ECode CTelephonyManager::GetLine1Number(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)
    return GetLine1NumberForSubscriber(GetDefaultSubscription(), res);
}

ECode CTelephonyManager::GetLine1NumberForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    String number;
    // try {
    GetITelephony()->GetLine1NumberForDisplay(subId, &number);
    // } catch (RemoteException ex) {
    // } catch (NullPointerException ex) {
    // }
    if (!number.IsNull()) {
        *res = number;
        return NOERROR;
    }
    // try {
    return GetSubscriberInfo()->GetLine1NumberForSubscriber(subId, res);
    // } catch (RemoteException ex) {
    //     return null;
    // } catch (NullPointerException ex) {
    //     // This could happen before phone restarts due to crashing
    //     return null;
    // }
}

ECode CTelephonyManager::SetLine1NumberForDisplay(
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& number)
{
    return SetLine1NumberForDisplayForSubscriber(
            GetDefaultSubscription(), alphaTag, number);
}

ECode CTelephonyManager::SetLine1NumberForDisplayForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& number)
{
    // try {
    return GetITelephony()->SetLine1NumberForDisplayForSubscriber(
            subId, alphaTag, number);
    // } catch (RemoteException ex) {
    // } catch (NullPointerException ex) {
    // }
}

ECode CTelephonyManager::GetLine1AlphaTag(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)
    return GetLine1AlphaTagForSubscriber(GetDefaultSubscription(), res);
}

ECode CTelephonyManager::GetLine1AlphaTagForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    String alphaTag;
    // try {
    GetITelephony()->GetLine1AlphaTagForDisplay(subId, &alphaTag);
    // } catch (RemoteException ex) {
    // } catch (NullPointerException ex) {
    // }
    if (!alphaTag.IsNull()) {
        *res = alphaTag;
        return NOERROR;
    }
    // try {
    return GetSubscriberInfo()->GetLine1AlphaTagForSubscriber(subId, res);
    // } catch (RemoteException ex) {
    //     return null;
    // } catch (NullPointerException ex) {
    //     // This could happen before phone restarts due to crashing
    //     return null;
    // }
}

ECode CTelephonyManager::GetMsisdn(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)
    return GetMsisdn(GetDefaultSubscription(), res);
}

ECode CTelephonyManager::GetMsisdn(
    /* [in] */ Int64 subId,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetSubscriberInfo()->GetMsisdnForSubscriber(subId, res);
    // } catch (RemoteException ex) {
    //     return null;
    // } catch (NullPointerException ex) {
    //     // This could happen before phone restarts due to crashing
    //     return null;
    // }
}

ECode CTelephonyManager::GetVoiceMailNumber(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)
    return GetVoiceMailNumber(GetDefaultSubscription(), res);
}

ECode CTelephonyManager::GetVoiceMailNumber(
    /* [in] */ Int64 subId,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetSubscriberInfo()->GetVoiceMailNumberForSubscriber(subId, res);
    // } catch (RemoteException ex) {
    //     return null;
    // } catch (NullPointerException ex) {
    //     // This could happen before phone restarts due to crashing
    //     return null;
    // }
}

ECode CTelephonyManager::GetCompleteVoiceMailNumber(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)
    return GetCompleteVoiceMailNumber(GetDefaultSubscription(), res);
}

ECode CTelephonyManager::GetCompleteVoiceMailNumber(
    /* [in] */ Int64 subId,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetSubscriberInfo()->GetCompleteVoiceMailNumberForSubscriber(subId, res);
    // } catch (RemoteException ex) {
    //     return null;
    // } catch (NullPointerException ex) {
    //     // This could happen before phone restarts due to crashing
    //     return null;
    // }
}

ECode CTelephonyManager::GetVoiceMessageCount(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    return GetVoiceMessageCount(GetDefaultSubscription(), res);
}

ECode CTelephonyManager::GetVoiceMessageCount(
    /* [in] */ Int64 subId,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->GetVoiceMessageCountForSubscriber(subId, res);
    // } catch (RemoteException ex) {
    //     return 0;
    // } catch (NullPointerException ex) {
    //     // This could happen before phone restarts due to crashing
    //     return 0;
    // }
}

ECode CTelephonyManager::GetVoiceMailAlphaTag(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)
    return GetVoiceMailAlphaTag(GetDefaultSubscription(), res);
}

ECode CTelephonyManager::GetVoiceMailAlphaTag(
    /* [in] */ Int64 subId,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetSubscriberInfo()->GetVoiceMailAlphaTagForSubscriber(subId, res);
    // } catch (RemoteException ex) {
    //     return null;
    // } catch (NullPointerException ex) {
    //     // This could happen before phone restarts due to crashing
    //     return null;
    // }
}

ECode CTelephonyManager::GetIsimImpi(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetSubscriberInfo()->GetIsimImpi(res);
    // } catch (RemoteException ex) {
    //     return null;
    // } catch (NullPointerException ex) {
    //     // This could happen before phone restarts due to crashing
    //     return null;
    // }
}

ECode CTelephonyManager::GetIsimDomain(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetSubscriberInfo()->GetIsimDomain(res);
    // } catch (RemoteException ex) {
    //     return null;
    // } catch (NullPointerException ex) {
    //     // This could happen before phone restarts due to crashing
    //     return null;
    // }
}

ECode CTelephonyManager::GetIsimImpu(
    /* [out, callee] */ ArrayOf<String>** res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetSubscriberInfo()->GetIsimImpu(res);
    // } catch (RemoteException ex) {
    //     return null;
    // } catch (NullPointerException ex) {
    //     // This could happen before phone restarts due to crashing
    //     return null;
    // }
}

ECode CTelephonyManager::GetCallState(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetTelecomService()->GetCallState(res);
    // } catch (RemoteException | NullPointerException e) {
    //     return CALL_STATE_IDLE;
    // }
}

ECode CTelephonyManager::GetCallState(
    /* [in] */ Int64 subId,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
        return GetITelephony()->GetCallStateForSubscriber(subId, res);
    // } catch (RemoteException ex) {
    //     // the phone process is restarting.
    //     return CALL_STATE_IDLE;
    // } catch (NullPointerException ex) {
    //   // the phone process is restarting.
    //   return CALL_STATE_IDLE;
    // }
}

ECode CTelephonyManager::GetDataActivity(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->GetDataActivity(res);
    // } catch (RemoteException ex) {
    //     // the phone process is restarting.
    //     return DATA_ACTIVITY_NONE;
    // } catch (NullPointerException ex) {
    //   // the phone process is restarting.
    //   return DATA_ACTIVITY_NONE;
    // }
}

ECode CTelephonyManager::GetDataState(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->GetDataState(res);
    // } catch (RemoteException ex) {
    //     // the phone process is restarting.
    //     return DATA_DISCONNECTED;
    // } catch (NullPointerException ex) {
    //     return DATA_DISCONNECTED;
    // }
}

ECode CTelephonyManager::Listen(
    /* [in] */ IPhoneStateListener* listener,
    /* [in] */ Int32 events)
{
    String pkgForDebug;
    if (mContext != NULL) {
        mContext->GetPackageName(&pkgForDebug);
    }
    else {
        pkgForDebug = "<unknown>";
    }

    // try {
    Boolean notifyNow = (GetITelephony() != NULL);
    PhoneStateListener* l = (PhoneStateListener*)listener;
    return sRegistry->ListenForSubscriber(l->mSubId, pkgForDebug, l->mCallback, events, notifyNow);
    // } catch (RemoteException ex) {
    //     // system process dead
    // } catch (NullPointerException ex) {
    //     // system process dead
    // }
}

ECode CTelephonyManager::GetCdmaEriIconIndex(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    return GetCdmaEriIconIndex(GetDefaultSubscription(), res);
}

ECode CTelephonyManager::GetCdmaEriIconIndex(
    /* [in] */ Int64 subId,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->GetCdmaEriIconIndexForSubscriber(subId, res);
    // } catch (RemoteException ex) {
    //     // the phone process is restarting.
    //     return -1;
    // } catch (NullPointerException ex) {
    //     return -1;
    // }
}

ECode CTelephonyManager::GetCdmaEriIconMode(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    return GetCdmaEriIconMode(GetDefaultSubscription(), res);
}

ECode CTelephonyManager::GetCdmaEriIconMode(
    /* [in] */ Int64 subId,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->GetCdmaEriIconModeForSubscriber(subId, res);
    // } catch (RemoteException ex) {
    //     // the phone process is restarting.
    //     return -1;
    // } catch (NullPointerException ex) {
    //     return -1;
    // }
}

ECode CTelephonyManager::GetCdmaEriText(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)
    return GetCdmaEriText(GetDefaultSubscription(), res);
}

ECode CTelephonyManager::GetCdmaEriText(
    /* [in] */ Int64 subId,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->GetCdmaEriTextForSubscriber(subId, res);
    // } catch (RemoteException ex) {
    //     // the phone process is restarting.
    //     return null;
    // } catch (NullPointerException ex) {
    //     return null;
    // }
}

ECode CTelephonyManager::IsVoiceCapable(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    if (mContext == NULL) {
        *res = TRUE;
        return NOERROR;
    }

    AutoPtr<IResources> r;
    mContext->GetResources((IResources**)&r);
    return r->GetBoolean(R::bool_::config_voice_capable, res);
}

ECode CTelephonyManager::IsSmsCapable(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    if (mContext == NULL) {
        *res = TRUE;
        return NOERROR;
    }

    AutoPtr<IResources> r;
    mContext->GetResources((IResources**)&r);
    return r->GetBoolean(R::bool_::config_sms_capable, res);
}

ECode CTelephonyManager::GetAllCellInfo(
    /* [out] */ IList** res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->GetAllCellInfo(res);
    // } catch (RemoteException ex) {
    //     return null;
    // } catch (NullPointerException ex) {
    //     return null;
    // }
}

ECode CTelephonyManager::SetCellInfoListRate(
    /* [in] */ Int32 rateInMillis)
{
    // try {
    return GetITelephony()->SetCellInfoListRate(rateInMillis);
    // } catch (RemoteException ex) {
    // } catch (NullPointerException ex) {
    // }
}

ECode CTelephonyManager::GetMmsUserAgent(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)
    if (mContext == NULL) {
        *res = String(NULL);
        return NOERROR;
    }

    AutoPtr<IResources> r;
    mContext->GetResources((IResources**)&r);
    return r->GetString(R::string::config_mms_user_agent, res);
}

ECode CTelephonyManager::GetMmsUAProfUrl(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)
    if (mContext == NULL) {
        *res = String(NULL);
        return NOERROR;
    }

    AutoPtr<IResources> r;
    mContext->GetResources((IResources**)&r);
    return r->GetString(R::string::config_mms_user_agent_profile_url, res);
}

ECode CTelephonyManager::IccOpenLogicalChannel(
    /* [in] */ const String& AID,
    /* [out] */ IIccOpenLogicalChannelResponse** res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->IccOpenLogicalChannel(AID, res);
    // } catch (RemoteException ex) {
    // } catch (NullPointerException ex) {
    // }
}

ECode CTelephonyManager::IccCloseLogicalChannel(
    /* [in] */ Int32 channel,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->IccCloseLogicalChannel(channel, res);
    // } catch (RemoteException ex) {
    // } catch (NullPointerException ex) {
    // }
}

ECode CTelephonyManager::IccTransmitApduLogicalChannel(
    /* [in] */ Int32 channel,
    /* [in] */ Int32 cla,
    /* [in] */ Int32 instruction,
    /* [in] */ Int32 p1,
    /* [in] */ Int32 p2,
    /* [in] */ Int32 p3,
    /* [in] */ const String& data,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->IccTransmitApduLogicalChannel(channel, cla,
            instruction, p1, p2, p3, data, res);
    // } catch (RemoteException ex) {
    // } catch (NullPointerException ex) {
    // }
}

ECode CTelephonyManager::IccTransmitApduBasicChannel(
    /* [in] */ Int32 cla,
    /* [in] */ Int32 instruction,
    /* [in] */ Int32 p1,
    /* [in] */ Int32 p2,
    /* [in] */ Int32 p3,
    /* [in] */ const String& data,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->IccTransmitApduBasicChannel(cla,
            instruction, p1, p2, p3, data, res);
    // } catch (RemoteException ex) {
    // } catch (NullPointerException ex) {
    // }
}

ECode CTelephonyManager::IccExchangeSimIO(
    /* [in] */ Int32 fileID,
    /* [in] */ Int32 command,
    /* [in] */ Int32 p1,
    /* [in] */ Int32 p2,
    /* [in] */ Int32 p3,
    /* [in] */ const String& filePath,
    /* [out, callee] */ ArrayOf<Byte>** res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->IccExchangeSimIO(fileID, command, p1, p2,
        p3, filePath, res);
    // } catch (RemoteException ex) {
    // } catch (NullPointerException ex) {
    // }
}

ECode CTelephonyManager::SendEnvelopeWithStatus(
    /* [in] */ const String& content,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->SendEnvelopeWithStatus(content, res);
    // } catch (RemoteException ex) {
    // } catch (NullPointerException ex) {
    // }
}

ECode CTelephonyManager::NvReadItem(
    /* [in] */ Int32 itemID,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->NvReadItem(itemID, res);
    // } catch (RemoteException ex) {
    //     Rlog.e(TAG, "nvReadItem RemoteException", ex);
    // } catch (NullPointerException ex) {
    //     Rlog.e(TAG, "nvReadItem NPE", ex);
    // }
}

ECode CTelephonyManager::NvWriteItem(
    /* [in] */ Int32 itemID,
    /* [in] */ const String& itemValue,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->NvWriteItem(itemID, itemValue, res);
    // } catch (RemoteException ex) {
    //     Rlog.e(TAG, "nvWriteItem RemoteException", ex);
    // } catch (NullPointerException ex) {
    //     Rlog.e(TAG, "nvWriteItem NPE", ex);
    // }
}

ECode CTelephonyManager::NvWriteCdmaPrl(
    /* [in] */ ArrayOf<Byte>* preferredRoamingList,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->NvWriteCdmaPrl(preferredRoamingList, res);
    // } catch (RemoteException ex) {
    //     Rlog.e(TAG, "nvWriteCdmaPrl RemoteException", ex);
    // } catch (NullPointerException ex) {
    //     Rlog.e(TAG, "nvWriteCdmaPrl NPE", ex);
    // }
}

ECode CTelephonyManager::NvResetConfig(
    /* [in] */ Int32 resetType,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->NvResetConfig(resetType, res);
    // } catch (RemoteException ex) {
    //     Rlog.e(TAG, "nvResetConfig RemoteException", ex);
    // } catch (NullPointerException ex) {
    //     Rlog.e(TAG, "nvResetConfig NPE", ex);
    // }
}

ECode CTelephonyManager::GetDefaultSim(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    //TODO Need to get it from Telephony Devcontroller
    *res = 0;
    return NOERROR;
}

ECode CTelephonyManager::GetSimCount(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    Boolean b;
    if(IsMultiSimEnabled(&b), b) {
    //TODO Need to get it from Telephony Devcontroller
        *res = 2;
    }
    else {
       *res = 1;
    }
    return NOERROR;
}

ECode CTelephonyManager::GetIsimIst(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetSubscriberInfo()->GetIsimIst(res);
    // } catch (RemoteException ex) {
    //     return null;
    // } catch (NullPointerException ex) {
    //     // This could happen before phone restarts due to crashing
    //     return null;
    // }
}

ECode CTelephonyManager::GetIsimPcscf(
    /* [out, callee] */ ArrayOf<String>** res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetSubscriberInfo()->GetIsimPcscf(res);
    // } catch (RemoteException ex) {
    //     return null;
    // } catch (NullPointerException ex) {
    //     // This could happen before phone restarts due to crashing
    //     return null;
    // }
}


ECode CTelephonyManager::GetIsimChallengeResponse(
    /* [in] */ const String& nonce,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetSubscriberInfo()->GetIsimChallengeResponse(nonce, res);
    // } catch (RemoteException ex) {
    //     return null;
    // } catch (NullPointerException ex) {
    //     // This could happen before phone restarts due to crashing
    //     return null;
    // }
}

ECode CTelephonyManager::GetIccSimChallengeResponse(
    /* [in] */ Int64 subId,
    /* [in] */ Int32 appType,
    /* [in] */ const String& data,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetSubscriberInfo()->GetIccSimChallengeResponse(subId, appType, data, res);
    // } catch (RemoteException ex) {
    //     return null;
    // } catch (NullPointerException ex) {
    //     // This could happen before phone starts
    //     return null;
    // }
}

ECode CTelephonyManager::GetIccSimChallengeResponse(
    /* [in] */ Int32 appType,
    /* [in] */ const String& data,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)
    return GetIccSimChallengeResponse(GetDefaultSubscription(), appType, data, res);
}

ECode CTelephonyManager::GetPcscfAddress(
    /* [in] */ const String& appType,
    /* [out, callee] */ ArrayOf<String>** res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->GetPcscfAddress(appType, res);
    // } catch (RemoteException e) {
    //     return new String[0];
    // }
}

ECode CTelephonyManager::SetImsRegistrationState(
    /* [in] */ Boolean registered)
{
    // try {
    return GetITelephony()->SetImsRegistrationState(registered);
    // } catch (RemoteException e) {
    // }
}

ECode CTelephonyManager::GetPreferredNetworkType(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->GetPreferredNetworkType(res);
    // } catch (RemoteException ex) {
    //     Rlog.e(TAG, "getPreferredNetworkType RemoteException", ex);
    // } catch (NullPointerException ex) {
    //     Rlog.e(TAG, "getPreferredNetworkType NPE", ex);
    // }
}

ECode CTelephonyManager::SetPreferredNetworkType(
    /* [in] */ Int32 networkType,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->SetPreferredNetworkType(networkType, res);
    // } catch (RemoteException ex) {
    //     Rlog.e(TAG, "setPreferredNetworkType RemoteException", ex);
    // } catch (NullPointerException ex) {
    //     Rlog.e(TAG, "setPreferredNetworkType NPE", ex);
    // }
}

ECode CTelephonyManager::SetGlobalPreferredNetworkType(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    return SetPreferredNetworkType(IRILConstants::NETWORK_MODE_LTE_CDMA_EVDO_GSM_WCDMA, res);
}

ECode CTelephonyManager::HasCarrierPrivileges(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->HasCarrierPrivileges(res);
    // } catch (RemoteException ex) {
    //     Rlog.e(TAG, "hasCarrierPrivileges RemoteException", ex);
    // } catch (NullPointerException ex) {
    //     Rlog.e(TAG, "hasCarrierPrivileges NPE", ex);
    // }
    // return CARRIER_PRIVILEGE_STATUS_NO_ACCESS;
}

ECode CTelephonyManager::SetOperatorBrandOverride(
    /* [in] */ const String& brand,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->SetOperatorBrandOverride(brand, res);
    // } catch (RemoteException ex) {
    //     Rlog.e(TAG, "setOperatorBrandOverride RemoteException", ex);
    // } catch (NullPointerException ex) {
    //     Rlog.e(TAG, "setOperatorBrandOverride NPE", ex);
    // }
    // return false;
}

ECode CTelephonyManager::GetCdmaMdn(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)
    return GetCdmaMdn(GetDefaultSubscription(), res);
}

ECode CTelephonyManager::GetCdmaMdn(
    /* [in] */ Int64 subId,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->GetCdmaMdn(subId, res);
    // } catch (RemoteException ex) {
    //     return null;
    // } catch (NullPointerException ex) {
    //     return null;
    // }
}

ECode CTelephonyManager::GetCdmaMin(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)
    return GetCdmaMin(GetDefaultSubscription(), res);
}

ECode CTelephonyManager::GetCdmaMin(
    /* [in] */ Int64 subId,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->GetCdmaMin(subId, res);
    // } catch (RemoteException ex) {
    //     return null;
    // } catch (NullPointerException ex) {
    //     return null;
    // }
}

ECode CTelephonyManager::CheckCarrierPrivilegesForPackage(
    /* [in] */ const String& pkgname,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->CheckCarrierPrivilegesForPackage(pkgname, res);
    // } catch (RemoteException ex) {
    //     Rlog.e(TAG, "hasCarrierPrivileges RemoteException", ex);
    // } catch (NullPointerException ex) {
    //     Rlog.e(TAG, "hasCarrierPrivileges NPE", ex);
    // }
    // return CARRIER_PRIVILEGE_STATUS_NO_ACCESS;
}

ECode CTelephonyManager::GetCarrierPackageNamesForIntent(
    /* [in] */ IIntent* intent,
    /* [out] */ IList** res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->GetCarrierPackageNamesForIntent(intent, res);
    // } catch (RemoteException ex) {
    //     Rlog.e(TAG, "getCarrierPackageNamesForIntent RemoteException", ex);
    // } catch (NullPointerException ex) {
    //     Rlog.e(TAG, "getCarrierPackageNamesForIntent NPE", ex);
    // }
    // return NULL;
}

ECode CTelephonyManager::Dial(
    /* [in] */ const String& number)
{
    // try {
    return GetITelephony()->Dial(number);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Error calling ITelephony#dial", e);
    // }
}

ECode CTelephonyManager::Call(
    /* [in] */ const String& callingPackage,
    /* [in] */ const String& number)
{
    // try {
    return GetITelephony()->Call(callingPackage, number);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Error calling ITelephony#call", e);
    // }
}

ECode CTelephonyManager::EndCall(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->EndCall(res);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Error calling ITelephony#endCall", e);
    // }
    // return FALSE;
}

ECode CTelephonyManager::AnswerRingingCall()
{
    // try {
    return GetITelephony()->AnswerRingingCall();
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Error calling ITelephony#answerRingingCall", e);
    // }
}

ECode CTelephonyManager::SilenceRinger()
{
    // try {
    return GetTelecomService()->SilenceRinger();
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Error calling ITelecomService#silenceRinger", e);
    // }
}

ECode CTelephonyManager::IsOffhook(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->IsOffhook(res);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Error calling ITelephony#isOffhook", e);
    // }
    // return false;
}

ECode CTelephonyManager::IsRinging(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->IsRinging(res);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Error calling ITelephony#isRinging", e);
    // }
    // return false;
}

ECode CTelephonyManager::IsIdle(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->IsIdle(res);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Error calling ITelephony#isIdle", e);
    // }
    // return true;
}

ECode CTelephonyManager::IsRadioOn(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->IsRadioOn(res);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Error calling ITelephony#isRadioOn", e);
    // }
    // return false;
}

ECode CTelephonyManager::IsSimPinEnabled(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->IsSimPinEnabled(res);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Error calling ITelephony#isSimPinEnabled", e);
    // }
    // return false;
}

ECode CTelephonyManager::SupplyPin(
    /* [in] */ const String& pin,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->SupplyPin(pin, res);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Error calling ITelephony#supplyPin", e);
    // }
    // return false;
}

ECode CTelephonyManager::SupplyPuk(
    /* [in] */ const String& puk,
    /* [in] */ const String& pin,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->SupplyPuk(puk, pin, res);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Error calling ITelephony#supplyPuk", e);
    // }
    // return false;
}

ECode CTelephonyManager::SupplyPinReportResult(
    /* [in] */ const String& pin,
    /* [out, callee] */ ArrayOf<Int32>** res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->SupplyPinReportResult(pin, res);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Error calling ITelephony#supplyPinReportResult", e);
    // }
    // return new int[0];
}

ECode CTelephonyManager::SupplyPukReportResult(
    /* [in] */ const String& puk,
    /* [in] */ const String& pin,
    /* [out, callee] */ ArrayOf<Int32>** res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->SupplyPukReportResult(puk, pin, res);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Error calling ITelephony#]", e);
    // }
    // return new int[0];
}

ECode CTelephonyManager::HandlePinMmi(
    /* [in] */ const String& dialString,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->HandlePinMmi(dialString, res);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Error calling ITelephony#handlePinMmi", e);
    // }
    // return false;
}

ECode CTelephonyManager::ToggleRadioOnOff()
{
    // try {
    return GetITelephony()->ToggleRadioOnOff();
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Error calling ITelephony#toggleRadioOnOff", e);
    // }
}

ECode CTelephonyManager::SetRadio(
    /* [in] */ Boolean turnOn,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->SetRadio(turnOn, res);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Error calling ITelephony#setRadio", e);
    // }
    // return false;
}

ECode CTelephonyManager::SetRadioPower(
    /* [in] */ Boolean turnOn,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->SetRadioPower(turnOn, res);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Error calling ITelephony#setRadioPower", e);
    // }
    // return false;
}

ECode CTelephonyManager::UpdateServiceLocation()
{
    // try {
    return GetITelephony()->UpdateServiceLocation();
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Error calling ITelephony#updateServiceLocation", e);
    // }
}

ECode CTelephonyManager::EnableDataConnectivity(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->EnableDataConnectivity(res);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Error calling ITelephony#enableDataConnectivity", e);
    // }
    // return false;
}

ECode CTelephonyManager::DisableDataConnectivity(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->DisableDataConnectivity(res);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Error calling ITelephony#disableDataConnectivity", e);
    // }
    // return false;
}

ECode CTelephonyManager::IsDataConnectivityPossible(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->IsDataConnectivityPossible(res);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Error calling ITelephony#isDataConnectivityPossible", e);
    // }
    // return false;
}

ECode CTelephonyManager::NeedsOtaServiceProvisioning(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->NeedsOtaServiceProvisioning(res);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Error calling ITelephony#needsOtaServiceProvisioning", e);
    // }
    // return false;
}

ECode CTelephonyManager::SetDataEnabled(
    /* [in] */ Boolean enable)
{
    // try {
    return GetITelephony()->SetDataEnabled(enable);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Error calling ITelephony#setDataEnabled", e);
    // }
}

ECode CTelephonyManager::GetDataEnabled(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->GetDataEnabled(res);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Error calling ITelephony#getDataEnabled", e);
    // }
    // return false;
}

ECode CTelephonyManager::EnableSimplifiedNetworkSettings(
    /* [in] */ Boolean enable)
{
    return EnableSimplifiedNetworkSettingsForSubscriber(GetDefaultSubscription(), enable);
}

ECode CTelephonyManager::EnableSimplifiedNetworkSettingsForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ Boolean enable)
{
    // try {
    return GetITelephony()->EnableSimplifiedNetworkSettingsForSubscriber(subId, enable);
    // } catch (RemoteException ex) {
    // } catch (NullPointerException ex) {
    // }
}

ECode CTelephonyManager::GetSimplifiedNetworkSettingsEnabled(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    return GetSimplifiedNetworkSettingsEnabledForSubscriber(GetDefaultSubscription(), res);
}

ECode CTelephonyManager::GetSimplifiedNetworkSettingsEnabledForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->GetSimplifiedNetworkSettingsEnabledForSubscriber(subId, res);
    // } catch (RemoteException ex) {
    // } catch (NullPointerException ex) {
    // }
    // return false;
}

ECode CTelephonyManager::InvokeOemRilRequestRaw(
    /* [in] */ ArrayOf<Byte>* oemReq,
    /* [out, callee] */ ArrayOf<Byte>** oemResp,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    // try {
    return GetITelephony()->InvokeOemRilRequestRaw(oemReq, oemResp, res);
    // } catch (RemoteException ex) {
    // } catch (NullPointerException ex) {
    // }
    // return -1;
}

ECode CTelephonyManager::GetDefault(
    /* [out] */ ITelephonyManager** res)
{
    VALIDATE_NOT_NULL(res)
    *res = sInstance;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode CTelephonyManager::From(
    /* [in] */ IContext* context,
    /* [out] */ ITelephonyManager** res)
{
    VALIDATE_NOT_NULL(res)

    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::TELEPHONY_SERVICE, ((IInterface**)&service));
    *res = ITelephonyManager::Probe(service);
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode CTelephonyManager::GetPhoneType(
    /* [in] */ Int32 networkMode,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    switch(networkMode) {
    case IRILConstants::NETWORK_MODE_CDMA:
    case IRILConstants::NETWORK_MODE_CDMA_NO_EVDO:
    case IRILConstants::NETWORK_MODE_EVDO_NO_CDMA:
        *res = IPhoneConstants::PHONE_TYPE_CDMA;
        return NOERROR;

    case IRILConstants::NETWORK_MODE_WCDMA_PREF:
    case IRILConstants::NETWORK_MODE_GSM_ONLY:
    case IRILConstants::NETWORK_MODE_WCDMA_ONLY:
    case IRILConstants::NETWORK_MODE_GSM_UMTS:
    case IRILConstants::NETWORK_MODE_LTE_GSM_WCDMA:
    case IRILConstants::NETWORK_MODE_LTE_WCDMA:
    case IRILConstants::NETWORK_MODE_LTE_CDMA_EVDO_GSM_WCDMA:
        *res = IPhoneConstants::PHONE_TYPE_GSM;
        return NOERROR;

    // Use CDMA Phone for the global mode including CDMA
    case IRILConstants::NETWORK_MODE_GLOBAL:
    case IRILConstants::NETWORK_MODE_LTE_CDMA_EVDO:
        *res = IPhoneConstants::PHONE_TYPE_CDMA;
        return NOERROR;

    case IRILConstants::NETWORK_MODE_LTE_ONLY: {
        Int32 val;
        GetLteOnCdmaModeStatic(&val);
        if (val == IPhoneConstants::LTE_ON_CDMA_TRUE) {
            *res = IPhoneConstants::PHONE_TYPE_CDMA;
            return NOERROR;
        }
        else {
            *res = IPhoneConstants::PHONE_TYPE_GSM;
            return NOERROR;
        }
    }
    default:
        *res = IPhoneConstants::PHONE_TYPE_GSM;
    }
    return NOERROR;
}

ECode CTelephonyManager::GetLteOnCdmaModeStatic(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    Int32 retVal;
    Int32 curVal;
    String productType;

    SystemProperties::GetInt32(ITelephonyProperties::PROPERTY_LTE_ON_CDMA_DEVICE,
            IPhoneConstants::LTE_ON_CDMA_UNKNOWN, &curVal);
    retVal = curVal;

    if (retVal == IPhoneConstants::LTE_ON_CDMA_UNKNOWN) {
        AutoPtr<IMatcher> matcher;
        sProductTypePattern->Matcher(sKernelCmdLine, (IMatcher**)&matcher);

        Boolean b;
        if (matcher->Find(&b), b) {
            IMatchResult::Probe(matcher)->Group(1, &productType);

            if (sLteOnCdmaProductType.Equals(productType)) {
                retVal = IPhoneConstants::LTE_ON_CDMA_TRUE;
            }
            else {
                retVal = IPhoneConstants::LTE_ON_CDMA_FALSE;
            }
        }
        else {
            retVal = IPhoneConstants::LTE_ON_CDMA_FALSE;
        }
    }

    Logger::D(TAG, "getLteOnCdmaMode=%d curVal=%d product_type='%s' lteOnCdmaProductType='%s'",
            retVal, curVal, productType.string(), sLteOnCdmaProductType.string());
    *res = retVal;
    return NOERROR;
}

ECode CTelephonyManager::GetNetworkClass(
    /* [in] */ Int32 networkType,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)

    switch (networkType) {
        case NETWORK_TYPE_GPRS:
        case NETWORK_TYPE_GSM:
        case NETWORK_TYPE_EDGE:
        case NETWORK_TYPE_CDMA:
        case NETWORK_TYPE_1xRTT:
        case NETWORK_TYPE_IDEN:
            *res = NETWORK_CLASS_2_G;
            return NOERROR;
        case NETWORK_TYPE_UMTS:
        case NETWORK_TYPE_EVDO_0:
        case NETWORK_TYPE_EVDO_A:
        case NETWORK_TYPE_HSDPA:
        case NETWORK_TYPE_HSUPA:
        case NETWORK_TYPE_HSPA:
        case NETWORK_TYPE_EVDO_B:
        case NETWORK_TYPE_EHRPD:
        case NETWORK_TYPE_HSPAP:
            *res = NETWORK_CLASS_3_G;
            return NOERROR;
        case NETWORK_TYPE_LTE:
            *res = NETWORK_CLASS_4_G;
            return NOERROR;
        default:
            *res = NETWORK_CLASS_UNKNOWN;
    }
    return NOERROR;
}

ECode CTelephonyManager::GetNetworkTypeName(
    /* [in] */ Int32 type,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    switch (type) {
        case NETWORK_TYPE_GPRS:
            *res = "GPRS";
            return NOERROR;
        case NETWORK_TYPE_EDGE:
            *res = "EDGE";
            return NOERROR;
        case NETWORK_TYPE_UMTS:
            *res = "UMTS";
            return NOERROR;
        case NETWORK_TYPE_HSDPA:
            *res = "HSDPA";
            return NOERROR;
        case NETWORK_TYPE_HSUPA:
            *res = "HSUPA";
            return NOERROR;
        case NETWORK_TYPE_HSPA:
            *res = "HSPA";
            return NOERROR;
        case NETWORK_TYPE_CDMA:
            *res = "CDMA";
            return NOERROR;
        case NETWORK_TYPE_EVDO_0:
            *res = "CDMA - EvDo rev. 0";
            return NOERROR;
        case NETWORK_TYPE_EVDO_A:
            *res = "CDMA - EvDo rev. A";
            return NOERROR;
        case NETWORK_TYPE_EVDO_B:
            *res = "CDMA - EvDo rev. B";
            return NOERROR;
        case NETWORK_TYPE_1xRTT:
            *res = "CDMA - 1xRTT";
            return NOERROR;
        case NETWORK_TYPE_LTE:
            *res = "LTE";
            return NOERROR;
        case NETWORK_TYPE_EHRPD:
            *res = "CDMA - eHRPD";
            return NOERROR;
        case NETWORK_TYPE_IDEN:
            *res = "iDEN";
            return NOERROR;
        case NETWORK_TYPE_HSPAP:
            *res = "HSPA+";
            return NOERROR;
        case NETWORK_TYPE_GSM:
            *res = "GSM";
            return NOERROR;
        default:
            *res = "UNKNOWN";
    }
    return NOERROR;
}

ECode CTelephonyManager::SetTelephonyProperty(
    /* [in] */ const String& property,
    /* [in] */ Int64 subId,
    /* [in] */ const String& _value)
{
    String propVal;
    AutoPtr<ArrayOf<String> > p;
    String prop;
    SystemProperties::Get(property, &prop);
    Int32 phoneId;
    SubscriptionManager::GetPhoneId(subId, &phoneId);

    String value = _value;
    if (value == NULL) {
        value = "";
    }

    if (prop != NULL) {
        StringUtils::Split(prop, String(","), (ArrayOf<String>**)&p);
    }

    if (phoneId < 0) return NOERROR;

    for (Int32 i = 0; i < phoneId; i++) {
        String str("");
        if ((p != NULL) && (i < p->GetLength())) {
            str = (*p)[i];
        }
        propVal = propVal + str + ",";
    }

    propVal = propVal + value;
    if (p != NULL) {
        for (Int32 i = phoneId + 1; i < p->GetLength(); i++) {
            propVal = propVal + "," + (*p)[i];
        }
    }

    // TODO: workaround for QC
    if (property.GetLength() > ISystemProperties::PROP_NAME_MAX ||
            propVal.GetLength() > ISystemProperties::PROP_VALUE_MAX) {
        Logger::D(TAG, "setTelephonyProperty length too long:%s, %s",
                property.string(), propVal.string());
        return NOERROR;
    }

    Logger::D(TAG, "setTelephonyProperty property=%s propVal=%s",
            property.string(), propVal.string());

    return SystemProperties::Set(property, propVal);
}

ECode CTelephonyManager::GetInt32AtIndex(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& name,
    /* [in] */ Int32 index,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    *res = 0;

    String v;
    Settings::Global::GetString(resolver, name, &v);

    if (v != NULL) {
        AutoPtr<ArrayOf<String> > valArray;
        StringUtils::Split(v, String(","), (ArrayOf<String>**)&valArray);

        if ((index >= 0) && (index < valArray->GetLength()) && ((*valArray)[index] != NULL)) {
            // try {
            return StringUtils::Parse((*valArray)[index], res);
            // } catch (NumberFormatException e) {
            //     //Log.e(TAG, "Exception while parsing Integer: ", e);
            // }
        }
    }
    // throw new android.provider.Settings.SettingNotFoundException(name);
    return E_SETTING_NOT_FOUND_EXCEPTION;
}

ECode CTelephonyManager::PutInt32AtIndex(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& name,
    /* [in] */ Int32 index,
    /* [in] */ Int32 value,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    String data("");
    AutoPtr<ArrayOf<String> > valArray;
    String v;
    Settings::Global::GetString(resolver, name, &v);

    if (v != NULL) {
        StringUtils::Split(v, String(","), (ArrayOf<String>**)&valArray);
    }

    // Copy the elements from valArray till index
    for (Int32 i = 0; i < index; i++) {
        String str("");
        if ((valArray != NULL) && (i < valArray->GetLength())) {
            str = (*valArray)[i];
        }
        data = data + str + ",";
    }

    data = data + value;

    // Copy the remaining elements from valArray if any.
    if (valArray != NULL) {
        for (Int32 i = index+1; i < valArray->GetLength(); i++) {
            data = data + "," + (*valArray)[i];
        }
    }
    return Settings::Global::PutString(resolver, name, data, res);
}

ECode CTelephonyManager::GetTelephonyProperty(
    /* [in] */ const String& property,
    /* [in] */ Int64 subId,
    /* [in] */ const String& defaultVal,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    String propVal;
    Int32 phoneId;
    SubscriptionManager::GetPhoneId(subId, &phoneId);
    String prop;
    SystemProperties::Get(property, &prop);

    if ((prop != NULL) && (prop.GetLength() > 0)) {
        AutoPtr<ArrayOf<String> > values;
        StringUtils::Split(prop, String(","), (ArrayOf<String>**)&values);

        if ((phoneId >= 0) && (phoneId < values->GetLength()) && (!(*values)[phoneId].IsNull())) {
            propVal = (*values)[phoneId];
        }
    }
    *res = propVal == NULL ? defaultVal : propVal;
    return NOERROR;
}

ECode CTelephonyManager::NetworkTypeToString(
    /* [in] */ Int32 networkType,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String(NULL);

    String ratClassName("");
    Int32 networkClass;
    GetNetworkClass(networkType, &networkClass);
    if (mContext == NULL) return NOERROR;

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    switch (networkClass) {
        case ITelephonyManager::NETWORK_CLASS_2_G:
            res->GetString(R::string::config_rat_2g, &ratClassName);
            break;
        case ITelephonyManager::NETWORK_CLASS_3_G:
            res->GetString(R::string::config_rat_3g, &ratClassName);
             break;
        case ITelephonyManager::NETWORK_CLASS_4_G:
            res->GetString(R::string::config_rat_4g, &ratClassName);
            break;
        default:
            ratClassName = "";
            break;
    }
    *str = ratClassName;
    return NOERROR;
}

Int32 CTelephonyManager::GetPhoneTypeFromProperty()
{
    return GetPhoneTypeFromProperty(GetDefaultSubscription());
}

Int32 CTelephonyManager::GetPhoneTypeFromProperty(
    /* [in] */ Int64 subId)
{
    String type;
    GetTelephonyProperty(ITelephonyProperties::CURRENT_ACTIVE_PHONE, subId, String(NULL), &type);
    if (!type.IsNull()) {
        Int32 val;
        StringUtils::Parse(type, &val);
        return val;
    }
    else {
        return GetPhoneTypeFromNetworkType(subId);
    }
}

Int32 CTelephonyManager::GetPhoneTypeFromNetworkType()
{
    return GetPhoneTypeFromNetworkType(GetDefaultSubscription());
}

Int32 CTelephonyManager::GetPhoneTypeFromNetworkType(
    /* [in] */ Int64 subId)
{
    // When the system property CURRENT_ACTIVE_PHONE, has not been set,
    // use the system property for default network type.
    // This is a fail safe, and can only happen at first boot.
    String mode;
    GetTelephonyProperty(String("ro.telephony.default_network"), subId, String(NULL), &mode);

    if (mode != NULL) {
        Int32 val;
        StringUtils::Parse(mode, &val);
        Int32 result;
        CTelephonyManager::GetPhoneType(val, &result);
        return result;
    }
    return ITelephonyManager::PHONE_TYPE_NONE;
}

String CTelephonyManager::GetProcCmdLine()
{
    String cmdline;
    AutoPtr<IFileInputStream> is;

    ECode ec;
    do {
        ec = CFileInputStream::New(String("/proc/cmdline"), (IFileInputStream**)&is);
        if (FAILED(ec))
            break;
        AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(2048);

        Int32 count;
        ec = IInputStream::Probe(is)->Read(buffer, &count);
        if (FAILED(ec))
            break;
        if (count > 0) {
            cmdline = String(*buffer, 0, count);
        }
    } while (0);

    if (FAILED(ec)) {
        Logger::D(TAG, "No /proc/cmdline exception=");
    }

    if (is != NULL) {
        ICloseable::Probe(is)->Close();
    }

    Logger::D(TAG, "/proc/cmdline=%s", cmdline.string());
    return cmdline;
}

AutoPtr<IIPhoneSubInfo> CTelephonyManager::GetSubscriberInfo()
{
    // get it each time because that process crashes a lot
    AutoPtr<IInterface> b = ServiceManager::GetService(String("iphonesubinfo"));
    AutoPtr<IIPhoneSubInfo> svc = IIPhoneSubInfo::Probe(b);
    return svc;
}

AutoPtr<IITelephony> CTelephonyManager::GetITelephony()
{
    AutoPtr<IInterface> b = ServiceManager::GetService(IContext::TELEPHONY_SERVICE);
    AutoPtr<IITelephony> svc = IITelephony::Probe(b);
    return svc;
}

AutoPtr<IITelecomService> CTelephonyManager::GetTelecomService()
{
    AutoPtr<IInterface> b = ServiceManager::GetService(IContext::TELECOM_SERVICE);
    AutoPtr<IITelecomService> svc = IITelecomService::Probe(b);
    return svc;
}

Int64 CTelephonyManager::GetDefaultSubscription()
{
    Int64 val;
    SubscriptionManager::GetDefaultSubId(&val);
    return val;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos
