
#ifndef __ELASTOS_DROID_INCALLUI_CONTACTINFOCACHE_H__
#define __ELASTOS_DROID_INCALLUI_CONTACTINFOCACHE_H__

#include "elastos/droid/incallui/CallerInfo.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/Set.h>

using Elastos::Droid::InCallUI::Service::IPhoneNumberInfo;
using Elastos::Droid::InCallUI::Service::IImageLookupListener;
using Elastos::Droid::InCallUI::Service::INumberLookupListener;
using Elastos::Droid::InCallUI::Service::IPhoneNumberService;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::Set;

namespace Elastos {
namespace Droid {
namespace InCallUI {

class Call;

/**
 * Class responsible for querying Contact Information for Call objects. Can perform asynchronous
 * requests to the Contact Provider for information as well as respond synchronously for any data
 * that it currently has cached from previous queries. This class always gets called from the UI
 * thread so it does not need thread protection.
 */
class ContactInfoCache
    : public Object
{
public:
    class PhoneNumberServiceListener
        : public Object
        , public INumberLookupListener
        , public IImageLookupListener
    {
    public:
        PhoneNumberServiceListener(
            /* [in] */ const String& callId,
            /* [in] */ ContactInfoCache* host);

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnPhoneNumberInfoComplete(
            /* [in] */ IPhoneNumberInfo* info);

        // @Override
        CARAPI OnImageFetchComplete(
            /* [in] */ IBitmap* bitmap);

    private:
        String mCallId;
        ContactInfoCache* mHost;
    };

    class ContactCacheEntry
        : public Object
        , public IContactCacheEntry
    {
    public:
        ContactCacheEntry()
            : mIsSipCall(FALSE)
        {}

        CAR_INTERFACE_DECL();

        CARAPI ToString(
            /* [out] */ String* str);

    public:
        String mName;
        String mNumber;
        String mLocation;
        String mLabel;
        AutoPtr<IDrawable> mPhoto;
        Boolean mIsSipCall;
        /** This will be used for the "view" notification. */
        AutoPtr<IUri> mContactUri;
        /** Either a display photo or a thumbnail URI. */
        AutoPtr<IUri> mDisplayPhotoUri;
        AutoPtr<IUri> mLookupUri; // Sent to NotificationMananger
        String mLookupKey;
    };

private:
    class FindInfoCallback
        : public Object
        , public IOnQueryCompleteListener
    {
    public:
        FindInfoCallback(
            /* [in] */ Boolean isIncoming,
            /* [in] */ ContactInfoCache* host)
            : mIsIncoming(isIncoming)
            , mHost(host)
        {}

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnQueryComplete(
            /* [in] */ Int32 token,
            /* [in] */ IInterface* cookie,
            /* [in] */ ICallerInfo* ci);

    private:
        Boolean mIsIncoming;
        ContactInfoCache* mHost;
    };

    class ImageLoadCompleteListener
        : public Object
        , public IOnImageLoadCompleteListener
    {
    public:
        CAR_INTERFACE_DECL()

        ImageLoadCompleteListener(
            /* [in] */ ContactInfoCache* host);

        CARAPI OnImageLoadComplete(
            /* [in] */ Int32 token,
            /* [in] */ IDrawable* photo,
            /* [in] */ IBitmap* photoIcon,
            /* [in] */ IInterface* cookie);

    private:
        ContactInfoCache* mHost;
    };
public:
    static CARAPI_(AutoPtr<ContactInfoCache>) GetInstance(
        /* [in] */ IContext* mContext);

    CARAPI_(AutoPtr<ContactCacheEntry>) GetInfo(
        /* [in] */ const String& callId);

    static CARAPI_(AutoPtr<ContactCacheEntry>) BuildCacheEntryFromCall(
        /* [in] */ IContext* context,
        /* [in] */ Call* call,
        /* [in] */ Boolean isIncoming);

    /**
     * Requests contact data for the Call object passed in.
     * Returns the data through callback.  If callback is null, no response is made, however the
     * query is still performed and cached.
     *
     * @param callback The function to call back when the call is found. Can be null.
     */
    CARAPI_(void) FindInfo(
        /* [in] */ Call* call,
        /* [in] */ Boolean isIncoming,
        /* [in] */ IContactInfoCacheCallback* callback);

    /**
     * Implemented for ContactsAsyncHelper.OnImageLoadCompleteListener interface.
     * make sure that the call state is reflected after the image is loaded.
     */
    // @Override
    virtual CARAPI OnImageLoadComplete(
        /* [in] */ Int32 token,
        /* [in] */ IDrawable* photo,
        /* [in] */ IBitmap* photoIcon,
        /* [in] */ IInterface* cookie);

    /**
     * Blows away the stored cache values.
     */
    CARAPI_(void) ClearCache();

    /**
     * Populate a cache entry from a call (which got converted into a caller info).
     */
    static CARAPI_(void) PopulateCacheEntry(
        /* [in] */ IContext* context,
        /* [in] */ CallerInfo* info,
        /* [in] */ ContactCacheEntry* cce,
        /* [in] */ Int32 presentation,
        /* [in] */ Boolean isIncoming);

private:
    ContactInfoCache(
        /* [in] */ IContext* context);

    CARAPI_(void) FindInfoQueryComplete(
        /* [in] */ Call* call,
        /* [in] */ CallerInfo* callerInfo,
        /* [in] */ Boolean isIncoming,
        /* [in] */ Boolean didLocalLookup);

    CARAPI_(AutoPtr<ContactCacheEntry>) BuildEntry(
        /* [in] */ IContext* context,
        /* [in] */ const String& callId,
        /* [in] */ CallerInfo* info,
        /* [in] */ Int32 presentation,
        /* [in] */ Boolean isIncoming);

    /**
     * Sends the updated information to call the callbacks for the entry.
     */
    CARAPI_(void) SendInfoNotifications(
        /* [in] */ const String& callId,
        /* [in] */ ContactCacheEntry* entry);

    CARAPI_(void) SendImageNotifications(
        /* [in] */ const String& callId,
        /* [in] */ ContactCacheEntry* entry);

    CARAPI_(void) ClearCallbacks(
        /* [in] */ const String& callId);

    /**
     * Gets name strings based on some special presentation modes.
     */
    static CARAPI_(String) GetPresentationString(
        /* [in] */ IContext* context,
        /* [in] */ Int32 presentation);

private:
    static const String TAG;
    static const Int32 TOKEN_UPDATE_PHOTO_FOR_CALL_STATE = 0;

    AutoPtr<IContext> mContext;
    AutoPtr<IPhoneNumberService> mPhoneNumberService;
    HashMap<String, AutoPtr<ContactCacheEntry> > mInfoMap;
    HashMap<String, AutoPtr<Set<AutoPtr<IContactInfoCacheCallback> > > > mCallBacks;

    static AutoPtr<ContactInfoCache> sCache;

    friend class FindInfoCallback;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_CONTACTINFOCACHE_H__
