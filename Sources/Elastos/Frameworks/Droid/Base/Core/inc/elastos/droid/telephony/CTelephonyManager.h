
#ifndef __ELASTOS_DROID_TELEPHONY_CTELEPHONYMANAGER_H__
#define __ELASTOS_DROID_TELEPHONY_CTELEPHONYMANAGER_H__

#include "_Elastos_Droid_Telephony_CTelephonyManager.h"
#include "elastos/droid/ext/frameworkdef.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::IIPhoneSubInfo;
using Elastos::Droid::Internal::Telephony::IITelephony;
using Elastos::Droid::Internal::Telephony::IITelephonyRegistry;
using Elastos::Droid::Telephony::ICellInfo;
using Elastos::Droid::Telephony::ICellLocation;
using Elastos::Droid::Telephony::IPhoneStateListener;
using Elastos::Utility::Regex::IPattern;

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CTelephonyManager)
{
public:
    static CARAPI GetDefault(
        /* [out] */ ITelephonyManager** tm);

    static CARAPI GetPhoneType(
        /* [in] */ Int32 networkMode,
        /* [out] */ Int32* res);

    static CARAPI GetLteOnCdmaModeStatic(
        /* [out] */ Int32* res);

    static CARAPI_(String) GetNetworkTypeName(
        /* [in] */ Int32 type);

    static CARAPI_(Int32) GetNetworkClass(
        /* [in] */ Int32 networkType);

    static CARAPI From(
        /* [in] */ IContext* context,
        /* [out] */ ITelephonyManager** tm);

    CARAPI GetDeviceSoftwareVersion(
        /* [out] */ String* res);

    CARAPI GetDeviceId(
        /* [out] */ String* res);

    CARAPI GetCellLocation(
        /* [out] */ ICellLocation** res);

    CARAPI EnableLocationUpdates();

    CARAPI DisableLocationUpdates();

    CARAPI GetNeighboringCellInfo(
        /* [out, callee] */ ArrayOf<INeighboringCellInfo *>** res);

    CARAPI GetCurrentPhoneType(
        /* [out] */ Int32* val);

    CARAPI GetPhoneType(
        /* [out] */ Int32* val);

    CARAPI GetNetworkOperatorName(
        /* [out] */ String* res);

    CARAPI GetNetworkOperator(
        /* [out] */ String* res);

    CARAPI IsNetworkRoaming(
        /* [out] */ Boolean* res);

    CARAPI GetNetworkCountryIso(
        /* [out] */ String* res);

    CARAPI GetNetworkType(
        /* [out] */ Int32* val);

    CARAPI GetNetworkTypeName(
        /* [out] */ String* res);

    CARAPI HasIccCard(
        /* [out] */ Boolean* res);

    CARAPI GetSimState(
        /* [out] */ Int32* val);

    CARAPI GetSimOperator(
        /* [out] */ String* res);

    CARAPI GetSimOperatorName(
        /* [out] */ String* res);

    CARAPI GetSimCountryIso(
        /* [out] */ String* res);

    CARAPI GetSimSerialNumber(
        /* [out] */ String* res);

    CARAPI GetLteOnCdmaMode(
        /* [out] */ Int32* val);

    CARAPI GetSubscriberId(
        /* [out] */ String* res);

    CARAPI GetLine1Number(
        /* [out] */ String* res);

    CARAPI GetLine1AlphaTag(
        /* [out] */ String* res);

    CARAPI GetMsisdn(
        /* [out] */ String* res);

    CARAPI GetVoiceMailNumber(
        /* [out] */ String* res);

    CARAPI GetCompleteVoiceMailNumber(
        /* [out] */ String* res);

    CARAPI GetVoiceMessageCount(
        /* [out] */ Int32* val);

    CARAPI GetVoiceMailAlphaTag(
        /* [out] */ String* res);

    CARAPI GetIsimImpi(
        /* [out] */ String* res);

    CARAPI GetIsimDomain(
        /* [out] */ String* res);

    CARAPI GetIsimImpu(
        /* [out, callee] */ ArrayOf<String>** res);

    CARAPI GetCallState(
        /* [out] */ Int32* val);

    CARAPI GetDataActivity(
        /* [out] */ Int32* val);

    CARAPI GetDataState(
        /* [out] */ Int32* val);

    CARAPI Listen(
        /* [in] */ IPhoneStateListener* funclistener,
        /* [in] */ Int32 Events);

    CARAPI GetCdmaEriIconIndex(
        /* [out] */ Int32* val);

    CARAPI GetCdmaEriIconMode(
        /* [out] */ Int32* val);

    CARAPI GetCdmaEriText(
        /* [out] */ String* res);

    CARAPI IsVoiceCapable(
        /* [out] */ Boolean* res);

    CARAPI IsSmsCapable(
        /* [out] */ Boolean* res);

    CARAPI GetAllCellInfo(
        /* [out, callee] */ ArrayOf<ICellInfo *>** res);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CTelephonyManager();

private:

    static CARAPI_(String) GetProcCmdLine();

    CARAPI GetPhoneTypeFromProperty(
        /* [out] */ Int32* res);

    CARAPI GetPhoneTypeFromNetworkType(
        /* [out] */ Int32* res);

    CARAPI GetSubscriberInfo(
        /* [out] */ IIPhoneSubInfo** res);

    CARAPI GetITelephony(
        /* [out] */ IITelephony** res);

    /** Kernel command line */
    static const String sKernelCmdLine/* = getProcCmdLine()*/;

    /** Pattern for selecting the product type from the kernel command line */
    static const AutoPtr<IPattern> sProductTypePattern/* =
        Pattern.compile("\\sproduct_type\\s*=\\s*(\\w+)")*/;

    /** The ProductType used for LTE on CDMA devices */
    static const String sLteOnCdmaProductType/* =
        SystemProperties.get(TelephonyProperties.PROPERTY_LTE_ON_CDMA_PRODUCT_TYPE, "")*/;

    static AutoPtr<ITelephonyManager> sInstance;
    static const String TAG/* = "TelephonyManager"*/;

    static AutoPtr<IContext> sContext;
    static AutoPtr<IITelephonyRegistry> sRegistry;

    static String mImei;
    static const Int32 IMEI_LEN = 15;

};

}
}
}

#endif // __ELASTOS_DROID_TELEPHONY_CTELEPHONYMANAGER_H__
