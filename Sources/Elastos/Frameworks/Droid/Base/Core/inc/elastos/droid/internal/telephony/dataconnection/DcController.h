
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCCONTROLLER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCCONTROLLER_H__

#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/internal/utility/State.h>
#include <elastos/droid/internal/utility/StateMachine.h>
#include <elastos/droid/os/Process.h>

using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Utility::State;
using Elastos::Droid::Internal::Utility::StateMachine;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::Process;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

/**
 * Data Connection Controller which is a package visible class and controls
 * multiple data connections. For instance listening for unsolicited messages
 * and then demultiplexing them to the appropriate DC.
 */
class DcController
    : public StateMachine
    , public IDcController
{
private:
    class DccDefaultState
        : public State
    {
    public:
        DccDefaultState(
            /* [in] */ DcController* host);

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI Exit();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        /**
         * Process the new list of "known" Data Calls
         * @param dcsList as sent by RIL_UNSOL_DATA_CALL_LIST_CHANGED
         */
        CARAPI OnDataStateChanged(
            /* [in] */ IArrayList* dcsList);

    private:
        DcController* mHost;
    };


public:
    CAR_INTERFACE_DECL()

    DcController();

    static CARAPI MakeDcc(
        /* [in] */ IPhoneBase* phone,
        /* [in] */ IDcTrackerBase* dct,
        /* [in] */ IHandler* handler,
        /* [out] */ IDcController** result);

    CARAPI Dispose();

    CARAPI AddDc(
        /* [in] */ IDataConnection* dc);

    CARAPI RemoveDc(
        /* [in] */ IDataConnection* dc);

    CARAPI AddActiveDcByCid(
        /* [in] */ IDataConnection* dc);

    CARAPI RemoveActiveDcByCid(
        /* [in] */ IDataConnection* dc);

    // @Override
    CARAPI Log(
        /* [in] */ const char *fmt, ...);

    // @Override
    CARAPI Loge(
        /* [in] */ const char *fmt, ...);

    /**
     * @return the string for msg.what as our info.
     */
    // @Override
    CARAPI GetWhatToString(
        /* [in] */ Int32 what,
        /* [out] */ String* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    // @Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

private:
    /**
     * Constructor.
     *
     * @param name to be used for the Controller
     * @param phone the phone associated with Dcc and Dct
     * @param dct the DataConnectionTracker associated with Dcc
     * @param handler defines the thread/looper to be used with Dcc
     */
    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ IPhoneBase* phone,
        /* [in] */ IDcTrackerBase* dct,
        /* [in] */ IHandler* handler);

    /**
     * lr is short name for logAndAddLogRec
     * @param s
     */
    CARAPI Lr(
        /* [in] */ const char *fmt, ...);

public:
    // package as its used by Testing code
    AutoPtr<IArrayList> mDcListAll;

    /**
     * Constants for the data connection activity:
     * physical link down/up
     *
     * TODO: Move to RILConstants.java
     */
    static const Int32 DATA_CONNECTION_ACTIVE_PH_LINK_INACTIVE;

    static const Int32 DATA_CONNECTION_ACTIVE_PH_LINK_DORMANT;

    static const Int32 DATA_CONNECTION_ACTIVE_PH_LINK_UP;

    static const Int32 DATA_CONNECTION_ACTIVE_UNKNOWN;

    // One of the DATA_CONNECTION_ACTIVE_XXX values
    Int32 mOverallDataConnectionActiveState;

private:
    static const Boolean DBG;

    static const Boolean VDBG;

    AutoPtr<IPhoneBase> mPhone;

    AutoPtr<IDcTrackerBase> mDct;

    AutoPtr<IDcTesterDeactivateAll> mDcTesterDeactivateAll;

    AutoPtr<IHashMap> mDcListActiveByCid;

    AutoPtr<DccDefaultState> mDccDefaultState;
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCCONTROLLER_H__
