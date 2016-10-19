
#include "Elastos.Droid.Provider.h"
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/contacts/common/ContactPhotoManager.h"
#include "elastos/droid/contacts/common/CallUtil.h"
#include "elastos/droid/contacts/common/util/UriUtils.h"
#include "elastos/droid/dialer/calllog/CallLogAdapter.h"
#include "elastos/droid/dialer/calllog/CallTypeHelper.h"
#include "elastos/droid/dialer/calllog/PhoneNumberUtilsWrapper.h"
#include "elastos/droid/dialer/calllog/PhoneAccountUtils.h"
#include "elastos/droid/dialer/calllog/IntentProvider.h"
#include "elastos/droid/dialer/calllog/PhoneQuery.h"
#include "elastos/droid/dialer/calllog/CallLogQuery.h"
#include "elastos/droid/dialer/calllog/CallLogListItemViews.h"
#include "elastos/droid/dialer/calllog/CallLogGroupBuilder.h"
#include "elastos/droid/dialer/util/DialerUtils.h"
#include "elastos/droid/dialer/util/ExpirableCache.h"
#include "elastos/droid/dialer/DialtactsActivity.h"
#include "elastos/droid/dialer/PhoneCallDetailsHelper.h"
#include "elastos/droid/dialer/PhoneCallDetailsViews.h"
#include <elastos/droid/text/TextUtils.h>
#include <elastos/droid/view/LayoutInflater.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include "elastos/utility/Objects.h"
#include "R.h"

using Elastos::Droid::Contacts::Common::IContactPhotoManagerDefaultImageProvider;
using Elastos::Droid::Contacts::Common::ContactPhotoManager;
using Elastos::Droid::Contacts::Common::CallUtil;
using Elastos::Droid::Contacts::Common::Util::UriUtils;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Dialer::DialtactsActivity;
using Elastos::Droid::Dialer::PhoneCallDetailsHelper;
using Elastos::Droid::Dialer::PhoneCallDetailsViews;
using Elastos::Droid::Dialer::CallLog::EIID_INumberWithCountryIso;
using Elastos::Droid::Dialer::CallLog::EIID_IContactInfoRequest;
using Elastos::Droid::Dialer::Util::DialerUtils;
using Elastos::Droid::Dialer::Util::ExpirableCache;
using Elastos::Droid::Dialer::Util::IExpirableCacheCachedValue;
using Elastos::Droid::Provider::ICalls;
using Elastos::Droid::Provider::CCalls;
using Elastos::Droid::Provider::IContactsContractPhoneLookup;
using Elastos::Droid::Provider::CContactsContractPhoneLookup;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IViewStub;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IOnPreDrawListener;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Core::AutoLock;
using Elastos::Core::IThread;
using Elastos::Core::ISynchronize;
using Elastos::Core::CString;
using Elastos::Core::CInteger32;
using Elastos::IO::ICloseable;
using Elastos::Utility::CLinkedList;
using Elastos::Utility::Objects;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

//=================================================================
// CallLogAdapter::NumberWithCountryIso
//=================================================================
CAR_INTERFACE_IMPL(CallLogAdapter::NumberWithCountryIso, Object, INumberWithCountryIso)

ECode CallLogAdapter::NumberWithCountryIso::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (o == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<INumberWithCountryIso> iso = INumberWithCountryIso::Probe(o);
    if (iso == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<NumberWithCountryIso> other = (NumberWithCountryIso*)iso.Get();
    *result = TextUtils::Equals(mNumber, other->mNumber)
            && TextUtils::Equals(mCountryIso, other->mCountryIso);

    return NOERROR;
}

ECode CallLogAdapter::NumberWithCountryIso::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)

    *hashCode = (mNumber.IsNull() ? 0 : mNumber.GetHashCode())
            ^ (mCountryIso.IsNull() ? 0 : mCountryIso.GetHashCode());

    return NOERROR;
}


//=================================================================
// CallLogAdapter::ContactInfoRequest
//=================================================================
CAR_INTERFACE_IMPL(CallLogAdapter::ContactInfoRequest, Object, IContactInfoRequest)

ECode CallLogAdapter::ContactInfoRequest::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (TO_IINTERFACE(this) == obj) {
        *result = TRUE;
        return NOERROR;
    }
    if (obj == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IContactInfoRequest> request = IContactInfoRequest::Probe(obj);
    if (request == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<ContactInfoRequest> other = (ContactInfoRequest*)request.Get();

    if (!TextUtils::Equals(mNumber, other->mNumber)) {
        *result = FALSE;
        return NOERROR;
    }
    if (!TextUtils::Equals(mCountryIso, other->mCountryIso)) {
        *result = FALSE;
        return NOERROR;
    }
    if (!Objects::Equals(mCallLogInfo, other->mCallLogInfo)) {
        *result = FALSE;
        return NOERROR;
    }

    *result = TRUE;
    return NOERROR;
}

ECode CallLogAdapter::ContactInfoRequest::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)

    Int32 prime = 31;
    Int32 result = 1;
    Int32 calllogInfoHash;
    result = prime * result + ((mCallLogInfo == NULL)
            ? 0 : ((IObject*)mCallLogInfo->Probe(EIID_IObject))->GetHashCode(&calllogInfoHash), calllogInfoHash);
    result = prime * result + (mCountryIso.IsNull() ? 0 : mCountryIso.GetHashCode());
    result = prime * result + (mCountryIso.IsNull() ? 0 : mNumber.GetHashCode());
    *hashCode = result;

    return NOERROR;
}


//=================================================================
// CallLogAdapter::ActionListener
//=================================================================
CAR_INTERFACE_IMPL(CallLogAdapter::ActionListener, Object, IViewOnClickListener)

ECode CallLogAdapter::ActionListener::OnClick(
    /* [in] */ IView* view)
{
    mHost->StartActivityForAction(view);
    return NOERROR;
}

//=================================================================
// CallLogAdapter::ActionListener
//=================================================================
CAR_INTERFACE_IMPL(CallLogAdapter::ExpandCollapseListener, Object, IViewOnClickListener)

ECode CallLogAdapter::ExpandCollapseListener::OnClick(
    /* [in] */ IView* view)
{
    AutoPtr<IViewParent> parent;
    view->GetParent((IViewParent**)&parent);
    AutoPtr<IViewParent> grandfather;
    parent->GetParent((IViewParent**)&grandfather);

    ICallLogListItemView* callLogItem = ICallLogListItemView::Probe(grandfather);
    mHost->HandleRowExpanded(callLogItem, TRUE /* animate */, FALSE /* forceExpand */);

    return NOERROR;
}

//=================================================================
// CallLogAdapter::AccessibilityDelegate
//=================================================================
ECode CallLogAdapter::AccessibilityDelegate::OnRequestSendAccessibilityEvent(
    /* [in] */ IViewGroup* host,
    /* [in] */ IView* child,
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    Int32 type;
    event->GetEventType(&type);
    if (type == IAccessibilityEvent::TYPE_VIEW_ACCESSIBILITY_FOCUSED) {
        mHost->HandleRowExpanded(ICallLogListItemView::Probe(host),
                FALSE /* animate */, TRUE /* forceExpand */);
    }

    return Elastos::Droid::View::View::AccessibilityDelegate::OnRequestSendAccessibilityEvent(host, child, event, res);
}


//=================================================================
// CallLogAdapter::CallLogAdapterHandler
//=================================================================
ECode CallLogAdapter::CallLogAdapterHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case REDRAW:
            mHost->NotifyDataSetChanged();
            break;
        case START_THREAD:
            mHost->StartRequestProcessing();
            break;
    }

    return NOERROR;
}

//=================================================================
// CallLogAdapter::QueryThread
//=================================================================
CallLogAdapter::QueryThread::QueryThread(
    /* [in] */ CallLogAdapter* host)
    : mDone(FALSE)
    , mHost(host)
{
    Thread::constructor(String("CallLogAdapter.QueryThread"));
}

void CallLogAdapter::QueryThread::StopProcessing()
{
    mDone = TRUE;
}

ECode CallLogAdapter::QueryThread::Run()
{
    Boolean needRedraw = TRUE;
    while (TRUE) {
        // Check if thread is finished, and if so return immediately.
        if (mDone) {
            return NOERROR;
        }

        // Obtain next request, if any is available.
        // Keep synchronized section small.
        AutoPtr<CallLogAdapter::ContactInfoRequest> req;
        AutoPtr<IContactInfoRequest> request;
        {
            AutoLock syncLock(mHost->mRequests);
            Boolean isEmpty = TRUE;
            if (mHost->mRequests->IsEmpty(&isEmpty), !isEmpty) {
                AutoPtr<IInterface> item;
                mHost->mRequests->RemoveFirst((IInterface**)&item);
                request = IContactInfoRequest::Probe(item);
            }
        }

        if (request != NULL) {
            // Process the request. If the lookup succeeds, schedule a
            // redraw.
            req = (CallLogAdapter::ContactInfoRequest*)request.Get();
            needRedraw |= mHost->QueryContactInfo(req->mNumber, req->mCountryIso, req->mCallLogInfo);
        }
        else {
            // Throttle redraw rate by only sending them when there are
            // more requests.
            if (needRedraw) {
                needRedraw = FALSE;
                Boolean result;
                mHost->mHandler->SendEmptyMessage(REDRAW, &result);
            }

            // Wait until another request is available, or until this
            // thread is no longer needed (as indicated by being
            // interrupted).
            // try {
            {
                AutoLock syncLock(mHost->mRequests);
                ECode ec = ISynchronize::Probe(mHost->mRequests)->Wait(1000);
                if (ec == (ECode)E_INTERRUPTED_EXCEPTION) {
                    return NOERROR;
                }
            }
            // } catch (InterruptedException ie) {
            //     // Ignore, and attempt to continue processing requests.
            // }
        }
    }

    return NOERROR;
}

//=================================================================
// CallLogAdapter::ReportButtonClickListener
//=================================================================
CAR_INTERFACE_IMPL(CallLogAdapter::ReportButtonClickListener, Object, IViewOnClickListener)

ECode CallLogAdapter::ReportButtonClickListener::OnClick(
    /* [in] */ IView* view)
{
    if (mHost->mOnReportButtonClickListener != NULL) {
        mHost->mOnReportButtonClickListener->OnReportButtonClick(mViews->mNumber);
    }
    return NOERROR;
}


//=================================================================
// CallLogAdapter::BadgeContainerClickListener
//=================================================================
ECode CallLogAdapter::BadgeContainerClickListener::OnClick(
    /* [in] */ IView* view)
{
    AutoPtr<IIntent> intent = DialtactsActivity::GetAddNumberToContactIntent(mDetails->mNumber);
    mHost->mContext->StartActivity(intent);
    return NOERROR;
}


//=================================================================
// CallLogAdapter
//=================================================================
const Int32 CallLogAdapter::VOICEMAIL_TRANSCRIPTION_MAX_LINES;
const Int32 CallLogAdapter::START_PROCESSING_REQUESTS_DELAY_MILLIS;
const Int32 CallLogAdapter::CONTACT_INFO_CACHE_SIZE;
const Int64 CallLogAdapter::NONE_EXPANDED;
const Int32 CallLogAdapter::REDRAW;
const Int32 CallLogAdapter::START_THREAD;

CAR_INTERFACE_IMPL_3(CallLogAdapter, GroupingListAdapter, ICallLogAdapter, IOnPreDrawListener, ICallLogGroupBuilderGroupCreator)

CallLogAdapter::CallLogAdapter()
    : mPreviouslyExpanded(NONE_EXPANDED)
    , mCurrentlyExpanded(NONE_EXPANDED)
    , mLoading(TRUE)
    , mRequestProcessingDisabled(FALSE)
    , mIsCallLog(TRUE)
    , mCallLogBackgroundColor(0)
    , mExpandedBackgroundColor(0)
    , mExpandedTranslationZ(0.0)
{
    mActionListener = (IViewOnClickListener*)new ActionListener(this);
    mExpandCollapseListener = (IViewOnClickListener*)new ExpandCollapseListener(this);
    mAccessibilityDelegate = new AccessibilityDelegate(this);
    mHandler = new CallLogAdapterHandler(this);
}

void CallLogAdapter::StartActivityForAction(
    /* [in] */ IView* view)
{
    AutoPtr<IInterface> tag;
    view->GetTag((IInterface**)&tag);
    AutoPtr<IIntentProvider> intentProvider = IIntentProvider::Probe(tag);
    if (intentProvider != NULL) {
        AutoPtr<IIntent> intent;
        intentProvider->GetIntent(mContext, (IIntent**)&intent);
        // See IntentProvider.getCallDetailIntentProvider() for why this may be null.
        if (intent != NULL) {
            DialerUtils::StartActivityWithErrorToast(mContext, intent);
        }
    }
}

ECode CallLogAdapter::OnPreDraw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // We only wanted to listen for the first draw (and this is it).
    UnregisterPreDrawListener();

    // Only schedule a thread-creation message if the thread hasn't been
    // created yet. This is purely an optimization, to queue fewer messages.
    if (mCallerIdThread == NULL) {
        Boolean res;
        mHandler->SendEmptyMessageDelayed(START_THREAD, START_PROCESSING_REQUESTS_DELAY_MILLIS, &res);
    }

    *result = true;
    return NOERROR;
}

ECode CallLogAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICallFetcher* callFetcher,
    /* [in] */ ContactInfoHelper* contactInfoHelper,
    /* [in] */ ICallItemExpandedListener* callItemExpandedListener,
    /* [in] */ ICallLogAdapterOnReportButtonClickListener* onReportButtonClickListener,
    /* [in] */ Boolean isCallLog)
{
    GroupingListAdapter::constructor(context);

    mContext = context;
    mCallFetcher = callFetcher;
    mContactInfoHelper = contactInfoHelper;
    mIsCallLog = isCallLog;
    mCallItemExpandedListener = callItemExpandedListener;

    mOnReportButtonClickListener = onReportButtonClickListener;
    AutoPtr<IToastHelper> helper;
    CToastHelper::AcquireSingleton((IToastHelper**)&helper);
    helper->MakeText(mContext, Elastos::Droid::Dialer::R::string::toast_caller_id_reported,
             IToast::LENGTH_SHORT, (IToast**)&mReportedToast);

    mContactInfoCache = ExpirableCache::Create(CONTACT_INFO_CACHE_SIZE);
    CLinkedList::New((ILinkedList**)&mRequests);

    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    AutoPtr<CallTypeHelper> callTypeHelper = new CallTypeHelper(resources);
    resources->GetColor(Elastos::Droid::Dialer::R::color::background_dialer_list_items, &mCallLogBackgroundColor);
    resources->GetColor(Elastos::Droid::Dialer::R::color::call_log_expanded_background_color, &mExpandedBackgroundColor);
    resources->GetDimension(Elastos::Droid::Dialer::R::dimen::call_log_expanded_translation_z, &mExpandedTranslationZ);

    mContactPhotoManager = ContactPhotoManager::GetInstance(mContext);
    mPhoneNumberHelper = new PhoneNumberDisplayHelper(resources);
    AutoPtr<PhoneNumberUtilsWrapper> wrapper = new PhoneNumberUtilsWrapper();
    AutoPtr<PhoneCallDetailsHelper> phoneCallDetailsHelper = new PhoneCallDetailsHelper(
            resources, callTypeHelper, wrapper);

    mCallLogViewsHelper = new CallLogListItemHelper(phoneCallDetailsHelper, mPhoneNumberHelper, resources);
    mCallLogGroupBuilder = new CallLogGroupBuilder(ICallLogGroupBuilderGroupCreator::Probe(this));

    return NOERROR;
}

ECode CallLogAdapter::OnContentChanged()
{
    return mCallFetcher->FetchCalls();
}

ECode CallLogAdapter::SetLoading(
    /* [in] */ Boolean loading)
{
    VALIDATE_NOT_NULL(loading)
    mLoading = loading;
    return NOERROR;
}

ECode CallLogAdapter::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (mLoading) {
        // We don't want the empty state to show when loading.
        return NOERROR;
    }
    else {
        return GroupingListAdapter::IsEmpty(result);
    }
}

void CallLogAdapter::StartRequestProcessing()
{
    AutoLock syncLock(this);
    // For unit-testing.
    if (mRequestProcessingDisabled) return;

    // Idempotence... if a thread is already started, don't start another.
    if (mCallerIdThread != NULL) return;

    mCallerIdThread = new QueryThread(this);
    mCallerIdThread->SetPriority(IThread::MIN_PRIORITY);
    mCallerIdThread->Start();
}

ECode CallLogAdapter::StopRequestProcessing()
{
    AutoLock syncLock(this);
    // Remove any pending requests to start the processing thread.
    mHandler->RemoveMessages(START_THREAD);
    if (mCallerIdThread != NULL) {
        // Stop the thread; we are finished with it.
        mCallerIdThread->StopProcessing();
        mCallerIdThread->Interrupt();
        mCallerIdThread = NULL;
    }

    return NOERROR;
}

void CallLogAdapter::UnregisterPreDrawListener()
{
    Boolean isAlive;
    if (mViewTreeObserver != NULL && (mViewTreeObserver->IsAlive(&isAlive), isAlive)) {
        mViewTreeObserver->RemoveOnPreDrawListener(IOnPreDrawListener::Probe(this));
    }
    mViewTreeObserver = NULL;
}

ECode CallLogAdapter::InvalidateCache()
{
    mContactInfoCache->ExpireAll();

    // Restart the request-processing thread after the next draw.
    StopRequestProcessing();
    UnregisterPreDrawListener();

    return NOERROR;
}

void CallLogAdapter::EnqueueRequest(
    /* [in] */ const String& number,
    /* [in] */ const String& countryIso,
    /* [in] */ ContactInfo* callLogInfo,
    /* [in] */ Boolean immediate)
{
    AutoPtr<ContactInfoRequest> request = new ContactInfoRequest(number, countryIso, callLogInfo);
    {
        AutoLock syncLock(mRequests);
        Boolean result;
        mRequests->Contains((IContactInfoRequest*)request, &result);
        if (!result) {
            mRequests->Add((IContactInfoRequest*)request);
            ISynchronize::Probe(mRequests)->NotifyAll();
        }
    }
    if (immediate) {
        StartRequestProcessing();
    }
}

Boolean CallLogAdapter::QueryContactInfo(
    /* [in] */ const String& number,
    /* [in] */ const String& countryIso,
    /* [in] */ ContactInfo* callLogInfo)
{
    AutoPtr<ContactInfo> info = mContactInfoHelper->LookupNumber(number, countryIso);

    if (info == NULL) {
        // The lookup failed, just return without requesting to update the view.
        return FALSE;
    }

    // Check the existing entry in the cache: only if it has changed we should update the
    // view.
    AutoPtr<NumberWithCountryIso> numberCountryIso = new NumberWithCountryIso(number, countryIso);
    AutoPtr<IInterface> value;
    mContactInfoCache->GetPossiblyExpired((IObject*)numberCountryIso, (IInterface**)&value);
    AutoPtr<ContactInfo> existingInfo = (ContactInfo*)(IObject*)value.Get();

    Boolean isRemoteSource = existingInfo->mSourceType != 0;

    // Don't force redraw if existing info in the cache is equal to {@link ContactInfo#EMPTY}
    // to avoid updating the data set for every new row that is scrolled into view.
    // see (https://googleplex-android-review.git.corp.google.com/#/c/166680/)

    // Exception: Photo uris for contacts from remote sources are not cached in the call log
    // cache, so we have to force a redraw for these contacts regardless.
    Boolean equals;
    Boolean updated = (existingInfo != ContactInfo::EMPTY || isRemoteSource) &&
            (info->Equals(TO_IINTERFACE(existingInfo), &equals), !equals);

    // Store the data in the cache so that the UI thread can use to display it. Store it
    mContactInfoCache->Put((IObject*)numberCountryIso, (IObject*)info);
    // Update the call log even if the cache it is up-to-date: it is possible that the cache
    // contains the value from a different call log entry.
    UpdateCallLogContactInfoCache(number, countryIso, info, callLogInfo);
    return updated;
}

void CallLogAdapter::AddGroups(
    /* [in] */ ICursor* cursor)
{
    mCallLogGroupBuilder->AddGroups(cursor);
}

AutoPtr<IView> CallLogAdapter::NewStandAloneView(
    /* [in] */ IContext* context,
    /* [in] */ IViewGroup* parent)
{
    return NewChildView(context, parent);
}

AutoPtr<IView> CallLogAdapter::NewGroupView(
    /* [in] */ IContext* context,
    /* [in] */ IViewGroup* parent)
{
    return NewChildView(context, parent);
}

AutoPtr<IView> CallLogAdapter::NewChildView(
    /* [in] */ IContext* context,
    /* [in] */ IViewGroup* parent)
{
    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(context, (ILayoutInflater**)&inflater);
    AutoPtr<IView> temp;
    inflater->Inflate(Elastos::Droid::Dialer::R::layout::call_log_list_item, parent, FALSE, (IView**)&temp);
    AutoPtr<ICallLogListItemView> view = ICallLogListItemView::Probe(temp);

    // Get the views to bind to and cache them.
    AutoPtr<CallLogListItemViews> views = CallLogListItemViews::FromView(IView::Probe(view));
    IView::Probe(view)->SetTag((IObject*)views);

    // Set text height to false on the TextViews so they don't have extra padding.
    views->mPhoneCallDetailsViews->mNameView->SetElegantTextHeight(FALSE);
    views->mPhoneCallDetailsViews->mCallLocationAndDate->SetElegantTextHeight(FALSE);

    return temp;
}

void CallLogAdapter::BindStandAloneView(
    /* [in] */ IView* view,
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor)
{
    BindView(view, cursor, 1);
}

void CallLogAdapter::BindChildView(
    /* [in] */ IView* view,
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor)
{
    BindView(view, cursor, 1);
}

void CallLogAdapter::BindGroupView(
    /* [in] */ IView* view,
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 groupSize,
    /* [in] */ Boolean expanded)
{
    BindView(view, cursor, groupSize);
}

void CallLogAdapter::FindAndCacheViews(
    /* [in] */ IView* view)
{}

void CallLogAdapter::BindView(
    /* [in] */ IView* view,
    /* [in] */ ICursor* c,
    /* [in] */ Int32 count)
{
    view->SetAccessibilityDelegate(mAccessibilityDelegate);
    AutoPtr<ICallLogListItemView> callLogItemView = ICallLogListItemView::Probe(view);
    AutoPtr<IInterface> temp;
    view->GetTag((IInterface**)&temp);
    AutoPtr<CallLogListItemViews> views = (CallLogListItemViews*)(IObject*)temp.Get();

    // Default case: an item in the call log.
    views->mPrimaryActionView->SetVisibility(IView::VISIBLE);

    String number;
    c->GetString(CallLogQuery::NUMBER, &number);
    Int32 numberPresentation;
    c->GetInt32(CallLogQuery::NUMBER_PRESENTATION, &numberPresentation);
    Int64 date;
    c->GetInt64(CallLogQuery::DATE, &date);
    Int64 duration;
    c->GetInt64(CallLogQuery::DURATION, &duration);
    Int32 callType;
    c->GetInt32(CallLogQuery::CALL_TYPE, &callType);
    String componentName, id;
    c->GetString(CallLogQuery::ACCOUNT_COMPONENT_NAME, &componentName);
    c->GetString(CallLogQuery::ACCOUNT_ID, &id);
    AutoPtr<IPhoneAccountHandle> accountHandle = PhoneAccountUtils::GetAccount(componentName, id);

    AutoPtr<IDrawable> accountIcon = PhoneAccountUtils::GetAccountIcon(
            mContext, accountHandle);
    String countryIso;
    c->GetString(CallLogQuery::COUNTRY_ISO, &countryIso);

    Int64 rowId;
    c->GetInt64(CallLogQuery::ID, &rowId);
    views->mRowId = rowId;

    // For entries in the call log, check if the day group has changed and display a header
    // if necessary.
    if (mIsCallLog) {
        Int32 currentGroup = GetDayGroupForCall(rowId);
        Int32 previousGroup = GetPreviousDayGroup(c);
        if (currentGroup != previousGroup) {
            IView::Probe(views->mDayGroupHeader)->SetVisibility(IView::VISIBLE);
            views->mDayGroupHeader->SetText(
                    GetGroupDescription(currentGroup));
        }
        else {
            IView::Probe(views->mDayGroupHeader)->SetVisibility(IView::GONE);
        }
    }
    else {
        IView::Probe(views->mDayGroupHeader)->SetVisibility(IView::GONE);
    }

    // Store some values used when the actions ViewStub is inflated on expansion of the actions
    // section.
    views->mNumber = number;
    views->mNumberPresentation = numberPresentation;
    views->mCallType = callType;
    // NOTE: This is currently not being used, but can be used in future versions.
    views->mAccountHandle = accountHandle;
    c->GetString(CallLogQuery::VOICEMAIL_URI, &views->mVoicemailUri);
    // Stash away the Ids of the calls so that we can support deleting a row in the call log.
    views->mCallIds = GetCallIds(c, count);

    AutoPtr<ContactInfo> cachedContactInfo = GetContactInfoFromCallLog(c);

    AutoPtr<ICharSequence> cs;
    CString::New(number, (ICharSequence**)&cs);
    Boolean isVoicemailNumber = PhoneNumberUtilsWrapper::INSTANCE->IsVoicemailNumber(cs);

    // Where binding and not in the call log, use default behaviour of invoking a call when
    // tapping the primary view.
    if (!mIsCallLog) {
        views->mPrimaryActionView->SetOnClickListener(mActionListener);

        // Set return call intent, otherwise null.
        if (PhoneNumberUtilsWrapper::CanPlaceCallsTo(cs, numberPresentation)) {
            // Sets the primary action to call the number.
            AutoPtr<IIntentProvider> p = IntentProvider::GetReturnCallIntentProvider(number);
            views->mPrimaryActionView->SetTag(p);
        }
        else {
            // Number is not callable, so hide button.
            views->mPrimaryActionView->SetTag(NULL);
        }
    }
    else {
        // In the call log, expand/collapse an actions section for the call log entry when
        // the primary view is tapped.
        views->mPrimaryActionView->SetOnClickListener(mExpandCollapseListener);

        // Note: Binding of the action buttons is done as required in configureActionViews
        // when the user expands the actions ViewStub.
    }

    // Lookup contacts with this number
    AutoPtr<NumberWithCountryIso> numberCountryIso = new NumberWithCountryIso(number, countryIso);
    AutoPtr<IExpirableCacheCachedValue> cachedInfo;
    mContactInfoCache->GetCachedValue((IObject*)numberCountryIso, (IExpirableCacheCachedValue**)&cachedInfo);

    AutoPtr<ContactInfo> info;
    if (cachedInfo != NULL) {
        AutoPtr<IInterface> value;
        cachedInfo->GetValue((IInterface**)&value);
        AutoPtr<ContactInfo> info = (ContactInfo*)(IObject*)value.Get();
    }
    if (!PhoneNumberUtilsWrapper::CanPlaceCallsTo(cs, numberPresentation)
            || isVoicemailNumber) {
        // If this is a number that cannot be dialed, there is no point in looking up a contact
        // for it.
        info = ContactInfo::EMPTY;
    }
    else if (cachedInfo == NULL) {
        mContactInfoCache->Put((IObject*)numberCountryIso, (IObject*)ContactInfo::EMPTY);
        // Use the cached contact info from the call log.
        info = cachedContactInfo;
        // The db request should happen on a non-UI thread.
        // Request the contact details immediately since they are currently missing.
        EnqueueRequest(number, countryIso, cachedContactInfo, TRUE);
        // We will format the phone number when we make the background request.
    }
    else {
        Boolean isExpired;
        if (cachedInfo->IsExpired(&isExpired), isExpired) {
            // The contact info is no longer up to date, we should request it. However, we
            // do not need to request them immediately.
            EnqueueRequest(number, countryIso, cachedContactInfo, FALSE);
        }
        else  if (!CallLogInfoMatches(cachedContactInfo, info)) {
            // The call log information does not match the one we have, look it up again.
            // We could simply update the call log directly, but that needs to be done in a
            // background thread, so it is easier to simply request a new lookup, which will, as
            // a side-effect, update the call log.
            EnqueueRequest(number, countryIso, cachedContactInfo, FALSE);
        }

        if (info == ContactInfo::EMPTY) {
            // Use the cached contact info from the call log.
            info = cachedContactInfo;
        }
    }

    AutoPtr<IUri> lookupUri = info->mLookupUri;
    String name = info->mName;
    Int32 ntype = info->mType;
    String label = info->mLabel;
    Int64 photoId = info->mPhotoId;
    AutoPtr<IUri> photoUri = info->mPhotoUri;
    AutoPtr<ICharSequence> formattedNumber;
    CString::New(info->mFormattedNumber, (ICharSequence**)&formattedNumber);
    AutoPtr<ArrayOf<Int32> > callTypes = GetCallTypes(c, count);
    String geocode;
    c->GetString(CallLogQuery::GEOCODED_LOCATION, &geocode);
    Int32 sourceType = info->mSourceType;
    Int32 features = GetCallFeatures(c, count);
    String transcription;
    c->GetString(CallLogQuery::TRANSCRIPTION, &transcription);
    Int64 dataUsage;
    Boolean isNull;
    if (c->IsNull(CallLogQuery::DATA_USAGE, &isNull), !isNull) {
        c->GetInt64(CallLogQuery::DATA_USAGE, &dataUsage);
    }

    AutoPtr<PhoneCallDetails> details;

    views->mReported = info->mIsBadData;

    // The entry can only be reported as invalid if it has a valid ID and the source of the
    // entry supports marking entries as invalid.
    views->mCanBeReportedAsInvalid = mContactInfoHelper->CanReportAsInvalid(info->mSourceType,
                info->mObjectId);

    // Restore expansion state of the row on rebind.  Inflate the actions ViewStub if required,
    // and set its visibility state accordingly.
    ExpandOrCollapseActions(callLogItemView, IsExpanded(rowId));

    if (TextUtils::IsEmpty(name)) {
        details = new PhoneCallDetails(cs, numberPresentation,
                formattedNumber, countryIso, geocode, callTypes, date,
                duration, String(NULL), accountIcon, features, dataUsage,
                transcription);
    }
    else {
        details = new PhoneCallDetails(cs, numberPresentation,
                formattedNumber, countryIso, geocode, callTypes, date,
                duration, cs, ntype,
                CoreUtils::Convert(label), lookupUri, photoUri, sourceType,
                String(NULL), accountIcon, features, dataUsage,
                transcription);
    }

    mCallLogViewsHelper->SetPhoneCallDetails(mContext, views, details);

    Int32 contactType = ContactPhotoManager::TYPE_DEFAULT;

    if (isVoicemailNumber) {
        contactType = ContactPhotoManager::TYPE_VOICEMAIL;
    }
    else if (mContactInfoHelper->IsBusiness(info->mSourceType)) {
        contactType = ContactPhotoManager::TYPE_BUSINESS;
    }

    String lookupKey = lookupUri == NULL ? String(NULL)
            : ContactInfoHelper::GetLookupKeyFromUri(lookupUri);

    String nameForDefaultImage;
    if (TextUtils::IsEmpty(cs)) {
        AutoPtr<ICharSequence> displayNumber = mPhoneNumberHelper->GetDisplayNumber(details->mNumber,
                details->mNumberPresentation,
                details->mFormattedNumber);
        displayNumber->ToString(&nameForDefaultImage);
    }
    else {
        nameForDefaultImage = name;
    }

    if (photoId == 0 && photoUri != NULL) {
        SetPhoto(views, photoUri, lookupUri, nameForDefaultImage, lookupKey, contactType);
    }
    else {
        SetPhoto(views, photoId, lookupUri, nameForDefaultImage, lookupKey, contactType);
    }

    // Listen for the first NULL
    if (mViewTreeObserver == NULL) {
        view->GetViewTreeObserver((IViewTreeObserver**)&mViewTreeObserver);
        mViewTreeObserver->AddOnPreDrawListener(IOnPreDrawListener::Probe(this));
    }

    BindBadge(view, info, details, callType);
}

Int32 CallLogAdapter::GetPreviousDayGroup(
    /* [in] */ ICursor* cursor)
{
    // We want to restore the position in the cursor at the end.
    Int32 startingPosition;
    cursor->GetPosition(&startingPosition);
    Int32 dayGroup = CallLogGroupBuilder::DAY_GROUP_NONE;
    Boolean succeeded;
    if (cursor->MoveToPrevious(&succeeded), succeeded) {
        Int64 previousRowId;
        cursor->GetInt64(CallLogQuery::ID, &previousRowId);
        dayGroup = GetDayGroupForCall(previousRowId);
    }
    cursor->MoveToPosition(startingPosition, &succeeded);
    return dayGroup;
}

Int32 CallLogAdapter::GetDayGroupForCall(
    /* [in] */ Int64 callId)
{
    HashMap<Int64, AutoPtr<IInteger32> >::Iterator it = mDayGroups.Begin();
    if (it != mDayGroups.End()) {
        Int32 value;
        it->mSecond->GetValue(&value);
        return value;
    }

    return CallLogGroupBuilder::DAY_GROUP_NONE;
}

Boolean CallLogAdapter::IsExpanded(
    /* [in] */ Int64 rowId)
{
    return mCurrentlyExpanded == rowId;
}

Boolean CallLogAdapter::ToggleExpansion(
    /* [in] */ Int64 rowId)
{
    if (rowId == mCurrentlyExpanded) {
        // Collapsing currently expanded row.
        mPreviouslyExpanded = NONE_EXPANDED;
        mCurrentlyExpanded = NONE_EXPANDED;

        return FALSE;
    }
    else {
        // Expanding a row (collapsing current expanded one).

        mPreviouslyExpanded = mCurrentlyExpanded;
        mCurrentlyExpanded = rowId;
        return TRUE;
    }
}

void CallLogAdapter::ExpandOrCollapseActions(
    /* [in] */ ICallLogListItemView* callLogItem,
    /* [in] */ Boolean isExpanded)
{
    AutoPtr<IInterface> tag;
    IView::Probe(callLogItem)->GetTag((IInterface**)&tag);
    AutoPtr<CallLogListItemViews> views = (CallLogListItemViews*)(IObject*)tag.Get();

    ExpandVoicemailTranscriptionView(views, isExpanded);
    if (isExpanded) {
        // Inflate the view stub if necessary, and wire up the event handlers.
        InflateActionViewStub(IView::Probe(callLogItem));

        views->mActionsView->SetVisibility(IView::VISIBLE);
        views->mActionsView->SetAlpha(1.0f);
        views->mCallLogEntryView->SetBackgroundColor(mExpandedBackgroundColor);
        views->mCallLogEntryView->SetTranslationZ(mExpandedTranslationZ);
        IView::Probe(callLogItem)->SetTranslationZ(mExpandedTranslationZ); // WAR
    }
    else {
        // When recycling a view, it is possible the actionsView ViewStub was previously
        // inflated so we should hide it in this case.
        if (views->mActionsView != NULL) {
            views->mActionsView->SetVisibility(IView::GONE);
        }

        views->mCallLogEntryView->SetBackgroundColor(mCallLogBackgroundColor);
        views->mCallLogEntryView->SetTranslationZ(0);
        IView::Probe(callLogItem)->SetTranslationZ(0); // WAR
    }
}

void CallLogAdapter::ExpandVoicemailTranscriptionView(
    /* [in] */ CallLogListItemViews* views,
    /* [in] */ Boolean isExpanded)
{
    if (views->mCallType != ICalls::VOICEMAIL_TYPE) {
        return;
    }

    AutoPtr<ITextView> view = views->mPhoneCallDetailsViews->mVoicemailTranscriptionView;
    AutoPtr<ICharSequence> text;
    view->GetText((ICharSequence**)&text);
    if (TextUtils::IsEmpty(text)) {
        return;
    }
    view->SetMaxLines(isExpanded ? VOICEMAIL_TRANSCRIPTION_MAX_LINES : 1);
    view->SetSingleLine(!isExpanded);
}

void CallLogAdapter::InflateActionViewStub(
    /* [in] */ IView* callLogItem)
{
    AutoPtr<IInterface> tag;
    callLogItem->GetTag((IInterface**)&tag);
    AutoPtr<CallLogListItemViews> views = (CallLogListItemViews*)(IObject*)tag.Get();

    AutoPtr<IView> viewStub;
    callLogItem->FindViewById(Elastos::Droid::Dialer::R::id::call_log_entry_actions_stub, (IView**)&viewStub);
    AutoPtr<IViewStub> stub = IViewStub::Probe(viewStub);
    if (stub != NULL) {
        stub->Inflate((IView**)&views->mActionsView);
    }

    if (views->mCallBackButtonView == NULL) {
        AutoPtr<IView> temp;
        views->mActionsView->FindViewById(
                Elastos::Droid::Dialer::R::id::call_back_action, (IView**)&temp);
        views->mCallBackButtonView = ITextView::Probe(temp);
    }

    if (views->mVideoCallButtonView == NULL) {
        AutoPtr<IView> temp;
        views->mActionsView->FindViewById(
                Elastos::Droid::Dialer::R::id::video_call_action, (IView**)&temp);
        views->mVideoCallButtonView = ITextView::Probe(temp);
    }

    if (views->mVoicemailButtonView == NULL) {
        AutoPtr<IView> temp;
        views->mActionsView->FindViewById(
                Elastos::Droid::Dialer::R::id::voicemail_action, (IView**)&temp);
        views->mVoicemailButtonView = ITextView::Probe(temp);
    }

    if (views->mDetailsButtonView == NULL) {
        AutoPtr<IView> temp;
        views->mActionsView->FindViewById(
                Elastos::Droid::Dialer::R::id::details_action, (IView**)&temp);
        views->mDetailsButtonView = ITextView::Probe(temp);
    }

    if (views->mReportButtonView == NULL) {
        AutoPtr<IView> temp;
        views->mActionsView->FindViewById(
                Elastos::Droid::Dialer::R::id::report_action, (IView**)&temp);
        views->mReportButtonView = ITextView::Probe(temp);
        AutoPtr<ReportButtonClickListener> listener = new ReportButtonClickListener(views, this);
        IView::Probe(views->mReportButtonView)->SetOnClickListener((IViewOnClickListener*)listener);
    }

    BindActionButtons(views);
}

void CallLogAdapter::BindActionButtons(
    /* [in] */ CallLogListItemViews* views)
{
    AutoPtr<ICharSequence> numberCS;
    CString::New(views->mNumber, (ICharSequence**)&numberCS);
    Boolean canPlaceCallToNumber = PhoneNumberUtilsWrapper::CanPlaceCallsTo(
            numberCS, views->mNumberPresentation);
    // Set return call intent, otherwise null.
    AutoPtr<IView> callBackButtonView = IView::Probe(views->mCallBackButtonView);
    if (canPlaceCallToNumber) {
        // Sets the primary action to call the number.
        callBackButtonView->SetTag(
                IntentProvider::GetReturnCallIntentProvider(views->mNumber));
        callBackButtonView->SetVisibility(IView::VISIBLE);
        callBackButtonView->SetOnClickListener(mActionListener);
    }
    else {
        // Number is not callable, so hide button.
        callBackButtonView->SetTag(NULL);
        callBackButtonView->SetVisibility(IView::GONE);
    }

    // If one of the calls had video capabilities, show the video call button.
    AutoPtr<IView> videoCallButtonView = IView::Probe(views->mVideoCallButtonView);
    Boolean isShown;
    if (CallUtil::IsVideoEnabled(mContext) && canPlaceCallToNumber &&
            views->mPhoneCallDetailsViews->mCallTypeIcons->IsVideoShown(&isShown), isShown) {
        videoCallButtonView->SetTag(
                IntentProvider::GetReturnVideoCallIntentProvider(views->mNumber));
        videoCallButtonView->SetVisibility(IView::VISIBLE);
        videoCallButtonView->SetOnClickListener(mActionListener);
    }
    else {
        videoCallButtonView->SetTag(NULL);
        videoCallButtonView->SetVisibility(IView::GONE);
    }

    // For voicemail calls, show the "VOICEMAIL" action button; hide otherwise.
    AutoPtr<IView> voicemailButtonView = IView::Probe(views->mVoicemailButtonView);
    AutoPtr<IView> detailsButtonView = IView::Probe(views->mDetailsButtonView);
    if (views->mCallType == ICalls::VOICEMAIL_TYPE) {
        voicemailButtonView->SetOnClickListener(mActionListener);
        voicemailButtonView->SetTag(IntentProvider::GetPlayVoicemailIntentProvider(
                views->mRowId, views->mVoicemailUri));
        voicemailButtonView->SetVisibility(IView::VISIBLE);

        detailsButtonView->SetVisibility(IView::GONE);
    }
    else {
        voicemailButtonView->SetTag(NULL);
        voicemailButtonView->SetVisibility(IView::GONE);

        detailsButtonView->SetOnClickListener(mActionListener);
        detailsButtonView->SetTag(IntentProvider::GetCallDetailIntentProvider(
                views->mRowId, views->mCallIds, String(NULL)));

        if (views->mCanBeReportedAsInvalid && !views->mReported) {
            IView::Probe(views->mReportButtonView)->SetVisibility(IView::VISIBLE);
        }
        else {
            IView::Probe(views->mReportButtonView)->SetVisibility(IView::GONE);
        }
    }

    mCallLogViewsHelper->SetActionContentDescriptions(views);
}

void CallLogAdapter::BindBadge(
    /* [in] */ IView* view,
    /* [in] */ ContactInfo* info,
    /* [in] */ PhoneCallDetails* details,
    /* [in] */ Int32 callType)
{
    // Do not show badge in call log.
    if (!mIsCallLog) {
        AutoPtr<IView> temp;
        view->FindViewById(Elastos::Droid::Dialer::R::id::link_stub, (IView**)&temp);
        AutoPtr<IViewStub> stub = IViewStub::Probe(temp);
        if (UriUtils::IsEncodedContactUri(info->mLookupUri)) {
            if (stub != NULL) {
                AutoPtr<IView> inflated;
                stub->Inflate((IView**)&inflated);
                inflated->SetVisibility(IView::VISIBLE);
                inflated->FindViewById(Elastos::Droid::Dialer::R::id::badge_link_container, (IView**)&mBadgeContainer);
                temp = NULL;
                inflated->FindViewById(Elastos::Droid::Dialer::R::id::badge_image, (IView**)&temp);
                mBadgeImageView = IImageView::Probe(temp);
                temp = NULL;
                inflated->FindViewById(Elastos::Droid::Dialer::R::id::badge_text, (IView**)&temp);
                mBadgeText = ITextView::Probe(temp);
            }

            AutoPtr<BadgeContainerClickListener> listener = new BadgeContainerClickListener(details, this);
            mBadgeContainer->SetOnClickListener((IViewOnClickListener*)listener);
            mBadgeImageView->SetImageResource(Elastos::Droid::Dialer::R::drawable::ic_person_add_24dp);
            mBadgeText->SetText(Elastos::Droid::Dialer::R::string::recentCalls_addToContact);
        }
        else {
            // Hide badge if it was previously shown.
            if (stub == NULL) {
                AutoPtr<IView> container;
                view->FindViewById(Elastos::Droid::Dialer::R::id::badge_container, (IView**)&container);
                if (container != NULL) {
                    container->SetVisibility(IView::GONE);
                }
            }
        }
    }
}

Boolean CallLogAdapter::CallLogInfoMatches(
    /* [in] */ ContactInfo* callLogInfo,
    /* [in] */ ContactInfo* info)
{
    // The call log only contains a subset of the fields in the contacts db.
    // Only check those.
    return TextUtils::Equals(callLogInfo->mName, info->mName)
            && callLogInfo->mType == info->mType
            && TextUtils::Equals(callLogInfo->mLabel, info->mLabel);
}

void CallLogAdapter::UpdateCallLogContactInfoCache(
    /* [in] */ const String& number,
    /* [in] */ const String& countryIso,
    /* [in] */ ContactInfo* updatedInfo,
    /* [in] */ ContactInfo* callLogInfo)
{
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    Boolean needsUpdate = FALSE;

    if (callLogInfo != NULL) {
        if (!TextUtils::Equals(updatedInfo->mName, callLogInfo->mName)) {
            values->Put(ICalls::CACHED_NAME, updatedInfo->mName);
            needsUpdate = TRUE;
        }

        if (updatedInfo->mType != callLogInfo->mType) {
            values->Put(ICalls::CACHED_NUMBER_TYPE, updatedInfo->mType);
            needsUpdate = TRUE;
        }

        if (!TextUtils::Equals(updatedInfo->mLabel, callLogInfo->mLabel)) {
            values->Put(ICalls::CACHED_NUMBER_LABEL, updatedInfo->mLabel);
            needsUpdate = TRUE;
        }
        if (!UriUtils::AreEqual(updatedInfo->mLookupUri, callLogInfo->mLookupUri)) {
            values->Put(ICalls::CACHED_LOOKUP_URI, UriUtils::UriToString(updatedInfo->mLookupUri));
            needsUpdate = TRUE;
        }
        // Only replace the normalized number if the new updated normalized number isn't empty.
        if (!TextUtils::IsEmpty(updatedInfo->mNormalizedNumber) &&
                !TextUtils::Equals(updatedInfo->mNormalizedNumber, callLogInfo->mNormalizedNumber)) {
            values->Put(ICalls::CACHED_NORMALIZED_NUMBER, updatedInfo->mNormalizedNumber);
            needsUpdate = TRUE;
        }
        if (!TextUtils::Equals(updatedInfo->mNumber, callLogInfo->mNumber)) {
            values->Put(ICalls::CACHED_MATCHED_NUMBER, updatedInfo->mNumber);
            needsUpdate = TRUE;
        }
        if (updatedInfo->mPhotoId != callLogInfo->mPhotoId) {
            values->Put(ICalls::CACHED_PHOTO_ID, updatedInfo->mPhotoId);
            needsUpdate = TRUE;
        }
        if (!TextUtils::Equals(updatedInfo->mFormattedNumber, callLogInfo->mFormattedNumber)) {
            values->Put(ICalls::CACHED_FORMATTED_NUMBER, updatedInfo->mFormattedNumber);
            needsUpdate = TRUE;
        }
    }
    else {
        // No previous values, store all of them.
        values->Put(ICalls::CACHED_NAME, updatedInfo->mName);
        values->Put(ICalls::CACHED_NUMBER_TYPE, updatedInfo->mType);
        values->Put(ICalls::CACHED_NUMBER_LABEL, updatedInfo->mLabel);
        values->Put(ICalls::CACHED_LOOKUP_URI, UriUtils::UriToString(updatedInfo->mLookupUri));
        values->Put(ICalls::CACHED_MATCHED_NUMBER, updatedInfo->mNumber);
        values->Put(ICalls::CACHED_NORMALIZED_NUMBER, updatedInfo->mNormalizedNumber);
        values->Put(ICalls::CACHED_PHOTO_ID, updatedInfo->mPhotoId);
        values->Put(ICalls::CACHED_FORMATTED_NUMBER, updatedInfo->mFormattedNumber);
        needsUpdate = TRUE;
    }

    if (!needsUpdate) return;

    AutoPtr<IContentResolver> resovler;
    mContext->GetContentResolver((IContentResolver**)&resovler);
    Int32 rowsAffected;
    AutoPtr<ArrayOf<String> > selectionArgs;
    if (countryIso.IsNull()) {
        selectionArgs = ArrayOf<String>::Alloc(1);
        (*selectionArgs)[0] = number;

        AutoPtr<ICalls> calls;
        CCalls::AcquireSingleton((ICalls**)&calls);
        AutoPtr<IUri> uri;
        calls->GetCONTENT_URI_WITH_VOICEMAIL((IUri**)&uri);
        resovler->Update(uri, values,
                ICalls::NUMBER + " = ? AND " + ICalls::COUNTRY_ISO + " IS NULL",
                selectionArgs, &rowsAffected);
    }
    else {
        selectionArgs = ArrayOf<String>::Alloc(2);
        (*selectionArgs)[0] = number;
        (*selectionArgs)[1] = countryIso;
        AutoPtr<ICalls> calls;
        CCalls::AcquireSingleton((ICalls**)&calls);
        AutoPtr<IUri> uri;
        calls->GetCONTENT_URI_WITH_VOICEMAIL((IUri**)&uri);
        resovler->Update(uri, values,
                ICalls::NUMBER + " = ? AND " + ICalls::COUNTRY_ISO + " = ?",
                selectionArgs, &rowsAffected);
    }
}

AutoPtr<ContactInfo> CallLogAdapter::GetContactInfoFromCallLog(
    /* [in] */ ICursor* c)
{
    AutoPtr<ContactInfo> info = new ContactInfo();

    String uriStr;
    c->GetString(CallLogQuery::CACHED_LOOKUP_URI, &uriStr);
    info->mLookupUri = UriUtils::ParseUriOrNull(uriStr);
    c->GetString(CallLogQuery::CACHED_NAME, &info->mName);
    c->GetInt32(CallLogQuery::CACHED_NUMBER_TYPE, &info->mType);
    c->GetString(CallLogQuery::CACHED_NUMBER_LABEL, &info->mLabel);

    String matchedNumber;
    c->GetString(CallLogQuery::CACHED_MATCHED_NUMBER, &matchedNumber);
    if (matchedNumber.IsNull()) {
        c->GetString(CallLogQuery::NUMBER, &info->mNumber);
    }
    else {
        info->mNumber = matchedNumber;
    }

    c->GetString(CallLogQuery::CACHED_NORMALIZED_NUMBER, &info->mNormalizedNumber);
    c->GetInt64(CallLogQuery::CACHED_PHOTO_ID, &info->mPhotoId);
    info->mPhotoUri = NULL;  // We do not cache the photo URI.
    c->GetString(CallLogQuery::CACHED_FORMATTED_NUMBER, &info->mFormattedNumber);
    return info;
}

AutoPtr<ArrayOf<Int32> > CallLogAdapter::GetCallTypes(
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 count)
{
    Int32 position;
    cursor->GetPosition(&position);
    AutoPtr<ArrayOf<Int32> > callTypes = ArrayOf<Int32>::Alloc(count);
    Boolean succeeded;
    for (Int32 index = 0; index < count; ++index) {
        cursor->GetInt32(CallLogQuery::CALL_TYPE, &(*callTypes)[index]);
        cursor->MoveToNext(&succeeded);
    }
    cursor->MoveToPosition(position, &succeeded);
    return callTypes;
}

Int32 CallLogAdapter::GetCallFeatures(
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 count)
{
    Int32 features = 0;
    Int32 position;
    cursor->GetPosition(&position);
    Int32 value = 0;
    Boolean succeeded;
    for (Int32 index = 0; index < count; ++index) {
        cursor->GetInt32(CallLogQuery::FEATURES, &value);
        features |= value;
        cursor->MoveToNext(&succeeded);
    }
    cursor->MoveToPosition(position, &succeeded);
    return features;
}

void CallLogAdapter::SetPhoto(
    /* [in] */ CallLogListItemViews* views,
    /* [in] */ Int64 photoId,
    /* [in] */ IUri* contactUri,
    /* [in] */ const String& displayName,
    /* [in] */ const String& identifier,
    /* [in] */ Int32 contactType)
{
    views->mQuickContactView->AssignContactUri(contactUri);
    views->mQuickContactView->SetOverlay(NULL);
    AutoPtr<ContactPhotoManager::DefaultImageRequest> request
            = new ContactPhotoManager::DefaultImageRequest(displayName, identifier,
                    contactType, TRUE /* isCircular */);
    mContactPhotoManager->LoadThumbnail(IImageView::Probe(views->mQuickContactView), photoId, FALSE /* darkTheme */,
            TRUE /* isCircular */, request);
}

void CallLogAdapter::SetPhoto(
    /* [in] */ CallLogListItemViews* views,
    /* [in] */ IUri* photoUri,
    /* [in] */ IUri* contactUri,
    /* [in] */ const String& displayName,
    /* [in] */ const String& identifier,
    /* [in] */ Int32 contactType)
{
    views->mQuickContactView->AssignContactUri(contactUri);
    views->mQuickContactView->SetOverlay(NULL);
    AutoPtr<ContactPhotoManager::DefaultImageRequest> request = new ContactPhotoManager::DefaultImageRequest(
            displayName, identifier, contactType, TRUE /* isCircular */);
    mContactPhotoManager->LoadDirectoryPhoto(IImageView::Probe(views->mQuickContactView), photoUri,
            FALSE /* darkTheme */, TRUE /* isCircular */, request);
}

void CallLogAdapter::BindViewForTest(
    /* [in] */ IView* view,
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor)
{
    BindStandAloneView(view, context, cursor);
    InflateActionViewStub(view);
}

void CallLogAdapter::DisableRequestProcessingForTest()
{
    mRequestProcessingDisabled = TRUE;
}

void CallLogAdapter::InjectContactInfoForTest(
    /* [in] */ const String& number,
    /* [in] */ const String& countryIso,
    /* [in] */ ContactInfo* contactInfo)
{
    AutoPtr<NumberWithCountryIso> numberCountryIso = new NumberWithCountryIso(number, countryIso);
    mContactInfoCache->Put((IObject*)numberCountryIso, (IObject*)contactInfo);
}

ECode CallLogAdapter::AddGroup(
    /* [in] */ Int32 cursorPosition,
    /* [in] */ Int32 size,
    /* [in] */ Boolean expanded)
{
    GroupingListAdapter::AddGroup(cursorPosition, size, expanded);
    return NOERROR;
}

ECode CallLogAdapter::SetDayGroup(
    /* [in] */ Int64 rowId,
    /* [in] */ Int32 dayGroup)
{
    if (mDayGroups.Find(rowId) == mDayGroups.End()) {
        AutoPtr<IInteger32> integer;
        CInteger32::New(dayGroup, (IInteger32**)&integer);
        mDayGroups[rowId] = integer;
    }
    return NOERROR;
}

ECode CallLogAdapter::ClearDayGroups()
{
    mDayGroups.Clear();
    return NOERROR;
}

ECode CallLogAdapter::GetBetterNumberFromContacts(
    /* [in] */ const String& _number,
    /* [in] */ const String& countryIso,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    String number = _number;

    String matchingNumber;
    // Look in the cache first. If it's not found then query the Phones db
    AutoPtr<NumberWithCountryIso> numberCountryIso = new NumberWithCountryIso(number, countryIso);
    AutoPtr<IInterface> value;
    mContactInfoCache->GetPossiblyExpired((IObject*)numberCountryIso, (IInterface**)&value);
    AutoPtr<ContactInfo> ci = (ContactInfo*)(IObject*)value.Get();
    if (ci != NULL && ci != ContactInfo::EMPTY) {
        matchingNumber = ci->mNumber;
    }
    else {
        // try {
        AutoPtr<IContentResolver> resolver;
        mContext->GetContentResolver((IContentResolver**)&resolver);

        AutoPtr<IContactsContractPhoneLookup> phoneLookup;
        CContactsContractPhoneLookup::AcquireSingleton((IContactsContractPhoneLookup**)&phoneLookup);
        AutoPtr<IUri> contentUri;
        phoneLookup->GetCONTENT_FILTER_URI((IUri**)&contentUri);
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        AutoPtr<IUri> uri;
        helper->WithAppendedPath(contentUri, number, (IUri**)&uri);
        AutoPtr<ICursor> phonesCursor;
        resolver->Query(uri, PhoneQuery::_PROJECTION,
                String(NULL), NULL, String(NULL), (ICursor**)&phonesCursor);
        if (phonesCursor != NULL) {
            // try {
            Boolean succeeded;
            if (phonesCursor->MoveToFirst(&succeeded), succeeded) {
                phonesCursor->GetString(PhoneQuery::MATCHED_NUMBER, &matchingNumber);
            }
            ICloseable::Probe(phonesCursor)->Close();
            // } finally {
            //     phonesCursor.close();
            // }
        }
        // } catch (Exception e) {
        //     // Use the number from the call log
        // }
    }
    if (!TextUtils::IsEmpty(matchingNumber) &&
            (matchingNumber.StartWith("+")
                    || matchingNumber.GetLength() > number.GetLength())) {
        number = matchingNumber;
    }

    *result = number;
    return NOERROR;
}

AutoPtr<ArrayOf<Int64> > CallLogAdapter::GetCallIds(
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 groupSize)
{
    // We want to restore the position in the cursor at the end.
    Int32 startingPosition;
    cursor->GetPosition(&startingPosition);
    AutoPtr<ArrayOf<Int64> > ids = ArrayOf<Int64>::Alloc(groupSize);

    // Copy the ids of the rows in the group.
    Boolean succeeded;
    for (Int32 index = 0; index < groupSize; ++index) {
        cursor->GetInt64(CallLogQuery::ID, &(*ids)[index]);
        cursor->MoveToNext(&succeeded);
    }
    cursor->MoveToPosition(startingPosition, &succeeded);
    return ids;
}

AutoPtr<ICharSequence> CallLogAdapter::GetGroupDescription(
    /* [in] */ Int32 group)
{
    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    String str;
    if (group == CallLogGroupBuilder::DAY_GROUP_TODAY) {
        resources->GetString(Elastos::Droid::Dialer::R::string::call_log_header_today, &str);
    }
    else if (group == CallLogGroupBuilder::DAY_GROUP_YESTERDAY) {
        resources->GetString(Elastos::Droid::Dialer::R::string::call_log_header_yesterday, &str);
    }
    else {
        resources->GetString(Elastos::Droid::Dialer::R::string::call_log_header_other, &str);
    }

    AutoPtr<ICharSequence> cs;
    CString::New(str, (ICharSequence**)&cs);
    return cs;
}

ECode CallLogAdapter::OnBadDataReported(
    /* [in] */ const String& number)
{
    mContactInfoCache->ExpireAll();
    mReportedToast->Show();
    return NOERROR;
}

void CallLogAdapter::HandleRowExpanded(
    /* [in] */ ICallLogListItemView* view,
    /* [in] */ Boolean animate,
    /* [in] */ Boolean forceExpand)
{
    AutoPtr<IInterface> temp;
    IView::Probe(view)->GetTag((IInterface**)&temp);
    AutoPtr<CallLogListItemViews> views = (CallLogListItemViews*)(IObject*)temp.Get();

    if (forceExpand && IsExpanded(views->mRowId)) {
        return;
    }

    // Hide or show the actions view.
    Boolean expanded = ToggleExpansion(views->mRowId);

    // Trigger loading of the viewstub and visual expand or collapse.
    ExpandOrCollapseActions(view, expanded);

    // Animate the expansion or collapse.
    if (mCallItemExpandedListener != NULL) {
        if (animate) {
            mCallItemExpandedListener->OnItemExpanded(view);
        }

        // Animate the collapse of the previous item if it is still visible on screen.
        if (mPreviouslyExpanded != NONE_EXPANDED) {
            AutoPtr<ICallLogListItemView> previousItem;
            mCallItemExpandedListener->GetViewForCallId(
                    mPreviouslyExpanded, (ICallLogListItemView**)&previousItem);

            if (previousItem != NULL) {
                ExpandOrCollapseActions(previousItem, FALSE);
                if (animate) {
                    mCallItemExpandedListener->OnItemExpanded(previousItem);
                }
            }
            mPreviouslyExpanded = NONE_EXPANDED;
        }
    }
}

} // CallLog
} // Dialer
} // Droid
} // Elastos
