
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telephony.h"
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/contacts/common/ContactPhotoManager.h"
#include "elastos/droid/contacts/common/GeoUtil.h"
#include "elastos/droid/contacts/common/CallUtil.h"
#include "elastos/droid/dialer/CCallDetailActivity.h"
#include "elastos/droid/dialer/calllog/PhoneNumberUtilsWrapper.h"
#include "elastos/droid/dialer/calllog/CallDetailHistoryAdapter.h"
#include "elastos/droid/dialer/calllog/PhoneAccountUtils.h"
#include "elastos/droid/dialer/calllog/ContactInfo.h"
#include "elastos/droid/dialer/util/AsyncTaskExecutors.h"
#include "elastos/droid/dialer/util/DialerUtils.h"
// #include "elastos/droid/dialer/voicemail/VoicemailPlaybackFragment.h"
#include "elastos/droid/dialer/voicemail/VoicemailStatusHelperImpl.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::App::IActionBar;
using Elastos::Droid::App::IFragmentManager;
using Elastos::Droid::App::IFragmentTransaction;
using Elastos::Droid::Contacts::Common::ContactPhotoManager;
using Elastos::Droid::Contacts::Common::GeoUtil;
using Elastos::Droid::Contacts::Common::CallUtil;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Dialer::EIID_IProximitySensorAware;
using Elastos::Droid::Dialer::EIID_ICallDetailActivity;
using Elastos::Droid::Dialer::EIID_IProximitySensorManagerListener;
using Elastos::Droid::Dialer::CallLog::PhoneNumberUtilsWrapper;
using Elastos::Droid::Dialer::CallLog::CallDetailHistoryAdapter;
using Elastos::Droid::Dialer::CallLog::PhoneAccountUtils;
using Elastos::Droid::Dialer::CallLog::ContactInfo;
using Elastos::Droid::Dialer::Util::AsyncTaskExecutors;
using Elastos::Droid::Dialer::Util::DialerUtils;
using Elastos::Droid::Dialer::Voicemail::VoicemailStatusHelperImpl;
// using Elastos::Droid::Dialer::Voicemail::VoicemailPlaybackFragment;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Provider::IVoicemails;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsPhone;
using Elastos::Droid::Provider::CContactsContractCommonDataKindsPhone;
using Elastos::Droid::Provider::ICalls;
using Elastos::Droid::Provider::CCalls;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Text::IBidiFormatterHelper;
using Elastos::Droid::Text::CBidiFormatterHelper;
using Elastos::Droid::Text::ITextDirectionHeuristics;
using Elastos::Droid::Text::CTextDirectionHeuristics;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::ITextDirectionHeuristic;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Core::AutoLock;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::CString;
using Elastos::Core::IStringBuilder;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Dialer {

//=================================================================
// CCallDetailActivity::ProximitySensorListener::BlankRunnable
//=================================================================
ECode CCallDetailActivity::ProximitySensorListener::BlankRunnable::Run()
{
    AutoPtr<IView> blankView;
    mHost->FindViewById(Elastos::Droid::Dialer::R::id::blank, (IView**)&blankView);
    blankView->SetVisibility(IView::VISIBLE);

    AutoPtr<IActionBar> ab;
    mHost->GetActionBar((IActionBar**)&ab);
    ab->Hide();
    return NOERROR;
}


//=================================================================
// CCallDetailActivity::ProximitySensorListener::UnblankRunnable
//=================================================================
CCallDetailActivity::ProximitySensorListener::UnblankRunnable::UnblankRunnable(
    /* [in] */ CCallDetailActivity* host)
    : mHost(host)
{}

ECode CCallDetailActivity::ProximitySensorListener::UnblankRunnable::Run()
{
    AutoPtr<IView> blankView;
    mHost->FindViewById(Elastos::Droid::Dialer::R::id::blank, (IView**)&blankView);
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
    AutoLock syncLock(this);
    ClearPendingRequests();
    PostDelayed(mBlankRunnable, PROXIMITY_BLANK_DELAY_MILLIS);
    return NOERROR;
}

ECode CCallDetailActivity::ProximitySensorListener::OnFar()
{
    AutoLock syncLock(this);
    ClearPendingRequests();
    PostDelayed(mUnblankRunnable, PROXIMITY_UNBLANK_DELAY_MILLIS);
    return NOERROR;
}

void CCallDetailActivity::ProximitySensorListener::ClearPendingRequests()
{
    AutoLock syncLock(this);
    AutoPtr<IView> blankView;
    mHost->FindViewById(Elastos::Droid::Dialer::R::id::blank, (IView**)&blankView);
    Boolean result;
    blankView->RemoveCallbacks(mBlankRunnable, &result);
    blankView->RemoveCallbacks(mUnblankRunnable, &result);
}

void CCallDetailActivity::ProximitySensorListener::PostDelayed(
    /* [in] */ IRunnable* runnable,
    /* [in] */ Int64 delayMillis)
{
    // Post these instead of executing immediately so that:
    // - They are guaranteed to be executed on the main thread.
    // - If the sensor values changes rapidly for some time, the UI will not be
    //   updated immediately.
    AutoLock syncLock(this);
    AutoPtr<IView> blankView;
    mHost->FindViewById(Elastos::Droid::Dialer::R::id::blank, (IView**)&blankView);
    Boolean result;
    blankView->PostDelayed(runnable, delayMillis, &result);
}


//=================================================================
// CCallDetailActivity::MarkVoiceMailAsReadTask
//=================================================================
CCallDetailActivity::MarkVoiceMailAsReadTask::MarkVoiceMailAsReadTask(
    /* [in] */ IUri* voicemailUri,
    /* [in] */ CCallDetailActivity* host)
    : mVoicemailUri(voicemailUri)
    , mHost(host)
{}

ECode CCallDetailActivity::MarkVoiceMailAsReadTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    values->Put(IVoicemails::IS_READ, TRUE);
    AutoPtr<IContentResolver> resolver;
    mHost->GetContentResolver((IContentResolver**)&resolver);
    Int32 out;
    resolver->Update(mVoicemailUri, values, IVoicemails::IS_READ + " = 0", NULL, &out);
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
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    // TODO: All phone calls correspond to the same person, so we can make a single
    // lookup.
    Int32 numCalls = callUris->GetLength();
    AutoPtr<IArrayOf> details;
    CArrayOf::New(EIID_IInterface, numCalls, (IArrayOf**)&details);
    // try {
    for (Int32 index = 0; index < numCalls; ++index) {
        AutoPtr<PhoneCallDetails> callDetails;
        ECode ec = mHost->GetPhoneCallDetailsForUri(
                IUri::Probe((*callUris)[index]), (PhoneCallDetails**)&callDetails);
        if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
            // Something went wrong reading in our primary data.
            Logger::W(CCallDetailActivity::TAG, "invalid URI starting call details 0x%08x", ec);
            return NOERROR;
        }
        details->Set(index, TO_IINTERFACE(details));
    }
    *result = details;
    REFCOUNT_ADD(*result)
    return NOERROR;
    // } catch (IllegalArgumentException e) {
    //     // Something went wrong reading in our primary data.
    //     Log.w(TAG, "invalid URI starting call details", e);
    //     return null;
    // }
}

ECode CCallDetailActivity::UpdateContactDetailsTask::OnPostExecute(
    /* [in] */ IInterface* _details)
{
    if (_details == NULL) {
        // Somewhere went wrong: we're going to bail out and show error to users.
        AutoPtr<IToastHelper> helper;
        CToastHelper::AcquireSingleton((IToastHelper**)&helper);
        AutoPtr<IToast> toast;
        helper->MakeText(IContext::Probe(this), Elastos::Droid::Dialer::R::string::toast_call_detail_error,
                IToast::LENGTH_SHORT, (IToast**)&toast);
        toast->Show();
        mHost->Finish();
        return NOERROR;
    }

    // We know that all calls are from the same number and the same contact, so pick the
    // first.
    AutoPtr<IArrayOf> details = IArrayOf::Probe(_details);
    AutoPtr<IInterface> first;
    details->Get(0, (IInterface**)&first);
    AutoPtr<PhoneCallDetails> firstDetails = (PhoneCallDetails*)(IObject*)first.Get();
    firstDetails->mNumber->ToString(&mHost->mNumber);
    Int32 numberPresentation = firstDetails->mNumberPresentation;
    AutoPtr<IUri> contactUri = firstDetails->mContactUri;
    AutoPtr<IUri> photoUri = firstDetails->mPhotoUri;

    // Cache the details about the phone number.
    AutoPtr<ICharSequence> number;
    CString::New(mHost->mNumber, (ICharSequence**)&number);
    Boolean canPlaceCallsTo = PhoneNumberUtilsWrapper::CanPlaceCallsTo(number, numberPresentation);
    AutoPtr<PhoneNumberUtilsWrapper> phoneUtils = new PhoneNumberUtilsWrapper();
    Boolean isVoicemailNumber = phoneUtils->IsVoicemailNumber(number);
    Boolean isSipNumber = phoneUtils->IsSipNumber(number);

    AutoPtr<ICharSequence> callLocationOrType = GetNumberTypeOrLocation(firstDetails);

    AutoPtr<ICharSequence> displayNumber = mHost->mPhoneNumberHelper->GetDisplayNumber(
            firstDetails->mNumber,
            firstDetails->mNumberPresentation,
            firstDetails->mFormattedNumber);
    String str;
    displayNumber->ToString(&str);
    AutoPtr<ITextDirectionHeuristics> henuristics;
    CTextDirectionHeuristics::AcquireSingleton((ITextDirectionHeuristics**)&henuristics);
    AutoPtr<ITextDirectionHeuristic> heuristic;
    henuristics->GetLTR((ITextDirectionHeuristic**)&heuristic);
    String displayNumberStr;
    mHost->mBidiFormatter->UnicodeWrap(str, heuristic, &displayNumberStr);

    if (!TextUtils::IsEmpty(firstDetails->mName)) {
        mHost->mCallerName->SetText(firstDetails->mName);
        String type;
        callLocationOrType->ToString(&type);
        AutoPtr<ICharSequence> text;
        CString::New(type + " " + displayNumberStr, (ICharSequence**)&text);
        mHost->mCallerNumber->SetText(text);
    }
    else {
        AutoPtr<ICharSequence> text;
        CString::New(displayNumberStr, (ICharSequence**)&text);
        mHost->mCallerName->SetText(text);
        if (!TextUtils::IsEmpty(callLocationOrType)) {
            mHost->mCallerNumber->SetText(callLocationOrType);
            IView::Probe(mHost->mCallerNumber)->SetVisibility(IView::VISIBLE);
        }
        else {
            IView::Probe(mHost->mCallerNumber)->SetVisibility(IView::GONE);
        }
    }

    if (!TextUtils::IsEmpty(firstDetails->mAccountLabel)) {
        AutoPtr<ICharSequence> text;
        CString::New(firstDetails->mAccountLabel, (ICharSequence**)&text);
        mHost->mAccountLabel->SetText(text);
        IView::Probe(mHost->mAccountLabel)->SetVisibility(IView::VISIBLE);
    }
    else {
        IView::Probe(mHost->mAccountLabel)->SetVisibility(IView::GONE);
    }

    mHost->mHasEditNumberBeforeCallOption =
            canPlaceCallsTo && !isSipNumber && !isVoicemailNumber;
    mHost->mHasTrashOption = mHost->HasVoicemail();
    mHost->mHasRemoveFromCallLogOption = !mHost->HasVoicemail();
    mHost->InvalidateOptionsMenu();

    AutoPtr<IView> temp;
    mHost->FindViewById(Elastos::Droid::Dialer::R::id::history, (IView**)&temp);
    AutoPtr<IListView> historyList = IListView::Probe(temp);
    AutoPtr<CallDetailHistoryAdapter> adapter = new CallDetailHistoryAdapter(
            IContext::Probe(this), mHost->mInflater,
            mHost->mCallTypeHelper, details);
    IAdapterView::Probe(historyList)->SetAdapter(IAdapter::Probe(adapter));

    String lookupKey = contactUri == NULL ? String(NULL)
            : ContactInfoHelper::GetLookupKeyFromUri(contactUri);

    Boolean isBusiness = mHost->mContactInfoHelper->IsBusiness(firstDetails->mSourceType);

    Int32 contactType =
            isVoicemailNumber? ContactPhotoManager::TYPE_VOICEMAIL :
            isBusiness ? ContactPhotoManager::TYPE_BUSINESS : ContactPhotoManager::TYPE_DEFAULT;

    String nameForDefaultImage;
    if (TextUtils::IsEmpty(firstDetails->mName)) {
        mHost->mPhoneNumberHelper->GetDisplayNumber(firstDetails->mNumber,
                firstDetails->mNumberPresentation,
                firstDetails->mFormattedNumber)->ToString(&nameForDefaultImage);
    }
    else {
        firstDetails->mName->ToString(&nameForDefaultImage);
    }

    if (mHost->HasVoicemail() && !TextUtils::IsEmpty(firstDetails->mTranscription)) {
        AutoPtr<ICharSequence> text;
        CString::New(firstDetails->mTranscription, (ICharSequence**)&text);
        mHost->mVoicemailTranscription->SetText(text);
        IView::Probe(mHost->mVoicemailTranscription)->SetVisibility(IView::VISIBLE);
    }

    mHost->LoadContactPhotos(
            contactUri, photoUri, nameForDefaultImage, lookupKey, contactType);
    AutoPtr<IView> detail;
    mHost->FindViewById(Elastos::Droid::Dialer::R::id::call_detail, (IView**)&detail);
    detail->SetVisibility(IView::VISIBLE);
    return NOERROR;
}

AutoPtr<ICharSequence> CCallDetailActivity::UpdateContactDetailsTask::GetNumberTypeOrLocation(
    /* [in] */ PhoneCallDetails* details)
{
    AutoPtr<ICharSequence> cs;
    if (!TextUtils::IsEmpty(details->mName)) {
        AutoPtr<IContactsContractCommonDataKindsPhone> phone;
        CContactsContractCommonDataKindsPhone::AcquireSingleton((IContactsContractCommonDataKindsPhone**)&phone);
        phone->GetTypeLabel(mHost->mResources, details->mNumberType,
                details->mNumberLabel, (ICharSequence**)&cs);
    }
    else {
        CString::New(details->mGeocode, (ICharSequence**)&cs);
    }
    return cs;
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
    /* [in] */ VoicemailStatusHelperStatusMessage* message,
    /* [in] */ CCallDetailActivity* host)
    : mMessage(message)
    , mHost(host)
{}

CAR_INTERFACE_IMPL(CCallDetailActivity::StatusMessageViewOnClickListener, Object, IViewOnClickListener)

ECode CCallDetailActivity::StatusMessageViewOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_VIEW, mMessage->mActionUri, (IIntent**)&intent);
    DialerUtils::StartActivityWithErrorToast(IContext::Probe(mHost), intent);
    return NOERROR;
}


//=================================================================
// CCallDetailActivity::RemoveMenuFromCallLogTask
//=================================================================
CCallDetailActivity::RemoveMenuFromCallLogTask::RemoveMenuFromCallLogTask(
    /* [in] */ StringBuilder* callIds,
    /* [in] */ CCallDetailActivity* host)
    : mCallIds(callIds)
    , mHost(host)
{}

ECode CCallDetailActivity::RemoveMenuFromCallLogTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IContentResolver> resolver;
    mHost->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ICalls> calls;
    CCalls::AcquireSingleton((ICalls**)&calls);
    AutoPtr<IUri> uri;
    calls->GetCONTENT_URI_WITH_VOICEMAIL((IUri**)&uri);
    StringBuilder sb(IBaseColumns::ID);
    sb.Append(" IN (");
    sb.Append((IStringBuilder*)mCallIds);
    sb.Append(")");
    Int32 value;
    resolver->Delete(uri, sb.ToString(), NULL, &value);
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
    VALIDATE_NOT_NULL(result)
    AutoPtr<IContentResolver> resolver;
    mHost->GetContentResolver((IContentResolver**)&resolver);
    Int32 value;
    resolver->Delete(mHost->mVoicemailUri, String(NULL), NULL, &value);
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
const String CCallDetailActivity::TAG("CallDetail");
const Int32 CCallDetailActivity::LOADER_ID;
const String CCallDetailActivity::BUNDLE_CONTACT_URI_EXTRA("contact_uri_extra");
const Char32 CCallDetailActivity::LEFT_TO_RIGHT_EMBEDDING;
const Char32 CCallDetailActivity::POP_DIRECTIONAL_FORMATTING;
const Int64 CCallDetailActivity::PROXIMITY_BLANK_DELAY_MILLIS;
const Int64 CCallDetailActivity::PROXIMITY_UNBLANK_DELAY_MILLIS;

static AutoPtr<ArrayOf<String> > InitProjection()
{
    AutoPtr<ArrayOf<String> > projection = ArrayOf<String>::Alloc(12);
    (*projection)[0] = ICalls::DATE;
    (*projection)[1] = ICalls::DURATION;
    (*projection)[2] = ICalls::NUMBER;
    (*projection)[3] = ICalls::TYPE;
    (*projection)[4] = ICalls::COUNTRY_ISO;
    (*projection)[5] = ICalls::GEOCODED_LOCATION;
    (*projection)[6] = ICalls::NUMBER_PRESENTATION;
    (*projection)[7] = ICalls::PHONE_ACCOUNT_COMPONENT_NAME;
    (*projection)[8] = ICalls::PHONE_ACCOUNT_ID;
    (*projection)[9] = ICalls::FEATURES;
    (*projection)[10] = ICalls::DATA_USAGE;
    (*projection)[11] = ICalls::TRANSCRIPTION;
    return projection;
}
const AutoPtr<ArrayOf<String> > CCallDetailActivity::CALL_LOG_PROJECTION = InitProjection();

const Int32 CCallDetailActivity::DATE_COLUMN_INDEX;
const Int32 CCallDetailActivity::DURATION_COLUMN_INDEX;
const Int32 CCallDetailActivity::NUMBER_COLUMN_INDEX;
const Int32 CCallDetailActivity::CALL_TYPE_COLUMN_INDEX;
const Int32 CCallDetailActivity::COUNTRY_ISO_COLUMN_INDEX;
const Int32 CCallDetailActivity::GEOCODED_LOCATION_COLUMN_INDEX;
const Int32 CCallDetailActivity::NUMBER_PRESENTATION_COLUMN_INDEX;
const Int32 CCallDetailActivity::ACCOUNT_COMPONENT_NAME;
const Int32 CCallDetailActivity::ACCOUNT_ID;
const Int32 CCallDetailActivity::FEATURES;
const Int32 CCallDetailActivity::DATA_USAGE;
const Int32 CCallDetailActivity::TRANSCRIPTION_COLUMN_INDEX;

CAR_INTERFACE_IMPL_2(CCallDetailActivity, AnalyticsActivity, IProximitySensorAware, ICallDetailActivity)

CAR_OBJECT_IMPL(CCallDetailActivity)

CCallDetailActivity::CCallDetailActivity()
    : mHasEditNumberBeforeCallOption(FALSE)
    , mHasTrashOption(FALSE)
    , mHasRemoveFromCallLogOption(FALSE)
{
    AutoPtr<IBidiFormatterHelper> helper;
    CBidiFormatterHelper::AcquireSingleton((IBidiFormatterHelper**)&helper);
    helper->GetInstance((IBidiFormatter**)&mBidiFormatter);
    mProximitySensorListener = new ProximitySensorListener(this);
}

ECode CCallDetailActivity::constructor()
{
    return AnalyticsActivity::constructor();
}

ECode CCallDetailActivity::OnCreate(
    /* [in] */ IBundle* icicle)
{
    AnalyticsActivity::OnCreate(icicle);

    SetContentView(Elastos::Droid::Dialer::R::layout::call_detail);

    mAsyncTaskExecutor = AsyncTaskExecutors::CreateThreadPoolExecutor();

    AutoPtr<IInterface> service;
    GetSystemService(LAYOUT_INFLATER_SERVICE, (IInterface**)&service);
    mInflater = ILayoutInflater::Probe(service);
    mResources = NULL;
    GetResources((IResources**)&mResources);

    mCallTypeHelper = new CallTypeHelper(mResources);
    mPhoneNumberHelper = new PhoneNumberDisplayHelper(mResources);
    mVoicemailStatusHelper = new VoicemailStatusHelperImpl();
    mAsyncQueryHandler = new CallDetailActivityQueryHandler();
    mAsyncQueryHandler->constructor(this);

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    AutoPtr<IParcelable> parcelable;
    intent->GetParcelableExtra(EXTRA_VOICEMAIL_URI, (IParcelable**)&parcelable);
    mVoicemailUri = IUri::Probe(parcelable);

    AutoPtr<IView> view;
    FindViewById(Elastos::Droid::Dialer::R::id::quick_contact_photo, (IView**)&view);
    mQuickContactBadge = IQuickContactBadge::Probe(view);
    mQuickContactBadge->SetOverlay(NULL);

    view = NULL;
    FindViewById(Elastos::Droid::Dialer::R::id::caller_name, (IView**)&view);
    mCallerName = ITextView::Probe(view);
    view = NULL;
    FindViewById(Elastos::Droid::Dialer::R::id::caller_number, (IView**)&view);
    mCallerNumber = ITextView::Probe(view);
    view = NULL;
    FindViewById(Elastos::Droid::Dialer::R::id::phone_account_label, (IView**)&view);
    mAccountLabel = ITextView::Probe(view);

    AutoPtr<IContext> ctx = IContext::Probe(this);
    mDefaultCountryIso = GeoUtil::GetCurrentCountryIso(ctx);
    mContactPhotoManager = ContactPhotoManager::GetInstance(ctx);
    mProximitySensorManager = new ProximitySensorManager(ctx, mProximitySensorListener);
    mContactInfoHelper = new ContactInfoHelper(ctx, GeoUtil::GetCurrentCountryIso(ctx));
    AutoPtr<IActionBar> bar;
    GetActionBar((IActionBar**)&bar);
    bar->SetDisplayHomeAsUpEnabled(TRUE);

    OptionallyHandleVoicemail();
    Boolean value;
    if (intent->GetBooleanExtra(EXTRA_FROM_NOTIFICATION, FALSE, &value), value) {
        CloseSystemDialogs();
    }

    return NOERROR;
}

ECode CCallDetailActivity::OnResume()
{
    AnalyticsActivity::OnResume();
    AutoPtr<ArrayOf<IUri*> > uris = GetCallLogEntryUris();
    UpdateData(uris);
    return NOERROR;
}

void CCallDetailActivity::OptionallyHandleVoicemail()
{
    assert(0);
    // if (HasVoicemail()) {
    //     AutoPtr<IInterface> service;
    //     GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&service);
    //     AutoPtr<ILayoutInflater> inflater = ILayoutInflater::Probe(service);

    //     AutoPtr<IView> view;
    //     inflater->Inflate(Elastos::Droid::Dialer::R::layout::call_details_voicemail_header,
    //             NULL, (IView**)&view);
    //     mVoicemailHeader = ILinearLayout::Probe(view);

    //     AutoPtr<IView> voicemailContainer;
    //     mVoicemailHeader->FindViewById(
    //             Elastos::Droid::Dialer::R::id::voicemail_container, (IView**)&voicemailContainer);

    //     mVoicemailHeader->FindViewById(
    //             Elastos::Droid::Dialer::R::id::voicemail_status, (IView**)&mStatusMessageView);
    //     view = NULL;
    //     mVoicemailHeader->FindViewById(
    //             Elastos::Droid::Dialer::R::id::voicemail_status_message, (IView**)&view);
    //     mStatusMessageText = ITextView::Probe(view);

    //     view = NULL;
    //     mVoicemailHeader->FindViewById(
    //             Elastos::Droid::Dialer::R::id::voicemail_status_action, (IView**)&view);
    //     mStatusMessageAction = ITextView::Probe(view);
    //     view = NULL;
    //     mVoicemailHeader->FindViewById(
    //             Elastos::Droid::Dialer::R::id::voicemail_transcription, (IView**)&view);
    //     mVoicemailTranscription = ITextView::Probe(view);

    //     view = NULL;
    //     FindViewById(Elastos::Droid::Dialer::R::id::history, (IView**)&view);
    //     AutoPtr<IListView> historyList = IListView::Probe(view);
    //     historyList->AddHeaderView(mVoicemailHeader);

    //     // Has voicemail: add the voicemail fragment.  Add suitable arguments to set the uri
    //     // to play and optionally start the playback.
    //     // Do a query to fetch the voicemail status messages.
    //     AutoPtr<IFragmentManager> fragmentManager;
    //     GetFragmentManager((IFragmentManager**)&fragmentManager);
    //     AutoPtr<IFragment> fragment;
    //     fragmentManager->FindFragmentByTag(VOICEMAIL_FRAGMENT_TAG, (IFragment**)&fragment);
    //     AutoPtr<VoicemailPlaybackFragment> playbackFragment;
    //     if (fragment == NULL) {
    //         playbackFragment = new VoicemailPlaybackFragment();
    //         playbackFragment->constructor();
    //         AutoPtr<IBundle> fragmentArguments;
    //         CBundle::New((IBundle**)&fragmentArguments);
    //         fragmentArguments->PutParcelable(EXTRA_VOICEMAIL_URI, IParcelable::Probe(mVoicemailUri));
    //         AutoPtr<IIntent> intent;
    //         GetIntent((IIntent**)&intent);
    //         Boolean extra;
    //         if (intent->GetBooleanExtra(EXTRA_VOICEMAIL_START_PLAYBACK, FALSE, &extra), extra) {
    //             fragmentArguments->PutBoolean(EXTRA_VOICEMAIL_START_PLAYBACK, TRUE);
    //         }
    //         playbackFragment->SetArguments(fragmentArguments);
    //         AutoPtr<IFragmentTransaction> transaction;
    //         fragmentManager->BeginTransaction((IFragmentTransaction**)&transaction);
    //         transaction->Add(Elastos::Droid::::R::id::voicemail_container, IFragment::Probe(playbackFragment), VOICEMAIL_FRAGMENT_TAG);
    //         Int32 result;
    //         transaction->commitAllowingStateLoss(&result);
    //     }

    //     voicemailContainer->SetVisibility(IView::VISIBLE);
    //     mAsyncQueryHandler->StartVoicemailStatusQuery(mVoicemailUri);
    //     MarkVoicemailAsRead(mVoicemailUri);
    // }
}

Boolean CCallDetailActivity::HasVoicemail()
{
    return mVoicemailUri != NULL;
}

void CCallDetailActivity::MarkVoicemailAsRead(
    /* [in] */ IUri* voicemailUri)
{
    AutoPtr<MarkVoiceMailAsReadTask> task = new MarkVoiceMailAsReadTask(voicemailUri, this);
    AutoPtr<IInteger32> identifier;
    CInteger32::New(MARK_VOICEMAIL_READ, (IInteger32**)&identifier);
    mAsyncTaskExecutor->Submit(identifier, (IObject*)task, NULL);
}

AutoPtr<ArrayOf<IUri*> > CCallDetailActivity::GetCallLogEntryUris()
{
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    AutoPtr<IUri> uri;
    intent->GetData((IUri**)&uri);
    if (uri != NULL) {
        // If there is a data on the intent, it takes precedence over the extra.
        AutoPtr<ArrayOf<IUri*> > result = ArrayOf<IUri*>::Alloc(1);
        result->Set(0, uri);
        return result;
    }

    AutoPtr<ArrayOf<Int64> > ids;
    intent->GetInt64ArrayExtra(EXTRA_CALL_LOG_IDS, (ArrayOf<Int64>**)&ids);
    Int32 numIds = ids == NULL ? 0 : ids->GetLength();
    AutoPtr<ArrayOf<IUri*> > uris = ArrayOf<IUri*>::Alloc(numIds);
    AutoPtr<IContentUris> contentUris;
    CContentUris::AcquireSingleton((IContentUris**)&contentUris);
    AutoPtr<ICalls> calls;
    CCalls::AcquireSingleton((ICalls**)&calls);
    AutoPtr<IUri> voicemailUri;
    calls->GetCONTENT_URI_WITH_VOICEMAIL((IUri**)&voicemailUri);
    for (Int32 index = 0; index < numIds; ++index) {
        AutoPtr<IUri> item;
        contentUris->WithAppendedId(voicemailUri, (*ids)[index], (IUri**)&item);
        uris->Set(index, item);
    }
    return uris;
}

ECode CCallDetailActivity::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    switch (keyCode) {
        case IKeyEvent::KEYCODE_CALL: {
            // Make sure phone isn't already busy before starting direct call
            AutoPtr<IInterface> service;
            GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&service);
            AutoPtr<ITelephonyManager> tm = ITelephonyManager::Probe(service);
            Int32 state;
            if (tm->GetCallState(&state), state == ITelephonyManager::CALL_STATE_IDLE) {
                AutoPtr<IUriHelper> helper;
                CUriHelper::AcquireSingleton((IUriHelper**)&helper);
                AutoPtr<IUri> uri;
                helper->FromParts(IPhoneAccount::SCHEME_TEL, mNumber, String(NULL), (IUri**)&uri);
                DialerUtils::StartActivityWithErrorToast(IContext::Probe(this),
                        CallUtil::GetCallIntent(uri), Elastos::Droid::Dialer::R::string::call_not_available);
                *result = TRUE;
                return NOERROR;
            }
        }
    }

    return AnalyticsActivity::OnKeyDown(keyCode, event, result);
}

void CCallDetailActivity::UpdateData(
    /* [in] */ ArrayOf<IUri*>* callUris)
{
    AutoPtr<UpdateContactDetailsTask> task = new UpdateContactDetailsTask(this);
    AutoPtr<IInteger32> identifier;
    CInteger32::New(UPDATE_PHONE_CALL_DETAILS, (IInteger32**)&identifier);
    mAsyncTaskExecutor->Submit(identifier, (IObject*)task, NULL);
}

ECode CCallDetailActivity::GetPhoneCallDetailsForUri(
    /* [in] */ IUri* callUri,
    /* [out] */ PhoneCallDetails** details)
{
    VALIDATE_NOT_NULL(details)
    *details = NULL;

    AutoPtr<IContentResolver> resolver;
    GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ICursor> callCursor;
    resolver->Query(callUri,
            CALL_LOG_PROJECTION, String(NULL), NULL, String(NULL), (ICursor**)&callCursor);

    // try {
    Boolean succeeded;
    if (callCursor == NULL || (callCursor->MoveToFirst(&succeeded), !succeeded)) {
        Logger::E(TAG, "Cannot find content: %s", TO_CSTR(callUri));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // Read call log specifics.
    String number;
    callCursor->GetString(NUMBER_COLUMN_INDEX, &number);
    Int32 numberPresentation;
    callCursor->GetInt32(NUMBER_PRESENTATION_COLUMN_INDEX, &numberPresentation);
    Int64 date;
    callCursor->GetInt64(DATE_COLUMN_INDEX, &date);
    Int64 duration;
    callCursor->GetInt64(DURATION_COLUMN_INDEX, &date);
    Int32 callType;
    callCursor->GetInt32(CALL_TYPE_COLUMN_INDEX, &callType);
    String countryIso;
    callCursor->GetString(COUNTRY_ISO_COLUMN_INDEX, &countryIso);
    String geocode;
    callCursor->GetString(GEOCODED_LOCATION_COLUMN_INDEX, &geocode);
    String transcription;
    callCursor->GetString(TRANSCRIPTION_COLUMN_INDEX, &transcription);

    String componentName, id;
    callCursor->GetString(ACCOUNT_COMPONENT_NAME, &componentName);
    callCursor->GetString(ACCOUNT_ID, &id);
    String accountLabel = PhoneAccountUtils::GetAccountLabel(IContext::Probe(this),
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
    // If this is not a regular number, there is no point in looking it up in the contacts.
    AutoPtr<ContactInfo> info;
    AutoPtr<ICharSequence> numberCS;
    CString::New(number, (ICharSequence**)&number);
    if (PhoneNumberUtilsWrapper::CanPlaceCallsTo(numberCS, numberPresentation)) {
        AutoPtr<PhoneNumberUtilsWrapper> wrapper = new PhoneNumberUtilsWrapper();
        if (!wrapper->IsVoicemailNumber(numberCS)) {
            info = mContactInfoHelper->LookupNumber(number, countryIso);
        }
    }
    if (info == NULL) {
        formattedNumber = mPhoneNumberHelper->GetDisplayNumber(numberCS,
                numberPresentation, NULL);
        CString::New(String(""), (ICharSequence**)&nameText);
        numberType = 0;
        CString::New(String(""), (ICharSequence**)&numberLabel);
        photoUri = NULL;
        lookupUri = NULL;
        sourceType = 0;
    }
    else {
        AutoPtr<ICharSequence> formattedNumberCS;
        CString::New(info->mFormattedNumber, (ICharSequence**)&formattedNumberCS);
        formattedNumber = formattedNumberCS;
        CString::New(info->mName, (ICharSequence**)&nameText);
        numberType = info->mType;
        CString::New(info->mLabel, (ICharSequence**)&numberLabel);
        photoUri = info->mPhotoUri;
        lookupUri = info->mLookupUri;
        sourceType = info->mSourceType;
    }
    Int32 features;
    callCursor->GetInt32(FEATURES, &features);
    Int64 dataUsage = 0;
    Boolean isNull;
    if (callCursor->IsNull(DATA_USAGE, &isNull), !isNull) {
        callCursor->GetInt64(DATA_USAGE, &dataUsage);
    }

    AutoPtr<ArrayOf<Int32> > types = ArrayOf<Int32>::Alloc(1);
    types->Set(0, callType);
    *details = new PhoneCallDetails(numberCS, numberPresentation,
            formattedNumber, countryIso, geocode,
            types, date, duration, nameText, numberType,
            numberLabel, lookupUri, photoUri, sourceType, accountLabel,
            NULL, features, dataUsage, transcription);
    REFCOUNT_ADD(*details)
    // } finally {
    //     if (callCursor != null) {
    //         callCursor.close();
    //     }
    // }

    if (callCursor != NULL) {
        ICloseable::Probe(callCursor)->Close();
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
    AutoPtr<ContactPhotoManager::DefaultImageRequest> request = new ContactPhotoManager::DefaultImageRequest(
            displayName, lookupKey, contactType, TRUE /* isCircular */);

    mQuickContactBadge->AssignContactUri(contactUri);
    AutoPtr<ArrayOf<IInterface*> > attrs = ArrayOf<IInterface*>::Alloc(1);
    AutoPtr<ICharSequence> cs;
    CString::New(displayName, (ICharSequence**)&cs);
    attrs->Set(0, cs);
    String description;
    mResources->GetString(
            Elastos::Droid::Dialer::R::string::description_contact_details, attrs, &description);
    AutoPtr<ICharSequence> descriptionCS;
    CString::New(description, (ICharSequence**)&descriptionCS);
    IView::Probe(mQuickContactBadge)->SetContentDescription(descriptionCS);

    mContactPhotoManager->LoadDirectoryPhoto(IImageView::Probe(mQuickContactBadge), photoUri,
            FALSE /* darkTheme */, TRUE /* isCircular */, request);
}

void CCallDetailActivity::UpdateVoicemailStatusMessage(
    /* [in] */ ICursor* statusCursor)
{
    AutoPtr<IView> messageView = IView::Probe(mStatusMessageView);
    if (statusCursor == NULL) {
        messageView->SetVisibility(IView::GONE);
        return;
    }

    AutoPtr<VoicemailStatusHelperStatusMessage> message = GetStatusMessage(statusCursor);
    if (message == NULL || !message->ShowInCallDetails()) {
        messageView->SetVisibility(IView::GONE);
        return;
    }

    messageView->SetVisibility(IView::VISIBLE);
    AutoPtr<ICharSequence> cs;
    CString::New(StringUtils::ToString(message->mCallDetailsMessageId), (ICharSequence**)&cs);
    mStatusMessageText->SetText(cs);
    if (message->mActionMessageId != -1) {
        mStatusMessageAction->SetText(message->mActionMessageId);
    }
    if (message->mActionUri != NULL) {
        messageView->SetClickable(TRUE);
        AutoPtr<StatusMessageViewOnClickListener> listener = new StatusMessageViewOnClickListener(message, this);
        messageView->SetOnClickListener(listener);
    }
    else {
        messageView->SetClickable(FALSE);
    }
}

AutoPtr<VoicemailStatusHelperStatusMessage> CCallDetailActivity::GetStatusMessage(
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
        Logger::W(TAG, "Expected 1, found (%d) num of status messages. Will use the first one.", size);
    }

    AutoPtr<IInterface> item;
    messages->Get(0, (IInterface**)&item);
    return (VoicemailStatusHelperStatusMessage*)(IObject*)item.Get();
}

ECode CCallDetailActivity::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* allowToShow)
{
    AutoPtr<IMenuInflater> inflater;
    GetMenuInflater((IMenuInflater**)&inflater);
    inflater->Inflate(Elastos::Droid::Dialer::R::menu::call_details_options, menu);
    return AnalyticsActivity::OnCreateOptionsMenu(menu, allowToShow);
}

ECode CCallDetailActivity::OnPrepareOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* res)
{
    // This action deletes all elements in the group from the call log.
    // We don't have this action for voicemails, because you can just use the trash button.
    AutoPtr<IMenuItem> menuItem;
    menu->FindItem(Elastos::Droid::Dialer::R::id::menu_remove_from_call_log, (IMenuItem**)&menuItem);
    IView::Probe(menuItem)->SetVisibility(mHasRemoveFromCallLogOption);

    menuItem = NULL;
    menu->FindItem(Elastos::Droid::Dialer::R::id::menu_edit_number_before_call, (IMenuItem**)&menuItem);
    IView::Probe(menuItem)->SetVisibility(mHasEditNumberBeforeCallOption);

    menuItem = NULL;
    menu->FindItem(Elastos::Droid::Dialer::R::id::menu_trash, (IMenuItem**)&menuItem);
    IView::Probe(menuItem)->SetVisibility(mHasTrashOption);

    return AnalyticsActivity::OnPrepareOptionsMenu(menu, res);
}

void CCallDetailActivity::OnMenuRemoveFromCallLog(
    /* [in] */ IMenuItem* menuItem)
{
    AutoPtr<StringBuilder> callIds = new StringBuilder();
    AutoPtr<ArrayOf<IUri*> > uris = GetCallLogEntryUris();
    Int32 length;
    for (Int32 i = 0; i < uris->GetLength(); i++) {
        callIds->GetLength(&length);
        if (length != 0) {
            callIds->Append(",");
        }
        AutoPtr<IContentUris> contentUris;
        CContentUris::AcquireSingleton((IContentUris**)&contentUris);
        Int64 id;
        contentUris->ParseId((*uris)[i], &id);
        callIds->Append(id);
    }
    AutoPtr<RemoveMenuFromCallLogTask> task = new RemoveMenuFromCallLogTask(callIds, this);
    AutoPtr<IInteger32> identifier;
    CInteger32::New(REMOVE_FROM_CALL_LOG_AND_FINISH, (IInteger32**)&identifier);
    mAsyncTaskExecutor->Submit(identifier, (IObject*)task, NULL);
}

void CCallDetailActivity::OnMenuEditNumberBeforeCall(
    /* [in] */ IMenuItem* menuItem)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_DIAL, CallUtil::GetCallUri(mNumber), (IIntent**)&intent);
    StartActivity(intent);
}

void CCallDetailActivity::OnMenuTrashVoicemail(
    /* [in] */ IMenuItem* menuItem)
{
    AutoPtr<MenuTrashVoicemailTask> task = new MenuTrashVoicemailTask(this);
    AutoPtr<IInteger32> identifier;
    CInteger32::New(DELETE_VOICEMAIL_AND_FINISH, (IInteger32**)&identifier);
    mAsyncTaskExecutor->Submit(identifier, (IObject*)task, NULL);
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
    StringBuilder sb;
    sb.AppendChar(LEFT_TO_RIGHT_EMBEDDING);
    sb.Append(text);
    sb.AppendChar(POP_DIRECTIONAL_FORMATTING);
    return ICharSequence::Probe(&sb);
}

} // Dialer
} // Droid
} // Elastos
