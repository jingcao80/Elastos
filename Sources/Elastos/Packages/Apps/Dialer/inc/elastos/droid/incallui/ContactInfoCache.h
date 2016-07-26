
#ifndef __ELASTOS_DROID_INCALLUI_CONTACTINFOCACHE_H__
#define __ELASTOS_DROID_INCALLUI_CONTACTINFOCACHE_H__

#include "_Elastos.Droid.Dialer.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Net.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Net::IUri;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace InCallUI {

class Call;

class ContactInfoCache
    : public Object
{
public:
    class ContactCacheEntry
        : public Object
        , public IContactCacheEntry
    {
    public:
        CAR_INTERFACE_DECL();

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

public:
    static AutoPtr<ContactInfoCache> GetInstance(
        /* [in] */ IContext* mContext);

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
     * Blows away the stored cache values.
     */
    CARAPI_(void) ClearCache();
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_CONTACTINFOCACHE_H__
