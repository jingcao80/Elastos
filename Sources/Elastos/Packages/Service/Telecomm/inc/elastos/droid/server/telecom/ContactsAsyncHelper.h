
#ifndef __ELASTOS_DROID_SERVER_TELECOM_CONTACTSASYNCHELPER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_CONTACTSASYNCHELPER_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include <elastos/core/Object.h>
#include <elastos/core/Thread.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/os/Handler.h>

using Elastos::Core::Thread;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Helper class for loading contacts photo asynchronously.
 */
class ContactsAsyncHelper
    : public Object
{
private:
    class SubHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("ContactsAsyncHelper::SubHandler")

        SubHandler(
            /* [in] */ ILooper* looper);

        /** Called when loading is done. */
        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    };

    class WorkerArgs
        : public Object
    {
    public:
        AutoPtr<IContext> mContext;

        AutoPtr<IUri> mDisplayPhotoUri;

        AutoPtr<IDrawable> mPhoto;

        AutoPtr<IBitmap> mPhotoIcon;

        AutoPtr<IObject> mCookie;

        AutoPtr<IContactsAsyncHelperOnImageLoadCompleteListener> mListener;
    };

    /**
     * Thread worker class that handles the task of opening the stream and loading
     * the images.
     */
    class WorkerHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("ContactsAsyncHelper::WorkerHandler")

        CARAPI constructor(
            /* [in] */ ILooper* looper);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        /**
         * Returns a Bitmap object suitable for {@link Notification}'s large icon. This might
         * return null when the given Drawable isn't BitmapDrawable, or if the system fails to
         * create a scaled Bitmap for the Drawable.
         */
        CARAPI GetPhotoIconWhenAppropriate(
            /* [in] */ IContext* context,
            /* [in] */ IDrawable* photo,
            /* [out] */ IBitmap** result);
    };

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
    static CARAPI StartObtainPhotoAsync(
        /* [in] */ Int32 token,
        /* [in] */ IContext* context,
        /* [in] */ IUri* displayPhotoUri,
        /* [in] */ IContactsAsyncHelperOnImageLoadCompleteListener* listener,
        /* [in] */ IObject* cookie);

private:
    ContactsAsyncHelper();

    static CARAPI_(AutoPtr<IHandler>) InitThreadHandler();

private:
    static const String LOG__TAG;
    // constants
    static const Int32 EVENT_LOAD_IMAGE;

    static AutoPtr<IHandler> sResultHandler;

    /** Handler run on a worker thread to load photo asynchronously. */
    static const AutoPtr<IHandler> sThreadHandler;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_CONTACTSASYNCHELPER_H__
