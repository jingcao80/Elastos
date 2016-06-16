#include "elastos/apps/dialer/calllog/CallLogAdapter.h"
#include "elastos/apps/dialer/calllog/CallLogListItemViews.h"
#include "elastos/apps/dialer/calllog/CCallLogGroupBuilder.h"
#include "elastos/apps/dialer/calllog/CCallTypeHelper.h"
#include "elastos/apps/dialer/calllog/CallLogListItemHelper.h"
#include "elastos/apps/dialer/calllog/CPhoneNumberDisplayHelper.h"
#include "elastos/apps/dialer/calllog/CPhoneNumberUtilsWrapper.h"
#include "elastos/apps/dialer/calllog/CContactInfo.h"
#include "elastos/apps/dialer/calllog/PhoneAccountUtils.h"
#include "elastos/apps/dialer/calllog/IntentProvider.h"
#include "elastos/apps/dialer/calllog/ContactInfoHelper.h"
#include "elastos/apps/dialer/calllog/CPhoneQuery.h"
#include "elastos/apps/dialer/util/DialerUtils.h"
#include "elastos/apps/dialer/util/ExpirableCache.h"
#include "elastos/apps/dialer/DialtactsActivity.h"
#include "elastos/apps/dialer/CPhoneCallDetails.h"
#include "elastos/apps/dialer/CPhoneCallDetailsHelper.h"
#include "elastos/apps/dialer/PhoneCallDetailsViews.h"
#include <elastos/droid/text/TextUtils.h>
#include <elastos/droid/view/LayoutInflater.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include "Elastos.Droid.Provider.h"
#include "Elastos.CoreLibrary.IO.h"
#include "R.h"

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IIntent;
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
using Elastos::IO::ICloseable;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CLinkedList;
using Elastos::Apps::Dialer::CPhoneCallDetails;
using Elastos::Apps::Dialer::Util::DialerUtils;
using Elastos::Apps::Dialer::Util::ExpirableCache;
using Elastos::Apps::Dialer::Util::IExpirableCacheCachedValue;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace CallLog {

const Int32 CallLogAdapter::VOICEMAIL_TRANSCRIPTION_MAX_LINES = 10;

const Int32 CallLogAdapter::START_PROCESSING_REQUESTS_DELAY_MILLIS = 1000;

const Int32 CallLogAdapter::CONTACT_INFO_CACHE_SIZE = 100;

const Int64 CallLogAdapter::NONE_EXPANDED = -1;

const Int32 CallLogAdapter::REDRAW = 1;
const Int32 CallLogAdapter::START_THREAD = 2;

//=================================================================
// CallLogAdapter::NumberWithCountryIso
//=================================================================
CAR_INTERFACE_IMPL(CallLogAdapter::NumberWithCountryIso, Object, ICallLogAdapterNumberWithCountryIso);

CallLogAdapter::NumberWithCountryIso::NumberWithCountryIso(
    /* [in] */ const String& number,
    /* [in] */ const String& countryIso)
{
    mNumber = number;
    mCountryIso = countryIso;
}

ECode CallLogAdapter::NumberWithCountryIso::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (o == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    ICallLogAdapterNumberWithCountryIso* other = ICallLogAdapterNumberWithCountryIso::Probe(o);
    if (other == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    *result = TextUtils::Equals(mNumber, ((NumberWithCountryIso*)other)->mNumber)
            && TextUtils::Equals(mCountryIso, ((NumberWithCountryIso*)other)->mCountryIso);

    return NOERROR;
}

ECode CallLogAdapter::NumberWithCountryIso::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);

    *hashCode = (mNumber.IsNull() ? 0 : mNumber.GetHashCode())
            ^ (mCountryIso.IsNull() ? 0 : mCountryIso.GetHashCode());

    return NOERROR;
}

//=================================================================
// CallLogAdapter::ContactInfoRequest
//=================================================================
CAR_INTERFACE_IMPL(CallLogAdapter::ContactInfoRequest, Object, ICallLogAdapterContactInfoRequest)

CallLogAdapter::ContactInfoRequest::ContactInfoRequest(
    /* [in] */ const String& number,
    /* [in] */ const String& countryIso,
    /* [in] */ IContactInfo* callLogInfo)
    : mCallLogInfo(callLogInfo)
{
    mNumber = number;
    mCountryIso = countryIso;
}

ECode CallLogAdapter::ContactInfoRequest::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean equals;
    if (((IObject*)this)->Equals(obj, &equals), equals) {
        *result = TRUE;
        return NOERROR;
    }

    if (obj == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    ICallLogAdapterContactInfoRequest* other = ICallLogAdapterContactInfoRequest::Probe(obj);
    if (other == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    if (!TextUtils::Equals(mNumber, ((ContactInfoRequest*)other)->mNumber)) {
        *result = FALSE;
        return NOERROR;
    }
    if (!TextUtils::Equals(mCountryIso, ((ContactInfoRequest*)other)->mCountryIso)) {
        *result = FALSE;
        return NOERROR;
    }

    IObject::Probe(mCallLogInfo)->Equals(((ContactInfoRequest*)other)->mCallLogInfo, &equals);
    if (!equals) {
        *result = FALSE;
        return NOERROR;
    }

    *result = TRUE;

    return NOERROR;
}

ECode CallLogAdapter::ContactInfoRequest::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);

    Int32 prime = 31;
    Int32 result = 1;
    Int32 calllogInfoHash;
    result = prime * result + ((mCallLogInfo == NULL)
            ? 0 : (IObject::Probe(mCallLogInfo))->GetHashCode(&calllogInfoHash), calllogInfoHash);
    result = prime * result + (mCountryIso.IsNull() ? 0 : mCountryIso.GetHashCode());
    result = prime * result + (mCountryIso.IsNull() ? 0 : mNumber.GetHashCode());
    *hashCode = result;

    return NOERROR;
}

//=================================================================
// CallLogAdapter::ActionListener
//=================================================================
CAR_INTERFACE_IMPL(CallLogAdapter::ActionListener, Object, IViewOnClickListener)

CallLogAdapter::ActionListener::ActionListener(
    /* [in] */ CallLogAdapter* host)
    : mHost(host)
{}

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

CallLogAdapter::ExpandCollapseListener::ExpandCollapseListener(
    /* [in] */ CallLogAdapter* host)
    : mHost(host)
{}

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
CallLogAdapter::AccessibilityDelegate::AccessibilityDelegate(
    /* [in] */ CallLogAdapter* host)
    : mHost(host)
{}

ECode CallLogAdapter::AccessibilityDelegate::OnRequestSendAccessibilityEvent(
    /* [in] */ IViewGroup* host,
    /* [in] */ IView* child,
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* res)
{
    Int32 type;
    event->GetEventType(&type);
    if (type == IAccessibilityEvent::TYPE_VIEW_ACCESSIBILITY_FOCUSED) {
        mHost->HandleRowExpanded(ICallLogListItemView::Probe(host),
                 FALSE /* animate */, TRUE /* forceExpand */);
    }

    return View::AccessibilityDelegate::OnRequestSendAccessibilityEvent(host, child, event, res);
}

//=================================================================
// CallLogAdapter::CallLogAdapterHandler
//=================================================================
CallLogAdapter::CallLogAdapterHandler::CallLogAdapterHandler(
    /* [in] */ CallLogAdapter* host)
    : mHost(host)
{}

ECode CallLogAdapter::CallLogAdapterHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case REDRAW:
            assert(0 && "TODO");
            // mHost->NotifyDataSetChanged();
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
{}

ECode CallLogAdapter::QueryThread::constructor()
{
    return Thread::constructor(String("CallLogAdapter.QueryThread"));
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
        AutoPtr<ICallLogAdapterContactInfoRequest> req;
        {    AutoLock syncLock(mHost->mRequestsLock);
            Boolean isEmpty = TRUE;
            if (mHost->mRequests->IsEmpty(&isEmpty), !isEmpty) {
                AutoPtr<IInterface> item;
                mHost->mRequests->RemoveFirst((IInterface**)&item);
                req = ICallLogAdapterContactInfoRequest::Probe(item);
            }
        }

        if (req != NULL) {
            // Process the request. If the lookup succeeds, schedule a
            // redraw.
            CallLogAdapter::ContactInfoRequest* request = (CallLogAdapter::ContactInfoRequest*)req.Get();
            needRedraw |= mHost->QueryContactInfo(request->mNumber,
                    request->mCountryIso, request->mCallLogInfo);
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
                AutoLock syncLock(mHost->mRequestsLock);
                assert(0 && "TODO");
                // mHost->mRequests->Wait(1000);
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

CallLogAdapter::ReportButtonClickListener::ReportButtonClickListener(
    /* [in] */ ICallLogListItemViews* views,
    /* [in] */ CallLogAdapter* host)
    : mViews(views)
    , mHost(host)
{}

ECode CallLogAdapter::ReportButtonClickListener::OnClick(
    /* [in] */ IView* view)
{
    if (mHost->mOnReportButtonClickListener != NULL) {
        mHost->mOnReportButtonClickListener->OnReportButtonClick(
                ((CallLogListItemViews*)mViews.Get())->mNumber);
    }
    return NOERROR;
}

//=================================================================
// CallLogAdapter::BadgeContainerClickListener
//=================================================================
CAR_INTERFACE_IMPL(CallLogAdapter::BadgeContainerClickListener, Object, IViewOnClickListener)

CallLogAdapter::BadgeContainerClickListener::BadgeContainerClickListener(
    /* [in] */ IPhoneCallDetails* details,
    /* [in] */ CallLogAdapter* host)
    : mDetails(details)
    , mHost(host)
{}

ECode CallLogAdapter::BadgeContainerClickListener::OnClick(
    /* [in] */ IView* view)
{
    AutoPtr<IIntent> intent;
    DialtactsActivity::GetAddNumberToContactIntent(
            ((CPhoneCallDetails*)mDetails.Get())->mNumber);
    mHost->mContext->StartActivity(intent);
    return NOERROR;
}

//=================================================================
// CallLogAdapter
//=================================================================

CAR_INTERFACE_IMPL_3(CallLogAdapter, Object, ICallLogAdapter, IOnPreDrawListener, ICallLogGroupBuilderGroupCreator)

CallLogAdapter::CallLogAdapter()
    : mPreviouslyExpanded(NONE_EXPANDED)
    , mCurrentlyExpanded(NONE_EXPANDED)
    , mLoading(TRUE)
    , mRequestProcessingDisabled(FALSE)
    , mIsCallLog(TRUE)
{
    CHashMap::New((IHashMap**)&mDayGroups);
    mActionListener = new ActionListener(this);
    mExpandCollapseListener = new ExpandCollapseListener(this);
    mAccessibilityDelegate = new AccessibilityDelegate(this);
    mHandler = new CallLogAdapterHandler(this);
}

void CallLogAdapter::StartActivityForAction(
    /* [in] */ IView* view)
{
    AutoPtr<IInterface> tag;
    view->GetTag((IInterface**)&tag);
    IIntentProvider* intentProvider = IIntentProvider::Probe(tag);
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
    VALIDATE_NOT_NULL(result);

    // We only wanted to listen for the first draw (and this is it).
    UnregisterPreDrawListener();

    // Only schedule a thread-creation message if the thread hasn't been
    // created yet. This is purely an optimization, to queue fewer messages.
    if (mCallerIdThread == NULL) {
        Boolean res;
        mHandler->SendEmptyMessageDelayed(START_THREAD,
                START_PROCESSING_REQUESTS_DELAY_MILLIS, &res);
    }

    *result = true;
    return NOERROR;
}

ECode CallLogAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICallLogAdapterCallFetcher* callFetcher,
    /* [in] */ IContactInfoHelper* contactInfoHelper,
    /* [in] */ ICallLogAdapterCallItemExpandedListener* callItemExpandedListener,
    /* [in] */ ICallLogAdapterOnReportButtonClickListener* onReportButtonClickListener,
    /* [in] */ Boolean isCallLog)
{
    // GroupingListAdapter(context);

    mContext = context;
    mCallFetcher = callFetcher;
    mContactInfoHelper = contactInfoHelper;
    mIsCallLog = isCallLog;
    mCallItemExpandedListener = callItemExpandedListener;

    mOnReportButtonClickListener = onReportButtonClickListener;
    AutoPtr<IToastHelper> helper;
    CToastHelper::AcquireSingleton((IToastHelper**)&helper);
    helper->MakeText(mContext, R::string::toast_caller_id_reported,
             IToast::LENGTH_SHORT, (IToast**)&mReportedToast);

    mContactInfoCache = ExpirableCache::Create(CONTACT_INFO_CACHE_SIZE);
    CLinkedList::New((ILinkedList**)&mRequests);

    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    AutoPtr<ICallTypeHelper> callTypeHelper;
    CCallTypeHelper::New(resources, (ICallTypeHelper**)&callTypeHelper);
    resources->GetColor(R::color::background_dialer_list_items, &mCallLogBackgroundColor);
    resources->GetColor(R::color::call_log_expanded_background_color, &mExpandedBackgroundColor);
    resources->GetDimension(R::dimen::call_log_expanded_translation_z, &mExpandedTranslationZ);

    assert(0 && "TODO");
    // mContactPhotoManager = ContactPhotoManager::GetInstance(mContext);
    CPhoneNumberDisplayHelper::New(resources, (IPhoneNumberDisplayHelper**)&mPhoneNumberHelper);
    AutoPtr<IPhoneNumberUtilsWrapper> wrapper;
    CPhoneNumberUtilsWrapper::New((IPhoneNumberUtilsWrapper**)&wrapper);
    AutoPtr<IPhoneCallDetailsHelper> phoneCallDetailsHelper;
    CPhoneCallDetailsHelper::New(resources, callTypeHelper,
            wrapper, (IPhoneCallDetailsHelper**)&phoneCallDetailsHelper);

    mCallLogViewsHelper = new CallLogListItemHelper(
             phoneCallDetailsHelper, mPhoneNumberHelper, resources);
    CCallLogGroupBuilder::New(this, (ICallLogGroupBuilder**)&mCallLogGroupBuilder);

    return NOERROR;
}

ECode CallLogAdapter::OnContentChanged()
{
    return mCallFetcher->FetchCalls();
}

ECode CallLogAdapter::SetLoading(
    /* [in] */ Boolean loading)
{
    VALIDATE_NOT_NULL(loading);

    mLoading = loading;
    return NOERROR;
}

ECode CallLogAdapter::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (mLoading) {
        // We don't want the empty state to show when loading.
        *result = NOERROR;
    } else {
        // GroupingListAdapter::IsEmpty(result);
    }

    return NOERROR;
}

void CallLogAdapter::StartRequestProcessing()
{
    {    AutoLock syncLock(this);
        // For unit-testing.
        if (mRequestProcessingDisabled) return;

        // Idempotence... if a thread is already started, don't start another.
        if (mCallerIdThread != NULL) return;

        mCallerIdThread = new QueryThread(this);
        mCallerIdThread->SetPriority(IThread::MIN_PRIORITY);
        mCallerIdThread->Start();
    }
}

ECode CallLogAdapter::StopRequestProcessing()
{
    {    AutoLock syncLock(this);
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
}

void CallLogAdapter::UnregisterPreDrawListener()
{
    Boolean isAlive = FALSE;
    if (mViewTreeObserver != NULL
            && mViewTreeObserver->IsAlive(&isAlive), isAlive) {
        mViewTreeObserver->RemoveOnPreDrawListener(this);
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
    /* [in] */ IContactInfo* callLogInfo,
    /* [in] */ Boolean immediate)
{
    AutoPtr<ContactInfoRequest> request = new ContactInfoRequest(
        number, countryIso, callLogInfo);
    {    AutoLock syncLock(mRequestsLock);
        Boolean result;
        mRequests->Contains((ICallLogAdapterContactInfoRequest*)request, &result);
        if (!result) {
            mRequests->Add((ICallLogAdapterContactInfoRequest*)request);
            assert(0 && "TODO");
            // mRequests->NotifyAll();
        }
    }
    if (immediate) {
        StartRequestProcessing();
    }
}

Boolean CallLogAdapter::QueryContactInfo(
    /* [in] */ const String& number,
    /* [in] */ const String& countryIso,
    /* [in] */ IContactInfo* callLogInfo)
{
    AutoPtr<IContactInfo> info;
    mContactInfoHelper->LookupNumber(number, countryIso, (IContactInfo**)&info);

    if (info == NULL) {
        // The lookup failed, just return without requesting to update the view.
        return FALSE;
    }

    // Check the existing entry in the cache: only if it has changed we should update the
    // view.
    AutoPtr<NumberWithCountryIso> numberCountryIso =
            new NumberWithCountryIso(number, countryIso);
    AutoPtr<IInterface> expired;
    mContactInfoCache->GetPossiblyExpired(
            (ICallLogAdapterNumberWithCountryIso*)numberCountryIso,
            (IInterface**)&expired);
    IContactInfo* existingInfo = IContactInfo::Probe(expired);

    Boolean isRemoteSource = ((CContactInfo*)info.Get())->mSourceType != 0;

    // Don't force redraw if existing info in the cache is equal to {@link ContactInfo#EMPTY}
    // to avoid updating the data set for every new row that is scrolled into view.
    // see (https://googleplex-android-review.git.corp.google.com/#/c/166680/)

    // Exception: Photo uris for contacts from remote sources are not cached in the call log
    // cache, so we have to force a redraw for these contacts regardless.
    Boolean equals;
    Boolean updated = (existingInfo != CContactInfo::EMPTY || isRemoteSource) &&
            (IObject::Probe(info)->Equals(existingInfo, &equals), !equals);

    // Store the data in the cache so that the UI thread can use to display it. Store it
    // even if it has not changed so that it is marked as not expired.
    mContactInfoCache->Put(
            (ICallLogAdapterNumberWithCountryIso*)numberCountryIso, info);
    // Update the call log even if the cache it is up-to-date: it is possible that the cache
    // contains the value from a different call log entry.
    UpdateCallLogContactInfoCache(number, countryIso, info, callLogInfo);
    return updated;
}

ECode CallLogAdapter::AddGroups(
    /* [in] */ ICursor* cursor)
{
    mCallLogGroupBuilder->AddGroups(cursor);
    return NOERROR;
}

ECode CallLogAdapter::NewStandAloneView(
    /* [in] */ IContext* context,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    return NewChildView(context, parent, view);
}

ECode CallLogAdapter::NewGroupView(
    /* [in] */ IContext* context,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    return NewChildView(context, parent, view);
}

// @Override
ECode CallLogAdapter::NewChildView(
    /* [in] */ IContext* context,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(context, (ILayoutInflater**)&inflater);
    AutoPtr<IView> temp;
    inflater->Inflate(R::layout::call_log_list_item, parent, FALSE, (IView**)&temp);
    ICallLogListItemView* view = ICallLogListItemView::Probe(temp);

    // Get the views to bind to and cache them.
    AutoPtr<ICallLogListItemViews> views = CallLogListItemViews::FromView(IView::Probe(view));
    IView::Probe(view)->SetTag(views);

    // Set text height to false on the TextViews so they don't have extra padding.
    CallLogListItemViews* cviews = (CallLogListItemViews*)views.Get();
    PhoneCallDetailsViews* detailsViews = (PhoneCallDetailsViews*)cviews->mPhoneCallDetailsViews.Get();
    detailsViews->mNameView->SetElegantTextHeight(FALSE);
    detailsViews->mCallLocationAndDate->SetElegantTextHeight(FALSE);

    *result = temp;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CallLogAdapter::BindStandAloneView(
    /* [in] */ IView* view,
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor)
{
    BindView(view, cursor, 1);
    return NOERROR;
}

ECode CallLogAdapter::BindChildView(
    /* [in] */ IView* view,
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor)
{
    BindView(view, cursor, 1);
    return NOERROR;
}

ECode CallLogAdapter::BindGroupView(
    /* [in] */ IView* view,
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 groupSize,
    /* [in] */ Boolean expanded)
{
    BindView(view, cursor, groupSize);
    return NOERROR;
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
    ICallLogListItemView* callLogItemView = ICallLogListItemView::Probe(view);
    AutoPtr<IInterface> temp;
    view->GetTag((IInterface**)&temp);
    ICallLogListItemViews* views = ICallLogListItemViews::Probe(temp);

    // Default case: an item in the call log.
    ((CallLogListItemViews*)views)->mPrimaryActionView->SetVisibility(IView::VISIBLE);

    String number;
    c->GetString(ICallLogQuery::NUMBER, &number);
    Int32 numberPresentation;
    c->GetInt32(ICallLogQuery::NUMBER_PRESENTATION, &numberPresentation);
    Int64 date;
    c->GetInt64(ICallLogQuery::DATE, &date);
    Int64 duration;
    c->GetInt64(ICallLogQuery::DURATION, &duration);
    Int32 callType;
    c->GetInt32(ICallLogQuery::CALL_TYPE, &callType);
    String componentName, id;
    c->GetString(ICallLogQuery::ACCOUNT_COMPONENT_NAME, &componentName);
    c->GetString(ICallLogQuery::ACCOUNT_ID, &id);
    AutoPtr<IPhoneAccountHandle> accountHandle = PhoneAccountUtils::GetAccount(
            componentName, id);

    AutoPtr<IDrawable> accountIcon = PhoneAccountUtils::GetAccountIcon(
            mContext, accountHandle);
    String countryIso;
    c->GetString(ICallLogQuery::COUNTRY_ISO, &countryIso);

    Int64 rowId;
    c->GetInt64(ICallLogQuery::ID, &rowId);
    ((CallLogListItemViews*)views)->mRowId = rowId;

    // For entries in the call log, check if the day group has changed and display a header
    // if necessary.
    if (mIsCallLog) {
        Int32 currentGroup = GetDayGroupForCall(rowId);
        Int32 previousGroup = GetPreviousDayGroup(c);
        if (currentGroup != previousGroup) {
            IView::Probe(((CallLogListItemViews*)views)->mDayGroupHeader)->SetVisibility(IView::VISIBLE);
            ((CallLogListItemViews*)views)->mDayGroupHeader->SetText(
                    GetGroupDescription(currentGroup));
        }
        else {
            IView::Probe(((CallLogListItemViews*)views)->mDayGroupHeader)->SetVisibility(IView::GONE);
        }
    }
    else {
        IView::Probe(((CallLogListItemViews*)views)->mDayGroupHeader)->SetVisibility(IView::GONE);
    }

    // Store some values used when the actions ViewStub is inflated on expansion of the actions
    // section.
    ((CallLogListItemViews*)views)->mNumber = number;
    ((CallLogListItemViews*)views)->mNumberPresentation = numberPresentation;
    ((CallLogListItemViews*)views)->mCallType = callType;
    // NOTE: This is currently not being used, but can be used in future versions.
    ((CallLogListItemViews*)views)->mAccountHandle = accountHandle;
    c->GetString(ICallLogQuery::VOICEMAIL_URI, &((CallLogListItemViews*)views)->mVoicemailUri);
    // Stash away the Ids of the calls so that we can support deleting a row in the call log.
    ((CallLogListItemViews*)views)->mCallIds = GetCallIds(c, count);

    AutoPtr<IContactInfo> cachedContactInfo = GetContactInfoFromCallLog(c);

    Boolean isVoicemailNumber;
    PhoneNumberUtilsWrapper::INSTANCE->IsVoicemailNumber(
            CoreUtils::Convert(number), &isVoicemailNumber);

    // Where binding and not in the call log, use default behaviour of invoking a call when
    // tapping the primary view.
    if (!mIsCallLog) {
        ((CallLogListItemViews*)views)->mPrimaryActionView->SetOnClickListener(mActionListener);

        // Set return call intent, otherwise null.
        if (PhoneNumberUtilsWrapper::CanPlaceCallsTo(CoreUtils::Convert(number), numberPresentation)) {
            // Sets the primary action to call the number.
            ((CallLogListItemViews*)views)->mPrimaryActionView->SetTag(
                IntentProvider::GetReturnCallIntentProvider(number));
        }
        else {
            // Number is not callable, so hide button.
            ((CallLogListItemViews*)views)->mPrimaryActionView->SetTag(NULL);
        }
    }
    else {
        // In the call log, expand/collapse an actions section for the call log entry when
        // the primary view is tapped.
        ((CallLogListItemViews*)views)->mPrimaryActionView->SetOnClickListener(
                mExpandCollapseListener);

        // Note: Binding of the action buttons is done as required in configureActionViews
        // when the user expands the actions ViewStub.
    }

    // Lookup contacts with this number
    AutoPtr<NumberWithCountryIso> numberCountryIso = new NumberWithCountryIso(number, countryIso);
    AutoPtr<IExpirableCacheCachedValue> cachedInfo;
    mContactInfoCache->GetCachedValue(
            (IObject*)numberCountryIso, (IExpirableCacheCachedValue**)&cachedInfo);

    AutoPtr<IContactInfo> info;
    if (cachedInfo != NULL) {
        AutoPtr<IInterface> temp;
        cachedInfo->GetValue((IInterface**)&temp);
        info = IContactInfo::Probe(temp);
    }

    if (!PhoneNumberUtilsWrapper::CanPlaceCallsTo(CoreUtils::Convert(number), numberPresentation)
            || isVoicemailNumber) {
        // If this is a number that cannot be dialed, there is no point in looking up a contact
        // for it.
        info = CContactInfo::EMPTY;
    }
    else if (cachedInfo == NULL) {
        mContactInfoCache->Put((IObject*)numberCountryIso, CContactInfo::EMPTY);
        // Use the cached contact info from the call log.
        info = cachedContactInfo;
        // The db request should happen on a non-UI thread.
        // Request the contact details immediately since they are currently missing.
        EnqueueRequest(number, countryIso, cachedContactInfo, TRUE);
        // We will format the phone number when we make the background request.
    }
    else {
        Boolean isExpired = FALSE;
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

        if (info == CContactInfo::EMPTY) {
            // Use the cached contact info from the call log.
            info = cachedContactInfo;
        }
    }

    CContactInfo* cInfo = (CContactInfo*)info.Get();
    AutoPtr<IUri> lookupUri = cInfo->mLookupUri;
    String name = cInfo->mName;
    Int32 ntype = cInfo->mType;
    String label = cInfo->mLabel;
    Int64 photoId = cInfo->mPhotoId;
    AutoPtr<IUri> photoUri = cInfo->mPhotoUri;
    AutoPtr<ICharSequence> formattedNumber = CoreUtils::Convert(cInfo->mFormattedNumber);
    AutoPtr<ArrayOf<Int32> > callTypes = GetCallTypes(c, count);
    String geocode;
    c->GetString(ICallLogQuery::GEOCODED_LOCATION, &geocode);
    Int32 sourceType = cInfo->mSourceType;
    Int32 features = GetCallFeatures(c, count);
    String transcription;
    c->GetString(ICallLogQuery::TRANSCRIPTION, &transcription);
    Int64 dataUsage = 0;
    Boolean isNull = TRUE;
    if (c->IsNull(ICallLogQuery::DATA_USAGE, &isNull), !isNull) {
        c->GetInt64(ICallLogQuery::DATA_USAGE, &dataUsage);
    }

    AutoPtr<IPhoneCallDetails> details;

    ((CallLogListItemViews*)views)->mReported = cInfo->mIsBadData;

    // The entry can only be reported as invalid if it has a valid ID and the source of the
    // entry supports marking entries as invalid.
    mContactInfoHelper->CanReportAsInvalid(cInfo->mSourceType,
            cInfo->mObjectId,
            &((CallLogListItemViews*)views)->mCanBeReportedAsInvalid);

    // Restore expansion state of the row on rebind.  Inflate the actions ViewStub if required,
    // and set its visibility state accordingly.
    ExpandOrCollapseActions(callLogItemView, IsExpanded(rowId));

    if (TextUtils::IsEmpty(name)) {
        CPhoneCallDetails::New(CoreUtils::Convert(number), numberPresentation,
                formattedNumber, countryIso, geocode, callTypes, date,
                duration, String(NULL), accountIcon, features, dataUsage,
                transcription, (IPhoneCallDetails**)&details);
    }
    else {
        CPhoneCallDetails::New(CoreUtils::Convert(number), numberPresentation,
                formattedNumber, countryIso, geocode, callTypes, date,
                duration, CoreUtils::Convert(name), ntype,
                CoreUtils::Convert(label), lookupUri, photoUri, sourceType,
                String(NULL), accountIcon, features, dataUsage,
                transcription, (IPhoneCallDetails**)&details);
    }

    mCallLogViewsHelper->SetPhoneCallDetails(mContext, views, details);

    Int32 contactType;
    assert(0 && "TODO");
    // contactType = IContactPhotoManager::TYPE_DEFAULT;

    Boolean isBusiness = FALSE;
    if (isVoicemailNumber) {
        assert(0 && "TODO");
        // contactType = IContactPhotoManager::TYPE_VOICEMAIL;
    }
    else if (mContactInfoHelper->IsBusiness(cInfo->mSourceType, &isBusiness), isBusiness) {
        assert(0 && "TODO");
        // contactType = IContactPhotoManager::TYPE_BUSINESS;
    }

    String lookupKey = lookupUri == NULL ? String(NULL)
            : ContactInfoHelper::GetLookupKeyFromUri(lookupUri);

    String nameForDefaultImage;
    if (TextUtils::IsEmpty(name)) {
        AutoPtr<ICharSequence> displayNumber;
        mPhoneNumberHelper->GetDisplayNumber(((CPhoneCallDetails*)details.Get())->mNumber,
                ((CPhoneCallDetails*)details.Get())->mNumberPresentation,
                ((CPhoneCallDetails*)details.Get())->mFormattedNumber,
                (ICharSequence**)&displayNumber);
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
        mViewTreeObserver->AddOnPreDrawListener(this);
    }

    BindBadge(view, info, details, callType);
}

Int32 CallLogAdapter::GetPreviousDayGroup(
    /* [in] */ ICursor* cursor)
{
    // We want to restore the position in the cursor at the end.
    Int32 startingPosition;
    cursor->GetPosition(&startingPosition);
    Int32 dayGroup = ICallLogGroupBuilder::DAY_GROUP_NONE;
    Boolean succeeded = FALSE;
    if (cursor->MoveToPrevious(&succeeded), succeeded) {
        Int64 previousRowId;
        cursor->GetInt64(ICallLogQuery::ID, &previousRowId);
        dayGroup = GetDayGroupForCall(previousRowId);
    }
    cursor->MoveToPosition(startingPosition, &succeeded);
    return dayGroup;
}

Int32 CallLogAdapter::GetDayGroupForCall(
    /* [in] */ Int64 callId)
{
    Boolean contains = FALSE;
    if (mDayGroups->ContainsKey(CoreUtils::Convert(callId), &contains), contains) {
        AutoPtr<IInterface> value;
        mDayGroups->Get(CoreUtils::Convert(callId), (IInterface**)&value);
        return CoreUtils::Unbox(IInteger32::Probe(value));
    }
    return ICallLogGroupBuilder::DAY_GROUP_NONE;
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
    AutoPtr<IInterface> view;
    IView::Probe(callLogItem)->GetTag((IInterface**)&view);
    CallLogListItemViews* views = (CallLogListItemViews*)ICallLogListItemViews::Probe(view);

    ExpandVoicemailTranscriptionView(ICallLogListItemViews::Probe(view), isExpanded);
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
    /* [in] */ ICallLogListItemViews* views,
    /* [in] */ Boolean isExpanded)
{
    CallLogListItemViews* cviews = (CallLogListItemViews*)views;
    if (cviews->mCallType != ICalls::VOICEMAIL_TYPE) {
        return;
    }

    PhoneCallDetailsViews* detailsViews = (PhoneCallDetailsViews*)cviews->mPhoneCallDetailsViews.Get();
    AutoPtr<ITextView> view = detailsViews->mVoicemailTranscriptionView;
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
    AutoPtr<IInterface> view;
    callLogItem->GetTag((IInterface**)&view);
    ICallLogListItemViews* iviews = ICallLogListItemViews::Probe(view);
    CallLogListItemViews* views = (CallLogListItemViews*)iviews;

    AutoPtr<IView> viewStub;
    callLogItem->FindViewById(R::id::call_log_entry_actions_stub, (IView**)&viewStub);
    IViewStub* stub = IViewStub::Probe(viewStub);
    if (stub != NULL) {
        stub->Inflate((IView**)&views->mActionsView);
    }

    if (views->mCallBackButtonView == NULL) {
        AutoPtr<IView> temp;
        views->mActionsView->FindViewById(
                R::id::call_back_action, (IView**)&temp);
        views->mCallBackButtonView = ITextView::Probe(temp);
    }

    if (views->mVideoCallButtonView == NULL) {
        AutoPtr<IView> temp;
        views->mActionsView->FindViewById(
                R::id::video_call_action, (IView**)&temp);
        views->mVideoCallButtonView = ITextView::Probe(temp);
    }

    if (views->mVoicemailButtonView == NULL) {
        AutoPtr<IView> temp;
        views->mActionsView->FindViewById(
                R::id::voicemail_action, (IView**)&temp);
        views->mVoicemailButtonView = ITextView::Probe(temp);
    }

    if (views->mDetailsButtonView == NULL) {
        AutoPtr<IView> temp;
        views->mActionsView->FindViewById(
                R::id::details_action, (IView**)&temp);
        views->mDetailsButtonView = ITextView::Probe(temp);
    }

    if (views->mReportButtonView == NULL) {
        AutoPtr<IView> temp;
        views->mActionsView->FindViewById(
                R::id::report_action, (IView**)&temp);
        views->mReportButtonView = ITextView::Probe(temp);
        AutoPtr<ReportButtonClickListener> listener = new ReportButtonClickListener(
                iviews, this);
        IView::Probe(views->mReportButtonView)->SetOnClickListener(
            (IViewOnClickListener*)listener);
    }

    BindActionButtons(iviews);
}

void CallLogAdapter::BindActionButtons(
    /* [in] */ ICallLogListItemViews* views)
{
    CallLogListItemViews* cviews = (CallLogListItemViews*)views;
    Boolean canPlaceCallToNumber =
            PhoneNumberUtilsWrapper::CanPlaceCallsTo(
            CoreUtils::Convert(cviews->mNumber), cviews->mNumberPresentation);
    // Set return call intent, otherwise null.
    if (canPlaceCallToNumber) {
        // Sets the primary action to call the number.
        IView::Probe(cviews->mCallBackButtonView)->SetTag(
                IntentProvider::GetReturnCallIntentProvider(cviews->mNumber));
        IView::Probe(cviews->mCallBackButtonView)->SetVisibility(IView::VISIBLE);
        IView::Probe(cviews->mCallBackButtonView)->SetOnClickListener(mActionListener);
    }
    else {
        // Number is not callable, so hide button.
        IView::Probe(cviews->mCallBackButtonView)->SetTag(NULL);
        IView::Probe(cviews->mCallBackButtonView)->SetVisibility(IView::GONE);
    }

    // If one of the calls had video capabilities, show the video call button.
    Boolean isShown = FALSE;
    assert(0 && "TODO");
    // if (CallUtil::IsVideoEnabled(mContext) && canPlaceCallToNumber &&
    //         cviews->mPhoneCallDetailsViews->mCallTypeIcons->IsVideoShown(&isShown), isShown) {
    //     IView::Probe(cviews->mVideoCallButtonView)->SetTag(
    //             IntentProvider::GetReturnVideoCallIntentProvider(
    //             cviews->mNumber));
    //     IView::Probe(cviews->mVideoCallButtonView)->SetVisibility(IView::VISIBLE);
    //     IView::Probe(cviews->mVideoCallButtonView)->SetOnClickListener(mActionListener);
    // }
    // else {
    //     IView::Probe(cviews->mVideoCallButtonView)->SetTag(NULL);
    //     IView::Probe(cviews->mVideoCallButtonView)->SetVisibility(IView::GONE);
    // }

    // For voicemail calls, show the "VOICEMAIL" action button; hide otherwise.
    if (cviews->mCallType == ICalls::VOICEMAIL_TYPE) {
        IView::Probe(cviews->mVoicemailButtonView)->SetOnClickListener(mActionListener);
        IView::Probe(cviews->mVoicemailButtonView)->SetTag(
                IntentProvider::GetPlayVoicemailIntentProvider(
                cviews->mRowId, cviews->mVoicemailUri));
        IView::Probe(cviews->mVoicemailButtonView)->SetVisibility(IView::VISIBLE);

        IView::Probe(cviews->mDetailsButtonView)->SetVisibility(IView::GONE);
    }
    else {
        IView::Probe(cviews->mVoicemailButtonView)->SetTag(NULL);
        IView::Probe(cviews->mVoicemailButtonView)->SetVisibility(IView::GONE);

        IView::Probe(cviews->mDetailsButtonView)->SetOnClickListener(mActionListener);
        IView::Probe(cviews->mDetailsButtonView)->SetTag(
                IntentProvider::GetCallDetailIntentProvider(
                cviews->mRowId, cviews->mCallIds, String(NULL)));

        if (cviews->mCanBeReportedAsInvalid && !cviews->mReported) {
            IView::Probe(cviews->mReportButtonView)->SetVisibility(IView::VISIBLE);
        }
        else {
            IView::Probe(cviews->mReportButtonView)->SetVisibility(IView::GONE);
        }
    }

    mCallLogViewsHelper->SetActionContentDescriptions(views);
}

void CallLogAdapter::BindBadge(
    /* [in] */ IView* view,
    /* [in] */ IContactInfo* info,
    /* [in] */ IPhoneCallDetails* details,
    /* [in] */ Int32 callType)
{
    // Do not show badge in call log.
    if (!mIsCallLog) {
        AutoPtr<IView> temp;
        view->FindViewById(R::id::link_stub, (IView**)&temp);
        AutoPtr<IViewStub> stub = IViewStub::Probe(temp);
        Boolean result;
        assert(0 && "TODO");
        // result = UriUtils::IsEncodedContactUri(((CContactInfo*)info)->mLookupUri);
        if (result) {
            if (stub != NULL) {
                AutoPtr<IView> inflated;
                stub->Inflate((IView**)&inflated);
                inflated->SetVisibility(IView::VISIBLE);
                inflated->FindViewById(R::id::badge_link_container, (IView**)&mBadgeContainer);
                temp = NULL;
                inflated->FindViewById(R::id::badge_image, (IView**)&temp);
                mBadgeImageView = IImageView::Probe(temp);
                temp = NULL;
                inflated->FindViewById(R::id::badge_text, (IView**)&temp);
                mBadgeText = ITextView::Probe(temp);
            }

            AutoPtr<BadgeContainerClickListener> listener = new BadgeContainerClickListener(details, this);
            mBadgeContainer->SetOnClickListener((IViewOnClickListener*)listener);
            mBadgeImageView->SetImageResource(R::drawable::ic_person_add_24dp);
            mBadgeText->SetText(R::string::recentCalls_addToContact);
        }
        else {
            // Hide badge if it was previously shown.
            if (stub == NULL) {
                AutoPtr<IView> container;
                view->FindViewById(R::id::badge_container, (IView**)&container);
                if (container != NULL) {
                    container->SetVisibility(IView::GONE);
                }
            }
        }
    }
}

Boolean CallLogAdapter::CallLogInfoMatches(
    /* [in] */ IContactInfo* callLogInfo,
    /* [in] */ IContactInfo* info)
{
    // The call log only contains a subset of the fields in the contacts db.
    // Only check those.
    CContactInfo* cCallLogInfo = (CContactInfo*)callLogInfo;
    CContactInfo* cInfo = (CContactInfo*)info;
    return TextUtils::Equals(cCallLogInfo->mName, cInfo->mName)
            && cCallLogInfo->mType == cInfo->mType
            && TextUtils::Equals(cCallLogInfo->mLabel, cInfo->mLabel);
}

void CallLogAdapter::UpdateCallLogContactInfoCache(
    /* [in] */ const String& number,
    /* [in] */ const String& countryIso,
    /* [in] */ IContactInfo* updatedInfo,
    /* [in] */ IContactInfo* callLogInfo)
{
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    Boolean needsUpdate = FALSE;

    CContactInfo* cupdatedInfo = (CContactInfo*)updatedInfo;
    CContactInfo* ccallLogInfo = (CContactInfo*)callLogInfo;

    if (callLogInfo != NULL) {
        if (!TextUtils::Equals(cupdatedInfo->mName, ccallLogInfo->mName)) {
            values->Put(ICalls::CACHED_NAME, cupdatedInfo->mName);
            needsUpdate = TRUE;
        }

        if (cupdatedInfo->mType != ccallLogInfo->mType) {
            values->Put(ICalls::CACHED_NUMBER_TYPE, cupdatedInfo->mType);
            needsUpdate = TRUE;
        }

        if (!TextUtils::Equals(cupdatedInfo->mLabel, ccallLogInfo->mLabel)) {
            values->Put(ICalls::CACHED_NUMBER_LABEL, cupdatedInfo->mLabel);
            needsUpdate = TRUE;
        }
        assert(0 && "TODO");
        // if (!UriUtils::AreEqual(cupdatedInfo->mLookupUri, ccallLogInfo->mLookupUri)) {
        //     values->Put(ICalls::CACHED_LOOKUP_URI, UriUtils::UriToString(cupdatedInfo->mLookupUri));
        //     needsUpdate = TRUE;
        // }
        // Only replace the normalized number if the new updated normalized number isn't empty.
        if (!TextUtils::IsEmpty(cupdatedInfo->mNormalizedNumber) &&
                !TextUtils::Equals(cupdatedInfo->mNormalizedNumber, ccallLogInfo->mNormalizedNumber)) {
            values->Put(ICalls::CACHED_NORMALIZED_NUMBER, cupdatedInfo->mNormalizedNumber);
            needsUpdate = TRUE;
        }
        if (!TextUtils::Equals(cupdatedInfo->mNumber, ccallLogInfo->mNumber)) {
            values->Put(ICalls::CACHED_MATCHED_NUMBER, cupdatedInfo->mNumber);
            needsUpdate = TRUE;
        }
        if (cupdatedInfo->mPhotoId != ccallLogInfo->mPhotoId) {
            values->Put(ICalls::CACHED_PHOTO_ID, cupdatedInfo->mPhotoId);
            needsUpdate = TRUE;
        }
        if (!TextUtils::Equals(cupdatedInfo->mFormattedNumber, ccallLogInfo->mFormattedNumber)) {
            values->Put(ICalls::CACHED_FORMATTED_NUMBER, cupdatedInfo->mFormattedNumber);
            needsUpdate = TRUE;
        }
    }
    else {
        // No previous values, store all of them.
        values->Put(ICalls::CACHED_NAME, cupdatedInfo->mName);
        values->Put(ICalls::CACHED_NUMBER_TYPE, cupdatedInfo->mType);
        values->Put(ICalls::CACHED_NUMBER_LABEL, cupdatedInfo->mLabel);
        assert(0 && "TODO");
        // values->Put(ICalls::CACHED_LOOKUP_URI, UriUtils::UriToString(cupdatedInfo->mLookupUri));
        values->Put(ICalls::CACHED_MATCHED_NUMBER, cupdatedInfo->mNumber);
        values->Put(ICalls::CACHED_NORMALIZED_NUMBER, cupdatedInfo->mNormalizedNumber);
        values->Put(ICalls::CACHED_PHOTO_ID, cupdatedInfo->mPhotoId);
        values->Put(ICalls::CACHED_FORMATTED_NUMBER, cupdatedInfo->mFormattedNumber);
        needsUpdate = TRUE;
    }

    if (!needsUpdate) return;

    AutoPtr<IContentResolver> resovler;
    mContext->GetContentResolver((IContentResolver**)&resovler);
    Int32 rowsAffected;
    AutoPtr<ArrayOf<String> > selectionArgs;
    if (countryIso.IsNull()) {
        String str[] = { number };
        selectionArgs = ArrayOf<String>::Alloc(str, 1);

        AutoPtr<ICalls> calls;
        CCalls::AcquireSingleton((ICalls**)&calls);
        AutoPtr<IUri> uri;
        calls->GetCONTENT_URI_WITH_VOICEMAIL((IUri**)&uri);
        resovler->Update(uri, values,
                ICalls::NUMBER + " = ? AND " + ICalls::COUNTRY_ISO + " IS NULL",
                selectionArgs, &rowsAffected);
    } else {
        String str[] = { number, countryIso };
        selectionArgs = ArrayOf<String>::Alloc(str, 2);
        AutoPtr<ICalls> calls;
        CCalls::AcquireSingleton((ICalls**)&calls);
        AutoPtr<IUri> uri;
        calls->GetCONTENT_URI_WITH_VOICEMAIL((IUri**)&uri);
        resovler->Update(uri, values,
                ICalls::NUMBER + " = ? AND " + ICalls::COUNTRY_ISO + " = ?",
                selectionArgs, &rowsAffected);
    }
}

AutoPtr<IContactInfo> CallLogAdapter::GetContactInfoFromCallLog(
    /* [in] */ ICursor* c)
{
    AutoPtr<CContactInfo> info;
    CContactInfo::NewByFriend((CContactInfo**)&info);

    String uriStr;
    c->GetString(ICallLogQuery::CACHED_LOOKUP_URI, &uriStr);
    assert(0 && "TODO");
    // info->mLookupUri = UriUtils::ParseUriOrNull(uriStr);
    c->GetString(ICallLogQuery::CACHED_NAME, &info->mName);
    c->GetInt32(ICallLogQuery::CACHED_NUMBER_TYPE, &info->mType);
    c->GetString(ICallLogQuery::CACHED_NUMBER_LABEL, &info->mLabel);

    String matchedNumber;
    c->GetString(ICallLogQuery::CACHED_MATCHED_NUMBER, &matchedNumber);
    if (matchedNumber.IsNull()) {
        c->GetString(ICallLogQuery::NUMBER, &info->mNumber);
    }
    else {
        info->mNumber = matchedNumber;
    }

    c->GetString(ICallLogQuery::CACHED_NORMALIZED_NUMBER, &info->mNormalizedNumber);
    c->GetInt64(ICallLogQuery::CACHED_PHOTO_ID, &info->mPhotoId);
    info->mPhotoUri = NULL;  // We do not cache the photo URI.
    c->GetString(ICallLogQuery::CACHED_FORMATTED_NUMBER, &info->mFormattedNumber);
    return IContactInfo::Probe(info);
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
        cursor->GetInt32(ICallLogQuery::CALL_TYPE, &(*callTypes)[index]);
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
        cursor->GetInt32(ICallLogQuery::FEATURES, &value);
        features |= value;
        cursor->MoveToNext(&succeeded);
    }
    cursor->MoveToPosition(position, &succeeded);
    return features;
}

void CallLogAdapter::SetPhoto(
    /* [in] */ ICallLogListItemViews* views,
    /* [in] */ Int64 photoId,
    /* [in] */ IUri* contactUri,
    /* [in] */ const String& displayName,
    /* [in] */ const String& identifier,
    /* [in] */ Int32 contactType)
{
    CallLogListItemViews* cviews = (CallLogListItemViews*)views;
    cviews->mQuickContactView->AssignContactUri(contactUri);
    cviews->mQuickContactView->SetOverlay(NULL);
    assert(0 && "TODO");
    // AutoPtr<IDefaultImageRequest> request;
    // CDefaultImageRequest::New(displayName, identifier,
    //         contactType, TRUE /* isCircular */, (IDefaultImageRequest**)&request);
    // mContactPhotoManager->LoadThumbnail(cviews->mQuickContactView, photoId, FALSE /* darkTheme */,
    //             TRUE /* isCircular */, request);
}

void CallLogAdapter::SetPhoto(
    /* [in] */ ICallLogListItemViews* views,
    /* [in] */ IUri* photoUri,
    /* [in] */ IUri* contactUri,
    /* [in] */ const String& displayName,
    /* [in] */ const String& identifier,
    /* [in] */ Int32 contactType)
{
    CallLogListItemViews* cviews = (CallLogListItemViews*)views;
    cviews->mQuickContactView->AssignContactUri(contactUri);
    cviews->mQuickContactView->SetOverlay(NULL);
    assert(0 && "TODO");
    // AutoPtr<IDefaultImageRequest> request;
    // CDefaultImageRequest::New(displayName, identifier,
    //         contactType, TRUE /* isCircular */, (IDefaultImageRequest**)&request);
    // mContactPhotoManager->LoadDirectoryPhoto(cviews->mQuickContactView, photoUri,
    //         FALSE /* darkTheme */, TRUE /* isCircular */, request);
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
    /* [in] */ IContactInfo* contactInfo)
{
    AutoPtr<NumberWithCountryIso> numberCountryIso = new NumberWithCountryIso(number, countryIso);
    mContactInfoCache->Put(
            (ICallLogAdapterNumberWithCountryIso*)numberCountryIso, contactInfo);
}

ECode CallLogAdapter::AddGroup(
    /* [in] */ Int32 cursorPosition,
    /* [in] */ Int32 size,
    /* [in] */ Boolean expanded)
{
    assert(0 && "TODO");
    // return GroupingListAdapter::AddGroup(cursorPosition, size, expanded);
    return NOERROR;
}

ECode CallLogAdapter::SetDayGroup(
    /* [in] */ Int64 rowId,
    /* [in] */ Int64 dayGroup)
{
    Boolean result;
    mDayGroups->ContainsKey(CoreUtils::Convert(rowId), &result);
    if (!result) {
        mDayGroups->Put(CoreUtils::Convert(rowId), CoreUtils::Convert(dayGroup));
    }
    return NOERROR;
}

ECode CallLogAdapter::ClearDayGroups()
{
    mDayGroups->Clear();
    return NOERROR;
}

ECode CallLogAdapter::GetBetterNumberFromContacts(
    /* [in] */ const String& number,
    /* [in] */ const String& countryIso,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    String matchingNumber;
    // Look in the cache first. If it's not found then query the Phones db
    AutoPtr<NumberWithCountryIso> numberCountryIso = new NumberWithCountryIso(number, countryIso);
    AutoPtr<IInterface> expired;
    mContactInfoCache->GetPossiblyExpired((IObject*)numberCountryIso, (IInterface**)&expired);
    IContactInfo* ci = IContactInfo::Probe(expired);
    if (ci != NULL && ci != CContactInfo::EMPTY) {
        matchingNumber = ((CContactInfo*)ci)->mNumber;
    }
    else {
        // try {
        AutoPtr<IContentResolver> resolver;
        mContext->GetContentResolver((IContentResolver**)&resolver);

        AutoPtr<IContactsContractPhoneLookup> phoneLookup;
        CContactsContractPhoneLookup::AcquireSingleton(
                (IContactsContractPhoneLookup**)&phoneLookup);
        AutoPtr<IUri> contentUri;
        phoneLookup->GetCONTENT_FILTER_URI((IUri**)&contentUri);
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        AutoPtr<IUri> uri;
        helper->WithAppendedPath(contentUri, number, (IUri**)&uri);
        AutoPtr<ICursor> phonesCursor;
        //TODO:
        AutoPtr<ArrayOf<String> > projection = ArrayOf<String>::Alloc(
                (String*)CPhoneQuery::_PROJECTION, 9);
        resolver->Query(uri, projection,
                String(NULL), NULL, String(NULL), (ICursor**)&phonesCursor);
        if (phonesCursor != NULL) {
            // try {
            Boolean succeeded = FALSE;
            if (phonesCursor->MoveToFirst(&succeeded), succeeded) {
                phonesCursor->GetString(IPhoneQuery::MATCHED_NUMBER, &matchingNumber);
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
        assert(0 && "TODO");
        // number = matchingNumber;
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
        cursor->GetInt64(ICallLogQuery::ID, &(*ids)[index]);
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
    if (group == ICallLogGroupBuilder::DAY_GROUP_TODAY) {
        resources->GetString(R::string::call_log_header_today, &str);
    }
    else if (group == ICallLogGroupBuilder::DAY_GROUP_YESTERDAY) {
       resources->GetString(R::string::call_log_header_yesterday, &str);
    }
    else {
       resources->GetString(R::string::call_log_header_other, &str);
    }

    return CoreUtils::Convert(str);
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
    CallLogListItemViews* views = (CallLogListItemViews*)ICallLogListItemViews::Probe(temp);

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
} // Apps
} // Elastos
