
#ifndef __ELASTOS_DROID_INCALLUI_CONTACTSASYNCHELPER_H__
#define __ELASTOS_DROID_INCALLUI_CONTACTSASYNCHELPER_H__

#include "_Elastos.Droid.Dialer.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Telecom.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace InCallUI {

/**
 * Helper class for loading contacts photo asynchronously.
 */
class ContactsAsyncHelper
{
public:
    /**
     * Starts an asynchronous image load. After finishing the load,
     * {@link OnImageLoadCompleteListener#onImageLoadComplete(int, Drawable, Bitmap, Object)}
     * will be called.
     *
     * @param token Arbitrary integer which will be returned as the first argument of
     * {@link OnImageLoadCompleteListener#onImageLoadComplete(int, Drawable, Bitmap, Object)}
     * @param context Context object used to do the time-consuming operation.
     * @param displayPhotoUri Uri to be used to fetch the photo
     * @param listener Callback object which will be used when the asynchronous load is done.
     * Can be null, which means only the asynchronous load is done while there's no way to
     * obtain the loaded photos.
     * @param cookie Arbitrary object the caller wants to remember, which will become the
     * fourth argument of {@link OnImageLoadCompleteListener#onImageLoadComplete(int, Drawable,
     * Bitmap, Object)}. Can be null, at which the callback will also has null for the argument.
     */
    static CARAPI_(void) StartObtainPhotoAsync(
        /* [in] */ Int32 token,
        /* [in] */ IContext* context,
        /* [in] */ IUri* displayPhotoUri,
        /* [in] */ IOnImageLoadCompleteListener* listener,
        /* [in] */ IInterface* cookie);
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_CONTACTSASYNCHELPER_H__
