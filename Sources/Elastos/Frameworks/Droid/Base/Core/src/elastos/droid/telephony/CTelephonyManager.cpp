
#include "CTelephonyManager.h"
#include "CCellLocationHelper.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/os/CSystemProperties.h"
#include "elastos/droid/R.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/Math.h>

using Elastos::Utility::CRandom;
using Elastos::Core::IRandom;
using Elastos::Core::StringUtils;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::IIRILConstants;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Internal::Telephony::IIPhoneSubInfo;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IFileInputStream;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Regex::CPatternHelper;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::IPatternHelper;

namespace Elastos {
namespace Droid {
namespace Telephony {

const String CTelephonyManager::TAG("CTelephonyManager");
String CTelephonyManager::mImei("");
const String CTelephonyManager::sKernelCmdLine = GetProcCmdLine();

static String InitsLteOnCdmaProductType() {
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    String result;
    sp->Get(ITelephonyProperties::PROPERTY_LTE_ON_CDMA_PRODUCT_TYPE, String(""), &result);
    return result;
}

static AutoPtr<IPattern> InitsProductTypePattern() {
    AutoPtr<IPattern> pattern;
    AutoPtr<IPatternHelper> helper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
    helper->Compile(String("\\sproduct_type\\s*=\\s*(\\w+)"), (IPattern**)&pattern);
    return pattern;
}

AutoPtr<ITelephonyManager> CTelephonyManager::sInstance;
AutoPtr<IITelephonyRegistry> CTelephonyManager::sRegistry;
AutoPtr<IContext> CTelephonyManager::sContext;
const String CTelephonyManager::sLteOnCdmaProductType = InitsLteOnCdmaProductType();
const AutoPtr<IPattern> CTelephonyManager::sProductTypePattern = InitsProductTypePattern();

CTelephonyManager::CTelephonyManager()
{}

ECode CTelephonyManager::GetDefault(
        /* [out] */ ITelephonyManager** tm)
{
    VALIDATE_NOT_NULL(tm);

    if(sInstance == NULL) {
        CTelephonyManager::New((ITelephonyManager**)&sInstance);
    }
    *tm = CTelephonyManager::sInstance;
    REFCOUNT_ADD(*tm);
    return NOERROR;
}

ECode CTelephonyManager::GetDeviceSoftwareVersion(
    /* [out] */ String* res)
{
    /*try {
        return getSubscriberInfo().getDeviceSvn();
    } catch (RemoteException ex) {
        return null;
    } catch (NullPointerException ex) {
        return null;
    }*/
    *res = String(NULL);
    AutoPtr<IIPhoneSubInfo> ps;
    GetSubscriberInfo((IIPhoneSubInfo**)&ps);
    return ps->GetDeviceSvn(res);
}

ECode CTelephonyManager::GetDeviceId(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);

    /**
     * Create a random and valid imei code
     */
    if (!mImei.IsNull()) {
        *res = mImei;
        return NOERROR;
    }
    else {
        AutoPtr <IRandom> value;
        CRandom::New((IRandom**)&value);
        mImei = String("35");
        Int32 tmp;
        for (Int32 i = 0; i < (IMEI_LEN - 2); i++)
        mImei += StringUtils::ToString((Elastos::Core::Math::Abs((value->NextInt32(&tmp), tmp)) % 10));
    }
    Slogger::D(TAG, "IMEI: %s", mImei.string());
    *res = mImei;
    return NOERROR;
}

ECode CTelephonyManager::GetCellLocation(
    /* [out] */ ICellLocation** res)
{
    VALIDATE_NOT_NULL(res);
    //try {
    AutoPtr<IBundle> bundle;
    AutoPtr<IITelephony> iit;
    GetITelephony((IITelephony**)&iit);
    iit->GetCellLocation((IBundle**)&bundle);
    Boolean isEmpty;
    if (bundle->IsEmpty(&isEmpty), isEmpty) {
        *res = NULL;
        return NOERROR;
    }
    AutoPtr<ICellLocation> cl;
    AutoPtr<ICellLocationHelper> helper;
    CCellLocationHelper::AcquireSingleton((ICellLocationHelper**)&helper);
    helper->NewFromBundle(bundle, (ICellLocation**)&cl);
    if (cl->IsEmpty(&isEmpty), isEmpty) {
        *res = NULL;
        return NOERROR;
    }
    *res = cl;
    REFCOUNT_ADD(*res);
    // } catch (RemoteException ex) {
    //     return null;
    // } catch (NullPointerException ex) {
    //     return null;
    // }

    return NOERROR;
}

ECode CTelephonyManager::EnableLocationUpdates()
{
    // try {
    AutoPtr<IITelephony> iit;
    Boolean isSuccess;
    GetITelephony((IITelephony**)&iit);
    iit->EnableLocationUpdates(&isSuccess);
    // } catch (RemoteException ex) {
    // } catch (NullPointerException ex) {
    // }
    return NOERROR;
}

ECode CTelephonyManager::DisableLocationUpdates()
{
    /*try {
        getITelephony().disableLocationUpdates();
    } catch (RemoteException ex) {
    } catch (NullPointerException ex) {
    }*/
    AutoPtr<IITelephony> iit;
    GetITelephony((IITelephony**)&iit);
    Boolean res;
    return iit->DisableLocationUpdates(&res);
}

ECode CTelephonyManager::GetNeighboringCellInfo(
    /* [out, callee] */ ArrayOf<INeighboringCellInfo *>** res)
{
    VALIDATE_NOT_NULL(res);
    /*try {
        return getITelephony().getNeighboringCellInfo();
    } catch (RemoteException ex) {
        return null;
    } catch (NullPointerException ex) {
        return null;
    }*/
    *res = NULL;
    AutoPtr<IITelephony> iit;
    GetITelephony((IITelephony**)&iit);
    iit->GetNeighboringCellInfo(res);
    return NOERROR;
}

ECode CTelephonyManager::GetCurrentPhoneType(
    /* [out] */ Int32* val)
{
    VALIDATE_NOT_NULL(val);

    /*try{
        ITelephony telephony = getITelephony();
        if (telephony != null) {
            return telephony.getActivePhoneType();
        } else {
            // This can happen when the ITelephony interface is not up yet.
            return getPhoneTypeFromProperty();
        }
    } catch (RemoteException ex) {
        // This shouldn't happen in the normal case, as a backup we
        // read from the system property.
        return getPhoneTypeFromProperty();
    } catch (NullPointerException ex) {
        // This shouldn't happen in the normal case, as a backup we
        // read from the system property.
        return getPhoneTypeFromProperty();
    }*/

    AutoPtr<IITelephony> iit;
    if(FAILED(GetITelephony((IITelephony**)&iit))) {
        return GetPhoneTypeFromProperty(val);
    }

    if (iit != NULL) {
        if(FAILED(iit->GetActivePhoneType(val))) {
            return GetPhoneTypeFromProperty(val);
        } else {
            return NOERROR;
        }
    } else {
        // This can happen when the ITelephony interface is not up yet.
        return GetPhoneTypeFromProperty(val);
    }

    return NOERROR;
}

ECode CTelephonyManager::GetPhoneType(
    /* [out] */ Int32* val)
{
    VALIDATE_NOT_NULL(val);
    Boolean vc;
    if (IsVoiceCapable(&vc), !vc) {
        return PHONE_TYPE_NONE;
    }
    return GetCurrentPhoneType(val);
}

ECode CTelephonyManager::GetNetworkOperatorName(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);
    SystemProperties::Get(ITelephonyProperties::PROPERTY_OPERATOR_ALPHA, res);
    return NOERROR;
}

ECode CTelephonyManager::GetNetworkOperator(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);
    SystemProperties::Get(ITelephonyProperties::PROPERTY_OPERATOR_NUMERIC, res);
    return NOERROR;
}

ECode CTelephonyManager::IsNetworkRoaming(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    String temp;
    sp->Get(ITelephonyProperties::PROPERTY_OPERATOR_ISROAMING, &temp);
    *res = String("true").Equals(temp);
    return NOERROR;
}

ECode CTelephonyManager::GetNetworkCountryIso(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);
    SystemProperties::Get(ITelephonyProperties::PROPERTY_OPERATOR_ISO_COUNTRY, res);
    return NOERROR;
}

ECode CTelephonyManager::GetNetworkType(
    /* [out] */ Int32* val)
{
    VALIDATE_NOT_NULL(val);

    /*try{
        ITelephony telephony = getITelephony();
        if (telephony != null) {
            return telephony.getNetworkType();
        } else {
            // This can happen when the ITelephony interface is not up yet.
            return NETWORK_TYPE_UNKNOWN;
        }
    } catch(RemoteException ex) {
        // This shouldn't happen in the normal case
        return NETWORK_TYPE_UNKNOWN;
    } catch (NullPointerException ex) {
        // This could happen before phone restarts due to crashing
        return NETWORK_TYPE_UNKNOWN;
    }*/
    *val = NETWORK_TYPE_UNKNOWN;
    AutoPtr<IITelephony> iit;
    GetITelephony((IITelephony**)&iit);
    if (iit != NULL) {
        return iit->GetNetworkType(val);
    }

    return NOERROR;
}

ECode CTelephonyManager::GetNetworkTypeName(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);
    Int32 networkType;
    GetNetworkType(&networkType);
    *res = GetNetworkTypeName(networkType);
    return NOERROR;
}

ECode CTelephonyManager::HasIccCard(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    /*try {
        return getITelephony().hasIccCard();
    } catch (RemoteException ex) {
        // Assume no ICC card if remote exception which shouldn't happen
        return false;
    } catch (NullPointerException ex) {
        // This could happen before phone restarts due to crashing
        return false;
    }*/

    AutoPtr<IITelephony> iit;
    GetITelephony((IITelephony**)&iit);
    return iit->HasIccCard(res);
}

ECode CTelephonyManager::GetSimState(
    /* [out] */ Int32* val)
{
    VALIDATE_NOT_NULL(val);
    String prop;
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    sp->Get(ITelephonyProperties::PROPERTY_SIM_STATE, &prop);

    if (String("ABSENT").Equals(prop)) {
        return ITelephonyManager::SIM_STATE_ABSENT;
    }
    else if (String("PIN_REQUIRED").Equals(prop)) {
        return ITelephonyManager::SIM_STATE_PIN_REQUIRED;
    }
    else if (String("PUK_REQUIRED").Equals(prop)) {
        return ITelephonyManager::SIM_STATE_PUK_REQUIRED;
    }
    else if (String("NETWORK_LOCKED").Equals(prop)) {
        return ITelephonyManager::SIM_STATE_NETWORK_LOCKED;
    }
    else if (String("READY").Equals(prop)) {
        return ITelephonyManager::SIM_STATE_READY;
    }
    else {
        return ITelephonyManager::SIM_STATE_UNKNOWN;
    }
}

ECode CTelephonyManager::GetSimOperator(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);
    SystemProperties::Get(ITelephonyProperties::PROPERTY_ICC_OPERATOR_NUMERIC, res);
    return NOERROR;
}

ECode CTelephonyManager::GetSimOperatorName(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);
    SystemProperties::Get(ITelephonyProperties::PROPERTY_ICC_OPERATOR_ALPHA, res);
    return NOERROR;
}

ECode CTelephonyManager::GetSimCountryIso(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);
    SystemProperties::Get(ITelephonyProperties::PROPERTY_ICC_OPERATOR_ISO_COUNTRY, res);
    return NOERROR;
}

ECode CTelephonyManager::GetSimSerialNumber(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);

    /*try {
        return getSubscriberInfo().getIccSerialNumber();
    } catch (RemoteException ex) {
        return null;
    } catch (NullPointerException ex) {
        // This could happen before phone restarts due to crashing
        return null;
    }*/
    *res = String(NULL);
    AutoPtr<IIPhoneSubInfo> pi;
    GetSubscriberInfo((IIPhoneSubInfo**)&pi);
    return pi->GetIccSerialNumber(res);
}

ECode CTelephonyManager::GetLteOnCdmaMode(
    /* [out] */ Int32* val)
{
    VALIDATE_NOT_NULL(val);

    /*try {
        return getITelephony().getLteOnCdmaMode();
    } catch (RemoteException ex) {
        // Assume no ICC card if remote exception which shouldn't happen
        return PhoneConstants.LTE_ON_CDMA_UNKNOWN;
    } catch (NullPointerException ex) {
        // This could happen before phone restarts due to crashing
        return PhoneConstants.LTE_ON_CDMA_UNKNOWN;
    }*/

    *val = IPhoneConstants::LTE_ON_CDMA_UNKNOWN;
    AutoPtr<IITelephony> iit;
    GetITelephony((IITelephony**)&iit);
    return iit->GetLteOnCdmaMode(val);
}

ECode CTelephonyManager::GetSubscriberId(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);

    /*try {
        return getSubscriberInfo().getSubscriberId();
    } catch (RemoteException ex) {
        return null;
    } catch (NullPointerException ex) {
        // This could happen before phone restarts due to crashing
        return null;
    }*/

    *res = String(NULL);
    AutoPtr<IIPhoneSubInfo> pi;
    GetSubscriberInfo((IIPhoneSubInfo**)&pi);
    return pi->GetSubscriberId(res);
}

ECode CTelephonyManager::GetLine1Number(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);

    /*try {
        return getSubscriberInfo().getLine1Number();
    } catch (RemoteException ex) {
        return null;
    } catch (NullPointerException ex) {
        // This could happen before phone restarts due to crashing
        return null;
    }*/
    *res = String(NULL);
    AutoPtr<IIPhoneSubInfo> pi;
    GetSubscriberInfo((IIPhoneSubInfo**)&pi);
    return pi->GetLine1Number(res);
}

ECode CTelephonyManager::GetLine1AlphaTag(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);
    /*try {
        return getSubscriberInfo().getLine1AlphaTag();
    } catch (RemoteException ex) {
        return null;
    } catch (NullPointerException ex) {
        // This could happen before phone restarts due to crashing
        return null;
    }*/
    *res = String(NULL);
    AutoPtr<IIPhoneSubInfo> pi;
    GetSubscriberInfo((IIPhoneSubInfo**)&pi);
    return pi->GetLine1AlphaTag(res);
}

ECode CTelephonyManager::GetMsisdn(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);

    /*try {
        return getSubscriberInfo().getMsisdn();
    } catch (RemoteException ex) {
        return null;
    } catch (NullPointerException ex) {
        // This could happen before phone restarts due to crashing
        return null;
    */
    *res = String(NULL);
    AutoPtr<IIPhoneSubInfo> pi;
    GetSubscriberInfo((IIPhoneSubInfo**)&pi);
    return pi->GetMsisdn(res);
}

ECode CTelephonyManager::GetVoiceMailNumber(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);

    /*try {
        return getSubscriberInfo().getVoiceMailNumber();
    } catch (RemoteException ex) {
        return null;
    } catch (NullPointerException ex) {
        // This could happen before phone restarts due to crashing
        return null;
    }*/
    *res = String(NULL);
    AutoPtr<IIPhoneSubInfo> pi;
    GetSubscriberInfo((IIPhoneSubInfo**)&pi);
    return pi->GetVoiceMailNumber(res);
}

ECode CTelephonyManager::GetCompleteVoiceMailNumber(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);

    /*try {
        return getSubscriberInfo().getCompleteVoiceMailNumber();
    } catch (RemoteException ex) {
        return null;
    } catch (NullPointerException ex) {
        // This could happen before phone restarts due to crashing
        return null;
    }*/
    *res = String(NULL);
    AutoPtr<IIPhoneSubInfo> pi;
    GetSubscriberInfo((IIPhoneSubInfo**)&pi);
    return pi->GetCompleteVoiceMailNumber(res);
}

ECode CTelephonyManager::GetVoiceMessageCount(
    /* [out] */ Int32* val)
{
    VALIDATE_NOT_NULL(val);

    /*try {
        return getITelephony().getVoiceMessageCount();
    } catch (RemoteException ex) {
        return 0;
    } catch (NullPointerException ex) {
        // This could happen before phone restarts due to crashing
        return 0;
    }
    */
    *val = 0;
    AutoPtr<IITelephony> iit;
    GetITelephony((IITelephony**)&iit);
    return iit->GetVoiceMessageCount(val);
}

ECode CTelephonyManager::GetVoiceMailAlphaTag(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);

    /*try {
        return getSubscriberInfo().getVoiceMailAlphaTag();
    } catch (RemoteException ex) {
        return null;
    } catch (NullPointerException ex) {
        // This could happen before phone restarts due to crashing
        return null;
    }
    */
    *res = String(NULL);
    AutoPtr<IIPhoneSubInfo> pi;
    GetSubscriberInfo((IIPhoneSubInfo**)&pi);
    return pi->GetVoiceMailAlphaTag(res);
}

ECode CTelephonyManager::GetIsimImpi(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);

    /*try {
        return getSubscriberInfo().getIsimImpi();
    } catch (RemoteException ex) {
        return null;
    } catch (NullPointerException ex) {
        // This could happen before phone restarts due to crashing
        return null;
    }
    */
    *res = String(NULL);
    AutoPtr<IIPhoneSubInfo> pi;
    GetSubscriberInfo((IIPhoneSubInfo**)&pi);
    return pi->GetIsimImpi(res);
}

ECode CTelephonyManager::GetIsimDomain(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);

    /*try {
        return getSubscriberInfo().getIsimDomain();
    } catch (RemoteException ex) {
        return null;
    } catch (NullPointerException ex) {
        // This could happen before phone restarts due to crashing
        return null;
    }
    */
    *res = String(NULL);
    AutoPtr<IIPhoneSubInfo> pi;
    GetSubscriberInfo((IIPhoneSubInfo**)&pi);
    return pi->GetIsimDomain(res);
}

ECode CTelephonyManager::GetIsimImpu(
    /* [out, callee] */ ArrayOf<String>** res)
{
    VALIDATE_NOT_NULL(res);

    /*try {
        return getSubscriberInfo().getIsimImpu();
    } catch (RemoteException ex) {
        return null;
    } catch (NullPointerException ex) {
        // This could happen before phone restarts due to crashing
        return null;
    }
    */
    *res = NULL;
    AutoPtr<IIPhoneSubInfo> pi;
    GetSubscriberInfo((IIPhoneSubInfo**)&pi);
    return pi->GetIsimImpu(res);
}

ECode CTelephonyManager::GetCallState(
    /* [out] */ Int32* val)
{
    VALIDATE_NOT_NULL(val);

    /*try {
      return getITelephony().getCallState();
    } catch (RemoteException ex) {
      // the phone process is restarting.
      return CALL_STATE_IDLE;
    } catch (NullPointerException ex) {
    // the phone process is restarting.
    return CALL_STATE_IDLE;
    }
    */
    *val = ITelephonyManager::CALL_STATE_IDLE;
    AutoPtr<IITelephony> iit;
    GetITelephony((IITelephony**)&iit);
    return iit->GetCallState(val);
    return NOERROR;
}

ECode CTelephonyManager::GetDataActivity(
    /* [out] */ Int32* val)
{
    VALIDATE_NOT_NULL(val);

    /*try {
          return getITelephony().getDataActivity();
    } catch (RemoteException ex) {
        // the phone process is restarting.
        return DATA_ACTIVITY_NONE;
    } catch (NullPointerException ex) {
        // the phone process is restarting.
        return DATA_ACTIVITY_NONE;
    }*/
    *val = ITelephonyManager::DATA_ACTIVITY_NONE;
    AutoPtr<IITelephony> iit;
    GetITelephony((IITelephony**)&iit);
    return iit->GetDataActivity(val);
}

ECode CTelephonyManager::GetDataState(
    /* [out] */ Int32* val)
{
    VALIDATE_NOT_NULL(val);
    /*try {
        return getITelephony().getDataState();
    } catch (RemoteException ex) {
        // the phone process is restarting.
        return DATA_DISCONNECTED;
    } catch (NullPointerException ex) {
        return DATA_DISCONNECTED;
    }*/

    AutoPtr<IITelephony> iit;
    GetITelephony((IITelephony**)&iit);
    *val = ITelephonyManager::DATA_DISCONNECTED;
    return iit->GetDataState(val);
}

ECode CTelephonyManager::Listen(
    /* [in] */ IPhoneStateListener* funclistener,
    /* [in] */ Int32 Events)
{
    String pkgName;
    sContext->GetPackageName(&pkgName);
    String pkgForDebug = sContext != NULL ? pkgName : String("<unknown>");
    /*try {
        Boolean notifyNow = (getITelephony() != null);
        sRegistry.listen(pkgForDebug, listener.callback, events, notifyNow);
    } catch (RemoteException ex) {
        // system process dead
    } catch (NullPointerException ex) {
        // system process dead
    }*/
    AutoPtr<IITelephony> iit;
    GetITelephony((IITelephony**)&iit);
    Boolean notifyNow = (iit != NULL);
    assert(0);
    // at first to implement PhoneStateListener
    //return sRegistry->Listen(pkgForDebug, listener.callback, events, notifyNow);
    return NOERROR;
}

ECode CTelephonyManager::GetCdmaEriIconIndex(
    /* [out] */ Int32* val)
{
    VALIDATE_NOT_NULL(val);

    /*try {
        return getITelephony().getCdmaEriIconIndex();
    } catch (RemoteException ex) {
        // the phone process is restarting.
        return -1;
    } catch (NullPointerException ex) {
        return -1;
    }*/

    *val = -1;
    AutoPtr<IITelephony> iit;
    GetITelephony((IITelephony**)&iit);
    return iit->GetCdmaEriIconIndex(val);
}

ECode CTelephonyManager::GetCdmaEriIconMode(
    /* [out] */ Int32* val)
{
    VALIDATE_NOT_NULL(val);

    /*try {
        return getITelephony().getCdmaEriIconMode();
    } catch (RemoteException ex) {
        // the phone process is restarting.
        return -1;
    } catch (NullPointerException ex) {
        return -1;
    }*/
    *val = -1;
    AutoPtr<IITelephony> iit;
    GetITelephony((IITelephony**)&iit);
    return iit->GetCdmaEriIconMode(val);
}

ECode CTelephonyManager::GetCdmaEriText(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);

    /*try {
        return getITelephony().getCdmaEriText();
    } catch (RemoteException ex) {
        // the phone process is restarting.
        return null;
    } catch (NullPointerException ex) {
        return null;
    }*/
    *res = String(NULL);
    AutoPtr<IITelephony> iit;
    GetITelephony((IITelephony**)&iit);
    return iit->GetCdmaEriText(res);
}

ECode CTelephonyManager::IsVoiceCapable(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    if (sContext == NULL) {
        *res = TRUE;
        return NOERROR;
    }
    AutoPtr<IResources> rs;
    sContext->GetResources((IResources**)&rs);
    return rs->GetBoolean(R::bool_::config_voice_capable, res);
}

ECode CTelephonyManager::IsSmsCapable(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    if (sContext == NULL) {
        *res = TRUE;
        return NOERROR;
    }
    AutoPtr<IResources> rs;
    sContext->GetResources((IResources**)&rs);
    return rs->GetBoolean(R::bool_::config_sms_capable, res);
}

ECode CTelephonyManager::GetAllCellInfo(
    /* [out, callee] */ ArrayOf<ICellInfo *>** res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IITelephony> iit;
    if(FAILED(GetITelephony((IITelephony**)&iit))) {
        *res = NULL;
        return NOERROR;
    }
    return (iit->GetAllCellInfo(res));
    // try {
    //     return getITelephony().getAllCellInfo();
    // } catch (RemoteException ex) {
    //     return null;
    // } catch (NullPointerException ex) {
    //     return null;
    // }

    return NOERROR;
}

ECode CTelephonyManager::constructor()
{
    return NOERROR;
}

ECode CTelephonyManager::constructor(
    /* [in] */ IContext* context)
{
    if (sContext == NULL) {
        AutoPtr<IContext> appContext;
        context->GetApplicationContext((IContext**)&appContext);
        if (appContext != NULL) {
            sContext = appContext;
        } else {
            sContext = context;
        }

        sRegistry = (IITelephonyRegistry*)ServiceManager::GetService(String("telephony.registry")).Get();
    }

    return NOERROR;
}

String CTelephonyManager::GetProcCmdLine()
{
    String cmdline("");
    AutoPtr<IFileInputStream> is;
    ECode ec = NOERROR;
    do {
        ec = CFileInputStream::New(String("/proc/cmdline"), (IFileInputStream**)&is);
        if (FAILED(ec))
            break;
        AutoPtr< ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(2048);
        Int32 count;
        if (FAILED(is->ReadBytes(buffer, &count))) {
            break;
        }
        if (count > 0) {
            cmdline = String((char*)buffer->GetPayload());
            Slogger::D(TAG, "GetProcCmdLine the cmd line is %s", cmdline.string());
        }
    } while (0);

    if (FAILED(ec)) {
        Slogger::D(TAG, "No /proc/cmdline exception= 0x%08x", ec);
    }

    if (is != NULL)
        is->Close();

    Slogger::D(TAG, "/proc/cmdline=%s", cmdline.string());
    return cmdline;
}

ECode CTelephonyManager::GetPhoneTypeFromProperty(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res);
    Int32 type;
    GetPhoneTypeFromNetworkType(&type);
    *res = SystemProperties::GetInt32(ITelephonyProperties::CURRENT_ACTIVE_PHONE, type);
    return NOERROR;
}

ECode CTelephonyManager::GetPhoneTypeFromNetworkType(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res);

    // When the system property CURRENT_ACTIVE_PHONE, has not been set,
    // use the system property for default network type.
    // This is a fail safe, and can only happen at first boot.
    Int32 mode = SystemProperties::GetInt32(String("ro.telephony.default_network"), -1);
    if (mode == -1) {
        *res = ITelephonyManager::PHONE_TYPE_NONE;
        return NOERROR;
    }
    return GetPhoneType(mode, res);
}

ECode CTelephonyManager::GetSubscriberInfo(
    /* [out] */ IIPhoneSubInfo** res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IInterface> service;
    service = ServiceManager::GetService(String("iphonesubinfo"));
    assert(service != NULL);
    *res = IIPhoneSubInfo::Probe(service.Get());
    REFCOUNT_ADD(*res);
    return NOERROR;
}

ECode CTelephonyManager::GetITelephony(
    /* [out] */ IITelephony** res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IInterface> telephonyService;
    telephonyService = ServiceManager::GetService(IContext::TELEPHONY_SERVICE);
    assert(telephonyService != NULL);
    *res = IITelephony::Probe(telephonyService.Get());
    REFCOUNT_ADD(*res);
    return NOERROR;
}

ECode CTelephonyManager::GetPhoneType(
    /* [in] */ Int32 networkMode,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res);
    switch(networkMode) {
    case IIRILConstants::NETWORK_MODE_CDMA:
    case IIRILConstants::NETWORK_MODE_CDMA_NO_EVDO:
    case IIRILConstants::NETWORK_MODE_EVDO_NO_CDMA:
        *res = IPhoneConstants::PHONE_TYPE_CDMA;
        return NOERROR;

    case IIRILConstants::NETWORK_MODE_WCDMA_PREF:
    case IIRILConstants::NETWORK_MODE_GSM_ONLY:
    case IIRILConstants::NETWORK_MODE_WCDMA_ONLY:
    case IIRILConstants::NETWORK_MODE_GSM_UMTS:
        *res = IPhoneConstants::PHONE_TYPE_GSM;
        return NOERROR;

    // Use CDMA Phone for the global mode including CDMA
    case IIRILConstants::NETWORK_MODE_GLOBAL:
    case IIRILConstants::NETWORK_MODE_LTE_CDMA_EVDO:
    case IIRILConstants::NETWORK_MODE_LTE_CMDA_EVDO_GSM_WCDMA:
        *res = IPhoneConstants::PHONE_TYPE_CDMA;
        return NOERROR;

    case IIRILConstants::NETWORK_MODE_LTE_ONLY:
        {
            Int32 tmp;
            if ((GetLteOnCdmaModeStatic(&tmp), tmp) == IPhoneConstants::LTE_ON_CDMA_TRUE) {
                *res = IPhoneConstants::PHONE_TYPE_CDMA;
                    return NOERROR;
            } else {
                *res = IPhoneConstants::PHONE_TYPE_GSM;
                    return NOERROR;
            }
        }
    default:
        *res = IPhoneConstants::PHONE_TYPE_GSM;
        return NOERROR;
    }

    return NOERROR;
}

ECode CTelephonyManager::GetLteOnCdmaModeStatic(
        /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res);
    Int32 retVal;
    Int32 curVal;
    String productType("");

    curVal = SystemProperties::GetInt32(ITelephonyProperties::PROPERTY_LTE_ON_CDMA_DEVICE,
                IPhoneConstants::LTE_ON_CDMA_UNKNOWN);
    retVal = curVal;
    if (retVal == IPhoneConstants::LTE_ON_CDMA_UNKNOWN) {
        AutoPtr<IMatcher> matcher;
        Boolean isFind;
        sProductTypePattern->Matcher(sKernelCmdLine, (IMatcher**)&matcher);
        if (matcher->Find(&isFind), isFind) {
            matcher->Group(1, &productType);
            if (sLteOnCdmaProductType.Equals(productType)) {
                retVal = IPhoneConstants::LTE_ON_CDMA_TRUE;
            } else {
                retVal = IPhoneConstants::LTE_ON_CDMA_FALSE;
            }
        } else {
            retVal = IPhoneConstants::LTE_ON_CDMA_FALSE;
        }
    }

    Slogger::D(TAG, "getLteOnCdmaMode=%d curVal=%d product_type=%d' lteOnCdmaProductType=%s''", retVal, curVal, productType.string(), sLteOnCdmaProductType.string());
    *res = retVal;
    return NOERROR;
}

String CTelephonyManager::GetNetworkTypeName(
        /* [in] */  Int32 type)
{
    switch (type) {
        case NETWORK_TYPE_GPRS:
            return String("GPRS");
        case NETWORK_TYPE_EDGE:
            return String("EDGE");
        case NETWORK_TYPE_UMTS:
            return String("UMTS");
        case NETWORK_TYPE_HSDPA:
            return String("HSDPA");
        case NETWORK_TYPE_HSUPA:
            return String("HSUPA");
        case NETWORK_TYPE_HSPA:
            return String("HSPA");
        case NETWORK_TYPE_CDMA:
            return String("CDMA");
        case NETWORK_TYPE_EVDO_0:
            return String("CDMA - EvDo rev. 0");
        case NETWORK_TYPE_EVDO_A:
            return String("CDMA - EvDo rev. A");
        case NETWORK_TYPE_EVDO_B:
            return String("CDMA - EvDo rev. B");
        case NETWORK_TYPE_1xRTT:
            return String("CDMA - 1xRTT");
        case NETWORK_TYPE_LTE:
            return String("LTE");
        case NETWORK_TYPE_EHRPD:
            return String("CDMA - eHRPD");
        case NETWORK_TYPE_IDEN:
            return String("iDEN");
        case NETWORK_TYPE_HSPAP:
            return String("HSPA+");
        default:
            return String("UNKNOWN");
    }
}

Int32 CTelephonyManager::GetNetworkClass(
    /* [in] */ Int32 networkType)
{
    switch (networkType) {
        case NETWORK_TYPE_GPRS:
        case NETWORK_TYPE_EDGE:
        case NETWORK_TYPE_CDMA:
        case NETWORK_TYPE_1xRTT:
        case NETWORK_TYPE_IDEN:
            return NETWORK_CLASS_2_G;
        case NETWORK_TYPE_UMTS:
        case NETWORK_TYPE_EVDO_0:
        case NETWORK_TYPE_EVDO_A:
        case NETWORK_TYPE_HSDPA:
        case NETWORK_TYPE_HSUPA:
        case NETWORK_TYPE_HSPA:
        case NETWORK_TYPE_EVDO_B:
        case NETWORK_TYPE_EHRPD:
        case NETWORK_TYPE_HSPAP:
            return NETWORK_CLASS_3_G;
        case NETWORK_TYPE_LTE:
            return NETWORK_CLASS_4_G;
        default:
            return NETWORK_CLASS_UNKNOWN;
    }

}

ECode CTelephonyManager::From(
    /* [in] */ IContext* context,
    /* [out] */ ITelephonyManager** tm)
{
    VALIDATE_NOT_NULL(tm);
    return context->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)tm);
}

}
}
}

