
#include "elastos/droid/incallui/ContactInfoCache.h"
#include "elastos/droid/incallui/Call.h"
#include "elastos/droid/incallui/CallerInfoUtils.h"
#include "elastos/droid/incallui/ContactsAsyncHelper.h"
#include "elastos/droid/contacts/common/util/PhoneNumberHelper.h"
#include "elastos/droid/os/Looper.h"
#include <elastos/droid/text/TextUtils.h>
#include <elastos/core/StringBuilder.h>
#include "elastos/core/StringUtils.h"
#include <elastos/core/Thread.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Provider.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Contacts::Common::Util::PhoneNumberHelper;
using Elastos::Droid::Graphics::Drawable::CBitmapDrawable;
using Elastos::Droid::InCallUI::EIID_IContactCacheEntry;
using Elastos::Droid::InCallUI::EIID_IOnQueryCompleteListener;
using Elastos::Droid::InCallUI::Service::EIID_IImageLookupListener;
using Elastos::Droid::InCallUI::Service::EIID_INumberLookupListener;
using Elastos::Droid::Internal::Utility::IPreconditions;
using Elastos::Droid::Internal::Utility::CPreconditions;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsBaseTypes;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsPhone;
using Elastos::Droid::Provider::CContactsContractCommonDataKindsPhone;
using Elastos::Droid::Provider::CContactsContractContacts;
using Elastos::Droid::Provider::IContactsContractContacts;
using Elastos::Droid::Telecom::ITelecomManager;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::Thread;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace InCallUI {

// ================================================================
// ContactInfoCache::FindInfoCallback
// ================================================================
CAR_INTERFACE_IMPL(ContactInfoCache::FindInfoCallback, Object, IOnQueryCompleteListener)

ECode ContactInfoCache::FindInfoCallback::OnQueryComplete(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ ICallerInfo* callerInfo)
{
    mHost->FindInfoQueryComplete((Call*)ICall::Probe(cookie), (CallerInfo*)callerInfo, mIsIncoming, TRUE);
    return NOERROR;
}


//================================================================
// ContactInfoCache::PhoneNumberServiceListener
//================================================================
ContactInfoCache::PhoneNumberServiceListener::PhoneNumberServiceListener(
    /* [in] */ const String& callId,
    /* [in] */ ContactInfoCache* host)
    : mCallId(callId)
    , mHost(host)
{}

CAR_INTERFACE_IMPL_2(ContactInfoCache::PhoneNumberServiceListener, Object, INumberLookupListener, IImageLookupListener)

ECode ContactInfoCache::PhoneNumberServiceListener::OnPhoneNumberInfoComplete(
    /* [in] */ IPhoneNumberInfo* info)
{
    // If we got a miss, this is the end of the lookup pipeline,
    // so clear the callbacks and return.
    if (info == NULL) {
        Logger::D(TAG, "Contact lookup done. Remote contact not found.");
        mHost->ClearCallbacks(mCallId);
        return NOERROR;
    }

    AutoPtr<ContactCacheEntry> entry = new ContactCacheEntry();
    info->GetDisplayName(&entry->mName);
    info->GetNumber(&entry->mNumber);
    Int32 type;
    info->GetPhoneType(&type);
    String label;
    info->GetPhoneLabel(&label);
    if (type == IContactsContractCommonDataKindsBaseTypes::TYPE_CUSTOM) {
        entry->mLabel = label;
    }
    else {
        AutoPtr<IContactsContractCommonDataKindsPhone> phone;
        CContactsContractCommonDataKindsPhone::AcquireSingleton((IContactsContractCommonDataKindsPhone**)&phone);
        AutoPtr<IResources> res;
        mHost->mContext->GetResources((IResources**)&res);
        AutoPtr<ICharSequence> typeStr;
        AutoPtr<ICharSequence> cs;
        CString::New(label, (ICharSequence**)&cs);
        phone->GetTypeLabel(res, type, cs, (ICharSequence**)&typeStr);
        if (typeStr != NULL) {
            typeStr->ToString(&entry->mLabel);
        }
        else {
            entry->mLabel = String(NULL);
        }
    }
    AutoPtr<ContactCacheEntry> oldEntry;
    HashMap<String, AutoPtr<ContactCacheEntry> >::Iterator it = mHost->mInfoMap.Find(mCallId);
    if (it != mHost->mInfoMap.End()) {
        // Location is only obtained from local lookup so persist
        // the value for remote lookups. Once we have a name this
        // field is no longer used; it is persisted here in case
        // the UI is ever changed to use it.
        oldEntry = it->mSecond;
        entry->mLocation = oldEntry->mLocation;
    }

    // If no image and it's a business, switch to using the default business avatar.
    String url;
    Boolean isBusiness;
    if ((info->GetImageUrl(&url), url.IsNull()) && (info->IsBusiness(&isBusiness), isBusiness)) {
        Logger::D(TAG, "Business has no image. Using default.");
        AutoPtr<IResources> res;
        mHost->mContext->GetResources((IResources**)&res);
        AutoPtr<IDrawable> temp;
        res->GetDrawable(Elastos::Droid::Dialer::R::drawable::img_business, (IDrawable**)&temp);
        entry->mPhoto = temp;
    }

    // Add the contact info to the cache.
    mHost->mInfoMap[mCallId] = entry;
    mHost->SendInfoNotifications(mCallId, entry);

    // If there is no image then we should not expect another callback.
    if (info->GetImageUrl(&url), url.IsNull()) {
        // We're done, so clear callbacks
        mHost->ClearCallbacks(mCallId);
    }
    return NOERROR;
}

ECode ContactInfoCache::PhoneNumberServiceListener::OnImageFetchComplete(
    /* [in] */ IBitmap* bitmap)
{
    AutoPtr<ICharSequence> cs;
    CString::New(mCallId, (ICharSequence**)&cs);
    return mHost->OnImageLoadComplete(TOKEN_UPDATE_PHOTO_FOR_CALL_STATE, NULL, bitmap, cs);
}

//================================================================
// ContactInfoCache::ImageLoadCompleteListener
//================================================================

CAR_INTERFACE_IMPL(ContactInfoCache::ImageLoadCompleteListener, Object, IOnImageLoadCompleteListener);

ContactInfoCache::ImageLoadCompleteListener::ImageLoadCompleteListener(
    /* [in] */ ContactInfoCache* host)
    : mHost(host)
{}

ECode ContactInfoCache::ImageLoadCompleteListener::OnImageLoadComplete(
    /* [in] */ Int32 token,
    /* [in] */ IDrawable* photo,
    /* [in] */ IBitmap* photoIcon,
    /* [in] */ IInterface* cookie)
{
    return mHost->OnImageLoadComplete(token, photo, photoIcon, cookie);
}

//================================================================
// ContactInfoCache::ContactCacheEntry
//================================================================
CAR_INTERFACE_IMPL(ContactInfoCache::ContactCacheEntry, Object, IContactCacheEntry);

ECode ContactInfoCache::ContactCacheEntry::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("name: ");
    sb.Append(mName);
    sb.Append("number: ");
    sb.Append(mNumber);
    sb.Append("location: ");
    sb.Append(mLocation);
    sb.Append("label: ");
    sb.Append(mLabel);
    sb.Append("photo: ");
    sb.Append(IInterface::Probe(mPhoto));
    sb.Append("isSipCall: ");
    sb.Append(mIsSipCall);
    sb.Append("contactUri: ");
    sb.Append(IInterface::Probe(mContactUri));
    sb.Append("displayPhotoUri: ");
    sb.Append(IInterface::Probe(mDisplayPhotoUri));
    return sb.ToString(str);
}


//================================================================
// ContactInfoCache
//================================================================
const String ContactInfoCache::TAG("ContactInfoCache");
const Int32 ContactInfoCache::TOKEN_UPDATE_PHOTO_FOR_CALL_STATE;
AutoPtr<ContactInfoCache> ContactInfoCache::sCache;

AutoPtr<ContactInfoCache> ContactInfoCache::GetInstance(
    /* [in] */ IContext* context)
{
    if (sCache == NULL) {
        AutoPtr<IContext> ctx;
        context->GetApplicationContext((IContext**)&ctx);
        sCache = new ContactInfoCache(ctx);
    }
    return sCache;
}

ContactInfoCache::ContactInfoCache(
    /* [in] */ IContext* context)
    : mContext(context)
{
    // TODO: no serviceFactory
    // mPhoneNumberService = ServiceFactory.newPhoneNumberService(context);
}

AutoPtr<ContactInfoCache::ContactCacheEntry> ContactInfoCache::GetInfo(
    /* [in] */ const String& callId)
{
    HashMap<String, AutoPtr<ContactCacheEntry> >::Iterator it = mInfoMap.Find(callId);
    if (it != mInfoMap.End()) return it->mSecond;
    return NULL;
}

AutoPtr<ContactInfoCache::ContactCacheEntry> ContactInfoCache::BuildCacheEntryFromCall(
    /* [in] */ IContext* context,
    /* [in] */ Call* call,
    /* [in] */ Boolean isIncoming)
{
    AutoPtr<ContactCacheEntry> entry = new ContactCacheEntry();

    // TODO: get rid of caller info.
    AutoPtr<CallerInfo> info = CallerInfoUtils::BuildCallerInfo(context, call);
    PopulateCacheEntry(context, info, entry, call->GetNumberPresentation(), isIncoming);
    return entry;
}

void ContactInfoCache::FindInfo(
    /* [in] */ Call* call,
    /* [in] */ Boolean isIncoming,
    /* [in] */ IContactInfoCacheCallback* callback)
{
    AutoPtr<IPreconditions> preconditions;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditions);
    AutoPtr<IThread> thread;
    Looper::GetMainLooper()->GetThread((IThread**)&thread);
    if (FAILED(preconditions->CheckState(thread == Thread::GetCurrentThread())))
        return;
    if (FAILED(preconditions->CheckNotNull(callback)))
        return;

    String callId = call->GetId();
    AutoPtr<ContactCacheEntry> cacheEntry;
    HashMap<String, AutoPtr<ContactCacheEntry> >::Iterator it = mInfoMap.Find(callId);
    if (it != mInfoMap.End()) {
        cacheEntry = it->mSecond;
    }
    AutoPtr<Set<AutoPtr<IContactInfoCacheCallback> > > callBacks;
    HashMap<String, AutoPtr<Set<AutoPtr<IContactInfoCacheCallback> > > >::Iterator cbsIt = mCallBacks.Find(callId);
    if (cbsIt != mCallBacks.End()) {
        callBacks = cbsIt->mSecond;
    }

    // If we have a previously obtained intermediate result return that now
    if (cacheEntry != NULL) {
        Logger::D(TAG, "Contact lookup. In memory cache hit; lookup %s"
                , (callBacks == NULL ? "complete" : "still running"));
        callback->OnContactInfoComplete(callId, (IContactCacheEntry*)cacheEntry.Get());
        // If no other callbacks are in flight, we're done.
        if (callBacks == NULL) {
            return;
        }
    }

    // If the entry already exists, add callback
    if (callBacks != NULL) {
        callBacks->Insert(callback);
        return;
    }
    Logger::D(TAG, "Contact lookup. In memory cache miss; searching provider.");
    // New lookup
    callBacks = new Set<AutoPtr<IContactInfoCacheCallback> >();
    callBacks->Insert(callback);
    mCallBacks[callId] = callBacks;

    /**
     * Performs a query for caller information.
     * Save any immediate data we get from the query. An asynchronous query may also be made
     * for any data that we do not already have. Some queries, such as those for voicemail and
     * emergency call information, will not perform an additional asynchronous query.
     */
    AutoPtr<IOnQueryCompleteListener> listener = (IOnQueryCompleteListener*)new FindInfoCallback(isIncoming, this);
    AutoPtr<CallerInfo> callerInfo = CallerInfoUtils::GetCallerInfoForCall(mContext, call, listener);

    FindInfoQueryComplete(call, callerInfo, isIncoming, FALSE);
}

void ContactInfoCache::FindInfoQueryComplete(
    /* [in] */ Call* call,
    /* [in] */ CallerInfo* callerInfo,
    /* [in] */ Boolean isIncoming,
    /* [in] */ Boolean didLocalLookup)
{
    String callId = call->GetId();
    Int32 presentationMode = call->GetNumberPresentation();
    if (callerInfo->mContactExists || callerInfo->IsEmergencyNumber() ||
            callerInfo->IsVoiceMailNumber()) {
        presentationMode = ITelecomManager::PRESENTATION_ALLOWED;
    }

    AutoPtr<ContactCacheEntry> cacheEntry;
    HashMap<String, AutoPtr<ContactCacheEntry> >::Iterator it = mInfoMap.Find(callId);
    if (it != mInfoMap.End()) {
        cacheEntry = it->mSecond;
    }
    // Rebuild the entry from the new data if:
    // 1) This is NOT the asynchronous local lookup (IOW, this is the first pass)
    // 2) The local lookup was done and the contact exists
    // 3) The existing cached entry is empty (no name).
    if (!didLocalLookup || callerInfo->mContactExists ||
            (cacheEntry != NULL && TextUtils::IsEmpty(cacheEntry->mName))) {
        cacheEntry = BuildEntry(mContext, callId, callerInfo, presentationMode, isIncoming);
        mInfoMap[callId] = cacheEntry;
    }

    SendInfoNotifications(callId, cacheEntry);

    if (didLocalLookup) {
        // Before issuing a request for more data from other services, We only check that the
        // contact wasn't found in the local DB.  We don't check the if the cache entry already
        // has a name because we allow overriding cnap data with data from other services.
        if (!callerInfo->mContactExists && mPhoneNumberService != NULL) {
            Logger::D(TAG, "Contact lookup. Local contacts miss, checking remote");
            AutoPtr<PhoneNumberServiceListener> listener = new PhoneNumberServiceListener(callId, this);
            mPhoneNumberService->GetPhoneNumberInfo(cacheEntry->mNumber, listener, listener, isIncoming);
        }
        else if (cacheEntry->mDisplayPhotoUri != NULL) {
            Logger::D(TAG, "Contact lookup. Local contact found, starting image load");
            // Load the image with a callback to update the image state.
            // When the load is finished, onImageLoadComplete() will be called.
            AutoPtr<ImageLoadCompleteListener> listener = new ImageLoadCompleteListener(this);
            AutoPtr<ICharSequence> cs;
            CString::New(callId, (ICharSequence**)&cs);
            ContactsAsyncHelper::StartObtainPhotoAsync(
                TOKEN_UPDATE_PHOTO_FOR_CALL_STATE, mContext, cacheEntry->mDisplayPhotoUri,
                listener, cs);
        }
        else {
            if (callerInfo->mContactExists) {
                Logger::D(TAG, "Contact lookup done. Local contact found, no image.");
            }
            else {
                Logger::D(TAG, "Contact lookup done. Local contact not found and no remote lookup service available.");
            }
            ClearCallbacks(callId);
        }
    }
}

ECode ContactInfoCache::OnImageLoadComplete(
    /* [in] */ Int32 token,
    /* [in] */ IDrawable* photo,
    /* [in] */ IBitmap* photoIcon,
    /* [in] */ IInterface* cookie)
{
    Logger::D(TAG, "Image load complete with context: %s", TO_CSTR(mContext));
    // TODO: may be nice to update the image view again once the newer one
    // is available on contacts database.

    String callId;
    ICharSequence::Probe(cookie)->ToString(&callId);
    AutoPtr<ContactCacheEntry> entry;
    HashMap<String, AutoPtr<ContactCacheEntry> >::Iterator it = mInfoMap.Find(callId);
    if (it != mInfoMap.End()) {
        entry = it->mSecond;
    }

    if (entry == NULL) {
        Logger::E(TAG, "Image Load received for empty search entry.");
        ClearCallbacks(callId);
        return NOERROR;
    }
    Logger::D(TAG, "setting photo for entry: %s", TO_CSTR(entry));

    // Conference call icons are being handled in CallCardPresenter.
    if (photo != NULL) {
        Logger::V(TAG, "direct drawable: %s", TO_CSTR(photo));
        entry->mPhoto = photo;
    }
    else if (photoIcon != NULL) {
        Logger::V(TAG, "photo icon: %s", TO_CSTR(photoIcon));
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        AutoPtr<IDrawable> temp;
        CBitmapDrawable::New(res, photoIcon, (IDrawable**)&temp);
        entry->mPhoto = temp;
    }
    else {
        Logger::V(TAG, "unknown photo");
        entry->mPhoto = NULL;
    }

    SendImageNotifications(callId, entry);
    ClearCallbacks(callId);
    return NOERROR;
}

void ContactInfoCache::ClearCache()
{
    mInfoMap.Clear();
    mCallBacks.Clear();
}

AutoPtr<ContactInfoCache::ContactCacheEntry> ContactInfoCache::BuildEntry(
    /* [in] */ IContext* context,
    /* [in] */ const String& callId,
    /* [in] */ CallerInfo* info,
    /* [in] */ Int32 presentation,
    /* [in] */ Boolean isIncoming)
{
    // The actual strings we're going to display onscreen:
    AutoPtr<IDrawable> photo;

    AutoPtr<ContactCacheEntry> cce = new ContactCacheEntry();
    PopulateCacheEntry(context, info, cce, presentation, isIncoming);

    // This will only be true for emergency numbers
    if (info->mPhotoResource != 0) {
        AutoPtr<IResources> res;
        context->GetResources((IResources**)&res);
        res->GetDrawable(info->mPhotoResource, (IDrawable**)&photo);
    }
    else if (info->mIsCachedPhotoCurrent) {
        if (info->mCachedPhoto != NULL) {
            photo = info->mCachedPhoto;
        }
        else {
            AutoPtr<IResources> res;
            context->GetResources((IResources**)&res);
            res->GetDrawable(Elastos::Droid::Dialer::R::drawable::img_no_image, (IDrawable**)&photo);
            photo->SetAutoMirrored(TRUE);
        }
    }
    else if (info->mContactDisplayPhotoUri == NULL) {
        AutoPtr<IResources> res;
        context->GetResources((IResources**)&res);
        res->GetDrawable(Elastos::Droid::Dialer::R::drawable::img_no_image, (IDrawable**)&photo);
        photo->SetAutoMirrored(TRUE);
    }
    else {
        cce->mDisplayPhotoUri = info->mContactDisplayPhotoUri;
    }

    if (info->mLookupKeyOrNull.IsNull() || info->mContactIdOrZero == 0) {
        Logger::V(TAG, "lookup key is null or contact ID is 0. Don't create a lookup uri.");
        cce->mLookupUri = NULL;
    }
    else {
        AutoPtr<IContactsContractContacts> contacts;
        CContactsContractContacts::AcquireSingleton((IContactsContractContacts**)&contacts);
        AutoPtr<IUri> temp;
        contacts->GetLookupUri(info->mContactIdOrZero, info->mLookupKeyOrNull, (IUri**)&temp);
        cce->mLookupUri = temp;
    }

    cce->mPhoto = photo;
    cce->mLookupKey = info->mLookupKeyOrNull;

    return cce;
}

void ContactInfoCache::PopulateCacheEntry(
    /* [in] */ IContext* context,
    /* [in] */ CallerInfo* info,
    /* [in] */ ContactCacheEntry* cce,
    /* [in] */ Int32 presentation,
    /* [in] */ Boolean isIncoming)
{
    AutoPtr<IPreconditions> preconditions;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditions);
    if (FAILED(preconditions->CheckNotNull((ICallerInfo*)info)))
        return;
    String displayName;
    String displayNumber;
    String displayLocation;
    String label;
    Boolean isSipCall = FALSE;

    // It appears that there is a small change in behaviour with the
    // PhoneUtils' startGetCallerInfo whereby if we query with an
    // empty number, we will get a valid CallerInfo object, but with
    // fields that are all null, and the isTemporary boolean input
    // parameter as true.

    // In the past, we would see a NULL callerinfo object, but this
    // ends up causing null pointer exceptions elsewhere down the
    // line in other cases, so we need to make this fix instead. It
    // appears that this was the ONLY call to PhoneUtils
    // .getCallerInfo() that relied on a NULL CallerInfo to indicate
    // an unknown contact.

    // Currently, infi.phoneNumber may actually be a SIP address, and
    // if so, it might sometimes include the "sip:" prefix. That
    // prefix isn't really useful to the user, though, so strip it off
    // if present. (For any other URI scheme, though, leave the
    // prefix alone.)
    // TODO: It would be cleaner for CallerInfo to explicitly support
    // SIP addresses instead of overloading the "phoneNumber" field.
    // Then we could remove this hack, and instead ask the CallerInfo
    // for a "user visible" form of the SIP address.
    String number = info->mPhoneNumber;

    if (!TextUtils::IsEmpty(number)) {
        isSipCall = PhoneNumberHelper::IsUriNumber(number);
        if (number.StartWith("sip:")) {
            number = number.Substring(4);
        }
    }

    if (TextUtils::IsEmpty(info->mName)) {
        // No valid "name" in the CallerInfo, so fall back to
        // something else.
        // (Typically, we promote the phone number up to the "name" slot
        // onscreen, and possibly display a descriptive string in the
        // "number" slot.)
        if (TextUtils::IsEmpty(number)) {
            // No name *or* number! Display a generic "unknown" string
            // (or potentially some other default based on the presentation.)
            displayName = GetPresentationString(context, presentation);
            Logger::D(TAG, "  ==> no name *or* number! displayName = %s", displayName.string());
        }
        else if (presentation != ITelecomManager::PRESENTATION_ALLOWED) {
            // This case should never happen since the network should never send a phone #
            // AND a restricted presentation. However we leave it here in case of weird
            // network behavior
            displayName = GetPresentationString(context, presentation);
            Logger::D(TAG, "  ==> presentation not allowed! displayName = %s", displayName.string());
        }
        else if (!TextUtils::IsEmpty(info->mCnapName)) {
            // No name, but we do have a valid CNAP name, so use that.
            displayName = info->mCnapName;
            info->mName = info->mCnapName;
            displayNumber = number;
            Logger::D(TAG, "  ==> cnapName available: displayName '%s', displayNumber '%s'"
                    , displayName.string(), displayNumber.string());
        }
        else {
            // No name; all we have is a number. This is the typical
            // case when an incoming call doesn't match any contact,
            // or if you manually dial an outgoing number using the
            // dialpad.
            displayNumber = number;

            // Display a geographical description string if available
            // (but only for incoming calls.)
            if (isIncoming) {
                // TODO (CallerInfoAsyncQuery cleanup): Fix the CallerInfo
                // query to only do the geoDescription lookup in the first
                // place for incoming calls.
                displayLocation = info->mGeoDescription; // may be null
                Logger::D(TAG, "Geodescrption: %s", info->mGeoDescription.string());
            }

            Logger::D(TAG, "  ==>  no name; falling back to number: displayNumber '%s', displayLocation '%s'"
                    , displayNumber.string(), displayLocation.string());
        }
    }
    else {
        // We do have a valid "name" in the CallerInfo. Display that
        // in the "name" slot, and the phone number in the "number" slot.
        if (presentation != ITelecomManager::PRESENTATION_ALLOWED) {
            // This case should never happen since the network should never send a name
            // AND a restricted presentation. However we leave it here in case of weird
            // network behavior
            displayName = GetPresentationString(context, presentation);
            Logger::D(TAG, "  ==> valid name, but presentation not allowed! displayName = %s", displayName.string());
        }
        else {
            displayName = info->mName;
            displayNumber = number;
            label = info->mPhoneLabel;
            Logger::D(TAG, "  ==>  name is present in CallerInfo: displayName '%s', displayNumber '%s'"
                    , displayName.string(), displayNumber.string());
        }
    }

    cce->mName = displayName;
    cce->mNumber = displayNumber;
    cce->mLocation = displayLocation;
    cce->mLabel = label;
    cce->mIsSipCall = isSipCall;
}

void ContactInfoCache::SendInfoNotifications(
    /* [in] */ const String& callId,
    /* [in] */ ContactCacheEntry* entry)
{
    AutoPtr<Set<AutoPtr<IContactInfoCacheCallback> > > callBacks;
    HashMap<String, AutoPtr<Set<AutoPtr<IContactInfoCacheCallback> > > >::Iterator it = mCallBacks.Find(callId);
    if (it != mCallBacks.End()) {
        callBacks = it->mSecond;
    }
    if (callBacks != NULL) {
        Set<AutoPtr<IContactInfoCacheCallback> >::Iterator cbIt = callBacks->Begin();
        for (; cbIt != callBacks->End(); ++cbIt) {
            (*cbIt)->OnContactInfoComplete(callId, (IContactCacheEntry*)entry);
        }
    }
}

void ContactInfoCache::SendImageNotifications(
    /* [in] */ const String& callId,
    /* [in] */ ContactCacheEntry* entry)
{
    AutoPtr<Set<AutoPtr<IContactInfoCacheCallback> > > callBacks;
    HashMap<String, AutoPtr<Set<AutoPtr<IContactInfoCacheCallback> > > >::Iterator it = mCallBacks.Find(callId);
    if (it != mCallBacks.End()) {
        callBacks = it->mSecond;
    }
    if (callBacks != NULL && entry->mPhoto != NULL) {
        Set<AutoPtr<IContactInfoCacheCallback> >::Iterator cbIt = callBacks->Begin();
        for (; cbIt != callBacks->End(); ++cbIt) {
            (*cbIt)->OnImageLoadComplete(callId, (IContactCacheEntry*)entry);
        }
    }
}

void ContactInfoCache::ClearCallbacks(
    /* [in] */ const String& callId)
{
    mCallBacks.Erase(callId);
}

String ContactInfoCache::GetPresentationString(
    /* [in] */ IContext* context,
    /* [in] */ Int32 presentation)
{
    String name;
    context->GetString(Elastos::Droid::Dialer::R::string::unknown, &name);
    if (presentation == ITelecomManager::PRESENTATION_RESTRICTED) {
        context->GetString(Elastos::Droid::Dialer::R::string::private_num, &name);
    }
    else if (presentation == ITelecomManager::PRESENTATION_PAYPHONE) {
        context->GetString(Elastos::Droid::Dialer::R::string::payphone, &name);
    }
    return name;
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos