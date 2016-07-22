#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_UICCCONTROLLER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_UICCCONTROLLER_H__

#include "_Elastos.Droid.Internal.h"
#include "elastos/core/Object.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/RegistrantList.h"

// import android.content.Context;
// import android.content.Intent;
// import android.os.Message;
// import android.os.Registrant;
// import android.os.SystemProperties;
// import android.telephony.TelephonyManager;
// import android.telephony.Rlog;
// import android.telephony.ServiceState;
// import com.android.internal.telephony.CommandsInterface;
// import com.android.internal.telephony.PhoneConstants;
// import com.android.internal.telephony.SubscriptionController;
// import com.android.internal.telephony.uicc.IccCardApplicationStatus.AppState;
// import java.io.FileDescriptor;
// import java.io.PrintWriter;
// import java.nio.ByteBuffer;
// import java.nio.ByteOrder;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::RegistrantList;
//using Elastos::Nio::IByteBuffer;
using Elastos::IO::IByteBuffer;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Core::IInteger32;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

/**
  * This class is responsible for keeping all knowledge about
  * Universal Integrated Circuit Card (UICC), also know as SIM's,
  * in the system. It is also used as API to get appropriate
  * applications to pass them to phone and service trackers.
  *
  * UiccController is created with the call to make() function.
  * UiccController is a singleton and make() must only be called once
  * and throws an exception if called multiple times.
  *
  * Once created UiccController registers with RIL for "on" and "unsol_sim_status_changed"
  * notifications. When such notification arrives UiccController will call
  * getIccCardStatus (GET_SIM_STATUS). Based on the response of GET_SIM_STATUS
  * request appropriate tree of uicc objects will be created.
  *
  * Following is class diagram for uicc classes:
  *
  *                       UiccController
  *                            #
  *                            |
  *                        UiccCard
  *                          #   #
  *                          |   ------------------
  *                    UiccCardApplication    CatService
  *                      #            #
  *                      |            |
  *                 IccRecords    IccFileHandler
  *                 ^ ^ ^           ^ ^ ^ ^ ^
  *    SIMRecords---- | |           | | | | ---SIMFileHandler
  *    RuimRecords----- |           | | | ----RuimFileHandler
  *    IsimUiccRecords---           | | -----UsimFileHandler
  *                                 | ------CsimFileHandler
  *                                 ----IsimFileHandler
  *
  * Legend: # stands for Composition
  *         ^ stands for Generalization
  *
  * See also {@link com.android.internal.telephony.IccCard}
  * and {@link com.android.internal.telephony.uicc.IccCardProxy}
  */
class UiccController
    : public Handler
    , public IUiccController
{
public:
    TO_STRING_IMPL("UiccController")

    CAR_INTERFACE_DECL();

    UiccController();

    CARAPI constructor(
        /* [in] */ IContext* c,
        /* [in] */ ArrayOf<ICommandsInterface*>* ci);

    /*
     public static UiccController make(Context c, CommandsInterface ci) {
         synchronized (mLock) {
             if (mInstance != null) {
                 throw new RuntimeException("UiccController.make() should only be called once");
             }
             mInstance = new UiccController(c, ci);
             return mInstance;
         }
     }
 */
    static CARAPI_(AutoPtr<IUiccController>) Make(
        /* [in] */ IContext* c,
        /* [in] */ ArrayOf<ICommandsInterface*>* ci);

    static CARAPI_(AutoPtr<IUiccController>) GetInstance();

    virtual CARAPI GetUiccCard(
        /* [out] */ IUiccCard** result);

    virtual CARAPI GetUiccCard(
        /* [in] */ Int32 slotId,
        /* [out] */ IUiccCard** result);

    virtual CARAPI GetUiccCards(
        /* [out] */ ArrayOf<IUiccCard*>** result);

    // Easy to use API
    virtual CARAPI GetUiccCardApplication(
        /* [in] */ Int32 family,
        /* [out] */ IUiccCardApplication** result);

    /*
     // Easy to use API
     public IccRecords getIccRecords(int family) {
         synchronized (mLock) {
             if (mUiccCard != null) {
                 UiccCardApplication app = mUiccCard.getApplication(family);
                 if (app != null) {
                     return app.getIccRecords();
                 }
             }
             return null;
         }
     }
 */
    // Easy to use API
    virtual CARAPI GetIccRecords(
        /* [in] */ Int32 slotId,
        /* [in] */ Int32 family,
        /* [out] */ IIccRecords** result);

    /*
     // Easy to use API
     public IccFileHandler getIccFileHandler(int family) {
         synchronized (mLock) {
             if (mUiccCard != null) {
                 UiccCardApplication app = mUiccCard.getApplication(family);
                 if (app != null) {
                     return app.getIccFileHandler();
                 }
             }
             return null;
         }
     }
 */
    // Easy to use API
    virtual CARAPI GetIccFileHandler(
        /* [in] */ Int32 slotId,
        /* [in] */ Int32 family,
        /* [out] */ IIccFileHandler** result);

    static CARAPI_(Int32) GetFamilyFromRadioTechnology(
        /* [in] */ Int32 radioTechnology);

    //Notifies when card status changes
    virtual CARAPI RegisterForIccChanged(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    virtual CARAPI UnregisterForIccChanged(
        /* [in] */ IHandler* h);

    // @Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    static CARAPI_(AutoPtr<IIccRefreshResponse>) ParseOemSimRefresh(
        /* [in] */ IByteBuffer* payload);

    /*
     private UiccController(Context c, CommandsInterface ci) {
         if (DBG) log("Creating UiccController");
         mContext = c;
         mCi = ci;
         mCi.registerForIccStatusChanged(this, EVENT_ICC_STATUS_CHANGED, null);
         // This is needed so that we query for sim status in the case when we boot in APM
         mCi.registerForAvailable(this, EVENT_ICC_STATUS_CHANGED, null);
     }
 */
    // Easy to use API
    virtual CARAPI GetUiccCardApplication(
        /* [in] */ Int32 slotId,
        /* [in] */ Int32 family,
        /* [out] */ IUiccCardApplication** result);

    virtual CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

private:
    CARAPI_(AutoPtr<ArrayOf<IUiccCard*> >) MiddleInitMuicccards();

    CARAPI_(AutoPtr<IInteger32>) GetCiIndex(
        /* [in] */ IMessage* msg);

    CARAPI_(void) HandleRefresh(
        /* [in] */ IIccRefreshResponse* refreshResponse,
        /* [in] */ Int32 index);

    // synchronized
    CARAPI_(void) OnGetIccCardStatusDone(
        /* [in] */ AsyncResult* ar,
        /* [in] */ IInteger32* index);

    CARAPI_(Boolean) IsValidCardIndex(
        /* [in] */ Int32 index);

    CARAPI_(void) Log(
        /* [in] */ const String& string);

protected:
    /*
     private CommandsInterface mCi;
     private UiccCard mUiccCard;
 */
    AutoPtr<RegistrantList> mIccChangedRegistrants;

private:
    static const Boolean DBG;
    static const String LOGTAG;
    static const Int32 EVENT_ICC_STATUS_CHANGED = 1;
    static const Int32 EVENT_GET_ICC_STATUS_DONE = 2;
    static const Int32 EVENT_RADIO_UNAVAILABLE = 3;
    static const Int32 EVENT_REFRESH = 4;
    static const Int32 EVENT_REFRESH_OEM = 5;
    AutoPtr<ArrayOf<ICommandsInterface*> > mCis;
    AutoPtr<ArrayOf<IUiccCard*> > mUiccCards;
    static Object mLock;
    static AutoPtr<IUiccController> mInstance;
    AutoPtr<IContext> mContext;
    Boolean mOEMHookSimRefresh;
    static Object THIS;
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_UICCCONTROLLER_H__
