
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/contacts/common/util/UriUtils.h"
#include "elastos/droid/dialer/voicemail/VoicemailStatusHelperImpl.h"
#include "R.h"

using Elastos::Droid::Contacts::Common::Util::UriUtils;
using Elastos::Droid::Dialer::Voicemail::EIID_IVoicemailStatusHelper;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Provider::IVoicemailContractStatus;
using Elastos::Core::EIID_IComparator;
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
Action VoicemailStatusHelperImpl::Action::CALL_VOICEMAIL(Elastos::Droid::Dialer::R::string::voicemail_status_action_call_server);
Action VoicemailStatusHelperImpl::Action::CONFIGURE_VOICEMAIL(Elastos::Droid::Dialer::R::string::voicemail_status_action_configure);

Int32 VoicemailStatusHelperImpl::Action::GetMessageId()
{
    return mMessageId;
}


//================================================================
// VoicemailStatusHelperImpl::OverallState
//================================================================
OverallState VoicemailStatusHelperImpl::OverallState::NO_CONNECTION(0,
        &Action::CALL_VOICEMAIL, Elastos::Droid::Dialer::R::string::voicemail_status_voicemail_not_available,
        Elastos::Droid::Dialer::R::string::voicemail_status_audio_not_available);
OverallState VoicemailStatusHelperImpl::OverallState::NO_DATA(1,
        &Action::CALL_VOICEMAIL, Elastos::Droid::Dialer::R::string::voicemail_status_voicemail_not_available,
        Elastos::Droid::Dialer::R::string::voicemail_status_audio_not_available);
OverallState VoicemailStatusHelperImpl::OverallState::MESSAGE_WAITING(2,
        &Action::CALL_VOICEMAIL, Elastos::Droid::Dialer::R::string::voicemail_status_messages_waiting,
        Elastos::Droid::Dialer::R::string::voicemail_status_audio_not_available);
OverallState VoicemailStatusHelperImpl::OverallState::NO_NOTIFICATIONS(3,
        &Action::CALL_VOICEMAIL, Elastos::Droid::Dialer::R::string::voicemail_status_voicemail_not_available);
OverallState VoicemailStatusHelperImpl::OverallState::INVITE_FOR_CONFIGURATION(4,
        &Action::CONFIGURE_VOICEMAIL, Elastos::Droid::Dialer::R::string::voicemail_status_configure_voicemail);
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
    : mPriority(priority)
    , mAction(action)
    , mCallLogMessageId(callLogMessageId)
    , mCallDetailsMessageId(-1)
{}

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
// VoicemailStatusHelperImpl::MyComparator
//================================================================
CAR_INTERFACE_IMPL(VoicemailStatusHelperImpl::MyComparator, Object, IComparator)

ECode VoicemailStatusHelperImpl::MyComparator::Compare(
    /* [in] */ IInterface* msg1,
    /* [in] */ IInterface* msg2,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = ((MessageStatusWithPriority*)msg1)->mPriority
            - ((MessageStatusWithPriority*)msg2)->mPriority;
    return NOERROR;
}


//================================================================
// VoicemailStatusHelperImpl
//================================================================

static AutoPtr<ArrayOf<String> > CreatePROJECTION()
{
    AutoPtr<ArrayOf<String> > projection = ArrayOf<String>::Alloc(NUM_COLUMNS);
    (*projection)[SOURCE_PACKAGE_INDEX] = IVoicemailContractStatus::SOURCE_PACKAGE;
    (*projection)[CONFIGURATION_STATE_INDEX] = IVoicemailContractStatus::CONFIGURATION_STATE;
    (*projection)[DATA_CHANNEL_STATE_INDEX] = IVoicemailContractStatus::DATA_CHANNEL_STATE;
    (*projection)[NOTIFICATION_CHANNEL_STATE_INDEX] = IVoicemailContractStatus::NOTIFICATION_CHANNEL_STATE;
    (*projection)[SETTINGS_URI_INDEX] = IVoicemailContractStatus::SETTINGS_URI;
    (*projection)[VOICEMAIL_ACCESS_URI_INDEX] = IVoicemailContractStatus::VOICEMAIL_ACCESS_URI;
    return projection;
}
const AutoPtr<ArrayOf<String> > VoicemailStatusHelperImpl::PROJECTION = CreatePROJECTION();

const Int32 VoicemailStatusHelperImpl::SOURCE_PACKAGE_INDEX;
const Int32 VoicemailStatusHelperImpl::CONFIGURATION_STATE_INDEX;
const Int32 VoicemailStatusHelperImpl::DATA_CHANNEL_STATE_INDEX;
const Int32 VoicemailStatusHelperImpl::NOTIFICATION_CHANNEL_STATE_INDEX;
const Int32 VoicemailStatusHelperImpl::SETTINGS_URI_INDEX;
const Int32 VoicemailStatusHelperImpl::VOICEMAIL_ACCESS_URI_INDEX;
const Int32 VoicemailStatusHelperImpl::NUM_COLUMNS;

CAR_INTERFACE_IMPL(VoicemailStatusHelperImpl, Object, IVoicemailStatusHelperImpl);

ECode VoicemailStatusHelperImpl::GetStatusMessages(
    /* [in] */ ICursor* cursor,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IList> messages;
    CArrayList::New((IList**)&messages);
    Boolean succeeded;
    cursor->MoveToPosition(-1, &succeeded);
    while(cursor->MoveToNext(&succeeded), succeeded) {
        AutoPtr<MessageStatusWithPriority> message = GetMessageForStatusEntry(cursor);
        if (message != NULL) {
            messages->Add((IInterface*)(IObject*)message);
        }
    }
    // Finally reorder the messages by their priority.
    AutoPtr<IList> list = ReorderMessages(messages);
    *result = list;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode VoicemailStatusHelperImpl::GetNumberActivityVoicemailSources(
    /* [in] */ ICursor* cursor,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    Int32 count = 0;
    Boolean succeeded;
    cursor->MoveToPosition(-1, &succeeded);
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
    Int32 index;
    return (cursor->GetString(SOURCE_PACKAGE_INDEX, &str), !str.IsNull())
            && (cursor->GetInt32(CONFIGURATION_STATE_INDEX, &index), index == IVoicemailContractStatus::CONFIGURATION_STATE_OK);
}

AutoPtr<IList> VoicemailStatusHelperImpl::ReorderMessages(
    /* [in] */ IList* messageWrappers)
{
    AutoPtr<IComparator> com = (IComparator*)new MyComparator(this);
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    collections->Sort(messageWrappers, com);
    AutoPtr<IList> reorderMessages;
    CArrayList::New((IList**)&reorderMessages);
    // Copy the ordered message objects into the final list.
    AutoPtr<IIterator> it;
    messageWrappers->GetIteratro((IIterator**)&it);
    Boolean result;
    while (it->HasNext(&result), result) {
        AutoPtr<IInterface> messageWrapper;
        it->GetNext((IInterface**)&messageWrapper);
        AutoPtr<MessageStatusWithPriority> priority = (MessageStatusWithPriority*)(IObject*)messageWrapper.Get();
        reorderMessages->Add((IObject*)priority->mMessage);
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
        actionUri = UriUtils::ParseUriOrNull(str);
        // If there is no settings URI, there is no point in showing the notification.
        if (actionUri == NULL) {
            return NULL;
        }
    }

    AutoPtr<VoicemailStatusHelperStatusMessage> statusMessage
            = new VoicemailStatusHelperStatusMessage(sourcePackage,
                    overallState->GetCallLogMessageId(),
                    overallState->GetCallDetailsMessageId(), action->GetMessageId(),
                    actionUri);
    return new MessageStatusWithPriority(statusMessage, overallState->GetPriority());
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
