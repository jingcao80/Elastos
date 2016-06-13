
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CALLTRACKER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CALLTRACKER_H__

#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "_Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/DriverCall.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::Handler;
using Elastos::Core::IThrowable;
using Elastos::Core::Object;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/**
 * {@hide}
 */
class CallTracker: public Handler
{
public:
    CallTracker();

    virtual ~CallTracker();

    //***** Overridden from Handler
    // @Override
    virtual CARAPI HandleMessage(
        /* [in] */ IMessage* msg) = 0;

    virtual CARAPI RegisterForVoiceCallStarted(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj) = 0;

    virtual CARAPI UnregisterForVoiceCallStarted(
        /* [in] */ IHandler* h) = 0;

    virtual CARAPI RegisterForVoiceCallEnded(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj) = 0;

    virtual CARAPI UnregisterForVoiceCallEnded(
        /* [in] */ IHandler* h) = 0;

    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    virtual CARAPI_(void) NotifySrvccState(
        /* [in] */ ICallSrvccState state,
        /* [in] */ IArrayList* c);

protected:
    virtual CARAPI_(void) PollCallsWhenSafe();

    virtual CARAPI_(void) PollCallsAfterDelay();

    virtual CARAPI_(Boolean) IsCommandExceptionRadioNotAvailable(
        /* [in] */ IThrowable* e);

    virtual CARAPI_(void) HandlePollCalls(
        /* [in] */ AsyncResult* ar) = 0;

    virtual CARAPI_(AutoPtr<IConnection>) GetHoConnection(
        /* [in] */ DriverCall* dc);

    virtual CARAPI_(void) HandleRadioAvailable();

    /**
     * Obtain a complete message that indicates that this operation
     * does not require polling of getCurrentCalls(). However, if other
     * operations that do need getCurrentCalls() are pending or are
     * scheduled while this operation is pending, the invocation
     * of getCurrentCalls() will be postponed until this
     * operation is also complete.
     */
    virtual CARAPI_(AutoPtr<IMessage>) ObtainNoPollCompleteMessage(
        /* [in] */ Int32 what);

    /**
     * Routine called from dial to check if the number is a test Emergency number
     * and if so remap the number. This allows a short emergency number to be remapped
     * to a regular number for testing how the frameworks handles emergency numbers
     * without actually calling an emergency number.
     *
     * This is not a full test and is not a substitute for testing real emergency
     * numbers but can be useful.
     *
     * To use this feature set a system property ril.test.emergencynumber to a pair of
     * numbers separated by a colon. If the first number matches the number parameter
     * this routine returns the second number. Example:
     *
     * ril.test.emergencynumber=112:1-123-123-45678
     *
     * To test Dial 112 take call then hang up on MO device to enter ECM
     * see RIL#processSolicited RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND
     *
     * @param dialString to test if it should be remapped
     * @return the same number or the remapped number.
     */
    virtual CARAPI_(String) CheckForTestEmergencyNumber(
        /* [in] */ const String& dialString);

    virtual CARAPI_(String) ConvertNumberIfNecessary(
        /* [in] */ IPhoneBase* phoneBase,
        /* [in] */ const String& dialNumber);

    virtual CARAPI_(void) Log(
        /* [in] */ const String& msg) = 0;

private:
    /**
     * @return TRUE if we're idle or there's a call to getCurrentCalls() pending
     * but nothing else
     */
    CARAPI_(Boolean) CheckNoOperationsPending();

    CARAPI_(Boolean) CompareGid1(
        /* [in] */ IPhoneBase* phoneBase,
        /* [in] */ const String& serviceGid1);

public:
    AutoPtr<ICommandsInterface> mCi;

protected:
    //***** Constants

    static const Int32 POLL_DELAY_MSEC;

    Int32 mPendingOperations;
    Boolean mNeedsPoll;
    AutoPtr<IMessage> mLastRelevantPoll;
    AutoPtr<IArrayList> mHandoverConnections;

    Boolean mNumberConverted;

    //***** Events
    static const Int32 EVENT_POLL_CALLS_RESULT;
    static const Int32 EVENT_CALL_STATE_CHANGE;
    static const Int32 EVENT_REPOLL_AFTER_DELAY;
    static const Int32 EVENT_OPERATION_COMPLETE;
    static const Int32 EVENT_GET_LAST_CALL_FAIL_CAUSE;

    static const Int32 EVENT_SWITCH_RESULT;
    static const Int32 EVENT_RADIO_AVAILABLE;
    static const Int32 EVENT_RADIO_NOT_AVAILABLE;
    static const Int32 EVENT_CONFERENCE_RESULT;
    static const Int32 EVENT_SEPARATE_RESULT;
    static const Int32 EVENT_ECT_RESULT;
    static const Int32 EVENT_EXIT_ECM_RESPONSE_CDMA;
    static const Int32 EVENT_CALL_WAITING_INFO_CDMA;
    static const Int32 EVENT_THREE_WAY_DIAL_L2_RESULT_CDMA;
    static const Int32 EVENT_THREE_WAY_DIAL_BLANK_FLASH;

private:
    static const Boolean DBG_POLL;
    const Int32 VALID_COMPARE_LENGTH;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CALLTRACKER_H__
