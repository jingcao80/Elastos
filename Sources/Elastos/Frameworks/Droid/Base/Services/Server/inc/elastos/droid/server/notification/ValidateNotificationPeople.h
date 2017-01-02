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

#ifndef __ELASTOS_DROID_SERVER_NOTIFICATION_VALIDATENOTIFICATIONPEOPLE_H__
#define __ELASTOS_DROID_SERVER_NOTIFICATION_VALIDATENOTIFICATIONPEOPLE_H__

#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Utility.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "elastos/droid/server/notification/RankingReconsideration.h"
#include "elastos/droid/database/ContentObserver.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Utility::ILruCache;
using Elastos::Utility::Concurrent::ISemaphore;
using Elastos::Utility::ILinkedList;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

/**
 * This {@link NotificationSignalExtractor} attempts to validate
 * people references. Also elevates the priority of real people.
 *
 * {@hide}
 */
class ValidateNotificationPeople
    : public Object
    , public IValidateNotificationPeople
    , public INotificationSignalExtractor
{
    friend class NotificationManagerService;
    friend class ZenModeHelper;
private:
    class LookupResult
        : public Object
    {
        friend class ValidateNotificationPeople;
    public:
        LookupResult();

        ~LookupResult();

        CARAPI_(void) MergeContact(
            /* [in] */ ICursor* cursor);

        CARAPI_(Float) GetAffinity();

    private:
        CARAPI_(Boolean) IsExpired();

        CARAPI_(Boolean) IsInvalid();

    private:
        static const Int64 CONTACT_REFRESH_MILLIS;  // 1hr

        Int64 mExpireMillis;
        Float mAffinity;
    };

    class PeopleRankingReconsideration
        : public RankingReconsideration
    {
    public:
        PeopleRankingReconsideration(
            /* [in] */ IContext* context,
            /* [in] */ const String& key,
            /* [in] */ ILinkedList* pendingLookups,
            /* [in] */ ValidateNotificationPeople* host);

        ~PeopleRankingReconsideration();

        // @Override
        CARAPI Work();

        // @Override
        CARAPI ApplyChangesLocked(
            /* [in] */ NotificationRecord* operand);

        CARAPI_(Float) GetContactAffinity();

    private:
        AutoPtr<ILinkedList> mPendingLookups;
        AutoPtr<IContext> mContext;
        Float mContactAffinity;
        ValidateNotificationPeople* mHost;
    };

    class MyContentObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("ValidateNotificationPeople::MyContentObserver")

        CARAPI constructor(
            /* [in] */ IHandler* handle,
            /* [in] */ ValidateNotificationPeople* host);

        ~MyContentObserver();

        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri,
            /* [in] */ Int32 userId);

    private:
        ValidateNotificationPeople* mHost;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ PeopleRankingReconsideration* p,
            /* [in] */ ISemaphore* s);

        ~MyRunnable();

        // @Override
        CARAPI Run();

    private:
        AutoPtr<PeopleRankingReconsideration> mP;
        AutoPtr<ISemaphore> mS;
    };

public:
    CAR_INTERFACE_DECL()

    ValidateNotificationPeople();

    ~ValidateNotificationPeople();

    // @Override
    CARAPI Initialize(
        /* [in] */ IContext* context);

    // @Override
    CARAPI Process(
        /* [in] */ INotificationRecord* record,
        /* [out] */ IRankingReconsideration** consideration);

    // @Override
    CARAPI SetConfig(
        /* [in] */ IRankingConfig* config);

    /**
     * @param extras extras of the notification with EXTRA_PEOPLE populated
     * @param timeoutMs timeout in milliseconds to wait for contacts response
     * @param timeoutAffinity affinity to return when the timeout specified via
     *                        <code>timeoutMs</code> is hit
     */
    CARAPI GetContactAffinity(
        /* [in] */ IUserHandle* userHandle,
        /* [in] */ IBundle* extras,
        /* [in] */ Int32 timeoutMs,
        /* [in] */ Float timeoutAffinity,
        /* [out] */ Float* result);

    // VisibleForTesting
    static CARAPI_(AutoPtr< ArrayOf<String> >) GetExtraPeople(
        /* [in] */ IBundle* extras);

private:
    CARAPI GetContextAsUser(
        /* [in] */ IUserHandle* userHandle,
        /* [out] */ IContext** context);

    CARAPI_(AutoPtr<RankingReconsideration>) ValidatePeople(
        /* [in] */ IContext* context,
        /* [in] */ NotificationRecord* record);

    CARAPI ValidatePeople(
        /* [in] */ IContext* context,
        /* [in] */ const String& key,
        /* [in] */ IBundle* extras,
        /* [in] */ ArrayOf<Float>* affinityOut,
        /* [out, callee] */ ArrayOf<Float>** outAffinity,
        /* [out] */ PeopleRankingReconsideration** ation);

    CARAPI_(String) GetCacheKey(
        /* [in] */ Int32 userId,
        /* [in] */ const String& handle);

    CARAPI_(AutoPtr<ValidateNotificationPeople::LookupResult>) ResolvePhoneContact(
        /* [in] */ IContext* context,
        /* [in] */ const String& number);

    CARAPI_(AutoPtr<ValidateNotificationPeople::LookupResult>) ResolveEmailContact(
        /* [in] */ IContext* context,
        /* [in] */ const String& email);

    CARAPI_(AutoPtr<ValidateNotificationPeople::LookupResult>) SearchContacts(
        /* [in] */ IContext* context,
        /* [in] */ IUri* lookupUri);

protected:
    /** Indicates that the notification does not reference any valid contacts. */
    static const Float NONE;

    /**
     * Affinity will be equal to or greater than this value on notifications
     * that reference a valid contact.
     */
    static const Float VALID_CONTACT;

    /**
     * Affinity will be equal to or greater than this value on notifications
     * that reference a starred contact.
     */
    static const Float STARRED_CONTACT;

    Boolean mEnabled;

private:
    // Using a shorter log tag since setprop has a limit of 32chars on variable name.
    static const String TAG;
    static const Boolean INFO;
    static const Boolean DEBUG; //Log.isLoggable(TAG, Log.DEBUG);

    static const Boolean ENABLE_PEOPLE_VALIDATOR;
    static const String SETTING_ENABLE_PEOPLE_VALIDATOR;
    static const AutoPtr< ArrayOf<String> > LOOKUP_PROJECTION;// = { Contacts._ID, Contacts.STARRED };
    static const Int32 MAX_PEOPLE;
    static const Int32 PEOPLE_CACHE_SIZE;

    AutoPtr<IContext> mBaseContext;

    // maps raw person handle to resolved person object
    AutoPtr<ILruCache> mPeopleCache; //LruCache<String, AutoPtr<LookupResult> >
    /* Map<Integer, Context> mUserToContextMap;*/
    AutoPtr<IMap> mUserToContextMap;
    AutoPtr<IHandler> mHandler;
    AutoPtr<MyContentObserver> mObserver;
    Int32 mEvictionCount;
};

} // Notification
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_NOTIFICATION_VALIDATENOTIFICATIONPEOPLE_H__
