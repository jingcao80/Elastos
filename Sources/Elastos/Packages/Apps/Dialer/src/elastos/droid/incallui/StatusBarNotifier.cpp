
#include "elastos/droid/incallui/StatusBarNotifier.h"
#include "elastos/droid/incallui/InCallPresenter.h"
#include "elastos/droid/incallui/InCallApp.h"
#include <elastos/droid/text/TextUtils.h>
#include <elastos/utility/logging/Logger.h>
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/R.h"
#include "R.h"

using Elastos::Droid::App::INotification;
using Elastos::Droid::App::CNotificationBuilder;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::InCallUI::EIID_IInCallStateListener;
using Elastos::Droid::InCallUI::EIID_IContactInfoCacheCallback;
using Elastos::Droid::Internal::Utility::IPreconditions;
using Elastos::Droid::Internal::Utility::CPreconditions;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Os::EIID_IHandlerCallback;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Telecom::IPhoneAccount;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::IClassLoader;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace InCallUI {

//================================================================
// StatusBarNotifier::MyNotificationTimer::MyHandlerCallback
//================================================================
CAR_INTERFACE_IMPL(StatusBarNotifier::MyNotificationTimer::MyHandlerCallback, Object, IHandlerCallback)

ECode StatusBarNotifier::MyNotificationTimer::MyHandlerCallback::HandleMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mHost->Fire();
    *result = TRUE;
    return NOERROR;
}


//================================================================
// StatusBarNotifier::MyNotificationTimer
//================================================================
StatusBarNotifier::MyNotificationTimer::MyNotificationTimer(
    /* [in] */ StatusBarNotifier* host)
    : mState(CLEAR)
    , mHost(host)
{
    AutoPtr<IHandlerCallback> cb = (IHandlerCallback*)new MyHandlerCallback(this);
    CHandler::New(cb, TRUE, (IHandler**)&mHandler);
}

StatusBarNotifier::MyNotificationTimer::State StatusBarNotifier::MyNotificationTimer::GetState()
{
    return mState;
}

void StatusBarNotifier::MyNotificationTimer::Schedule()
{
    if (mState == CLEAR) {
        Logger::D("StatusBarNotifier", "updateInCallNotification: timer scheduled");
        Boolean result;
        mHandler->SendEmptyMessageDelayed(0, IN_CALL_TIMEOUT, &result);
        mState = SCHEDULED;
    }
}

void StatusBarNotifier::MyNotificationTimer::Clear()
{
    Logger::D("StatusBarNotifier", "updateInCallNotification: timer cleared");
    mHandler->RemoveMessages(0);
    mState = CLEAR;
}

void StatusBarNotifier::MyNotificationTimer::Fire()
{
    Logger::D("StatusBarNotifier", "updateInCallNotification: timer fired");
    mState = FIRED;
    mHost->UpdateNotification(
            InCallPresenter::GetInstance()->GetInCallState(),
            InCallPresenter::GetInstance()->GetCallList());
}


//================================================================
// StatusBarNotifier::MyContactInfoCacheCallback
//================================================================
CAR_INTERFACE_IMPL(StatusBarNotifier::MyContactInfoCacheCallback, Object, IContactInfoCacheCallback)

ECode StatusBarNotifier::MyContactInfoCacheCallback::OnContactInfoComplete(
    /* [in] */ const String& callId,
    /* [in] */ IContactCacheEntry* entry)
{
    AutoPtr<Call> call = CallList::GetInstance()->GetCallById(callId);
    if (call != NULL) {
        mHost->BuildAndSendNotification(call, (ContactInfoCache::ContactCacheEntry*)entry);
    }
    return NOERROR;
}

ECode StatusBarNotifier::MyContactInfoCacheCallback::OnImageLoadComplete(
    /* [in] */ const String& callId,
    /* [in] */ IContactCacheEntry* entry)
{
    AutoPtr<Call> call = CallList::GetInstance()->GetCallById(callId);
    if (call != NULL) {
        mHost->BuildAndSendNotification(call, (ContactInfoCache::ContactCacheEntry*)entry);
    }
    return NOERROR;
}


//================================================================
// StatusBarNotifier
//================================================================
const Int32 StatusBarNotifier::IN_CALL_NOTIFICATION;

const Int64 StatusBarNotifier::IN_CALL_TIMEOUT;

CAR_INTERFACE_IMPL(StatusBarNotifier, Object, IInCallStateListener);

StatusBarNotifier::StatusBarNotifier(
    /* [in] */ IContext* context,
    /* [in] */ ContactInfoCache* contactInfoCache)
    : mContactInfoCache(contactInfoCache)
    , mIsShowingNotification(FALSE)
    , mCallState(Call::State::INVALID)
    , mSavedIcon(0)
    , mSavedContent(0)
{
    AutoPtr<IPreconditions> preconditions;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditions);
    if (FAILED(preconditions->CheckNotNull(context))) assert(0);
    mContext = context;
    AutoPtr<IInterface> service;
    mContext->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&service);
    mNotificationManager = INotificationManager::Probe(service);
    mNotificationTimer = new MyNotificationTimer(this);
}

ECode StatusBarNotifier::OnStateChange(
    /* [in] */ InCallState oldState,
    /* [in] */ InCallState newState,
    /* [in] */ ICallList* callList)
{
    Logger::D("StatusBarNotifier", "onStateChange");

    UpdateNotification(newState, (CallList*)callList);
    return NOERROR;
}

void StatusBarNotifier::UpdateNotification(
    /* [in] */ InCallState state,
    /* [in] */ CallList* callList)
{
    UpdateInCallNotification(state, callList);
}

void StatusBarNotifier::CancelInCall()
{
    Logger::D("StatusBarNotifier", "cancelInCall()...");
    mNotificationManager->Cancel(IN_CALL_NOTIFICATION);
    mIsShowingNotification = FALSE;
}

void StatusBarNotifier::ClearInCallNotification(
    /* [in] */ IContext* backupContext)
{
    Logger::I("StatusBarNotifier", "Something terrible happened. Clear all InCall notifications");

    AutoPtr<IInterface> service;
    backupContext->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&service);
    AutoPtr<INotificationManager> notificationManager = INotificationManager::Probe(service);
    notificationManager->Cancel(IN_CALL_NOTIFICATION);
}

void StatusBarNotifier::UpdateInCallNotification(
    /* [in] */ InCallState state,
    /* [in] */ CallList* callList)
{
    Logger::D("StatusBarNotifier", "updateInCallNotification...");

    AutoPtr<Call> call = GetCallToShow(callList);

    // Whether we have an outgoing call but the incall UI has yet to show up.
    // Since we don't normally show a notification while the incall screen is
    // in the foreground, if we show the outgoing notification before the activity
    // comes up the user will see it flash on and off on an outgoing call. We therefore
    // do not show the notification for outgoing calls before the activity has started.
    Boolean isOutgoingWithoutIncallUi =
            state == InCallState_OUTGOING &&
            !InCallPresenter::GetInstance()->IsActivityPreviouslyStarted();

    // Whether to show a notification immediately.
    Boolean showNotificationNow =

            // We can still be in the INCALL state when a call is disconnected (in order to show
            // the "Call ended" screen. So check that we have an active connection too.
            (call != NULL) &&

            // We show a notification iff there is an active call.
            (state == InCallState_INCOMING || state == InCallState_OUTGOING || state == InCallState_INCALL) &&

            // If the UI is already showing, then for most cases we do not want to show
            // a notification since that would be redundant, unless it is an incoming call,
            // in which case the notification is actually an important alert.
            (!InCallPresenter::GetInstance()->IsShowingInCallUi() || (state == InCallState_INCOMING)) &&

            // If we have an outgoing call with no UI but the timer has fired, we show
            // a notification anyway.
            (!isOutgoingWithoutIncallUi ||
                    mNotificationTimer->GetState() == NotificationTimer::FIRED);

    if (showNotificationNow) {
        ShowNotification(call);
    }
    else {
        CancelInCall();
        if (isOutgoingWithoutIncallUi &&
                mNotificationTimer->GetState() == NotificationTimer::CLEAR) {
            mNotificationTimer->Schedule();
        }
    }

    // If we see a UI, or we are done with calls for now, reset to ground state.
    if (InCallPresenter::GetInstance()->IsShowingInCallUi() || call == NULL) {
        mNotificationTimer->Clear();
    }
}

void StatusBarNotifier::ShowNotification(
    /* [in] */ Call* call)
{
    Boolean isIncoming = (call->GetState() == Call::State::INCOMING ||
            call->GetState() == Call::State::CALL_WAITING);

    // we make a call to the contact info cache to query for supplemental data to what the
    // call provides.  This includes the contact name and photo.
    // This callback will always get called immediately and synchronously with whatever data
    // it has available, and may make a subsequent call later (same thread) if it had to
    // call into the contacts provider for more data.
    AutoPtr<IContactInfoCacheCallback> cb = new MyContactInfoCacheCallback(this);
    mContactInfoCache->FindInfo(call, isIncoming, cb);
}

void StatusBarNotifier::BuildAndSendNotification(
    /* [in] */ Call* originalCall,
    /* [in] */ ContactInfoCache::ContactCacheEntry* contactInfo)
{
    // This can get called to update an existing notification after contact information has come
    // back. However, it can happen much later. Before we continue, we need to make sure that
    // the call being passed in is still the one we want to show in the notification.
    AutoPtr<Call> call = GetCallToShow(CallList::GetInstance());
    if (call == NULL || !call->GetId().Equals(originalCall->GetId())) {
        return;
    }

    Int32 state = call->GetState();
    Boolean isConference = call->IsConferenceCall();
    Boolean isVideoUpgradeRequest = call->GetSessionModificationState()
            == Call::SessionModificationState::RECEIVED_UPGRADE_TO_VIDEO_REQUEST;

    // Check if data has changed; if nothing is different, don't issue another notification.
    Int32 iconResId = GetIconToDisplay(call);
    AutoPtr<IBitmap> largeIcon = GetLargeIconToDisplay(contactInfo, isConference);
    Int32 contentResId = GetContentString(call);
    String contentTitle = GetContentTitle(contactInfo, isConference);

    if (!CheckForChangeAndSaveData(iconResId, contentResId, largeIcon, contentTitle, state)) {
        return;
    }

    /*
     * Nothing more to check...build and send it.
     */
    AutoPtr<INotificationBuilder> builder = GetNotificationBuilder();

    // Set up the main intent to send the user to the in-call screen
    AutoPtr<IPendingIntent> inCallPendingIntent = CreateLaunchPendingIntent();
    builder->SetContentIntent(inCallPendingIntent);

    // Set the intent as a full screen intent as well if a call is incoming
    if ((state == Call::State::INCOMING || state == Call::State::CALL_WAITING) &&
            !InCallPresenter::GetInstance()->IsShowingInCallUi()) {
        ConfigureFullScreenIntent(builder, inCallPendingIntent, call);
    }

    // Set the content
    String str;
    mContext->GetString(contentResId, &str);
    AutoPtr<ICharSequence> cs;
    CString::New(str, (ICharSequence**)&cs);
    builder->SetContentText(cs);
    builder->SetSmallIcon(iconResId);
    AutoPtr<ICharSequence> titleCs;
    CString::New(contentTitle, (ICharSequence**)&titleCs);
    builder->SetContentTitle(titleCs);
    builder->SetLargeIcon(largeIcon);
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Int32 color;
    res->GetColor(Elastos::Droid::Dialer::R::color::dialer_theme_color, &color);
    builder->SetColor(color);

    if (isVideoUpgradeRequest) {
        builder->SetUsesChronometer(FALSE);
        AddDismissUpgradeRequestAction(builder);
        AddAcceptUpgradeRequestAction(builder);
    }
    else {
        CreateIncomingCallNotification(call, state, builder);
    }

    AddPersonReference(builder, contactInfo, call);

    /*
     * Fire off the notification
     */
    AutoPtr<INotification> notification;
    builder->Build((INotification**)&notification);
    Logger::D("StatusBarNotifier", "Notifying IN_CALL_NOTIFICATION: %s", TO_CSTR(notification));
    mNotificationManager->Notify(IN_CALL_NOTIFICATION, notification);
    mIsShowingNotification = TRUE;
}

void StatusBarNotifier::CreateIncomingCallNotification(
    /* [in] */ Call* call,
    /* [in] */ Int32 state,
    /* [in] */ INotificationBuilder* builder)
{
    if (state == Call::State::ACTIVE) {
        builder->SetUsesChronometer(TRUE);
        builder->SetWhen(call->GetConnectTimeMillis());
    }
    else {
        builder->SetUsesChronometer(FALSE);
    }

    // Add hang up option for any active calls (active | onhold), outgoing calls (dialing).
    if (state == Call::State::ACTIVE || state == Call::State::ONHOLD ||
            Call::State::IsDialing(state)) {
        AddHangupAction(builder);
    }
    else if (state == Call::State::INCOMING || state == Call::State::CALL_WAITING) {
        AddDismissAction(builder);
        if (call->IsVideoCall(mContext)) {
            AddVoiceAction(builder);
            AddVideoCallAction(builder);
        }
        else {
            AddAnswerAction(builder);
        }
    }
}

Boolean StatusBarNotifier::CheckForChangeAndSaveData(
    /* [in] */ Int32 icon,
    /* [in] */ Int32 content,
    /* [in] */ IBitmap* largeIcon,
    /* [in] */ const String& contentTitle,
    /* [in] */ Int32 state)
{
    // The two are different:
    // if new title is not null, it should be different from saved version OR
    // if new title is null, the saved version should not be null
    Boolean contentTitleChanged =
            (!contentTitle.IsNull() && !contentTitle.Equals(mSavedContentTitle)) ||
            (contentTitle.IsNull() && !mSavedContentTitle.IsNull());

    // any change means we are definitely updating
    Boolean retval = (mSavedIcon != icon) || (mSavedContent != content) ||
            (mCallState != state) || (mSavedLargeIcon.Get() != largeIcon) || contentTitleChanged;

    // If we aren't showing a notification right now, definitely start showing one.
    if (!mIsShowingNotification) {
        Logger::D("StatusBarNotifier", "Showing notification for first time.");
        retval = TRUE;
    }

    mSavedIcon = icon;
    mSavedContent = content;
    mCallState = state;
    mSavedLargeIcon = largeIcon;
    mSavedContentTitle = contentTitle;

    if (retval) {
        Logger::D("StatusBarNotifier", "Data changed.  Showing notification");
    }

    return retval;
}

String StatusBarNotifier::GetContentTitle(
    /* [in] */ ContactInfoCache::ContactCacheEntry* contactInfo,
    /* [in] */ Boolean isConference)
{
    if (isConference) {
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        String str;
        res->GetString(Elastos::Droid::Dialer::R::string::card_title_conf_call, &str);
        return str;
    }
    if (TextUtils::IsEmpty(contactInfo->mName)) {
        return contactInfo->mNumber;
    }

    return contactInfo->mName;
}

void StatusBarNotifier::AddPersonReference(
    /* [in] */ INotificationBuilder* builder,
    /* [in] */ ContactInfoCache::ContactCacheEntry* contactInfo,
    /* [in] */ Call* call)
{
    if (contactInfo->mLookupUri != NULL) {
        String str;
        IObject::Probe(contactInfo->mLookupUri)->ToString(&str);
        builder->AddPerson(str);
    }
    else if (!TextUtils::IsEmpty(call->GetNumber())) {
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        AutoPtr<IUri> uri;
        helper->FromParts(IPhoneAccount::SCHEME_TEL, call->GetNumber(), String(NULL), (IUri**)&uri);
        String str;
        IObject::Probe(uri)->ToString(&str);
        builder->AddPerson(str);
    }
}

AutoPtr<IBitmap> StatusBarNotifier::GetLargeIconToDisplay(
    /* [in] */ ContactInfoCache::ContactCacheEntry* contactInfo,
    /* [in] */ Boolean isConference)
{
    AutoPtr<IBitmap> largeIcon;
    if (isConference) {
        AutoPtr<IBitmapFactory> factory;
        CBitmapFactory::AcquireSingleton((IBitmapFactory**)&factory);
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        factory->DecodeResource(res, Elastos::Droid::Dialer::R::drawable::img_conference, (IBitmap**)&largeIcon);
    }
    if (contactInfo->mPhoto != NULL ) {
        AutoPtr<IBitmapDrawable> drawable = IBitmapDrawable::Probe(contactInfo->mPhoto);
        if (drawable != NULL) {
            drawable->GetBitmap((IBitmap**)&largeIcon);
        }
    }

    if (largeIcon != NULL) {
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        Float fh, fw;
        res->GetDimension(Elastos::Droid::R::dimen::notification_large_icon_height, &fw);
        res->GetDimension(Elastos::Droid::R::dimen::notification_large_icon_width, &fh);
        Int32 width = (Int32)fw;
        Int32 height = (Int32)fh;
        AutoPtr<IBitmapHelper> helper;
        CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
        AutoPtr<IBitmap> tmpBM;
        helper->CreateScaledBitmap(largeIcon, width, height, FALSE, (IBitmap**)&tmpBM);
        largeIcon = tmpBM;
    }

    return largeIcon;
}

Int32 StatusBarNotifier::GetIconToDisplay(
    /* [in] */ Call* call)
{
    // Even if both lines are in use, we only show a single item in
    // the expanded Notifications UI.  It's labeled "Ongoing call"
    // (or "On hold" if there's only one call, and it's on hold.)
    // Also, we don't have room to display caller-id info from two
    // different calls.  So if both lines are in use, display info
    // from the foreground call.  And if there's a ringing call,
    // display that regardless of the state of the other calls.
    if (call->GetState() == Call::State::ONHOLD) {
        return Elastos::Droid::Dialer::R::drawable::ic_phone_paused_white_24dp;
    }
    else if (call->GetSessionModificationState()
            == Call::SessionModificationState::RECEIVED_UPGRADE_TO_VIDEO_REQUEST) {
        return Elastos::Droid::Dialer::R::drawable::ic_videocam;
    }
    return Elastos::Droid::Dialer::R::drawable::ic_call_white_24dp;
}

Int32 StatusBarNotifier::GetContentString(
    /* [in] */ Call* call)
{
    Int32 resId = Elastos::Droid::Dialer::R::string::notification_ongoing_call;

    if (call->GetState() == Call::State::INCOMING || call->GetState() == Call::State::CALL_WAITING) {
        resId = Elastos::Droid::Dialer::R::string::notification_incoming_call;
    }
    else if (call->GetState() == Call::State::ONHOLD) {
        resId = Elastos::Droid::Dialer::R::string::notification_on_hold;
    }
    else if (Call::State::IsDialing(call->GetState())) {
        resId = Elastos::Droid::Dialer::R::string::notification_dialing;
    }
    else if (call->GetSessionModificationState()
            == Call::SessionModificationState::RECEIVED_UPGRADE_TO_VIDEO_REQUEST) {
        resId = Elastos::Droid::Dialer::R::string::notification_requesting_video_call;
    }

    return resId;
}

AutoPtr<Call> StatusBarNotifier::GetCallToShow(
    /* [in] */ CallList* callList)
{
    if (callList == NULL) {
        return NULL;
    }
    AutoPtr<Call> call = callList->GetIncomingCall();
    if (call == NULL) {
        call = callList->GetOutgoingCall();
    }
    if (call == NULL) {
        call = callList->GetVideoUpgradeRequestCall();
    }
    if (call == NULL) {
        call = callList->GetActiveOrBackgroundCall();
    }
    return call;
}

void StatusBarNotifier::AddAnswerAction(
    /* [in] */ INotificationBuilder* builder)
{
    Logger::I("StatusBarNotifier", "Will show \"answer\" action in the incoming call Notification");

    AutoPtr<IPendingIntent> answerVoicePendingIntent = CreateNotificationPendingIntent(
            mContext, InCallApp::ACTION_ANSWER_VOICE_INCOMING_CALL);
    AutoPtr<ICharSequence> cs;
    mContext->GetText(Elastos::Droid::Dialer::R::string::description_target_answer, (ICharSequence**)&cs);
    builder->AddAction(Elastos::Droid::Dialer::R::drawable::ic_call_white_24dp,
            cs, answerVoicePendingIntent);
}

void StatusBarNotifier::AddDismissAction(
    /* [in] */ INotificationBuilder* builder)
{
    Logger::I("StatusBarNotifier", "Will show \"dismiss\" action in the incoming call Notification");

    AutoPtr<IPendingIntent> declinePendingIntent =
            CreateNotificationPendingIntent(mContext, InCallApp::ACTION_DECLINE_INCOMING_CALL);
    AutoPtr<ICharSequence> cs;
    mContext->GetText(Elastos::Droid::Dialer::R::string::notification_action_dismiss, (ICharSequence**)&cs);
    builder->AddAction(Elastos::Droid::Dialer::R::drawable::ic_close_dk,
            cs, declinePendingIntent);
}

void StatusBarNotifier::AddHangupAction(
    /* [in] */ INotificationBuilder* builder)
{
    Logger::I("StatusBarNotifier", "Will show \"hang-up\" action in the ongoing active call Notification");

    AutoPtr<IPendingIntent> hangupPendingIntent =
            CreateNotificationPendingIntent(mContext, InCallApp::ACTION_HANG_UP_ONGOING_CALL);
    AutoPtr<ICharSequence> cs;
    mContext->GetText(Elastos::Droid::Dialer::R::string::notification_action_end_call, (ICharSequence**)&cs);
    builder->AddAction(Elastos::Droid::Dialer::R::drawable::ic_call_end_white_24dp,
            cs, hangupPendingIntent);
}

void StatusBarNotifier::AddVideoCallAction(
    /* [in] */ INotificationBuilder* builder)
{
    Logger::I("StatusBarNotifier", "Will show \"video\" action in the incoming call Notification");

    AutoPtr<IPendingIntent> answerVideoPendingIntent = CreateNotificationPendingIntent(
            mContext, InCallApp::ACTION_ANSWER_VIDEO_INCOMING_CALL);
    AutoPtr<ICharSequence> cs;
    mContext->GetText(Elastos::Droid::Dialer::R::string::notification_action_answer_video, (ICharSequence**)&cs);
    builder->AddAction(Elastos::Droid::Dialer::R::drawable::ic_videocam,
            cs, answerVideoPendingIntent);
}

void StatusBarNotifier::AddVoiceAction(
    /* [in] */ INotificationBuilder* builder)
{
    Logger::I("StatusBarNotifier", "Will show \"voice\" action in the incoming call Notification");

    AutoPtr<IPendingIntent> answerVoicePendingIntent = CreateNotificationPendingIntent(
            mContext, InCallApp::ACTION_ANSWER_VOICE_INCOMING_CALL);
    AutoPtr<ICharSequence> cs;
    mContext->GetText(Elastos::Droid::Dialer::R::string::notification_action_answer_voice, (ICharSequence**)&cs);
    builder->AddAction(Elastos::Droid::Dialer::R::drawable::ic_call_white_24dp,
            cs, answerVoicePendingIntent);
}

void StatusBarNotifier::AddAcceptUpgradeRequestAction(
    /* [in] */ INotificationBuilder* builder)
{
    Logger::I("StatusBarNotifier", "Will show \"accept\" action in the incoming call Notification");

    AutoPtr<IPendingIntent> acceptVideoPendingIntent = CreateNotificationPendingIntent(
            mContext, InCallApp::ACTION_ANSWER_VOICE_INCOMING_CALL);
    AutoPtr<ICharSequence> cs;
    mContext->GetText(Elastos::Droid::Dialer::R::string::notification_action_accept, (ICharSequence**)&cs);
    builder->AddAction(0, cs, acceptVideoPendingIntent);
}

void StatusBarNotifier::AddDismissUpgradeRequestAction(
    /* [in] */ INotificationBuilder* builder)
{
    Logger::I("StatusBarNotifier", "Will show \"dismiss\" action in the incoming call Notification");

    AutoPtr<IPendingIntent> declineVideoPendingIntent = CreateNotificationPendingIntent(
            mContext, InCallApp::ACTION_ANSWER_VOICE_INCOMING_CALL);
    AutoPtr<ICharSequence> cs;
    mContext->GetText(Elastos::Droid::Dialer::R::string::notification_action_dismiss, (ICharSequence**)&cs);
    builder->AddAction(0, cs, declineVideoPendingIntent);
}

void StatusBarNotifier::ConfigureFullScreenIntent(
    /* [in] */ INotificationBuilder* builder,
    /* [in] */ IPendingIntent* intent,
    /* [in] */ Call* call)
{
    // Ok, we actually want to launch the incoming call
    // UI at this point (in addition to simply posting a notification
    // to the status bar).  Setting fullScreenIntent will cause
    // the InCallScreen to be launched immediately *unless* the
    // current foreground activity is marked as "immersive".
    Logger::D("StatusBarNotifier", "- Setting fullScreenIntent: %s", TO_CSTR(intent));
    builder->SetFullScreenIntent(intent, TRUE);

    // Ugly hack alert:
    //
    // The NotificationManager has the (undocumented) behavior
    // that it will *ignore* the fullScreenIntent field if you
    // post a new Notification that matches the ID of one that's
    // already active.  Unfortunately this is exactly what happens
    // when you get an incoming call-waiting call:  the
    // "ongoing call" notification is already visible, so the
    // InCallScreen won't get launched in this case!
    // (The result: if you bail out of the in-call UI while on a
    // call and then get a call-waiting call, the incoming call UI
    // won't come up automatically.)
    //
    // The workaround is to just notice this exact case (this is a
    // call-waiting call *and* the InCallScreen is not in the
    // foreground) and manually cancel the in-call notification
    // before (re)posting it.
    //
    // TODO: there should be a cleaner way of avoiding this
    // problem (see discussion in bug 3184149.)

    // If a call is onhold during an incoming call, the call actually comes in as
    // INCOMING.  For that case *and* traditional call-waiting, we want to
    // cancel the notification.
    Boolean isCallWaiting = (call->GetState() == Call::State::CALL_WAITING ||
            (call->GetState() == Call::State::INCOMING &&
                    CallList::GetInstance()->GetBackgroundCall() != NULL));

    if (isCallWaiting) {
        Logger::I("StatusBarNotifier", "updateInCallNotification: call-waiting! force relaunch...");
        // Cancel the IN_CALL_NOTIFICATION immediately before
        // (re)posting it; this seems to force the
        // NotificationManager to launch the fullScreenIntent.
        mNotificationManager->Cancel(IN_CALL_NOTIFICATION);
    }
}

AutoPtr<INotificationBuilder> StatusBarNotifier::GetNotificationBuilder()
{
    AutoPtr<INotificationBuilder> builder;
    CNotificationBuilder::New(mContext, (INotificationBuilder**)&builder);
    builder->SetOngoing(TRUE);

    // Make the notification prioritized over the other normal notifications.
    builder->SetPriority(INotification::PRIORITY_HIGH);

    return builder;
}

AutoPtr<IPendingIntent> StatusBarNotifier::CreateLaunchPendingIntent()
{
    AutoPtr<IIntent> intent = InCallPresenter::GetInstance()->GetInCallIntent(
            FALSE /* showDialpad */, FALSE /* newOutgoingCall */);

    // PendingIntent that can be used to launch the InCallActivity.  The
    // system fires off this intent if the user pulls down the windowshade
    // and clicks the notification's expanded view.  It's also used to
    // launch the InCallActivity immediately when when there's an incoming
    // call (see the "fullScreenIntent" field below).
    AutoPtr<IPendingIntentHelper> helper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
    AutoPtr<IPendingIntent> inCallPendingIntent;
    helper->GetActivity(mContext, 0, intent, 0, (IPendingIntent**)&inCallPendingIntent);

    return inCallPendingIntent;
}

AutoPtr<IPendingIntent> StatusBarNotifier::CreateNotificationPendingIntent(
    /* [in] */ IContext* context,
    /* [in] */ const String& action)
{
    AutoPtr<IIntent> intent;
    AutoPtr<IClassLoader> cl;
    context->GetClassLoader((IClassLoader**)&cl);
    AutoPtr<IClassInfo> ci;
    cl->LoadClass(String("Elastos.Droid.InCallUI.CNotificationBroadcastReceiver"), (IClassInfo**)&ci);
    CIntent::New(action, NULL, context, ci, (IIntent**)&intent);
    AutoPtr<IPendingIntentHelper> helper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
    AutoPtr<IPendingIntent> broadcast;
    helper->GetBroadcast(context, 0, intent, 0, (IPendingIntent**)&broadcast);
    return broadcast;
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
