
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_SUBINFORECORDUPDATER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_SUBINFORECORDUPDATER_H__

#include "_Elastos.Droid.Internal.h"
#include "Elastos.Droid.Telephony.h"
#include <elastos/core/Object.h>
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Core::Object;

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Internal::Telephony::Uicc::CardState;
using Elastos::Droid::Internal::Telephony::Uicc::IIccFileHandler;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::ITelephonyManagerHelper;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

class SubInfoRecordUpdater
    : public Handler
    , public ISubInfoRecordUpdater
{
private:
    class MyBroadcastReceiver : public BroadcastReceiver
    {
    public:
        MyBroadcastReceiver(
            /* [in] */ SubInfoRecordUpdater* host);

        virtual CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info);

    private:
        SubInfoRecordUpdater* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ArrayOf<IPhone*>* phoneProxy,
        /* [in] */ ArrayOf<ICommandsInterface*>* ci);

    CARAPI UpdateSubIdForNV(
        /* [in] */ Int32 slotId);

    CARAPI UpdateSimInfoByIccId();

    CARAPI Dispose();

    CARAPI HandleMessage(
    /* [in] */ IMessage* msg);

    static CARAPI SetDisplayNameForNewSub(
        /* [in] */ const String& newSubName,
        /* [in] */ Int32 subId,
        /* [in] */ Int32 newNameSource);

private:
    static Int32 EncodeEventId(
        /* [in] */ Int32 event,
        /* [in] */ Int32 slotId);

    Boolean IsAllIccIdQueryDone();

    void UpdateIccAvailability(
        /* [in] */ Int32 slotId);

    void QueryIccId(
        /* [in] */ Int32 slotId);

    static void SetUpdatedData(
        /* [in] */ Int32 detectedType,
        /* [in] */ Int32 subCount,
        /* [in] */ Int32 newSimStatus);

    static Boolean IsNewSim(
        /* [in] */ const String& iccId,
        /* [in] */ ArrayOf<String>* oldIccId);

    static void Logd(
        /* [in] */ const String& message);

    static Int32 InitPhoneCount();

    static CARAPI_(AutoPtr<ArrayOf<CardState> >) InitCardState();
    static CARAPI_(AutoPtr<ArrayOf<IIccFileHandler*> >) InitIccFileHandler();
    static CARAPI_(AutoPtr<ArrayOf<String> >) InitIccId();
    static CARAPI_(AutoPtr<ArrayOf<Int32> >) InitInsertSimState();

private:
    static const String LOGTAG;
    static const Int32 PROJECT_SIM_NUM;
    static const Int32 EVENT_OFFSET;
    static const Int32 EVENT_QUERY_ICCID_DONE;
    static const Int32 EVENT_ICC_CHANGED;
    static const Int32 EVENT_STACK_READY;
    static const String ICCID_STRING_FOR_NO_SIM;
    static const String ICCID_STRING_FOR_NV;

    static AutoPtr<ArrayOf<IPhone*> > sPhone;
    static AutoPtr<IContext> sContext;
    static AutoPtr<ArrayOf<CardState> > sCardState;
    static AutoPtr<IUiccController> mUiccController;
    static AutoPtr<ArrayOf<ICommandsInterface*> > sCi;
    static AutoPtr<ArrayOf<IIccFileHandler*> > sFh;
    static AutoPtr<ArrayOf<String> > sIccId;
    static AutoPtr<ArrayOf<Int32> > sInsertSimState;
    static AutoPtr<ITelephonyManager> sTelephonyMgr;
    static AutoPtr<ITelephonyManagerHelper> sTelephonyManagerHelper;
    // To prevent repeatedly update flow every time receiver SIM_STATE_CHANGE
    static Boolean sNeedUpdate;
    Boolean isNVSubAvailable;

    AutoPtr<MyBroadcastReceiver> sReceiver;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_SUBINFORECORDUPDATER_H__
