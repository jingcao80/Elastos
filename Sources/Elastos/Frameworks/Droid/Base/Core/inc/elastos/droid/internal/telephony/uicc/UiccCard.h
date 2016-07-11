#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_UICCCARD_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_UICCCARD_H__

#include "_Elastos.Droid.Internal.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/RegistrantList.h"

// import static android.Manifest.permission.READ_PHONE_STATE;
// import android.app.ActivityManagerNative;
// import android.app.AlertDialog;
// import android.content.Context;
// import android.content.DialogInterface;
// import android.content.Intent;
// import android.content.SharedPreferences;
// import android.content.pm.PackageManager;
// import android.content.pm.Signature;
// import android.content.res.Resources;
// import android.os.AsyncResult;
// import android.os.Message;
// import android.os.PowerManager;
// import android.os.Registrant;
// import android.os.RegistrantList;
// import android.preference.PreferenceManager;
// import android.telephony.Rlog;
// import android.telephony.TelephonyManager;
// import android.text.TextUtils;
// import android.view.WindowManager;
// import com.android.internal.telephony.CommandsInterface;
// import com.android.internal.telephony.PhoneBase;
// import com.android.internal.telephony.CommandsInterface.RadioState;
// import com.android.internal.telephony.IccCardConstants.State;
// import com.android.internal.telephony.gsm.GSMPhone;
// import com.android.internal.telephony.uicc.IccCardApplicationStatus.AppType;
// import com.android.internal.telephony.uicc.IccCardStatus.PinState;
// import com.android.internal.telephony.cat.CatService;
// import com.android.internal.telephony.cat.CatServiceFactory;
// import com.android.internal.telephony.cdma.CDMALTEPhone;
// import com.android.internal.telephony.cdma.CDMAPhone;
// import com.android.internal.telephony.cdma.CdmaSubscriptionSourceManager;
// import android.os.SystemProperties;
// import com.android.internal.R;
// import java.io.FileDescriptor;
// import java.io.PrintWriter;
// import java.util.List;

using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::ISignature;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::RegistrantList;
using Elastos::Droid::Internal::Telephony::ICommandsInterfaceRadioState;
using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::Cat::ICatService;
using Elastos::Droid::Internal::Telephony::Uicc::CardState;
using Elastos::Droid::Internal::Telephony::Uicc::PinState;
using Elastos::Droid::Internal::Telephony::Uicc::AppType;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

/**
  * {@hide}
  */
class UiccCard
    : public Object
    , public IUiccCard
{
private:
    class InnerDialogInterfaceOnClickListener1
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        InnerDialogInterfaceOnClickListener1(
            /* [in] */ UiccCard* owner);

        // @Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        UiccCard* mOwner;
    };

    class InnerHandler
        : public Handler
    {
    public:
        InnerHandler(
            /* [in] */ UiccCard* owner);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        UiccCard* mOwner;
    };

public:
    CAR_INTERFACE_DECL();

    UiccCard();

    CARAPI constructor(
        /* [in] */ IContext* c,
        /* [in] */ ICommandsInterface* ci,
        /* [in] */ IIccCardStatus* ics,
        /* [in] */ Int32 slotId);

    virtual CARAPI Dispose();

    virtual CARAPI Update(
        /* [in] */ IContext* c,
        /* [in] */ ICommandsInterface* ci,
        /* [in] */ IIccCardStatus* ics);

    virtual CARAPI GetCatService(
        /* [out] */ ICatService** result);

    /**
      * Notifies handler of any transition into State.ABSENT
      */
    virtual CARAPI RegisterForAbsent(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    virtual CARAPI UnregisterForAbsent(
        /* [in] */ IHandler* h);

    /**
      * Notifies handler when carrier privilege rules are loaded.
      */
    virtual CARAPI RegisterForCarrierPrivilegeRulesLoaded(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    virtual CARAPI UnregisterForCarrierPrivilegeRulesLoaded(
        /* [in] */ IHandler* h);

    virtual CARAPI IsApplicationOnIcc(
        /* [in] */  AppType type,
        /* [out] */ Boolean* result);

    virtual CARAPI GetCardState(
        /* [out] */ CardState* result);

    virtual CARAPI GetUniversalPinState(
        /* [out] */ PinState* result);//IccCardStatus.car

    virtual CARAPI GetSlotId(
        /* [out] */ Int32* result);

    virtual CARAPI GetApplication(
        /* [in] */ Int32 family,
        /* [out] */ IUiccCardApplication** result);

    virtual CARAPI GetApplicationIndex(
        /* [in] */ Int32 index,
        /* [out] */ IUiccCardApplication** result);

    /**
      * Returns the SIM application of the specified type.
      *
      * @param type ICC application type (@see com.android.internal.telephony.PhoneConstants#APPTYPE_xxx)
      * @return application corresponding to type or a null if no match found
      */
    virtual CARAPI GetApplicationByType(
        /* [in] */ Int32 type,
        /* [out] */ IUiccCardApplication** result);

    /**
      * Exposes {@link CommandsInterface.iccOpenLogicalChannel}
      */
    virtual CARAPI IccOpenLogicalChannel(
        /* [in] */ const String& AID,
        /* [in] */ IMessage* response);

    /**
      * Exposes {@link CommandsInterface.iccCloseLogicalChannel}
      */
    virtual CARAPI IccCloseLogicalChannel(
        /* [in] */ Int32 channel,
        /* [in] */ IMessage* response);

    /**
      * Exposes {@link CommandsInterface.iccTransmitApduLogicalChannel}
      */
    virtual CARAPI IccTransmitApduLogicalChannel(
        /* [in] */ Int32 channel,
        /* [in] */ Int32 cla,
        /* [in] */ Int32 command,
        /* [in] */ Int32 p1,
        /* [in] */ Int32 p2,
        /* [in] */ Int32 p3,
        /* [in] */ const String& data,
        /* [in] */ IMessage* response);

    /**
      * Exposes {@link CommandsInterface.iccTransmitApduBasicChannel}
      */
    virtual CARAPI IccTransmitApduBasicChannel(
        /* [in] */ Int32 cla,
        /* [in] */ Int32 command,
        /* [in] */ Int32 p1,
        /* [in] */ Int32 p2,
        /* [in] */ Int32 p3,
        /* [in] */ const String& data,
        /* [in] */ IMessage* response);

    /**
      * Exposes {@link CommandsInterface.iccIO}
      */
    virtual CARAPI IccExchangeSimIO(
        /* [in] */ Int32 fileID,
        /* [in] */ Int32 command,
        /* [in] */ Int32 p1,
        /* [in] */ Int32 p2,
        /* [in] */ Int32 p3,
        /* [in] */ const String& pathID,
        /* [in] */ IMessage* response);

    /**
      * Exposes {@link CommandsInterface.getAtr}
      */
    virtual CARAPI GetAtr(
        /* [in] */ IMessage* response);

    /**
      * Exposes {@link CommandsInterface.sendEnvelopeWithStatus}
      */
    virtual CARAPI SendEnvelopeWithStatus(
        /* [in] */ const String& contents,
        /* [in] */ IMessage* response);

    /* Returns number of applications on this card */
    virtual CARAPI GetNumApplications(
        /* [out] */ Int32* result);

    /**
      * Returns true iff carrier priveleges rules are null (dont need to be loaded) or loaded.
      */
    virtual CARAPI AreCarrierPriviligeRulesLoaded(
        /* [out] */ Boolean* result);

    /**
      * Exposes {@link UiccCarrierPrivilegeRules.getCarrierPrivilegeStatus}.
      */
    virtual CARAPI GetCarrierPrivilegeStatus(
        /* [in] */ ISignature* signature,
        /* [in] */ const String& packageName,
        /* [out] */ Int32* result);

    /**
      * Exposes {@link UiccCarrierPrivilegeRules.getCarrierPrivilegeStatus}.
      */
    virtual CARAPI GetCarrierPrivilegeStatus(
        /* [in] */ IPackageManager* packageManager,
        /* [in] */ const String& packageName,
        /* [out] */ Int32* result);

    /**
      * Exposes {@link UiccCarrierPrivilegeRules.getCarrierPrivilegeStatusForCurrentTransaction}.
      */
    virtual CARAPI GetCarrierPrivilegeStatusForCurrentTransaction(
        /* [in] */ IPackageManager* packageManager,
        /* [out] */ Int32* result);

    /**
      * Exposes {@link UiccCarrierPrivilegeRules.getCarrierPackageNamesForIntent}.
      */
    virtual CARAPI GetCarrierPackageNamesForIntent(
        /* [in] */ IPackageManager* packageManager,
        /* [in] */ IIntent* intent,
        /* [out] */ IList/*<String>*/** result);

    virtual CARAPI SetOperatorBrandOverride(
        /* [in] */ const String& brand,
        /* [out] */ Boolean* result);

    virtual CARAPI GetOperatorBrandOverride(
        /* [out] */ String* result);

    virtual CARAPI GetIccId(
        /* [out] */ String* result);

    virtual CARAPI GetUICCConfig(
        /* [out] */ IUICCConfig** result);

    virtual CARAPI OnRefresh(
        /* [in] */ IIccRefreshResponse* refreshResponse);

    virtual CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

protected:
    virtual CARAPI_(void) CreateAndUpdateCatService();

    // @Override
    CARAPI Finalize();

    virtual CARAPI_(void) Log(
        /* [in] */ const String& msg);

private:
    CARAPI_(AutoPtr<ArrayOf<IUiccCardApplication*> >) MiddleInitMuiccapplications();

    /**
      * This function makes sure that application indexes are valid
      * and resets invalid indexes. (This should never happen, but in case
      * RIL misbehaves we need to manage situation gracefully)
      */
    CARAPI_(void) SanitizeApplicationIndexes();

    CARAPI_(Int32) CheckIndex(
        /* [in] */ Int32 index,
        /* [in] */ AppType expectedAppType,
        /* [in] */ AppType altExpectedAppType);

    CARAPI_(void) OnIccSwap(
        /* [in] */ Boolean isAdded);

    CARAPI_(void) OnCarrierPriviligesLoadedMessage();

    CARAPI_(void) Loge(
        /* [in] */ const String& msg);

public:
    Int32 mSlotId;

protected:
    static const String LOGTAG;
    static const Boolean DBG;
    AutoPtr<IHandler> mHandler;

private:
    static const String OPERATOR_BRAND_OVERRIDE_PREFIX;
    Object mLock;
    CardState mCardState;
    PinState mUniversalPinState;
    Int32 mGsmUmtsSubscriptionAppIndex;
    Int32 mCdmaSubscriptionAppIndex;
    Int32 mImsSubscriptionAppIndex;
    AutoPtr<ArrayOf<IUiccCardApplication*> > mUiccApplications;
    AutoPtr<IContext> mContext;
    AutoPtr<ICommandsInterface> mCi;
    AutoPtr<ICatService> mCatService;
    Boolean mDestroyed;
    //set to true once this card is commanded to be disposed of.
    ICommandsInterfaceRadioState mLastRadioState;
    AutoPtr<IUiccCarrierPrivilegeRules> mCarrierPrivilegeRules;
    AutoPtr<IUICCConfig> mUICCConfig;
    AutoPtr<RegistrantList> mAbsentRegistrants;
    AutoPtr<RegistrantList> mCarrierPrivilegeRegistrants;
    static const Int32 EVENT_CARD_REMOVED = 13;
    static const Int32 EVENT_CARD_ADDED = 14;
    static const Int32 EVENT_OPEN_LOGICAL_CHANNEL_DONE = 15;
    static const Int32 EVENT_CLOSE_LOGICAL_CHANNEL_DONE = 16;
    static const Int32 EVENT_TRANSMIT_APDU_LOGICAL_CHANNEL_DONE = 17;
    static const Int32 EVENT_TRANSMIT_APDU_BASIC_CHANNEL_DONE = 18;
    static const Int32 EVENT_SIM_IO_DONE = 19;
    static const Int32 EVENT_CARRIER_PRIVILIGES_LOADED = 20;
    static const Int32 EVENT_SIM_GET_ATR_DONE = 21;
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_UICCCARD_H__
