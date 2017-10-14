//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_INBOUNDSMSHANDLER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_INBOUNDSMSHANDLER_H__

#include "elastos/droid/internal/telephony/CellBroadcastHandler.h"
#include "elastos/droid/internal/utility/State.h"
#include "elastos/droid/internal/utility/StateMachine.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/AsyncResult.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Internal::Utility::State;
using Elastos::Droid::Internal::Utility::StateMachine;
using Elastos::Droid::Telephony::ISmsMessage;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

class InboundSmsTracker;

/**
 * This class broadcasts incoming SMS messages to interested apps after storing them in
 * the SmsProvider "raw" table and ACKing them to the SMSC. After each message has been
 * broadcast, its parts are removed from the raw table. If the device crashes after ACKing
 * but before the broadcast completes, the pending messages will be rebroadcast on the next boot.
 *
 * <p>The state machine starts in {@link IdleState} state. When the {@link SMSDispatcher} receives a
 * new SMS from the radio, it calls {@link #dispatchNormalMessage},
 * which sends a message to the state machine, causing the wakelock to be acquired in
 * {@link #haltedProcessMessage}, which transitions to {@link DeliveringState} state, where the message
 * is saved to the raw table, then acknowledged via the {@link SMSDispatcher} which called us.
 *
 * <p>After saving the SMS, if the message is complete (either single-part or the final segment
 * of a multi-part SMS), we broadcast the completed PDUs as an ordered broadcast, then transition to
 * {@link WaitingState} state to wait for the broadcast to complete. When the local
 * {@link BroadcastReceiver} is called with the result, it sends {@link #EVENT_BROADCAST_COMPLETE}
 * to the state machine, causing us to either broadcast the next pending message (if one has
 * arrived while waiting for the broadcast to complete), or to transition back to the halted state
 * after all messages are processed. Then the wakelock is released and we wait for the next SMS.
 */
class InboundSmsHandler
    : public StateMachine
    , public IInboundSmsHandler
{
public:
    /**
     * This parent state throws an exception (for debug builds) or prints an error for unhandled
     * message types.
     */
    class DefaultState: public State
    {
    public:
        DefaultState(
            /* [in] */ InboundSmsHandler* host);

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName()
        {
            return String("Elastos.Droid.Internal.Telephony.InboundSmsHandler.DefaultState");
        }

    private:
        InboundSmsHandler* mHost;
    };

    /**
     * The Startup state waits for {@link SmsBroadcastUndelivered} to process the raw table and
     * notify the state machine to broadcast any complete PDUs that might not have been broadcast.
     */
    class StartupState: public State
    {
    public:
        StartupState(
            /* [in] */ InboundSmsHandler* host);

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName()
        {
            return String("Elastos.Droid.Internal.Telephony.InboundSmsHandler.StartupState");
        }

    private:
        InboundSmsHandler* mHost;
    };

    /**
     * In the idle state the wakelock is released until a new SM arrives, then we transition
     * to Delivering mode to handle it, acquiring the wakelock on exit.
     */
    class IdleState: public State
    {
    public:
        IdleState(
            /* [in] */ InboundSmsHandler* host);

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI Exit();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName()
        {
            return String("Elastos.Droid.Internal.Telephony.InboundSmsHandler.IdleState");
        }

    private:
        InboundSmsHandler* mHost;
    };

    /**
     * In the delivering state, the inbound SMS is processed and stored in the raw table.
     * The message is acknowledged before we exit this state. If there is a message to broadcast,
     * transition to {@link WaitingState} state to send the ordered broadcast and wait for the
     * results. When all messages have been processed, the halting state will release the wakelock.
     */
    class DeliveringState: public State
    {
    public:
        DeliveringState(
            /* [in] */ InboundSmsHandler* host);

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI Exit();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName()
        {
            return String("Elastos.Droid.Internal.Telephony.InboundSmsHandler.DeliveringState");
        }

    private:
        InboundSmsHandler* mHost;
    };

    /**
     * The waiting state delegates handling of new SMS to parent {@link DeliveringState}, but
     * defers handling of the {@link #EVENT_BROADCAST_SMS} phase until after the current
     * result receiver sends {@link #EVENT_BROADCAST_COMPLETE}. Before transitioning to
     * {@link DeliveringState}, {@link #EVENT_RETURN_TO_IDLE} is sent to transition to
     * {@link IdleState} after any deferred {@link #EVENT_BROADCAST_SMS} messages are handled.
     */
    class WaitingState: public State
    {
    public:
        WaitingState(
            /* [in] */ InboundSmsHandler* host);

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName()
        {
            return String("Elastos.Droid.Internal.Telephony.InboundSmsHandler.WaitingState");
        }

    private:
        InboundSmsHandler* mHost;
    };

    /**
     * Handler for an {@link InboundSmsTracker} broadcast. Deletes PDUs from the raw table and
     * logs the broadcast duration (as an error if the other receivers were especially slow).
     */
    class SmsBroadcastReceiver: public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("InboundSmsHandler::SmsBroadcastReceiver")

        SmsBroadcastReceiver(
            /* [in] */ InboundSmsTracker* tracker,
            /* [in] */ InboundSmsHandler* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        String mDeleteWhere;
        AutoPtr<ArrayOf<String> > mDeleteWhereArgs;
        Int64 mBroadcastTimeNano;
        InboundSmsHandler* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    /**
     * Tell the state machine to quit after processing all messages.
     */
    CARAPI Dispose();

    /**
     * Update the phone object when it changes.
     */
    CARAPI UpdatePhoneObject(
        /* [in] */ IPhoneBase* phone);

    // CAF_MSIM Is this used anywhere ? if not remove it
    CARAPI GetPhone(
        /* [out] */ IPhoneBase** result);

    virtual CARAPI_(void) HandleNewSms(
        /* [in] */ AsyncResult* ar);

    /**
     * This method is called when a new SMS PDU is injected into application framework.
     * @param ar is the AsyncResult that has the SMS PDU to be injected.
     */
    virtual CARAPI_(void) HandleInjectSms(
        /* [in] */ AsyncResult* ar);

    /**
     * Process an SMS message from the RIL, calling subclass methods to handle 3GPP and
     * 3GPP2-specific message types.
     *
     * @param smsb the SmsMessageBase object from the RIL
     * @return a result code from {@link android.provider.Telephony.Sms.Intents},
     *  or {@link Activity#RESULT_OK} for delayed acknowledgment to SMSC
     */
    CARAPI DispatchMessage(
        /* [in] */ ISmsMessageBase* smsb,
        /* [out] */ Int32* result);

    /**
     * Notify interested apps if the framework has rejected an incoming SMS,
     * and send an acknowledge message to the network.
     * @param success indicates that last message was successfully received.
     * @param result result code indicating any error
     * @param blacklistMatchType blacklist type if the message was blacklisted,
     *                           -1 if it wasn't blacklisted
     * @param sms incoming SMS
     * @param response callback message sent when operation completes.
     */
    virtual CARAPI_(void) NotifyAndAcknowledgeLastIncomingSms(
        /* [in] */ Boolean success,
        /* [in] */ Int32 result,
        /* [in] */ Int32 blacklistMatchType,
        /* [in] */ ISmsMessage* sms,
        /* [in] */ IMessage* response);

    /**
     * Process the inbound SMS segment. If the message is complete, send it as an ordered
     * broadcast to interested receivers and return TRUE. If the message is a segment of an
     * incomplete multi-part SMS, return FALSE.
     * @param tracker the tracker containing the message segment to process
     * @return TRUE if an ordered broadcast was sent; FALSE if waiting for more message segments
     */
    virtual CARAPI_(Boolean) ProcessMessagePart(
        /* [in] */ InboundSmsTracker* tracker);

    /**
     * Helper for {@link SmsBroadcastUndelivered} to delete an old message in the raw table.
     */
    virtual CARAPI_(void) DeleteFromRawTable(
        /* [in] */ const String& deleteWhere,
        /* [in] */ ArrayOf<String>* deleteWhereArgs);

    /**
     * Set the appropriate intent action and direct the intent to the default SMS app or the
     * appropriate port.
     *
     * @param intent the intent to set and direct
     * @param destPort the destination port
     */
    virtual CARAPI_(void) SetAndDirectIntent(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 destPort);

    /**
     * Returns whether the default message format for the current radio technology is 3GPP2.
     * @return TRUE if the radio technology uses 3GPP2 format by default, FALSE for 3GPP format
     */
    static CARAPI_(Boolean) IsCurrentFormat3gpp2();

protected:
    /**
     * Create a new SMS broadcast helper.
     * @param name the class name for logging
     * @param context the context of the phone app
     * @param storageMonitor the SmsStorageMonitor to check for storage availability
     */
    InboundSmsHandler(
        /* [in] */ const String& name,
        /* [in] */ IContext* context,
        /* [in] */ ISmsStorageMonitor* storageMonitor,
        /* [in] */ IPhoneBase* phone,
        /* [in] */ CellBroadcastHandler* cellBroadcastHandler);

    /**
     * Dispose of the WAP push object and release the wakelock.
     */
    // @Override
    CARAPI_(void) OnQuitting();

    /**
     * Process voicemail notification, SMS-PP data download, CDMA CMAS, CDMA WAP push, and other
     * 3GPP/3GPP2-specific messages. Regular SMS messages are handled by calling the shared
     * {@link #dispatchNormalMessage} from this class.
     *
     * @param smsb the SmsMessageBase object from the RIL
     * @return a result code from {@link android.provider.Telephony.Sms.Intents},
     *  or {@link Activity#RESULT_OK} for delayed acknowledgment to SMSC
     */
    virtual CARAPI_(Int32) DispatchMessageRadioSpecific(
        /* [in] */ ISmsMessageBase* smsb) = 0;

    /**
     * Send an acknowledge message to the SMSC.
     * @param success indicates that last message was successfully received.
     * @param result result code indicating any error
     * @param response callback message sent when operation completes.
     */
    virtual CARAPI_(void) AcknowledgeLastIncomingSms(
        /* [in] */ Boolean success,
        /* [in] */ Int32 result,
        /* [in] */ IMessage* response) = 0;

    /**
     * Called when the phone changes the default method updates mPhone
     * mStorageMonitor and mCellBroadcastHandler.updatePhoneObject.
     * Override if different or other behavior is desired.
     *
     * @param phone
     */
    virtual CARAPI_(void) OnUpdatePhoneObject(
        /* [in] */ IPhoneBase* phone);

    /**
     * Return TRUE if this handler is for 3GPP2 messages; FALSE for 3GPP format.
     * @return TRUE for the 3GPP2 handler; FALSE for the 3GPP handler
     */
    virtual CARAPI_(Boolean) Is3gpp2() = 0;

    /**
     * Dispatch a normal incoming SMS. This is called from {@link #dispatchMessageRadioSpecific}
     * if no format-specific handling was required. Saves the PDU to the SMS provider raw table,
     * creates an {@link InboundSmsTracker}, then sends it to the state machine as an
     * {@link #EVENT_BROADCAST_SMS}. Returns {@link Intents#RESULT_SMS_HANDLED} or an error value.
     *
     * @param sms the message to dispatch
     * @return {@link Intents#RESULT_SMS_HANDLED} if the message was accepted, or an error status
     */
    virtual CARAPI_(Int32) DispatchNormalMessage(
        /* [in] */ ISmsMessageBase* sms);

    /**
     * Helper to add the tracker to the raw table and then send a message to broadcast it, if
     * successful. Returns the SMS intent status to return to the SMSC.
     * @param tracker the tracker to save to the raw table and then deliver
     * @return {@link Intents#RESULT_SMS_HANDLED} or {@link Intents#RESULT_SMS_GENERIC_ERROR}
     * or {@link Intents#RESULT_SMS_DUPLICATED}
     */
    virtual CARAPI_(Int32) AddTrackerToRawTableAndSendMessage(
        /* [in] */ InboundSmsTracker* tracker);

    /**
     * Dispatch the intent with the specified permission, appOp, and result receiver, using
     * this state machine's handler thread to run the result receiver.
     *
     * @param intent the intent to broadcast
     * @param permission receivers are required to have this permission
     * @param appOp app op that is being performed when dispatching to a receiver
     * @param user user to deliver the intent to
     */
    virtual CARAPI_(void) DispatchIntent(
        /* [in] */ IIntent* intent,
        /* [in] */ const String& permission,
        /* [in] */ Int32 appOp,
        /* [in] */ IBroadcastReceiver* resultReceiver,
        /* [in] */ IUserHandle* user);

    virtual CARAPI_(void) StoreVoiceMailCount();

    /**
     * Log with debug level.
     * @param s the string to Log
     */
    // @Override
    virtual CARAPI_(void) Log(
        /* [in] */ const String& s);

    /**
     * Log with error level.
     * @param s the string to Log
     */
    // @Override
    virtual CARAPI_(void) Loge(
        /* [in] */ const String& s);

    /**
     * Log with error level.
     * @param s the string to Log
     * @param e is a Throwable which logs additional information.
     */
    // @Override
    virtual CARAPI_(void) Loge(
        /* [in] */ const String& s,
        /* [in] */ IThrowable* e);

private:
    CARAPI_(Int32) CheckIfBlacklisted(
        /* [in] */ ISmsMessageBase* sms);

    /**
     * Insert a message PDU into the raw table so we can acknowledge it immediately.
     * If the device crashes before the broadcast to listeners completes, it will be delivered
     * from the raw table on the next device boot. For single-part messages, the deleteWhere
     * and deleteWhereArgs fields of the tracker will be set to delete the correct row after
     * the ordered broadcast completes.
     *
     * @param tracker the tracker to add to the raw table
     * @return TRUE on success; FALSE on failure to write to database
     */
    CARAPI_(Int32) AddTrackerToRawTable(
        /* [in] */ InboundSmsTracker* tracker);

    /**
     * Store a received SMS into Telephony provider
     *
     * @param intent The intent containing the received SMS
     * @return The URI of written message
     */
    CARAPI_(AutoPtr<IUri>) WriteInboxMessage(
        /* [in] */ IIntent* intent);

    /**
     * Convert SmsMessage[] into SMS database schema columns
     *
     * @param msgs The SmsMessage array of the received SMS
     * @return ContentValues representing the columns of parsed SMS
     */
    static CARAPI_(AutoPtr<IContentValues>) ParseSmsMessage(
        /* [in] */ ArrayOf<ISmsMessage*>* msgs);

    /**
     * Build up the SMS message body from the SmsMessage array of received SMS
     *
     * @param msgs The SmsMessage array of the received SMS
     * @return The text message body
     */
    static CARAPI_(String) BuildMessageBodyFromPdus(
        /* [in] */ ArrayOf<ISmsMessage*>* msgs);

    // Some providers send formfeeds in their messages. Convert those formfeeds to newlines.
    static CARAPI_(String) ReplaceFormFeeds(
        /* [in] */ const String& s);

    static CARAPI_(AutoPtr<ArrayOf<String> >) InitStatic(
        /* [in] */ const String& flag);

    static CARAPI_(AutoPtr<IUri>) InitRawUri();

public:
    static const Boolean DBG;

    static const Int32 PDU_COLUMN;
    static const Int32 SEQUENCE_COLUMN;
    static const Int32 DESTINATION_PORT_COLUMN;
    static const Int32 DATE_COLUMN;
    static const Int32 REFERENCE_NUMBER_COLUMN;
    static const Int32 COUNT_COLUMN;
    static const Int32 ADDRESS_COLUMN;
    static const Int32 ID_COLUMN;

    static const String SELECT_BY_ID;
    static const String SELECT_BY_REFERENCE;

    /** Message type containing a {@link InboundSmsTracker} ready to broadcast to listeners. */
    static const Int32 EVENT_BROADCAST_SMS;

    /** Message from resultReceiver notifying {@link WaitingState} of a completed broadcast. */
    static const Int32 EVENT_BROADCAST_COMPLETE;

    /** Sent on exit from {@link WaitingState} to return to idle after sending all broadcasts. */
    static const Int32 EVENT_RETURN_TO_IDLE;

    /** Release wakelock after a short timeout when returning to idle state. */
    static const Int32 EVENT_RELEASE_WAKELOCK;

    /** Sent by {@link SmsBroadcastUndelivered} after cleaning the raw table. */
    static const Int32 EVENT_START_ACCEPTING_SMS;

    /** Update phone object */
    static const Int32 EVENT_UPDATE_PHONE_OBJECT;

    AutoPtr<IContext> mContext;

    /** Helper class to check whether storage is available for incoming messages. */
    AutoPtr<ISmsStorageMonitor> mStorageMonitor;

    AutoPtr<IPhoneBase> mPhone;

    AutoPtr<CellBroadcastHandler> mCellBroadcastHandler;

private:
    static const Boolean VDBG;  // STOPSHIP if TRUE, logs user data

    /** Query projection for checking for duplicate message segments. */
    static AutoPtr<ArrayOf<String> > PDU_PROJECTION;

    /** Query projection for combining concatenated message segments. */
    static AutoPtr<ArrayOf<String> > PDU_SEQUENCE_PORT_PROJECTION;

    /** Wakelock release delay when returning to idle state. */
    static const Int32 WAKELOCK_TIMEOUT;

    /** URI for raw table of SMS provider. */
    static AutoPtr<IUri> sRawUri;

    AutoPtr<IContentResolver> mResolver;

    /** Special handler for WAP push messages. */
    AutoPtr<IWapPushOverSms> mWapPush;

    /** Wake lock to ensure device stays awake while dispatching the SMS intents. */
    AutoPtr<IPowerManagerWakeLock> mWakeLock;

    /** DefaultState throws an exception or logs an error for unhandled message types. */
    AutoPtr<DefaultState> mDefaultState;

    /** Startup state. Waiting for {@link SmsBroadcastUndelivered} to complete. */
    AutoPtr<StartupState> mStartupState;

    /** Idle state. Waiting for messages to process. */
    AutoPtr<IdleState> mIdleState;

    /** Delivering state. Saves the PDU in the raw table and acknowledges to SMSC. */
    AutoPtr<DeliveringState> mDeliveringState;

    /** Broadcasting state. Waits for current broadcast to complete before delivering next. */
    AutoPtr<WaitingState> mWaitingState;

    Boolean mSmsReceiveDisabled;

    AutoPtr<IUserManager> mUserManager;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_INBOUNDSMSHANDLER_H__
