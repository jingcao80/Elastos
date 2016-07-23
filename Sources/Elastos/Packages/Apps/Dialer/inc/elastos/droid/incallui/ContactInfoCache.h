
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
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_CONTACTINFOCACHE_H__
