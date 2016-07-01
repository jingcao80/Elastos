
#include "elastos/droid/dialer/CCallDetailActivity.h"
#include "elastos/droid/widget/Toast.h"
#include "Elastos.Droid.Content.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::AutoLock;
using Elastos::Droid::App::IFragmentManager;
using Elastos::Droid::App::IFragmentTransaction;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Text::IBidiFormatterHelper;
using Elastos::Droid::Text::CBidiFormatterHelper;
using Elastos::Droid::Text::ITextDirectionHeuristics;
using Elastos::Droid::Text::CTextDirectionHeuristics;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::Toast;
using Elastos::Core::IStringBuilder;
using Elastos::Core::CStringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Dialer {

const String CCallDetailActivity::TAG("CallDetail");
const Int32 CCallDetailActivity::LOADER_ID = 0;

const String CCallDetailActivity::BUNDLE_CONTACT_URI_EXTRA("contact_uri_extra");
const Char32 CCallDetailActivity::LEFT_TO_RIGHT_EMBEDDING = '\u202A';

const Char32 CCallDetailActivity::POP_DIRECTIONAL_FORMATTING = '\u202C';

const Int64 CCallDetailActivity::PROXIMITY_BLANK_DELAY_MILLIS = 100;

const Int64 CCallDetailActivity::PROXIMITY_UNBLANK_DELAY_MILLIS = 500;

const String CCallDetailActivity::EXTRA_CALL_LOG_IDS("EXTRA_CALL_LOG_IDS");

const String CCallDetailActivity::EXTRA_VOICEMAIL_URI("EXTRA_VOICEMAIL_URI");


const String CCallDetailActivity::EXTRA_VOICEMAIL_START_PLAYBACK("EXTRA_VOICEMAIL_START_PLAYBACK");

const String CCallDetailActivity::EXTRA_FROM_NOTIFICATION("EXTRA_FROM_NOTIFICATION");
const String CCallDetailActivity::VOICEMAIL_FRAGMENT_TAG("voicemail_fragment");
const String CCallDetailActivity::CALL_LOG_PROJECTION[] = {
    ICalls::DATE,
    ICalls::DURATION,
    ICalls::NUMBER,
    ICalls::TYPE,
    ICalls::COUNTRY_ISO,
    ICalls::GEOCODED_LOCATION,
    ICalls::NUMBER_PRESENTATION,
    ICalls::PHONE_ACCOUNT_COMPONENT_NAME,
    ICalls::PHONE_ACCOUNT_ID,
    ICalls::FEATURES,
    ICalls::DATA_USAGE,
    ICalls::TRANSCRIPTION
};

Int32 CCallDetailActivity::DATE_COLUMN_INDEX = 0;

Int32 CCallDetailActivity::DURATION_COLUMN_INDEX = 1;

Int32 CCallDetailActivity::NUMBER_COLUMN_INDEX = 2;

Int32 CCallDetailActivity::CALL_TYPE_COLUMN_INDEX = 3;

Int32 CCallDetailActivity::COUNTRY_ISO_COLUMN_INDEX = 4;

Int32 CCallDetailActivity::GEOCODED_LOCATION_COLUMN_INDEX = 5;

Int32 CCallDetailActivity::NUMBER_PRESENTATION_COLUMN_INDEX = 6;

Int32 CCallDetailActivity::ACCOUNT_COMPONENT_NAME = 7;

Int32 CCallDetailActivity::ACCOUNT_ID = 8;

Int32 CCallDetailActivity::FEATURES = 9;

Int32 CCallDetailActivity::DATA_USAGE = 10;

Int32 CCallDetailActivity::TRANSCRIPTION_COLUMN_INDEX = 11;

//=================================================================
// CCallDetailActivity::ProximitySensorListener::BlankRunnable
//=================================================================
CAR_INTERFACE_IMPL(CCallDetailActivity::ProximitySensorListener::BlankRunnable, Object, IRunnable);

CCallDetailActivity::ProximitySensorListener::BlankRunnable::BlankRunnable()
    /* [in] */ ProximitySensorListener* host)
    : mHost(host)
{}

ECode CCallDetailActivity::ProximitySensorListener::BlankRunnable::Run()
{
    AutoPtr<IView> blankView;
    FAIL_RETURN(mHost->FindViewById(R::id::blank, (IView**)&blankView));
    blankView->SetVisibility(IView::VISIBLE);

    AutoPtr<IActionBar> ab;
    mHost->GetActionBar((IActionBar**)&ab);
    ab->Hide();
    return NOERROR;
}

//=================================================================
// CCallDetailActivity::ProximitySensorListener::UnblankRunnable
//=================================================================
CAR_INTERFACE_IMPL(CCallDetailActivity::ProximitySensorListener::UnblankRunnable, Object, IRunnable);

CCallDetailActivity::ProximitySensorListener::UnblankRunnable::UnblankRunnable(
    /* [in] */ CCallDetailActivity* host)
    : mHost(host)
{}

ECode CCallDetailActivity::ProximitySensorListener::UnblankRunnable::Run()
{
    AutoPtr<IView> blankView;
    FAIL_RETURN(mHost->FindViewById(R::id::blank, (IView**)&blankView));
    blankView->SetVisibility(IView::GONE);

    AutoPtr<IActionBar> ab;
    mHost->GetActionBar((IActionBar**)&ab);
    ab->Show();
    return NOERROR;
}

//=================================================================
// CCallDetailActivity::ProximitySensorListener
//=================================================================

CAR_INTERFACE_IMPL(CCallDetailActivity::ProximitySensorListener, Object, IProximitySensorManagerListener);

CCallDetailActivity::ProximitySensorListener::ProximitySensorListener(
    /* [in] */ CCallDetailActivity* host)
    : mHost(host)
{
    mBlankRunnable = (IRunnable*)new BlankRunnable(mHost);
    mUnblankRunnable = (IRunnable*)new UnblankRunnable(mHost);
}

ECode CCallDetailActivity::ProximitySensorListener::OnNear()
{
    {    AutoLock syncLock(this);
        ClearPendingRequests();
        PostDelayed(mBlankRunnable, PROXIMITY_BLANK_DELAY_MILLIS);
        return NOERROR;
    }
}

ECode CCallDetailActivity::ProximitySensorListener::OnFar()
{
    {    AutoLock syncLock(this);
        ClearPendingRequests();
        PostDelayed(mUnblankRunnable, PROXIMITY_UNBLANK_DELAY_MILLIS);
        return NOERROR;
    }
}

void CCallDetailActivity::ProximitySensorListener::ClearPendingRequests()
{
    AutoPtr<IView> blankView;
    FAIL_RETURN(mHost->FindViewById(R::id::blank, (IView**)&blankView));
    blankView->RemoveCallbacks(mBlankRunnable);
    blankView->RemoveCallbacks(mUnblankRunnable);
}

void CCallDetailActivity::ProximitySensorListener::PostDelayed(
    /* [in] */ IRunnable* runnable,
    /* [in] */ Int64 delayMillis)
{
    // Post these instead of executing immediately so that:
    // - They are guaranteed to be executed on the main thread.
    // - If the sensor values changes rapidly for some time, the UI will not be
    //   updated immediately.
    AutoPtr<IView> blankView;
    FAIL_RETURN(mHost->FindViewById(R::id::blank, (IView**)&blankView));
    blankView->PostDelayed(runnable, delayMillis);
}

//=================================================================
// CCallDetailActivity::MarkVoiceMailAsReadTask
//=================================================================
CCallDetailActivity::MarkVoiceMailAsReadTask::MarkVoiceMailAsReadTask(
    /* [in] */ CCallDetailActivity* host)
    : mHost(host)
{}

ECode CCallDetailActivity::MarkVoiceMailAsReadTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    values->Put(IVoicemails::IS_READ, TRUE);
    AutoPtr<IContentResolver> resolver;
    mHost->GetContentResolver((IContentResolver**)&resolver);
    resolver->Update(voicemailUri, values,
            IVoicemails::IS_READ + " = 0", NULL);
    *result = NULL;
    return NOERROR;
}

//=================================================================
// CCallDetailActivity::UpdateContactDetailsTask
//=================================================================
CCallDetailActivity::UpdateContactDetailsTask::UpdateContactDetailsTask(
    /* [in] */ CCallDetailActivity* host)
    : mHost(host)
{}

ECode CCallDetailActivity::UpdateContactDetailsTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* callUris,
    /* [out] */ IInterface** result)
{
    // TODO: All phone calls correspond to the same person, so we can make a single
    // lookup.
    Int32 numCalls = callUris->GetLength();
    AutoPtr<IArrayOf> details;
    CArrayOf::New(EIID_IInterface, numCalls, (IArrayOf**)&details);

    // try {
    ECode ec;
    for (Int32 index = 0; index < numCalls; ++index) {
        AutoPtr<IPhoneCallDetails> phonecallDetails;
        ec = GetPhoneCallDetailsForUri((*callUris)[index],
                (IPhoneCallDetails**)&phonecallDetails);
        if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
            // Something went wrong reading in our primary data.
            Logger::W(TAG, "invalid URI starting call details");
            *result = NULL;
            return NOERROR;
        }
        details->Set(index, phonecallDetails);
    }

    *result = details;
    REFCOUNT_ADD(*result);
    return NOERROR;;
    // } catch (IllegalArgumentException e) {
    //     // Something went wrong reading in our primary data.
    //     Log.w(TAG, "invalid URI starting call details", e);
    //     return null;
    // }
}

ECode CCallDetailActivity::UpdateContactDetailsTask::OnPostExecute(
    /* [in] */ IInterface* result)
{
    IArrayOf* details = IArrayOf::Probe(result);
    if (result == NULL || details == NULL) {
        // Somewhere went wrong: we're going to bail out and show error to users.
        AutoPtr<IToast> toast;
        Toast::MakeText(this, R::string::toast_call_detail_error,
                IToast::LENGTH_SHORT, (IToast**)&toast);
        toast->Show();
        Finish();
        return NOERROR;
    }

    // We know that all calls are from the same number and the same contact, so pick the
    // first.
    AutoPtr<IInterface> first;
    details->Get(0, (IInterface**)&first);
    IPhoneCallDetails* firstDetails = IPhoneCallDetails::Probe(first);
    AutoPtr<ICharSequence> number;
    firstDetails->GetNumber((ICharSequence**)&number);
    number->ToString(&(mHost->mNumber));

    Int32 numberPresentation;
    firstDetails->GetNumberPresentation(&numberPresentation);
    AutoPtr<IUri> contactUri;
    firstDetails->GetContactUri(&contactUri);
    AutoPtr<IUri> photoUri;
    firstDetails->GetPhotoUri(&photoUri);

    // Cache the details about the phone number.
    Boolean canPlaceCallsTo =
        PhoneNumberUtilsWrapper::CanPlaceCallsTo(mNumber, numberPresentation);
    AutoPtr<IPhoneNumberUtilsWrapper> phoneUtils;
    CPhoneNumberUtilsWrapper::New((IPhoneNumberUtilsWrapper**)&phoneUtils);
    Boolean isVoicemailNumber;
    phoneUtils->IsVoicemailNumber(mNumber, &isVoicemailNumber);
    Boolean isSipNumber;
    phoneUtils->IsSipNumber(mNumber, &isSipNumber);

    AutoPtr<ICharSequence> callLocationOrType = mHost->GetNumberTypeOrLocation(firstDetails);

    AutoPtr<ICharSequence> formattedNumber;
    firstDetails->GetFormattedNumber((ICharSequence**)&formattedNumber);
    AutoPtr<ICharSequence> displayNumber;
    mPhoneNumberHelper->GetDisplayNumber(number,
            numberPresentation, formattedNumber, (ICharSequence**)&displayNumber);

    AutoPtr<ITextDirectionHeuristics> tdh;
    CTextDirectionHeuristics::AcquireSingleton((ITextDirectionHeuristics**)&tdh);
    AutoPtr<ITextDirectionHeuristic> ltr;
    tdh->GetLTR((ITextDirectionHeuristic**)&ltr);`
    String displayNumberStr;
    mBidiFormatter->UnicodeWrap(
            CoreUtils::Unbox(displayNumber), ltr, &displayNumberStr);

    AutoPtr<ICharSequence> name;
    firstDetails->GetName(&name);
    if (!TextUtils::IsEmpty(name)) {
        mCallerName->SetText(name);
        mCallerNumber->SetText(CoreUtils::Unbox(callLocationOrType) + " " + displayNumberStr);
    }
    else {
        mCallerName->setText(displayNumberStr);
        if (!TextUtils::IsEmpty(callLocationOrType)) {
            mCallerNumber->SetText(callLocationOrType);
            mCallerNumber->SetVisibility(IView::VISIBLE);
        }
        else {
            mCallerNumber->SetVisibility(IView::GONE);
        }
    }

    String accountLabel;
    firstDetails->GetAccountLabel(&accountLabel);
    if (!TextUtils::IsEmpty(accountLabel)) {
        mAccountLabel->SetText(accountLabel);
        mAccountLabel->SetVisibility(IView::VISIBLE);
    }
    else {
        mAccountLabel->SetVisibility(IView::GONE);
    }

    mHasEditNumberBeforeCallOption =
            canPlaceCallsTo && !isSipNumber && !isVoicemailNumber;
    mHasTrashOption = mHost->HasVoicemail();
    mHasRemoveFromCallLogOption = !mHost->HasVoicemail();
    mHost->InvalidateOptionsMenu();

    AutoPtr<IView> historyList;
    FAIL_RETURN(FindViewById(R::id::history, (IView**)&historyList));
    AutoPtr<ICallDetailHistoryAdapter> adapter;
    CCallDetailHistoryAdapter::New(this, mInflater,
            mCallTypeHelper, details, (ICallDetailHistoryAdapter**)&details);
    IListView::Probe(historyList)->SetAdapter(adapter);

    String lookupKey = contactUri == NULL ? NULL
            : ContactInfoHelper::GetLookupKeyFromUri(contactUri);

    Int32 sourceType;
    firstDetails->GetSourceType(&sourceType);
    Boolean isBusiness;
    mContactInfoHelper->IsBusiness(sourceType, &isBusiness);

    Int32 contactType =
            isVoicemailNumber? IContactPhotoManager::TYPE_VOICEMAIL :
            isBusiness ? IContactPhotoManager::TYPE_BUSINESS :
            IContactPhotoManager::TYPE_DEFAULT;

    String nameForDefaultImage;
    if (TextUtils::IsEmpty(name)) {
        AutoPtr<ICharSequence> displayNumber;
        mPhoneNumberHelper->GetDisplayNumber(number,
                numberPresentation, formattedNumber, (ICharSequence**)&displayNumber);
        nameForDefaultImage = CoreUtils::Unbox(displayNumber);
    }
    else {
        nameForDefaultImage = CoreUtils::Unbox(name);
    }

    String transcription;
    firstDetails->GetTranscription(&transcription);
    if (mHost->HasVoicemail() && !TextUtils::IsEmpty(transcription)) {
        mVoicemailTranscription->SetText(transcription);
        mVoicemailTranscription->SetVisibility(IView::VISIBLE);
    }

    mHost->LoadContactPhotos(
            contactUri, photoUri, nameForDefaultImage, lookupKey, contactType);
    AutoPtr<IView> view;
    FAIL_RETURN(FindViewById(R::id::call_detail, (IView**)&view));
    view->SetVisibility(IView::VISIBLE);
    return NOERROR;
}

//=================================================================
// CCallDetailActivity::ViewEntry
//=================================================================
CCallDetailActivity::ViewEntry::ViewEntry(
    /* [in] */ const String& text,
    /* [in] */ IIntent* intent,
    /* [in] */ const String& description,
    /* [in] */ CCallDetailActivity* host)
    : mText(text)
    , mPrimaryIntent(intent)
    , mPrimaryDescription(description)
    , mSecondaryIcon(0)
    , mHost(host)
{}

void CCallDetailActivity::ViewEntry::SetSecondaryAction(
    /* [in] */ Int32 icon,
    /* [in] */ IIntent* intent,
    /* [in] */ const String& description)
{
    mSecondaryIcon = icon;
    mSecondaryIntent = intent;
    mSecondaryDescription = description;
}


//=================================================================
// CCallDetailActivity::StatusMessageViewOnClickListener
//=================================================================
CCallDetailActivity::StatusMessageViewOnClickListener::StatusMessageViewOnClickListener(
    /* [in] */ StatusMessage* messge;
    /* [in] */ CCallDetailActivity* host)
    : mMessage(message)
    , mHost(host)
{}

ECode CCallDetailActivity::StatusMessageViewOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_VIEW, mMessage->actionUri, (IIntent**)&intent);
    return DialerUtils::StartActivityWithErrorToast(mHost, intent);
}

//=================================================================
// CCallDetailActivity::RemoveMenuFromCallLogTask
//=================================================================
CCallDetailActivity::RemoveMenuFromCallLogTask::RemoveMenuFromCallLogTask(
    /* [in] */ IStringBuilder* callIds,
    /* [in] */ CCallDetailActivity* host)
    : mCallIds(callIds)
    , mHost(host)
{}

ECode CCallDetailActivity::RemoveMenuFromCallLogTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    AutoPtr<IContentResolver> resolver;
    mHost->GetContentResolver((IContentResolver**)&resolver);
    resolver->Delete(ICalls::CONTENT_URI_WITH_VOICEMAIL,
            ICalls::_ID + " IN (" + mCallIds + ")", NULL);
    *result = NULL;
    return NOERROR;
}

ECode CCallDetailActivity::RemoveMenuFromCallLogTask::OnPostExecute(
    /* [in] */ IInterface* result)
{
    mHost->Finish();
    return NOERROR;
}


//=================================================================
// CCallDetailActivity::MenuTrashVoicemailTask
//=================================================================
CCallDetailActivity::MenuTrashVoicemailTask::MenuTrashVoicemailTask(
    /* [in] */ CCallDetailActivity* host)
    : mHost(host)
{}

ECode CCallDetailActivity::MenuTrashVoicemailTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    AutoPtr<IContentResolver> resolver;
    mHost->GetContentResolver((IContentResolver**)&resolver);
    resolver->Delete(mHost->mVoicemailUri, NULL, NULL));
    *result = NULL;
    return NOERROR;
}

ECode CCallDetailActivity::MenuTrashVoicemailTask::OnPostExecute(
    /* [in] */ IInterface* result)
{
    mHost->Finish();
    return NOERROR;
}

//=================================================================
// CCallDetailActivity
//=================================================================

CAR_INTERFACE_IMPL_2(CCallDetailActivity, AnalyticsActivity, IProximitySensorAware, ICallDetailActivity);

CAR_OBJECT_IMPL(CCallDetailActivity)

CCallDetailActivity::CCallDetailActivity()
{
    AutoPtr<IBidiFormatterHelper> helper;
    CBidiFormatterHelper::AcquireSingleton((IBidiFormatterHelper**)&helper);
    helper->GetInstance((IBidiFormatter**)&mBidiFormatter);
    mProximitySensorListener = new ProximitySensorListener(this);
}

ECode CCallDetailActivity::OnCreate(
    /* [in] */ IBundle* icicle)
{
    AnalyticsActivity::OnCreate(icicle);

    SetContentView(R::layout::call_detail);

    mAsyncTaskExecutor = AsyncTaskExecutors::CreateThreadPoolExecutor();
    AutoPtr<IInterface> service;
    GetSystemService(LAYOUT_INFLATER_SERVICE, (IInterface**)&service);
    mInflater = ILayoutInflater::Probe(service);
    GetResources((IResources**)&mResources);

    CCallTypeHelper::New(mResources, (ICallTypeHelper**)&mCallTypeHelper);
    CPhoneNumberDisplayHelper::New(mResources, (IPhoneNumberDisplayHelper**)&mPhoneNumberHelper);
    CVoicemailStatusHelperImpl::New((IVoicemailStatusHelperImpl**)&mVoicemailStatusHelper);
    CCallDetailActivityQueryHandler::New(this, (ICallDetailActivityQueryHandler**)&mAsyncQueryHandler);

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    AutoPtr<IParcelable> parcelable;
    intent->GetParcelableExtra(EXTRA_VOICEMAIL_URI, (IParcelable**)&parcelable);
    mVoicemailUri = IUri::Probe(parcelable);

    AutoPtr<IView> view;
    FAIL_RETURN(FindViewById(R::id::quick_contact_photo, (IView**)&view));
    mQuickContactBadge = IQuickContactBadge::Probe(view);
    mQuickContactBadge->SetOverlay(NULL);

    view = NULL;
    FAIL_RETURN(FindViewById(R::id::caller_name, (IView**)&view));
    mCallerName = ITextView::Probe(view);
    view = NULL;
    FAIL_RETURN(FindViewById(R::id::caller_number, (IView**)&view));
    mCallerNumber = ITextView::Probe(view);
    view = NULL;
    FAIL_RETURN(FindViewById(R::id::phone_account_label, (IView**)&view));
    mAccountLabel = ITextView::Probe(view);

    mDefaultCountryIso = GeoUtil::GetCurrentCountryIso(this);
    mContactPhotoManager = ContactPhotoManager::GetInstance(this);
    CProximitySensorManager::New(this, mProximitySensorListener,
            (ProximitySensorManager**)&mProximitySensorManager);
    CContactInfoHelper::New(this, GeoUtil::GetCurrentCountryIso(this),
            (ContactInfoHelper**)&mContactInfoHelper);
    AutoPtr<IActionBar> actionBar;
    GetActionBar((IActionBar**)&actionBar);
    actionBar->SetDisplayHomeAsUpEnabled(TRUE);

    OptionallyHandleVoicemail();
    intent = NULL;
    GetIntent((IIntent**)&intent);
    Boolean extra = FALSE;
    if (intent->GetBooleanExtra(EXTRA_FROM_NOTIFICATION, FALSE, &extra), extra) {
        CloseSystemDialogs();
    }

    return NOERROR;
}

ECode CCallDetailActivity::OnResume()
{
    AnalyticsActivity::OnResume();
    UpdateData(GetCallLogEntryUris());
    return NOERROR;
}

void CCallDetailActivity::OptionallyHandleVoicemail()
{
    if (HasVoicemail()) {
        AutoPtr<IInterface> service;
        GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&service);
        ILayoutInflater* inflater = ILayoutInflater::Probe(service);

        AutoPtr<IView> view;
        inflater->Inflate(R::layout::call_details_voicemail_header,
                NULL, (IView**)&view);
        mVoicemailHeader = ILinearLayout::Probe(view);

        AutoPtr<IView> voicemailContainer;
        mVoicemailHeader->FindViewById(
                R::id::voicemail_container, (IView**)&voicemailContainer);

        mVoicemailHeader->FindViewById(
                R::id::voicemail_status, (IView**)&mStatusMessageView);
        view = NULL;
        mVoicemailHeader->FindViewById(
                R::id::voicemail_status_message, (IView**)&view);
        mStatusMessageText = ITextView::Probe(view);

        view = NULL;
        mVoicemailHeader->FindViewById(
                R::id::voicemail_status_action, (IView**)&view);
        mStatusMessageAction = ITextView::Probe(view);
        view = NULL;
        mVoicemailHeader->FindViewById(
                R::id::voicemail_transcription, (IView**)&view);
        mVoicemailTranscription = ITextView::Probe(view);

        view = NULL;
        FindViewById(R::id::history, (IView**)&view);
        AutoPtr<IListView> historyList = IListView::Probe(view);
        historyList->AddHeaderView(mVoicemailHeader);

        // Has voicemail: add the voicemail fragment.  Add suitable arguments to set the uri
        // to play and optionally start the playback.
        // Do a query to fetch the voicemail status messages.
        AutoPtr<IFragmentManager> fragmentManager;
        GetFragmentManager((IFragmentManager**)&fragmentManager);
        AutoPtr<IVoicemailPlaybackFragment> playbackFragment;
        fragmentManager->FindFragmentByTag(VOICEMAIL_FRAGMENT_TAG,
                (IVoicemailPlaybackFragment**)&playbackFragment);

        if (playbackFragment == NULL) {
            CVoicemailPlaybackFragment::New((IVoicemailPlaybackFragment**)&playbackFragment);
            AutoPtr<IBundle> fragmentArguments;
            CBundle::New((IBundle**)&fragmentArguments);
            fragmentArguments->PutParcelable(EXTRA_VOICEMAIL_URI, IParcelable::Probe(mVoicemailUri));
            AutoPtr<IIntent> intent;
            GetIntent((IIntent**)&intent);
            Boolean extra = FALSE;
            if (intent->GetBooleanExtra(EXTRA_VOICEMAIL_START_PLAYBACK, FALSE, &extra), extra) {
                fragmentArguments->PutBoolean(EXTRA_VOICEMAIL_START_PLAYBACK, TRUE);
            }
            playbackFragment->SetArguments(fragmentArguments);
            AutoPtr<IFragmentTransaction> transaction;
            fragmentManager->BeginTransaction((IFragmentTransaction**)&transaction);
            transaction->Add(R::id::voicemail_container, playbackFragment, VOICEMAIL_FRAGMENT_TAG);
            Int32 result;
            transaction->commitAllowingStateLoss(&result);
        }

        voicemailContainer->SetVisibility(IView::VISIBLE);
        mAsyncQueryHandler->StartVoicemailStatusQuery(mVoicemailUri);
        MarkVoicemailAsRead(mVoicemailUri);
    }
}

Boolean CCallDetailActivity::HasVoicemail()
{
    return mVoicemailUri != NULL;
}

void CCallDetailActivity::MarkVoicemailAsRead(
    /* [in] */ IUri* voicemailUri)
{
    AutoPtr<MarkVoiceMailAsReadTask> task = new MarkVoiceMailAsReadTask(this);
    mAsyncTaskExecutor->Submit(ITasks::MARK_VOICEMAIL_READ, (IAsyncTask*)task);
}

AutoPtr<ArrayOf<IUri> > CCallDetailActivity::GetCallLogEntryUris()
{
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    AutoPtr<IUri> uri;
    intent->GetData((IUri**)&uri);
    if (uri != NULL) {
        // If there is a data on the intent, it takes precedence over the extra.
        AutoPtr<ArrayOf<IUri> > result = ArrayOf<IUri>::Alloc(1);
        result->Set(0, uri);
        return result;
    }

    AutoPtr<ArrayOf<Int64> > ids;
    intent->GetInt64ArrayExtra(EXTRA_CALL_LOG_IDS, (ArrayOf<Int64>**)&ids);
    Int32 numIds = ids == NULL ? 0 : ids->GetLength();
    AutoPtr<ArrayOf<IUri> > uris = ArrayOf<IUri>::Alloc(numIds);
    AutoPtr<IContentUris> contentUris;
    CContentUris::AcquireSingleton((IContentUris**)&contentUris);
    for (Int32 index = 0; index < numIds; ++index) {
        AutoPtr<IUri> item;
        contentUris->WithAppendedId(ICalls::CONTENT_URI_WITH_VOICEMAIL,
                (*ids)[index], (IUri**)&item);
        (*uris)[index] = item;
    }
    return uris;
}

ECode CCallDetailActivity::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    switch (keyCode) {
        case IKeyEvent::KEYCODE_CALL: {
            // Make sure phone isn't already busy before starting direct call
            AutoPtr<IInterface> service;
            GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&service);
            ITelephonyManager* tm = ITelephonyManager::Probe(service);
            Int32 state;
            tm->GetCallState(&state);
            if (state == ITelephonyManager::CALL_STATE_IDLE) {
                AutoPtr<IUri> uri;
                Uri::FromParts(IPhoneAccount::SCHEME_TEL, mNumber, NULL, (IUri**)&uri);
                DialerUtils::StartActivityWithErrorToast(this,
                        CallUtil::GetCallIntent(uri), R::string::call_not_available);
                *result = TRUE;
                return NOERROR;
            }
        }
    }

    return AnalyticsActivity::OnKeyDown(keyCode, event, result);
}

void CCallDetailActivity::UpdateData(
    /* [in] */ ArrayOf<IUri>* callUris)
{
    AutoPtr<UpdateContactDetailsTask> task = new UpdateContactDetailsTask(this);
    mAsyncTaskExecutor.submit(Tasks.UPDATE_PHONE_CALL_DETAILS, (IAsyncTask*)task);
}

ECode CCallDetailActivity::GetPhoneCallDetailsForUri(
    /* [in] */ IUri* callUri,
    /* [out] */ IPhoneCallDetails ** details)
{
    AutoPtr<IContentResolver> resolver;
    GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ICursor> callCursor;
    resolver->Query(callUri,
            CALL_LOG_PROJECTION, NULL, NULL, NULL, (ICursor**)&callCursor);

    // try {
    Boolean succeeded;
    callCursor->MoveToFirst(&succeeded);
    if (callCursor == NULL || !succeeded) {
        String str;
        callUri->ToString(&str);
        Logger::E(TAG, "Cannot find content: %s", str.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int64 dataUsage = 0;
    Boolean isNull = TRUE;

    // Read call log specifics.
    String number;
    FAIL_GOTO(callCursor->GetString(NUMBER_COLUMN_INDEX, &number), exit);
    Int32 numberPresentation;
    FAIL_GOTO(callCursor->GetInt32(
            NUMBER_PRESENTATION_COLUMN_INDEX, &numberPresentation), exit);
    Int64 date;
    FAIL_GOTO(callCursor->GetInt64(DATE_COLUMN_INDEX, &date), exit);
    Int64 duration;
    FAIL_GOTO(callCursor->GetInt64(DURATION_COLUMN_INDEX, &date), exit);

    Int32 callType;
    FAIL_GOTO(callCursor->GetInt32(CALL_TYPE_COLUMN_INDEX, &callType), exit);
    String countryIso;
    FAIL_GOTO(callCursor->GetString(COUNTRY_ISO_COLUMN_INDEX, &countryIso), exit);
    String geocode;
    FAIL_GOTO(callCursor->GetString(GEOCODED_LOCATION_COLUMN_INDEX, &geocode), exit);
    String transcription;
    FAIL_GOTO(callCursor->GetString(TRANSCRIPTION_COLUMN_INDEX, &transcription), exit);

    String componentName, id;
    FAIL_GOTO(callCursor->GetString(ACCOUNT_COMPONENT_NAME, &componentName), exit);
    FAIL_GOTO(callCursor->GetString(ACCOUNT_ID, &id), exit);
    String accountLabel = PhoneAccountUtils::GetAccountLabel(this,
            PhoneAccountUtils::GetAccount(componentName, id));

    if (TextUtils::IsEmpty(countryIso)) {
        countryIso = mDefaultCountryIso;
    }

    // Formatted phone number.
    AutoPtr<ICharSequence> formattedNumber;
    // Read contact specifics.
    AutoPtr<ICharSequence> nameText;
    Int32 numberType;
    AutoPtr<ICharSequence> numberLabel;
    AutoPtr<IUri> photoUri;
    AutoPtr<IUri> lookupUri;
    Int32 sourceType;

    AutoPtr<IPhoneNumberUtilsWrapper> wrapper;
    CPhoneNumberUtilsWrapper::New((IPhoneNumberUtilsWrapper**)&wrapper);
    Boolean isVoicemailNumber;
    wrapper->IsVoicemailNumber(number, &isVoicemailNumber);
    // If this is not a regular number, there is no point in looking it up in the contacts.
    AutoPtr<IContactInfo> info =
            PhoneNumberUtilsWrapper::CanPlaceCallsTo(number, numberPresentation)
            && !isVoicemailNumber
                    ? mContactInfoHelper->LookupNumber(number, countryIso)
                    : NULL;
    if (info == NULL) {
        formattedNumber = mPhoneNumberHelper->GetDisplayNumber(number,
                numberPresentation, NULL);
        nameText = "";
        numberType = 0;
        numberLabel = "";
        photoUri = NULL;
        lookupUri = NULL;
        sourceType = 0;
    }
    else {
        formattedNumber = info.formattedNumber;
        nameText = info.name;
        numberType = info.type;
        numberLabel = info.label;
        photoUri = info.photoUri;
        lookupUri = info.lookupUri;
        sourceType = info.sourceType;
    }
    Int32 features;
    FAIL_GOTO(callCursor->GetInt32(FEATURES, &features), exit);

    if (callCursor->IsNull(DATA_USAGE, &isNull), !isNull) {
        FAIL_GOTO(callCursor->GetInt64(DATA_USAGE, &dataUsage), exit);
    }

    AutoPtr<ArrayOf<Int32> > types = ArrayOf<Int32>::Alloc(1);
    types->Set(0, callType);
    AutoPtr<IPhoneCallDetails> ipd;
    FAIL_GOTO(CPhoneCallDetails::New(number, numberPresentation,
            formattedNumber, countryIso, geocode,
            types, date, duration, nameText, numberType,
            numberLabel, lookupUri, photoUri, sourceType, accountLabel,
            NULL, features, dataUsage, transcription, &ipd), exit);
    *details = ipd;
    REFCOUNT_ADD(*details);
    // } finally {
    //     if (callCursor != null) {
    //         callCursor.close();
    //     }
    // }
exit:
    if (callCursor != NULL) {
        callCursor->Close();
    }
    return NOERROR;
}

void CCallDetailActivity::LoadContactPhotos(
    /* [in] */ IUri* contactUri,
    /* [in] */ IUri* photoUri,
    /* [in] */ const String& displayName,
    /* [in] */ const String& lookupKey,
    /* [in] */ Int32 contactType)
{
    AutoPtr<DefaultImageRequest> request = new DefaultImageRequest(
            displayName, lookupKey, contactType, TRUE /* isCircular */);

    mQuickContactBadge->AssignContactUri(contactUri);
    String description;
    mResources->GetString(
            R::string::description_contact_details, displayName, &description);
    mQuickContactBadge->SetContentDescription(description);

    mContactPhotoManager->LoadDirectoryPhoto(mQuickContactBadge, photoUri,
            FALSE /* darkTheme */, TRUE /* isCircular */, request);
}

void CCallDetailActivity::UpdateVoicemailStatusMessage(
    /* [in] */ ICursor* statusCursor)
{
    if (statusCursor == NULL) {
        mStatusMessageView->SetVisibility(IView::GONE);
        return;
    }

    AutoPtr<IStatusMessage> message = GetStatusMessage(statusCursor);
    Boolean result = FALSE;
    if (message == NULL || message->ShowInCallDetails(&result), !result) {
        mStatusMessageView->SetVisibility(IView::GONE);
        return;
    }

    mStatusMessageView->SetVisibility(IView::VISIBLE);
    mStatusMessageText->setText(((StatusMessage*)message)->callDetailsMessageId);
    if (((StatusMessage*)message)->actionMessageId != -1) {
        mStatusMessageAction->SetText(((StatusMessage*)message)->actionMessageId);
    }
    if (((StatusMessage*)message)->actionUri != null) {
        mStatusMessageAction->SetClickable(TRUE);
        AutoPtr<StatusMessageViewOnClickListener> listener =
                new StatusMessageViewOnClickListener((StatusMessage*)message, this);
        mStatusMessageAction->SetOnClickListener(listener);
    }
    else {
        mStatusMessageAction->SetClickable(FALSE);
    }
}

AutoPtr<IStatusMessage> CCallDetailActivity::GetStatusMessage(
    /* [in] */ ICursor* statusCursor)
{
    AutoPtr<IList> messages;
    mVoicemailStatusHelper->GetStatusMessages(statusCursor, (IList**)&messages);
    Int32 size;
    messages->GetSize(&size);
    if (size == 0) {
        return NULL;
    }
    // There can only be a single status message per source package, so num of messages can
    // at most be 1.
    if (size > 1) {
        Logger::W(TAG, "Expected 1, found (%d) num of status messages." +
                " Will use the first one.", size);
    }

    AutoPtr<IInterface> item;
    messages->Get(0, (IInterface**)&item);
    return IStatusMessage::Probe(item);
}

ECode CCallDetailActivity::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* allowToShow)
{
    AutoPtr<IMenuInflater> inflater;
    GetMenuInflater((IMenuInflater**)&inflater);
    inflater->Inflate(R::menu::call_details_options, menu);
    return AnalyticsActivity::OnCreateOptionsMenu(menu, allowToShow);
}

ECode CCallDetailActivity::OnPrepareOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* res)
{
    // This action deletes all elements in the group from the call log.
    // We don't have this action for voicemails, because you can just use the trash button.
    AutoPtr<IMenuItem> menuItem;
    menu->FindItem(R::id::menu_remove_from_call_log, (IMenuItem**)&menuItem);
    menuItem->SetVisible(mHasRemoveFromCallLogOption);

    menuItem = NULL;
    menu->FindItem(R::id::menu_edit_number_before_call, (IMenuItem**)&menuItem);
    menuItem->SetVisible(mHasEditNumberBeforeCallOption);

    menuItem = NULL;
    menu->FindItem(R::id::menu_trash, (IMenuItem**)&menuItem);
    menuItem->SetVisible(mHasTrashOption);

    return AnalyticsActivity::OnPrepareOptionsMenu(menu, res);
}

ECode CCallDetailActivity::OnMenuRemoveFromCallLog(
    /* [in] */ IMenuItem* menuItem)
{
    AutoPtr<IStringBuilder> callIds;
    CStringBuilder::New((IStringBuilder**)&callIds);
    AutoPtr<ArrayOf<IUri> > uris = GetCallLogEntryUris();
    Int32 length;
    for (Int32 i = 0; i < uris->GetLength(); i++) {
        ICharSequence::Probe(callIds)->GetLength(&length);
        if (length != 0) {
            callIds->Append(",");
        }
        callIds->Append(ContentUris::ParseId((*uris)[i]));
    }
    AutoPtr<RemoveMenuFromCallLogTask> task = new RemoveMenuFromCallLogTask(callIds, this);
    mAsyncTaskExecutor->Submit(ITasks::REMOVE_FROM_CALL_LOG_AND_FINISH, (IAsyncTask*)task);
    return NOERROR;
}

ECode CCallDetailActivity::OnMenuEditNumberBeforeCall(
    /* [in] */ IMenuItem* menuItem)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_DIAL, CallUtil::GetCallUri(mNumber), (IIntent**)&intent);
    StartActivity(intent);
    return NOERROR;
}

ECode CCallDetailActivity::OnMenuTrashVoicemail(
    /* [in] */ IMenuItem* menuItem)
{
    AutoPtr<MenuTrashVoicemailTask> task = new MenuTrashVoicemailTask(this);
    mAsyncTaskExecutor->Submit(ITasks::DELETE_VOICEMAIL_AND_FINISH, (IAsyncTask*)task);
    return NOERROR;
}

ECode CCallDetailActivity::OnPause()
{
    // Immediately stop the proximity sensor.
    DisableProximitySensor(FALSE);
    mProximitySensorListener->ClearPendingRequests();
    return AnalyticsActivity::OnPause();
}

ECode CCallDetailActivity::EnableProximitySensor()
{
    mProximitySensorManager->Enable();
    return NOERROR;
}

ECode CCallDetailActivity::DisableProximitySensor(
    /* [in] */ Boolean waitForFarState)
{
    mProximitySensorManager->Disable(waitForFarState);
    return NOERROR;
}

void CCallDetailActivity::CloseSystemDialogs()
{
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_CLOSE_SYSTEM_DIALOGS, (IIntent**)&intent);
    SendBroadcast(intent);
}

AutoPtr<ICharSequence> CCallDetailActivity::ForceLeftToRight(
    /* [in] */ ICharSequence* text)
{
    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New((IStringBuilder**)&sb);
    sb->Append(LEFT_TO_RIGHT_EMBEDDING);
    sb->Append(text);
    sb->Append(POP_DIRECTIONAL_FORMATTING);
    return ICharSequence::Probe(sb);
}

} // Dialer
} // Droid
} // Elastos
