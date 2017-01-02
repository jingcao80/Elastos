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

#include <Elastos.Droid.Provider.h>
#include "elastos/droid/server/notification/ValidateNotificationPeople.h"
#include "elastos/droid/os/AsyncTask.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::IContactsContractContactOptionsColumns;
using Elastos::Droid::Provider::CContactsContractContacts;
using Elastos::Droid::Provider::IContactsContractContacts;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CContactsContractPhoneLookup;
using Elastos::Droid::Provider::IContactsContractPhoneLookup;
using Elastos::Droid::Provider::CContactsContractCommonDataKindsEmail;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsEmail;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::ILogHelper;
using Elastos::Droid::Utility::CLruCache;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IString;
using Elastos::Core::EIID_IChar32;
using Elastos::Core::EIID_ICharSequence;
using Elastos::Core::IArrayOf;
using Elastos::Core::IString;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Core::StringBuilder;
using Elastos::Core::CoreUtils;
using Elastos::Core::AutoLock;
using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;
using Elastos::Utility::Concurrent::CTimeUnitHelper;
using Elastos::Utility::Concurrent::ITimeUnitHelper;
using Elastos::Utility::Concurrent::CSemaphore;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::CLinkedList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

const Float ValidateNotificationPeople::NONE = 0.0f;

const Float ValidateNotificationPeople::VALID_CONTACT = 0.5f;

const Float ValidateNotificationPeople::STARRED_CONTACT = 1.0f;

const String ValidateNotificationPeople::TAG("ValidateNoPeople");
const Boolean ValidateNotificationPeople::INFO = TRUE;
const Boolean ValidateNotificationPeople::DEBUG = Logger::IsLoggable("ValidateNoPeople", ILogHelper::DEBUG);

const Boolean ValidateNotificationPeople::ENABLE_PEOPLE_VALIDATOR = TRUE;
const String ValidateNotificationPeople::SETTING_ENABLE_PEOPLE_VALIDATOR("validate_notification_people_enabled");

static AutoPtr< ArrayOf<String> > InitLOOKUP_PROJECTION()
{
    AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(2);
    (*args)[0] = IBaseColumns::ID;
    (*args)[1] = IContactsContractContactOptionsColumns::STARRED;
    return args;
}

const AutoPtr< ArrayOf<String> > ValidateNotificationPeople::LOOKUP_PROJECTION = InitLOOKUP_PROJECTION();
const Int32 ValidateNotificationPeople::MAX_PEOPLE = 10;
const Int32 ValidateNotificationPeople::PEOPLE_CACHE_SIZE = 200;

const Int64 ValidateNotificationPeople::LookupResult::CONTACT_REFRESH_MILLIS = 60 * 60 * 1000LL;

//===============================================================================
//                  ValidateNotificationPeople::LookupResult
//===============================================================================

ValidateNotificationPeople::LookupResult::LookupResult()
    : mExpireMillis(0)
    , mAffinity(NONE)
{
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 now;
    sys->GetCurrentTimeMillis(&now);
    mExpireMillis = now + CONTACT_REFRESH_MILLIS;
}

ValidateNotificationPeople::LookupResult::~LookupResult()
{}

void ValidateNotificationPeople::LookupResult::MergeContact(
    /* [in] */ ICursor* cursor)
{
    using Elastos::Core::Math;
    mAffinity = Math::Max(mAffinity, VALID_CONTACT);

    // Contact ID
    Int32 id;
    Int32 idIdx;
    cursor->GetColumnIndex(IBaseColumns::ID, &idIdx);
    if (idIdx >= 0) {
        cursor->GetInt32(idIdx, &id);
        if (DEBUG) Slogger::D(TAG, "contact _ID is: %d", id);
    }
    else {
        id = -1;
        Slogger::I(TAG, "invalid cursor: no _ID");
    }

    // Starred
    Int32 starIdx;
    cursor->GetColumnIndex(IContactsContractContactOptionsColumns::STARRED, &starIdx);
    if (starIdx >= 0) {
        cursor->GetInt32(starIdx, &id);
        Boolean isStarred = id != 0;
        if (isStarred) {
            mAffinity = Math::Max(mAffinity, STARRED_CONTACT);
        }
        if (DEBUG) Slogger::D(TAG, "contact STARRED is: %d", isStarred);
    }
    else {
        if (DEBUG) Slogger::D(TAG, "invalid cursor: no STARRED");
    }
}

Boolean ValidateNotificationPeople::LookupResult::IsExpired()
{
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 now;
    sys->GetCurrentTimeMillis(&now);

    return mExpireMillis < now;
}

Boolean ValidateNotificationPeople::LookupResult::IsInvalid()
{
    return mAffinity == NONE || IsExpired();
}

Float ValidateNotificationPeople::LookupResult::GetAffinity()
{
    if (IsInvalid()) {
        return NONE;
    }
    return mAffinity;
}

//===============================================================================
//                  ValidateNotificationPeople::PeopleRankingReconsideration
//===============================================================================

ValidateNotificationPeople::PeopleRankingReconsideration::PeopleRankingReconsideration(
    /* [in] */ IContext* context,
    /* [in] */ const String& key,
    /* [in] */ ILinkedList* pendingLookups,
    /* [in] */ ValidateNotificationPeople* host)
    : RankingReconsideration(key)
    , mPendingLookups(pendingLookups)
    , mContext(context)
    , mContactAffinity(NONE)
    , mHost(host)
{
}

ValidateNotificationPeople::PeopleRankingReconsideration::~PeopleRankingReconsideration()
{
}

ECode ValidateNotificationPeople::PeopleRankingReconsideration::Work()
{
    if (INFO) Slogger::I(TAG, "Executing: validation for: %s", mKey.string());

    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 timeStartMs;
    sys->GetCurrentTimeMillis(&timeStartMs);

    Int32 size;
    mPendingLookups->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mPendingLookups->Get(i, (IInterface**)&obj);
        String handle;
        ICharSequence::Probe(obj)->ToString(&handle);

        AutoPtr<LookupResult> lookupResult;
        AutoPtr<IUriHelper> uriHelper;
        CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
        AutoPtr<IUri> uri;
        uriHelper->Parse(handle, (IUri**)&uri);
        String scheme;
        uri->GetScheme(&scheme);
        String part;
        uri->GetSchemeSpecificPart(&part);

        AutoPtr<IContactsContractContacts> con;
        CContactsContractContacts::AcquireSingleton((IContactsContractContacts**)&con);
        AutoPtr<IUri> lookupUri;
        con->GetCONTENT_LOOKUP_URI((IUri**)&lookupUri);
        String str = Object::ToString(lookupUri);

        if (scheme.Equals("tel")) {
            if (DEBUG) Slogger::D(TAG, "checking telephone URI: %s", handle.string());
            lookupResult = mHost->ResolvePhoneContact(mContext, part);
        }
        else if (scheme.Equals("mailto")) {
            if (DEBUG) Slogger::D(TAG, "checking mailto URI: %s", handle.string());
            lookupResult = mHost->ResolveEmailContact(mContext, part);
        }
        else if (handle.StartWith(str)) {
            if (DEBUG) Slogger::D(TAG, "checking lookup URI: %s", handle.string());
            lookupResult = mHost->SearchContacts(mContext, uri);
        }
        else {
            lookupResult = new LookupResult();  // invalid person for the cache
            Slogger::W(TAG, "unsupported URI %s", handle.string());
        }
        if (lookupResult != NULL) {
            {
                AutoLock lock(mHost->mPeopleCache);
                Int32 userId;
                mContext->GetUserId(&userId);
                const String cacheKey = mHost->GetCacheKey(userId, handle);
                AutoPtr<ICharSequence> key = CoreUtils::Convert(cacheKey);
                mHost->mPeopleCache->Put(key, (IObject*)lookupResult, NULL);
            }
            using Elastos::Core::Math;
            mContactAffinity = Math::Max(mContactAffinity, lookupResult->GetAffinity());
        }
    }
    if (DEBUG) {
        Int64 now;
        sys->GetCurrentTimeMillis(&now);
        Slogger::D(TAG, "Validation finished in %lld ms", (now - timeStartMs));
    }
    return NOERROR;
}

ECode ValidateNotificationPeople::PeopleRankingReconsideration::ApplyChangesLocked(
    /* [in] */ NotificationRecord* operand)
{
    Float affinityBound = operand->GetContactAffinity();
    using Elastos::Core::Math;
    operand->SetContactAffinity(Math::Max(mContactAffinity, affinityBound));
    if (INFO) Slogger::I(TAG, "final affinity: %f", operand->GetContactAffinity());
    return NOERROR;
}

Float ValidateNotificationPeople::PeopleRankingReconsideration::GetContactAffinity()
{
    return mContactAffinity;
}

//===============================================================================
//                  ValidateNotificationPeople::MyContentObserver
//===============================================================================

ECode ValidateNotificationPeople::MyContentObserver::constructor(
    /* [in] */ IHandler* handle,
    /* [in] */ ValidateNotificationPeople* host)
{
    mHost = host;
    return ContentObserver::constructor(handle);
}

ValidateNotificationPeople::MyContentObserver::~MyContentObserver()
{}

ECode ValidateNotificationPeople::MyContentObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 userId)
{
    ContentObserver::OnChange(selfChange, uri, userId);
    if (DEBUG || mHost->mEvictionCount % 100 == 0) {
        if (INFO) Slogger::I(TAG, "mEvictionCount: %d", mHost->mEvictionCount);
    }
    mHost->mPeopleCache->EvictAll();
    mHost->mEvictionCount++;
    return NOERROR;
}

//===============================================================================
//                  ValidateNotificationPeople::MyRunnable
//===============================================================================

ValidateNotificationPeople::MyRunnable::MyRunnable(
    /* [in] */ PeopleRankingReconsideration* p,
    /* [in] */ ISemaphore* s)
    : mP(p)
    , mS(s)
{}

ValidateNotificationPeople::MyRunnable::~MyRunnable()
{}

ECode ValidateNotificationPeople::MyRunnable::Run()
{
    mP->Work();
    mS->ReleasePermit();
    return NOERROR;
}

//===============================================================================
//                  ValidateNotificationPeople
//===============================================================================

CAR_INTERFACE_IMPL_2(ValidateNotificationPeople, Object, IValidateNotificationPeople, INotificationSignalExtractor);

ValidateNotificationPeople::ValidateNotificationPeople()
    : mEnabled(FALSE)
    , mEvictionCount(0)
{
}

ValidateNotificationPeople::~ValidateNotificationPeople()
{}

ECode ValidateNotificationPeople::Initialize(
    /* [in] */ IContext* context)
{
    if (DEBUG) Slogger::D(TAG, "Initializing  ValidateNotificationPeople.");
    CLruCache::New(PEOPLE_CACHE_SIZE, (ILruCache**)&mPeopleCache);
    CArrayMap::New((IMap**)&mUserToContextMap);
    mBaseContext = context;

    AutoPtr<IContentResolver> resolver;
    mBaseContext->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    Int32 value;
    global->GetInt32(resolver, SETTING_ENABLE_PEOPLE_VALIDATOR, 1, &value);

    mEnabled = ENABLE_PEOPLE_VALIDATOR && 1 == value;
    if (mEnabled) {
        CHandler::New((IHandler**)&mHandler);
        mObserver = new MyContentObserver();
        mObserver->constructor(mHandler, this);
        AutoPtr<IContactsContractContacts> con;
        CContactsContractContacts::AcquireSingleton((IContactsContractContacts**)&con);
        AutoPtr<IUri> uri;
        con->GetCONTENT_URI((IUri**)&uri);
        resolver->RegisterContentObserver(uri, TRUE, mObserver, IUserHandle::USER_ALL);
    }

    return NOERROR;
}

ECode ValidateNotificationPeople::Process(
    /* [in] */ INotificationRecord* record,
    /* [out] */ IRankingReconsideration** consideration)
{
    VALIDATE_NOT_NULL(consideration);
    *consideration = NULL;

    if (!mEnabled) {
        if (INFO) Slogger::I(TAG, "disabled");
        return NOERROR;
    }

    AutoPtr<NotificationRecord> r = (NotificationRecord*)record;

    if (r == NULL || r->GetNotification() == NULL) {
        if (INFO) Slogger::I(TAG, "skipping empty notification");
        return NOERROR;
    }

    if (r->GetUserId() == IUserHandle::USER_ALL) {
        if (INFO) Slogger::I(TAG, "skipping global notification");
        return NOERROR;
    }

    AutoPtr<IContext> context;
    GetContextAsUser(r->GetUser(), (IContext**)&context);
    if (context == NULL) {
        if (INFO) Slogger::I(TAG, "skipping notification that lacks a context");
        return NOERROR;
    }
    AutoPtr<RankingReconsideration> con = ValidatePeople(context, r);
    *consideration = (IRankingReconsideration*)con.Get();
    REFCOUNT_ADD(*consideration);
    return NOERROR;
}

ECode ValidateNotificationPeople::SetConfig(
    /* [in] */ IRankingConfig* config)
{
    // ignore: config has no relevant information yet.
    return NOERROR;
}

ECode ValidateNotificationPeople::GetContactAffinity(
    /* [in] */ IUserHandle* userHandle,
    /* [in] */ IBundle* extras,
    /* [in] */ Int32 timeoutMs,
    /* [in] */ Float timeoutAffinity,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);

    if (DEBUG) Slogger::D(TAG, "checking affinity for %p", userHandle);
    if (extras == NULL) {
        *result = NONE;
        return NOERROR;
    }

    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 value;
    sys->GetNanoTime(&value);

    String key = StringUtils::ToString(value);

    AutoPtr< ArrayOf<Float> > affinityOut = ArrayOf<Float>::Alloc(1);
    AutoPtr<IContext> context;
    GetContextAsUser(userHandle, (IContext**)&context);
    if (context == NULL) {
        *result = NONE;
        return NOERROR;
    }

    AutoPtr< ArrayOf<Float> > outAffinity;
    AutoPtr<PeopleRankingReconsideration> prr;
    ValidatePeople(context, key, extras, affinityOut,
            (ArrayOf<Float>**)&outAffinity, (PeopleRankingReconsideration**)&prr);

    Float affinity = (*outAffinity)[0];

    if (prr != NULL) {
        // Perform the heavy work on a background thread so we can abort when we hit the
        // timeout.
        AutoPtr<ISemaphore> s;
        CSemaphore::New(0, (ISemaphore**)&s);

        AutoPtr<MyRunnable> runnable = new MyRunnable(prr, s);
        AsyncTask::THREAD_POOL_EXECUTOR->Execute(runnable);

        // try {
        AutoPtr<ITimeUnitHelper> helper;
        CTimeUnitHelper::AcquireSingleton((ITimeUnitHelper**)&helper);
        AutoPtr<ITimeUnit> unit;
        helper->GetMILLISECONDS((ITimeUnit**)&unit);
        Boolean res;
        ECode ec = s->TryAcquire(timeoutMs, unit, &res);
        if (SUCCEEDED(ec)) {
            if (!res) {
                Slogger::W(TAG, "Timeout while waiting for affinity: %s. Returning timeoutAffinity=%f",
                        key.string(), timeoutAffinity);
                *result = timeoutAffinity;
                return NOERROR;
            }
        }
        else {
            Slogger::W(TAG, "InterruptedException while waiting for affinity: %s. Returning affinity=%f",
                    key.string(), affinity);
            *result = affinity;
            return NOERROR;
        }
        // } catch (InterruptedException e) {
            // Slog.w(TAG, "InterruptedException while waiting for affinity: " + key + ". "
            //         + "Returning affinity=" + affinity, e);
            // return affinity;
        // }

        affinity = Elastos::Core::Math::Max(prr->GetContactAffinity(), affinity);
    }

    *result = affinity;
    return NOERROR;
}

ECode ValidateNotificationPeople::GetContextAsUser(
    /* [in] */ IUserHandle* userHandle,
    /* [out] */ IContext** _context)
{
    VALIDATE_NOT_NULL(_context);

    Int32 id;
    userHandle->GetIdentifier(&id);
    AutoPtr<IInterface> obj;
    mUserToContextMap->Get(CoreUtils::Convert(id), (IInterface**)&obj);
    AutoPtr<IContext> context = IContext::Probe(obj);

    if (context == NULL) {
        // try {
        ECode ec = mBaseContext->CreatePackageContextAsUser(String("android"), 0, userHandle, (IContext**)&context);
        if (SUCCEEDED(ec)) {
            mUserToContextMap->Put(CoreUtils::Convert(id), context);
        }
        else {
            Logger::E(TAG, "failed to create package context for lookups 0x%08x", ec);
        }
        // } catch (PackageManager.NameNotFoundException e) {
        //    Log.e(TAG, "failed to create package context for lookups", e);
        // }
    }

    *_context = context;
    REFCOUNT_ADD(*_context);
    return NOERROR;
}

AutoPtr<RankingReconsideration> ValidateNotificationPeople::ValidatePeople(
    /* [in] */ IContext* context,
    /* [in] */ NotificationRecord* record)
{
    const String key = record->GetKey();
    AutoPtr<IBundle> extras;
    record->GetNotification()->GetExtras((IBundle**)&extras);
    AutoPtr< ArrayOf<Float> > affinityOut = ArrayOf<Float>::Alloc(1);
    AutoPtr< ArrayOf<Float> > outAffinity;
    AutoPtr<PeopleRankingReconsideration> sideration;
    ValidatePeople(context, key, extras, affinityOut,
            (ArrayOf<Float>**)&outAffinity, (PeopleRankingReconsideration**)&sideration);
    AutoPtr<RankingReconsideration> rr = (RankingReconsideration*)sideration.Get();
    record->SetContactAffinity((*outAffinity)[0]);
    return rr;
}

ECode ValidateNotificationPeople::ValidatePeople(
    /* [in] */ IContext* context,
    /* [in] */ const String& key,
    /* [in] */ IBundle* extras,
    /* [in] */ ArrayOf<Float>* affinityOut,
    /* [out, callee] */ ArrayOf<Float>** outAffinity,
    /* [out] */ PeopleRankingReconsideration** ation)
{
    VALIDATE_NOT_NULL(outAffinity);
    VALIDATE_NOT_NULL(ation);
    *outAffinity = NULL;
    *ation = NULL;

    AutoPtr< ArrayOf<Float> > cloneAffinityOut;
    if (affinityOut != NULL) {
        cloneAffinityOut = affinityOut->Clone();
    }
    else {
        cloneAffinityOut = ArrayOf<Float>::Alloc(1);
    }
    *outAffinity = cloneAffinityOut;
    REFCOUNT_ADD(*outAffinity)

    Float affinity = NONE;
    if (extras == NULL) {
        return NOERROR;
    }

    AutoPtr< ArrayOf<String> > people = GetExtraPeople(extras);
    if (people == NULL || people->GetLength() == 0) {
        return NOERROR;
    }

    if (INFO) Slogger::I(TAG, "Validating: %s", key.string());
    AutoPtr<ILinkedList> pendingLookups;
    CLinkedList::New((ILinkedList**)&pendingLookups);
    for (Int32 personIdx = 0; personIdx < people->GetLength() && personIdx < MAX_PEOPLE; personIdx++) {
        String handle = (*people)[personIdx];
        if (TextUtils::IsEmpty(handle)) continue;

        {
            AutoLock lock(mPeopleCache);
            Int32 userId;
            context->GetUserId(&userId);
            const String cacheKey = GetCacheKey(userId, handle);
            AutoPtr<ICharSequence> key = CoreUtils::Convert(cacheKey);
            AutoPtr<IInterface> value;
            mPeopleCache->Get(key, (IInterface**)&value);
            AutoPtr<LookupResult> lookupResult = (LookupResult*)IObject::Probe(value);
            if (lookupResult == NULL || lookupResult->IsExpired()) {
                pendingLookups->Add(CoreUtils::Convert(handle));
            }
            else {
                if (DEBUG) Slogger::D(TAG, "using cached lookupResult");
            }
            if (lookupResult != NULL) {
                affinity = Elastos::Core::Math::Max(affinity, lookupResult->GetAffinity());
            }
        }
    }

    // record the best available data, so far:
    (*cloneAffinityOut)[0] = affinity;

    Boolean res;
    if (pendingLookups->IsEmpty(&res), res) {
        if (INFO) Slogger::I(TAG, "final affinity: %f", affinity);
        return NOERROR;
    }

    if (DEBUG) Slogger::D(TAG, "Pending: future work scheduled for: %s", key.string());

    AutoPtr<PeopleRankingReconsideration> sideration = new PeopleRankingReconsideration(context, key, pendingLookups, this);
    *ation = sideration;
    REFCOUNT_ADD(*ation);
    return NOERROR;
}

String ValidateNotificationPeople::GetCacheKey(
    /* [in] */ Int32 userId,
    /* [in] */ const String& handle)
{
    StringBuilder builder;
    builder += userId;
    builder += ":";
    builder += handle;

    return builder.ToString();
}

AutoPtr< ArrayOf<String> > ValidateNotificationPeople::GetExtraPeople(
    /* [in] */ IBundle* extras)
{
    AutoPtr< ArrayOf<String> > people;
    extras->GetStringArray(INotification::EXTRA_PEOPLE, (ArrayOf<String>**)&people);
    return people;
}

AutoPtr<ValidateNotificationPeople::LookupResult> ValidateNotificationPeople::ResolvePhoneContact(
    /* [in] */ IContext* context,
    /* [in] */ const String& number)
{
    AutoPtr<IContactsContractPhoneLookup> lkHelper;
    CContactsContractPhoneLookup::AcquireSingleton((IContactsContractPhoneLookup**)&lkHelper);
    AutoPtr<IUri> lkUri;
    lkHelper->GetCONTENT_FILTER_URI((IUri**)&lkUri);

    AutoPtr<IUriHelper> uriHelper;
    CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
    String str;
    uriHelper->Encode(number, &str);
    AutoPtr<IUri> phoneUri;
    uriHelper->WithAppendedPath(lkUri, str, (IUri**)&phoneUri);

    return SearchContacts(context, phoneUri);
}

AutoPtr<ValidateNotificationPeople::LookupResult> ValidateNotificationPeople::ResolveEmailContact(
    /* [in] */ IContext* context,
    /* [in] */ const String& email)
{
    AutoPtr<IContactsContractCommonDataKindsEmail> emailHelper;
    CContactsContractCommonDataKindsEmail::AcquireSingleton((IContactsContractCommonDataKindsEmail**)&emailHelper);
    AutoPtr<IUri> emailUri;
    emailHelper->GetCONTENT_LOOKUP_URI((IUri**)&emailUri);

    AutoPtr<IUriHelper> uriHelper;
    CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
    String str;
    uriHelper->Encode(email, &str);
    AutoPtr<IUri> numberUri;
    uriHelper->WithAppendedPath(emailUri, str, (IUri**)&numberUri);

    return SearchContacts(context, numberUri);
}

AutoPtr<ValidateNotificationPeople::LookupResult> ValidateNotificationPeople::SearchContacts(
    /* [in] */ IContext* context,
    /* [in] */ IUri* lookupUri)
{
    AutoPtr<LookupResult> lookupResult = new LookupResult();
    AutoPtr<ICursor> c;
    // try {
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    ECode ec = resolver->Query(lookupUri, LOOKUP_PROJECTION, String(NULL), NULL, String(NULL), (ICursor**)&c);
    if (SUCCEEDED(ec)) {
        if (c == NULL) {
            Slogger::W(TAG, "Null cursor from contacts query.");
            return lookupResult;
        }
        Boolean res;
        while (c->MoveToNext(&res), res) {
            lookupResult->MergeContact(c);
        }
    }
    else {
        Slogger::W(TAG, "Problem getting content resolver or performing contacts query.");
    }
    // } catch (Throwable t) {
    // Slog.w(TAG, "Problem getting content resolver or performing contacts query.", t);
    // } finally {
    if (c != NULL) {
        ICloseable::Probe(c)->Close();
    }
    // }
    return lookupResult;
}

} // Notification
} // Server
} // Droid
} // Elastos
