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

#ifndef  __ELASTOS_DROID_PHONE_CALLERINFOCACHE_H__
#define  __ELASTOS_DROID_PHONE_CALLERINFOCACHE_H__

#include "_Elastos.Droid.TeleService.h"
#include "elastos/droid/os/AsyncTask.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Os.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * Holds "custom ringtone" and "send to voicemail" information for each contact as a fallback of
 * contacts database. The cached information is refreshed periodically and used when database
 * lookup (via ContentResolver) takes longer time than expected.
 *
 * The data inside this class shouldn't be treated as "primary"; they may not reflect the
 * latest information stored in the original database.
 */
class CallerInfoCache
    : public Object
{
public:
    class CacheEntry
        : public Object
    {
    public:
        CacheEntry(
            /* [in] */ const String& customRingtone,
            /* [in] */ Boolean shouldSendToVoicemail)
            : mCustomRingtone(customRingtone)
            , mSendToVoicemail(shouldSendToVoicemail)
        {}

        //@Override
        CARAPI ToString(
            /* [out] */ String* str);

    public:
        const String mCustomRingtone;
        const Boolean mSendToVoicemail;
    };

private:
    class CacheAsyncTask
        : public AsyncTask
    {
    public:
        CacheAsyncTask(
            /* [in] */ CallerInfoCache* host);

        TO_STRING_IMPL("CallerInfoCache::CacheAsyncTask")

        /**
         * Call {@link PowerManager.WakeLock#acquire} and call {@link AsyncTask#execute(Object...)},
         * guaranteeing the lock is held during the asynchronous task.
         */
        CARAPI AcquireWakeLockAndExecute();

    protected:
        //@Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

        //@Override
        CARAPI OnPostExecute(
            /* [in] */ IInterface* result);

        //@Override
        CARAPI OnCancelled(
            /* [in] */ IInterface* result);

    private:
        CARAPI ReleaseWakeLock();

    private:
        AutoPtr<IPowerManagerWakeLock> mWakeLock;
        CallerInfoCache* mHost;
    };

public:
    TO_STRING_IMPL("CallerInfoCache")

    static CARAPI_(AutoPtr<CallerInfoCache>) Init(
        /* [in] */ IContext* context);

    CARAPI StartAsyncCache();

    /**
     * Returns CacheEntry for the given number (PSTN number or SIP address).
     *
     * @param number OK to be unformatted.
     * @return CacheEntry to be used. Maybe null if there's no cache here. Note that this may
     * return null when the cache itself is not ready. BE CAREFUL. (or might be better to throw
     * an exception)
     */
    CARAPI_(AutoPtr<CacheEntry>) GetCacheEntry(
        /* [in] */ const String& number);

private:
    CallerInfoCache(
        /* [in] */ IContext* context);

    CARAPI_(void) RefreshCacheEntry();

    CARAPI_(void) PutNewEntryWhenAppropriate(
        /* [in] */ IHashMap* newNumberToEntry,
        /* [in] */ const String& numberOrSipAddress,
        /* [in] */ const String& customRingtone,
        /* [in] */ Boolean sendToVoicemail);

    static CARAPI_(void) Log(
        /* [in] */ const String& msg);

public:
    static const Int32 MESSAGE_UPDATE_CACHE;

private:
    static const String TAG;
    static const Boolean DBG;

    /** This must not be set to true when submitting changes. */
    static const Boolean VDBG;

    // Assuming DATA.DATA1 corresponds to Phone.NUMBER and SipAddress.ADDRESS, we just use
    // Data columns as much as we can. One exception: because normalized numbers won't be used in
    // SIP cases, Phone.NORMALIZED_NUMBER is used as is instead of using Data.
    static const AutoPtr<ArrayOf<String> > PROJECTION;

    static const Int32 INDEX_NUMBER;
    static const Int32 INDEX_NORMALIZED_NUMBER;
    static const Int32 INDEX_CUSTOM_RINGTONE;
    static const Int32 INDEX_SEND_TO_VOICEMAIL;

    static const String SELECTION;

    AutoPtr<IContext> mContext;

    /**
     * The mapping from number to CacheEntry.
     *
     * The number will be:
     * - last 7 digits of each "normalized phone number when it is for PSTN phone call, or
     * - a full SIP address for SIP call
     *
     * When cache is being refreshed, this whole object will be replaced with a newer object,
     * instead of updating elements inside the object.  "volatile" is used to make
     * {@link #getCacheEntry(String)} access to the newer one every time when the object is
     * being replaced.
     */
    /*volatile*/ AutoPtr<IHashMap> mNumberToEntry;

    /**
     * Used to remember if the previous task is finished or not. Should be set to null when done.
     */
    AutoPtr<CacheAsyncTask> mCacheAsyncTask;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CALLERINFOCACHE_H__