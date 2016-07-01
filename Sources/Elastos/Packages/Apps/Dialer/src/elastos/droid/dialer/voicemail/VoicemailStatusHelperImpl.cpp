
#include "elastos/droid/dialer/voicemail/VoicemailStatusHelperImpl.h"
#include "Elastos.Droid.Net.h"

using Elastos::Droid::Net::IUri;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Voicemail {

//================================================================
// VoicemailStatusHelperImpl::Action
//================================================================
Action VoicemailStatusHelperImpl::Action::NONE(-1);
Action VoicemailStatusHelperImpl::Action::CALL_VOICEMAIL(R::string::voicemail_status_action_call_server);
Action VoicemailStatusHelperImpl::Action::CONFIGURE_VOICEMAIL(R::string::voicemail_status_action_configure);

CAR_INTERFACE_IMPL(VoicemailStatusHelperImpl::Action, Object, IVoicemailStatusHelperImplAction);

VoicemailStatusHelperImpl::Action::Action(
    /* [in] */ Int32 messageId)
    : mMessageId(messageId)
{}

ECode VoicemailStatusHelperImpl::Action::GetMessageId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mMessageId;
    return NOERROR;
}

//================================================================
// VoicemailStatusHelperImpl::OverallState
//================================================================
OverallState VoicemailStatusHelperImpl::OverallState::NO_CONNECTION(0,
        &Action::CALL_VOICEMAIL, R::string::voicemail_status_voicemail_not_available,
        R::string::voicemail_status_audio_not_available);
OverallState VoicemailStatusHelperImpl::OverallState::NO_DATA(1,
        &Action::CALL_VOICEMAIL, R::string::voicemail_status_voicemail_not_available,
        R::string::voicemail_status_audio_not_available);
OverallState VoicemailStatusHelperImpl::OverallState::MESSAGE_WAITING(2,
        &Action::CALL_VOICEMAIL, R::string::voicemail_status_messages_waiting,
        R::string::voicemail_status_audio_not_available);
OverallState VoicemailStatusHelperImpl::OverallState::NO_NOTIFICATIONS(3,
        &Action::CALL_VOICEMAIL, R::string::voicemail_status_voicemail_not_available);
OverallState VoicemailStatusHelperImpl::OverallState::INVITE_FOR_CONFIGURATION(4,
        &Action::CONFIGURE_VOICEMAIL, R::string::voicemail_status_configure_voicemail);
OverallState VoicemailStatusHelperImpl::OverallState::NO_DETAILED_NOTIFICATION(
        5, &Action::NONE, -1);
OverallState VoicemailStatusHelperImpl::OverallState::NOT_CONFIGURED(
        6, &Action::NONE, -1);
OverallState VoicemailStatusHelperImpl::OverallState::OK(
        7, &Action::NONE, -1);
OverallState VoicemailStatusHelperImpl::OverallState::INVALID(
        8, &Action::NONE, -1);

VoicemailStatusHelperImpl::OverallState::OverallState(
    /* [in] */ Int32 priority,
    /* [in] */ Action* action,
    /* [in] */ Int32 callLogMessageId)
{
    OverallState(priority, action, callLogMessageId, -1);
}

VoicemailStatusHelperImpl::OverallState::OverallState(
    /* [in] */ Int32 priority,
    /* [in] */ Action* action,
    /* [in] */ Int32 callLogMessageId,
    /* [in] */ Int32 callDetailsMessageId)
    : mPriority(priority)
    , mAction(action)
    , mCallLogMessageId(callLogMessageId)
    , mCallDetailsMessageId(callDetailsMessageId)
{}

AutoPtr<Action> VoicemailStatusHelperImpl::OverallState::GetAction()
{
    return mAction;
}

Int32 VoicemailStatusHelperImpl::OverallState::GetPriority()
{
    return mPriority;
}

Int32 VoicemailStatusHelperImpl::OverallState::GetCallLogMessageId()
{
    return mCallLogMessageId;
}

Int32 VoicemailStatusHelperImpl::OverallState::GetCallDetailsMessageId()
{
    return mCallDetailsMessageId;
}

//================================================================
// VoicemailStatusHelperImpl::MessageStatusWithPriority
//================================================================
VoicemailStatusHelperImpl::MessageStatusWithPriority::MessageStatusWithPriority(
    /* [in] */ IVoicemailStatusHelperStatusMessage* message,
    /* [in] */ Int32 priority)
    : mMessage(message)
    , mPriority(priority)
{}

//================================================================
// VoicemailStatusHelperImpl::MyComparator
//================================================================
CAR_INTERFACE_IMPL(VoicemailStatusHelperImpl::MyComparator, Object, IComparator);

VoicemailStatusHelperImpl::MyComparator::MyComparator(
    /* [in] */ VoicemailStatusHelperImpl* host)
    : mHost(host)
{}

ECode VoicemailStatusHelperImpl::MyComparator::Compare(
    /* [in] */ IInterface* msg1,
    /* [in] */ IInterface* msg2,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = ((MessageStatusWithPriority*)msg1)->mPriority
            - ((MessageStatusWithPriority*)msg2)->mPriority;
    return NOERROR;
}

//================================================================
// VoicemailStatusHelperImpl
//================================================================

const Int32 VoicemailStatusHelperImpl::SOURCE_PACKAGE_INDEX = 0;
const Int32 VoicemailStatusHelperImpl::CONFIGURATION_STATE_INDEX = 1;
const Int32 VoicemailStatusHelperImpl::DATA_CHANNEL_STATE_INDEX = 2;
const Int32 VoicemailStatusHelperImpl::NOTIFICATION_CHANNEL_STATE_INDEX = 3;
const Int32 VoicemailStatusHelperImpl::SETTINGS_URI_INDEX = 4;
const Int32 VoicemailStatusHelperImpl::VOICEMAIL_ACCESS_URI_INDEX = 5;
const Int32 VoicemailStatusHelperImpl::NUM_COLUMNS = 6;

AutoPtr<ArrayOf<String> > CreatePROJECTION()
{
    AutoPtr<ArrayOf<String> > projection = ArrayOf<String>::Alloc(NUM_COLUMNS);
    projection->Set(SOURCE_PACKAGE_INDEX, IVoicemailContractStatus::SOURCE_PACKAGE);
    projection->Set(CONFIGURATION_STATE_INDEX, IVoicemailContractStatus::CONFIGURATION_STATE);
    projection->Set(DATA_CHANNEL_STATE_INDEX, IVoicemailContractStatus::DATA_CHANNEL_STATE);
    projection->Set(NOTIFICATION_CHANNEL_STATE_INDEX, IVoicemailContractStatus::NOTIFICATION_CHANNEL_STATE);
    projection->Set(SETTINGS_URI_INDEX, IVoicemailContractStatus::SETTINGS_URI);
    projection->Set(VOICEMAIL_ACCESS_URI_INDEX, IVoicemailContractStatus::VOICEMAIL_ACCESS_URI);
}

const AutoPtr<ArrayOf<String> > VoicemailStatusHelperImpl::PROJECTION = CreatePROJECTION();

CAR_INTERFACE_IMPL(VoicemailStatusHelperImpl, Object, IVoicemailStatusHelperImpl);

ECode VoicemailStatusHelperImpl::GetStatusMessages(
    /* [in] */ ICursor* cursor,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IList> messages;
    CArrayList::New((IList**)&messages);
    cursor->MoveToPosition(-1);
    Boolean succeeded;
    while(cursor->MoveToNext(&succeeded), succeeded) {
        AutoPtr<MessageStatusWithPriority> message = GetMessageForStatusEntry(cursor);
        if (message != NULL) {
            messages->Add((IInterface*)message);
        }
    }
    // Finally reorder the messages by their priority.
    AutoPtr<IList> list = ReorderMessages(messages);
    *result = list;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode VoicemailStatusHelperImpl::GetNumberActivityVoicemailSources(
    /* [in] */ ICursor* cursor,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    Int32 count = 0;
    cursor->MoveToPosition(-1);
    Boolean succeeded;
    while(cursor->MoveToNext(&succeeded), succeeded) {
        if (IsVoicemailSourceActive(cursor)) {
            ++count;
        }
    }
    *number = count;
    return NOERROR;
}

Boolean VoicemailStatusHelperImpl::IsVoicemailSourceActive(
    /* [in] */ ICursor* cursor)
{
    String str;
    cursor->GetString(SOURCE_PACKAGE_INDEX, &str);
    Int32 index;
    cursor->GetInt32(CONFIGURATION_STATE_INDEX, &index);
    return !str.IsNull()
            && index == IVoicemailContractStatus::CONFIGURATION_STATE_OK;
}

AutoPtr<IList> VoicemailStatusHelperImpl::ReorderMessages(
    /* [in] */ IList* messageWrappers)
{
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    collections->Sort(messageWrappers, new MyComparator(this))
    AutoPtr<IList> reorderMessages;
    CArrayList::New((IList**)&reorderMessages);
    // Copy the ordered message objects into the final list.
    AutoPtr<IIterator> it;
    messageWrappers->GetIteratro((IIterator**)&it);
    Boolean result;
    while (it->HasNext(&result), result) {
        AutoPtr<IInterface> messageWrapper;
        it->GetNext((IInterface**)&messageWrapper);
        reorderMessages->Add(((MessageStatusWithPriority*)messageWrapper)->mMessage);
    }
    return reorderMessages;
}

AutoPtr<MessageStatusWithPriority> VoicemailStatusHelperImpl::GetMessageForStatusEntry(
    /* [in] */ ICursor* cursor)
{
    String sourcePackage;
    cursor->GetString(SOURCE_PACKAGE_INDEX, &sourcePackage);
    if (sourcePackage.IsNull()) {
        return NULL;
    }
    Int32 configuration, dataChannel, notificationChannel;
    cursor->GetInt32(CONFIGURATION_STATE_INDEX, &configuration);
    cursor->GetInt32(DATA_CHANNEL_STATE_INDEX, &dataChannel);
    cursor->GetInt32(NOTIFICATION_CHANNEL_STATE_INDEX, &);
    AutoPtr<OverallState> overallState = GetOverallState(index,
            dataChannel, notificationChannel);
    AutoPtr<Action> action = overallState->GetAction();

    // No source package or no action, means no message shown.
    if (action == &Action::NONE) {
        return NULL;
    }

    AutoPtr<IUri> actionUri;
    if (action == &Action::CALL_VOICEMAIL) {
        String str;
        cursor->GetString(VOICEMAIL_ACCESS_URI_INDEX, &str);
        actionUri = UriUtils::ParseUriOrNull(str);
        // Even if actionUri is null, it is still be useful to show the notification.
    }
    else if (action == &Action::CONFIGURE_VOICEMAIL) {
        String str;
        cursor->GetString(SETTINGS_URI_INDEX, &str);
        assert(0 && "TODO");
        // actionUri = UriUtils::ParseUriOrNull(str);
        // If there is no settings URI, there is no point in showing the notification.
        if (actionUri == NULL) {
            return NULL;
        }
    }

    Int32 callLogMessageId, callLogDetailsMessageId;
    overallState->GetCallLogMessageId(&callLogMessageId);
    overallState->GetCallDetailsMessageId(&callLogDetailsMessageId);
    Int32 messageId;
    action->GetMessageId(&messageId);
    AutoPtr<IVoicemailStatusHelperStatusMessage> statusMessage = new StatusMessage(
            sourcePackage, callLogMessageId, callLogDetailsMessageId, messageId, actionUri);
    Int32 priority;
    overallState->GetPriority(&priority);
    return new MessageStatusWithPriority(
            (IVoicemailStatusHelperStatusMessage*)statusMessage, priority);
}

AutoPtr<OverallState> VoicemailStatusHelperImpl::GetOverallState(
    /* [in] */ Int32 configurationState,
    /* [in] */ Int32 dataChannelState,
    /* [in] */ Int32 notificationChannelState)
{
    if (configurationState == CONFIGURATION_STATE_OK) {
        // Voicemail is configured. Let's see how is the data channel.
        if (dataChannelState == DATA_CHANNEL_STATE_OK) {
            // Data channel is fine. What about notification channel?
            if (notificationChannelState == NOTIFICATION_CHANNEL_STATE_OK) {
                return &OverallState::OK;
            }
            else if (notificationChannelState == NOTIFICATION_CHANNEL_STATE_MESSAGE_WAITING) {
                return &OverallState::NO_DETAILED_NOTIFICATION;
            }
            else if (notificationChannelState == NOTIFICATION_CHANNEL_STATE_NO_CONNECTION) {
                return &OverallState::NO_NOTIFICATIONS;
            }
        }
        else if (dataChannelState == DATA_CHANNEL_STATE_NO_CONNECTION) {
            // Data channel is not working. What about notification channel?
            if (notificationChannelState == NOTIFICATION_CHANNEL_STATE_OK) {
                return &OverallState::NO_DATA;
            }
            else if (notificationChannelState == NOTIFICATION_CHANNEL_STATE_MESSAGE_WAITING) {
                return &OverallState::MESSAGE_WAITING;
            }
            else if (notificationChannelState == NOTIFICATION_CHANNEL_STATE_NO_CONNECTION) {
                return &OverallState::NO_CONNECTION;
            }
        }
    }
    else if (configurationState == CONFIGURATION_STATE_CAN_BE_CONFIGURED) {
        // Voicemail not configured. data/notification channel states are irrelevant.
        return &OverallState::INVITE_FOR_CONFIGURATION;
    }
    else if (configurationState == IVoicemailContractStatus::CONFIGURATION_STATE_NOT_CONFIGURED) {
        // Voicemail not configured. data/notification channel states are irrelevant.
        return &OverallStat::NOT_CONFIGURED;
    }
    // Will reach here only if the source has set an invalid value.
    return &OverallState::INVALID;
}

} // Voicemail
} // Dialer
} // Droid
} // Elastos
