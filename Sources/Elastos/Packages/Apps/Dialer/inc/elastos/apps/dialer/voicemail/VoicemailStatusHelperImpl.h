#ifndef __ELASTOS_APPS_DIALER_VOICEMAIL_VOICEMAILSTATUSHELPERIMPL_H__
#define __ELASTOS_APPS_DIALER_VOICEMAIL_VOICEMAILSTATUSHELPERIMPL_H__

#include "_Elastos.Apps.Dialer.h"
#include <elastos/core/Object.h>
#include "Elastos.Droid.Database.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Droid::Database::ICursor;
using Elastos::Core::IComparator;
using Elastos::Utility::IList;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Voicemail {

/** Implementation of {@link VoicemailStatusHelper}. */
class VoicemailStatusHelperImpl
    : public Object
    , public IVoicemailStatusHelperImpl
{
public:
    /** Possible user actions. */
    class Action
        : public Object
        , public IVoicemailStatusHelperImplAction
    {
    public:
        CAR_INTERFACE_DECL();

        CARAPI GetMessageId(
            /* [out] */ Int32* id);

    private:
        Action(
            /* [in] */ Int32 messageId);

    public:
        static Action NONE;
        static Action CALL_VOICEMAIL;
        static Action CONFIGURE_VOICEMAIL;

    private:
        Int32 mMessageId;
    };

    /**
     * Overall state of the source status. Each state is associated with the corresponding display
     * string and the corrective action. The states are also assigned a relative priority which is
     * used to order the messages from different sources.
     */
    class OverallState
        : public Object
    {
    public:
        CARAPI_(AutoPtr<Action>) GetAction();

        CARAPI_(Int32) GetPriority();

        CARAPI_(Int32) GetCallLogMessageId();

        CARAPI_(Int32) GetCallDetailsMessageId();

    private:
        OverallState(
            /* [in] */ Int32 priority,
            /* [in] */ Action* action,
            /* [in] */ Int32 callLogMessageId);

        OverallState(
            /* [in] */ Int32 priority,
            /* [in] */ Action* action,
            /* [in] */ Int32 callLogMessageId,
            /* [in] */ Int32 callDetailsMessageId);

    public:
        // TODO: Add separate string for call details and call log pages for the states that needs
        // to be shown in both.
        /** Both notification and data channel are not working. */
        static OverallState NO_CONNECTION;
        /** Notifications working, but data channel is not working. Audio cannot be downloaded. */
        static OverallState NO_DATA;
        /** Messages are known to be waiting but data channel is not working. */
        static OverallState MESSAGE_WAITING;
        /** Notification channel not working, but data channel is. */
        static OverallState NO_NOTIFICATIONS;
        /** Invite user to set up voicemail. */
        static OverallState INVITE_FOR_CONFIGURATION;
        /**
         * No detailed notifications, but data channel is working.
         * This is normal mode of operation for certain sources. No action needed.
         */
        static OverallState NO_DETAILED_NOTIFICATION;
        /** Visual voicemail not yet set up. No local action needed. */
        static OverallState NOT_CONFIGURED;
        /** Everything is OK. */
        static OverallState OK;
        /** If one or more state value set by the source is not valid. */
        static OverallState INVALID;

    private:
        Int32 mPriority;
        AutoPtr<Action> mAction;
        Int32 mCallLogMessageId;
        Int32 mCallDetailsMessageId;
    };

private:
    /** A wrapper on {@link StatusMessage} which additionally stores the priority of the message. */
    class MessageStatusWithPriority
        : public Object
    {
    public:
        MessageStatusWithPriority(
            /* [in] */ IVoicemailStatusHelperStatusMessage* message,
            /* [in] */ Int32 priority);

    private:
        AutoPtr<IVoicemailStatusHelperStatusMessage> mMessage;
        Int32 mPriority;
    };

    class MyComparator
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL();

        MyComparator(
            /* [in] */ VoicemailStatusHelperImpl* host);

        // @Override
        CARAPI Compare(
            /* [in] */ IInterface* msg1,
            /* [in] */ IInterface* msg2,
            /* [out] */ Int32* result);

    private:
        VoicemailStatusHelperImpl* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    // @Override
    CARAPI GetStatusMessages(
        /* [in] */ ICursor* cursor,
        /* [out] */ IList** messages);

    // @Override
    CARAPI GetNumberActivityVoicemailSources(
        /* [in] */ ICursor* cursor,
        /* [out] */ Int32* number);

private:
    /** Returns whether the source status in the cursor corresponds to an active source. */
    CARAPI_(Boolean) IsVoicemailSourceActive(
        /* [in] */ ICursor* cursor);

    CARAPI_(AutoPtr<IList>) ReorderMessages(
        /* [in] */ IList* messageWrappers);

    /**
     * Returns the message for the status entry pointed to by the cursor.
     */
    CARAPI_(AutoPtr<MessageStatusWithPriority>) GetMessageForStatusEntry(
        /* [in] */ ICursor* cursor);

    CARAPI_(AutoPtr<OverallState>) GetOverallState(
        /* [in] */ Int32 configurationState,
        /* [in] */ Int32 dataChannelState,
        /* [in] */ Int32 notificationChannelState);

public:
    /** Projection on the voicemail_status table used by this class. */
    static const AutoPtr<ArrayOf<String> > PROJECTION;

private:
    static const Int32 SOURCE_PACKAGE_INDEX; // = 0;
    static const Int32 CONFIGURATION_STATE_INDEX; // = 1;
    static const Int32 DATA_CHANNEL_STATE_INDEX; // = 2;
    static const Int32 NOTIFICATION_CHANNEL_STATE_INDEX; // = 3;
    static const Int32 SETTINGS_URI_INDEX; // = 4;
    static const Int32 VOICEMAIL_ACCESS_URI_INDEX; // = 5;
    static const Int32 NUM_COLUMNS; // = 6;
};

} // Voicemail
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_VOICEMAIL_VOICEMAILSTATUSHELPERIMPL_H__
