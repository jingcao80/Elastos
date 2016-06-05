#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMALTEPHONE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMALTEPHONE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/internal/telephony/cdma/CDMAPhone.h"

// package com.android.internal.telephony.cdma;
// import android.app.ActivityManagerNative;
// import android.content.ContentValues;
// import android.content.Context;
// import android.content.Intent;
// import android.content.SharedPreferences;
// import android.database.SQLException;
// import android.net.Uri;
// import android.os.AsyncResult;
// import android.os.Handler;
// import android.os.Message;
// import android.os.UserHandle;
// import android.preference.PreferenceManager;
// import android.os.PowerManager;
// import android.os.SystemProperties;
// import android.provider.Telephony;
// import android.text.TextUtils;
// import android.telephony.SubscriptionManager;
// import android.telephony.Rlog;
// import android.telephony.ServiceState;
// import com.android.internal.telephony.CommandsInterface;
// import android.telephony.TelephonyManager;
// import com.android.internal.telephony.dataconnection.DcTracker;
// import com.android.internal.telephony.MccTable;
// import com.android.internal.telephony.OperatorInfo;
// import com.android.internal.telephony.PhoneConstants;
// import com.android.internal.telephony.PhoneNotifier;
// import com.android.internal.telephony.PhoneProxy;
// import com.android.internal.telephony.PhoneFactory;
// import com.android.internal.telephony.PhoneSubInfo;
// import com.android.internal.telephony.SMSDispatcher;
// import com.android.internal.telephony.SmsBroadcastUndelivered;
// import com.android.internal.telephony.Subscription;
// import com.android.internal.telephony.SubscriptionController;
// import com.android.internal.telephony.gsm.GsmSMSDispatcher;
// import com.android.internal.telephony.gsm.SmsMessage;
// import com.android.internal.telephony.uicc.IccRecords;
// import com.android.internal.telephony.uicc.IsimRecords;
// import com.android.internal.telephony.uicc.IsimUiccRecords;
// import com.android.internal.telephony.uicc.RuimRecords;
// import com.android.internal.telephony.uicc.SIMRecords;
// import com.android.internal.telephony.uicc.UiccCardApplication;
// import com.android.internal.telephony.uicc.UiccController;
// import com.android.internal.telephony.ServiceStateTracker;
// import com.android.internal.telephony.TelephonyIntents;
// import com.android.internal.telephony.TelephonyProperties;
// import java.io.FileDescriptor;
// import java.io.PrintWriter;
// import static com.android.internal.telephony.TelephonyProperties.PROPERTY_ICC_OPERATOR_ALPHA;
// import static com.android.internal.telephony.TelephonyProperties.PROPERTY_ICC_OPERATOR_ISO_COUNTRY;
// import static com.android.internal.telephony.TelephonyProperties.PROPERTY_ICC_OPERATOR_NUMERIC;
// import static com.android.internal.telephony.PhoneConstants.EVENT_SUBSCRIPTION_ACTIVATED;
// import static com.android.internal.telephony.PhoneConstants.EVENT_SUBSCRIPTION_DEACTIVATED;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::IPhoneNotifier;
//using Elastos::Droid::Internal::Telephony::Uicc::IccRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IIsimRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IIsimUiccRecords;
using Elastos::Droid::Internal::Telephony::Uicc::ISIMRecords;
using Elastos::Droid::Internal::Telephony::PhoneConstantsState;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
//using Elastos::Droid::Internal::Telephony::Uicc::IsimRecords;
using Elastos::Droid::Os::IHandler;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

class CDMALTEPhone
    : public CDMAPhone
    , public ICDMALTEPhone
{
public:
    CAR_INTERFACE_DECL();

    // Constructors
    CDMALTEPhone(
        /* [in] */ IContext* context,
        /* [in] */ ICommandsInterface* ci,
        /* [in] */ IPhoneNotifier* notifier,
        /* [in] */ Int32 phoneId);

    CDMALTEPhone(
        /* [in] */ IContext* context,
        /* [in] */ ICommandsInterface* ci,
        /* [in] */ IPhoneNotifier* notifier,
        /* [in] */ Boolean unitTestMode,
        /* [in] */ Int32 phoneId);

    // Constructors
    CDMALTEPhone(
        /* [in] */ IContext* context,
        /* [in] */ ICommandsInterface* ci,
        /* [in] */ IPhoneNotifier* notifier);

    // @Override
    CARAPI Dispose();

    // @Override
    CARAPI RemoveReferences();

    // @Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    // @Override
    CARAPI GetDataConnectionState(
        /* [in] */ const String& apnType,
        /* [out] */ PhoneConstantsState* resutl);

    /**
      * Sets the "current" field in the telephony provider according to the
      * build-time operator numeric property
      *
      * @return true for success; false otherwise.
      */
    // @Override
    CARAPI UpdateCurrentCarrierInProvider(
        /* [in] */ const String& operatorNumeric,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI UpdateCurrentCarrierInProvider(
        /* [out] */ Boolean* resutl);

    // return IMSI from CSIM as subscriber ID if available, otherwise reads from USIM
    // @Override
    CARAPI GetSubscriberId(
        /* [out] */ String* result);

    // fix CTS test expecting IMEI to be used as device ID when in LteOnCdma mode
    // @Override
    CARAPI GetDeviceId(
        /* [out] */ String* result);

    // return GID1 from USIM
    // @Override
    CARAPI GetGroupIdLevel1(
        /* [out] */ String* result);

    // @Override
    CARAPI GetImei(
        /* [out] */ String* result);

    // @Override
    CARAPI GetDeviceSvn(
        /* [out] */ String* result);

    // @Override
    CARAPI GetIsimRecords(
        /* [out] */ IIsimRecords** result);

    // @Override
    CARAPI GetMsisdn(
        /* [out] */ String* result);

    // @Override
    CARAPI GetAvailableNetworks(
        /* [in] */ IMessage* response);

    // @Override
    CARAPI SetSystemProperty(
        /* [in] */ const String& property,
        /* [in] */ const String& value);

    virtual CARAPI GetSystemProperty(
        /* [in] */ const String& property,
        /* [in] */ const String& defValue,
        /* [out] */ String* result);

    virtual CARAPI UpdateDataConnectionTracker();

    virtual CARAPI SetInternalDataEnabled(
        /* [in] */ Boolean enable,
        /* [in] */ IMessage* onCompleteMsg);

    virtual CARAPI SetInternalDataEnabledFlag(
        /* [in] */ Boolean enable,
        /* [out] */ Boolean* result);

    /**
      * @return operator numeric.
      */
    virtual CARAPI GetOperatorNumeric(
        /* [out] */ String* result);

    virtual CARAPI RegisterForAllDataDisconnected(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    virtual CARAPI UnregisterForAllDataDisconnected(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI RegisterForSimRecordsLoaded(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    // @Override
    CARAPI UnregisterForSimRecordsLoaded(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

protected:
    // @Override
    CARAPI_(void) InitSstIcc();

    // @Override
    CARAPI_(void) OnUpdateIccAvailability();

    // @Override
    CARAPI_(void) Init(
        /* [in] */ IContext* context,
        /* [in] */ IPhoneNotifier* notifier);

    // @Override
    CARAPI_(void) Log(
        /* [in] */ const String& s);

    virtual CARAPI_(void) Loge(
        /* [in] */ const String& s);

    //virtual CARAPI_(void) Loge(
    //    /* [in] */ const String& s,
    //    /* [in] */ Throwable* e);

private:
    CARAPI_(void) OnSubscriptionActivated();

    CARAPI_(void) OnSubscriptionDeactivated();

    // Set the properties per subscription
    CARAPI_(void) SetProperties();

public:
    static const String LOG_LTE_TAG;

private:
    static const Boolean DBG;
    /** CdmaLtePhone in addition to RuimRecords available from
      * PhoneBase needs access to SIMRecords and IsimUiccRecords
      */
    AutoPtr<ISIMRecords> mSimRecords;
    AutoPtr<IIsimUiccRecords> mIsimUiccRecords;
};

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CDMALTEPHONE_H__

