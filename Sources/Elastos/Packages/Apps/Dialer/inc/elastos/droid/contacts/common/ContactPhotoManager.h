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

#ifndef __ELASTOS_DROID_CONTACTS_COMMON_CONTACTPHOTOMANAGER_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_CONTACTPHOTOMANAGER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.Utility.h"
#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/os/HandlerThread.h"
#include "elastos/droid/utility/LruCache.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/Object.h>
#include <elastos/utility/etl/Set.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Content::IComponentCallbacks2;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IHandlerCallback;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::HandlerThread;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Utility::ILruCache;
using Elastos::Droid::Utility::LruCache;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IImageView;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Concurrent::IConcurrentHashMap;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;
using Elastos::Utility::Etl::Set;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {

class ContactPhotoManager
    : public Object
    , public IContactPhotoManager
    , public IComponentCallbacks2
{
public:
    /**
     * Contains fields used to contain contact details and other user-defined settings that might
     * be used by the ContactPhotoManager to generate a default contact image. This contact image
     * takes the form of a letter or bitmap drawn on top of a colored tile.
     */
    class DefaultImageRequest
        : public Object
        , public IContactPhotoManagerDefaultImageRequest
    {
    public:
        DefaultImageRequest();

        DefaultImageRequest(
            /* [in] */ const String& displayName,
            /* [in] */ const String& identifier,
            /* [in] */ Boolean isCircular);

        DefaultImageRequest(
            /* [in] */ const String& displayName,
            /* [in] */ const String& identifier,
            /* [in] */ Int32 contactType,
            /* [in] */ Boolean isCircular);

        DefaultImageRequest(
            /* [in] */ const String& displayName,
            /* [in] */ const String& identifier,
            /* [in] */ Int32 contactType,
            /* [in] */ Float scale,
            /* [in] */ Float offset,
            /* [in] */ Boolean isCircular);

        CAR_INTERFACE_DECL()

    private:
        CARAPI_(void) Init(
            /* [in] */ const String& displayName,
            /* [in] */ const String& identifier,
            /* [in] */ Int32 contactType,
            /* [in] */ Float scale,
            /* [in] */ Float offset,
            /* [in] */ Boolean isCircular);

        static CARAPI_(AutoPtr<ContactPhotoManager::DefaultImageRequest>) InitDefaultImageRequest();

        static CARAPI_(AutoPtr<ContactPhotoManager::DefaultImageRequest>) InitDefaultBusinessImageRequest();

        static CARAPI_(AutoPtr<ContactPhotoManager::DefaultImageRequest>) InitCircularDefaultImageRequest();

        static CARAPI_(AutoPtr<ContactPhotoManager::DefaultImageRequest>) InitCircularBusinessImageRequest();

    public:
        /**
         * The contact's display name. The display name is used to
         */
        String mDisplayName;

        /**
         * A unique and deterministic string that can be used to identify this contact. This is
         * usually the contact's lookup key, but other contact details can be used as well,
         * especially for non-local or temporary contacts that might not have a lookup key. This
         * is used to determine the color of the tile.
         */
        String mIdentifier;

        /**
         * The type of this contact. This contact type may be used to decide the kind of
         * image to use in the case where a unique letter cannot be generated from the contact's
         * display name and identifier. See:
         * {@link #TYPE_PERSON}
         * {@link #TYPE_BUSINESS}
         * {@link #TYPE_PERSON}
         * {@link #TYPE_DEFAULT}
         */
        Int32 mContactType;

        /**
         * The amount to scale the letter or bitmap to, as a ratio of its default size (from a
         * range of 0.0f to 2.0f). The default value is 1.0f.
         */
        Float mScale;

        /**
         * The amount to vertically offset the letter or image to within the tile.
         * The provided offset must be within the range of -0.5f to 0.5f.
         * If set to -0.5f, the letter will be shifted upwards by 0.5 times the height of the canvas
         * it is being drawn on, which means it will be drawn with the center of the letter starting
         * at the top edge of the canvas.
         * If set to 0.5f, the letter will be shifted downwards by 0.5 times the height of the
         * canvas it is being drawn on, which means it will be drawn with the center of the letter
         * starting at the bottom edge of the canvas.
         * The default is 0.0f, which means the letter is drawn in the exact vertical center of
         * the tile.
         */
        Float mOffset;

        /**
         * Whether or not to draw the default image as a circle, instead of as a square/rectangle.
         */
        Boolean mIsCircular;

        /**
         * Used to indicate that a drawable that represents a contact without any contact details
         * should be returned.
         */
        static AutoPtr<DefaultImageRequest> EMPTY_DEFAULT_IMAGE_REQUEST;

        /**
         * Used to indicate that a drawable that represents a business without a business photo
         * should be returned.
         */
        static AutoPtr<DefaultImageRequest> EMPTY_DEFAULT_BUSINESS_IMAGE_REQUEST;

        /**
         * Used to indicate that a circular drawable that represents a contact without any contact
         * details should be returned.
         */
        static AutoPtr<DefaultImageRequest> EMPTY_CIRCULAR_DEFAULT_IMAGE_REQUEST;

        /**
         * Used to indicate that a circular drawable that represents a business without a business
         * photo should be returned.
         */
        static AutoPtr<DefaultImageRequest> EMPTY_CIRCULAR_BUSINESS_IMAGE_REQUEST;
    };

private:
    /**
     * A default image provider that applies a letter tile consisting of a colored background
     * and a letter in the foreground as the default image for a contact. The color of the
     * background and the type of letter is decided based on the contact's details.
     */
    class LetterTileDefaultImageProvider
        : public Object
        , public IContactPhotoManagerDefaultImageProvider
    {
    public:
        CAR_INTERFACE_DECL()

        // @Override
        CARAPI ApplyDefaultImage(
            /* [in] */ IImageView* view,
            /* [in] */ Int32 extent,
            /* [in] */ Boolean darkTheme,
            /* [in] */ IContactPhotoManagerDefaultImageRequest* defaultImageRequest);

        static CARAPI_(AutoPtr<IDrawable>) GetDefaultImageForContact(
            /* [in] */ IResources* resources,
            /* [in] */ DefaultImageRequest* defaultImageRequest);
    };

    class BlankDefaultImageProvider
        : public Object
        , public IContactPhotoManagerDefaultImageProvider
    {
    public:
        CAR_INTERFACE_DECL()

        // @Override
        CARAPI ApplyDefaultImage(
            /* [in] */ IImageView* view,
            /* [in] */ Int32 extent,
            /* [in] */ Boolean darkTheme,
            /* [in] */ IContactPhotoManagerDefaultImageRequest* defaultImageRequest);

    private:
        static AutoPtr<IDrawable> sDrawable;
    };

public:
    virtual ~ContactPhotoManager() {}

    CAR_INTERFACE_DECL()

    /**
     * Given a {@link DefaultImageRequest}, returns a {@link Drawable}, that when drawn, will
     * draw a letter tile avatar based on the request parameters defined in the
     * {@link DefaultImageRequest}.
     */
    static CARAPI_(AutoPtr<IDrawable>) GetDefaultAvatarDrawableForContact(
        /* [in] */ IResources* resources,
        /* [in] */ Boolean hires,
        /* [in] */ DefaultImageRequest* defaultImageRequest);

    /**
     * Given a {@link DefaultImageRequest}, returns an Uri that can be used to request a
     * letter tile avatar when passed to the {@link ContactPhotoManager}. The internal
     * implementation of this uri is not guaranteed to remain the same across application
     * versions, so the actual uri should never be persisted in long-term storage and reused.
     *
     * @param request A {@link DefaultImageRequest} object with the fields configured
     * to return a
     * @return A Uri that when later passed to the {@link ContactPhotoManager} via
     * {@link #loadPhoto(ImageView, Uri, int, boolean, DefaultImageRequest)}, can be
     * used to request a default contact image, drawn as a letter tile using the
     * parameters as configured in the provided {@link DefaultImageRequest}
     */
    static CARAPI_(AutoPtr<IUri>) GetDefaultAvatarUriForContact(
        /* [in] */ DefaultImageRequest* request);

    /**
     * Adds a business contact type encoded fragment to the URL.  Used to ensure photo URLS
     * from Nearby Places can be identified as business photo URLs rather than URLs for personal
     * contact photos.
     *
     * @param photoUrl The photo URL to modify.
     * @return URL with the contact type parameter added and set to TYPE_BUSINESS.
     */
    static CARAPI_(String) AppendBusinessContactType(
        /* [in] */ const String& photoUrl);

    /**
     * Removes the contact type information stored in the photo URI encoded fragment.
     *
     * @param photoUri The photo URI to remove the contact type from.
     * @return The photo URI with contact type removed.
     */
    static CARAPI_(AutoPtr<IUri>) RemoveContactType(
        /* [in] */ IUri* photoUri);

    /**
     * Inspects a photo URI to determine if the photo URI represents a business.
     *
     * @param photoUri The URI to inspect.
     * @return Whether the URI represents a business photo or not.
     */
    static CARAPI_(Boolean) IsBusinessContactUri(
        /* [in] */ IUri* photoUri);

    static CARAPI_(AutoPtr<IContactPhotoManager>) GetInstance(
        /* [in] */ IContext* context);

    static CARAPI_(AutoPtr<IContactPhotoManager>) CreateContactPhotoManager(
        /* [in] */ IContext* context);

    using IContactPhotoManager::LoadThumbnail;

    /**
     * Calls {@link #loadThumbnail(ImageView, long, boolean, DefaultImageRequest,
     * DefaultImageProvider)} using the {@link DefaultImageProvider} {@link #DEFAULT_AVATAR}.
    */
    CARAPI LoadThumbnail(
        /* [in] */ IImageView* view,
        /* [in] */ Int64 photoId,
        /* [in] */ Boolean darkTheme,
        /* [in] */ Boolean isCircular,
        /* [in] */ IContactPhotoManagerDefaultImageRequest* defaultImageRequest);

    using IContactPhotoManager::LoadPhoto;

    /**
     * Calls {@link #loadPhoto(ImageView, Uri, int, boolean, DefaultImageRequest,
     * DefaultImageProvider)} with {@link #DEFAULT_AVATAR} and {@code null} display names and
     * lookup keys.
     *
     * @param defaultImageRequest {@link DefaultImageRequest} object that specifies how a default
     * letter tile avatar should be drawn.
     */
    CARAPI LoadPhoto(
        /* [in] */ IImageView* view,
        /* [in] */ IUri* photoUri,
        /* [in] */ Int32 requestedExtent,
        /* [in] */ Boolean darkTheme,
        /* [in] */ Boolean isCircular,
        /* [in] */ IContactPhotoManagerDefaultImageRequest* defaultImageRequest);

    /**
     * Calls {@link #loadPhoto(ImageView, Uri, boolean, boolean, DefaultImageRequest,
     * DefaultImageProvider)} with {@link #DEFAULT_AVATAR} and with the assumption, that
     * the image is a thumbnail.
     *
     * @param defaultImageRequest {@link DefaultImageRequest} object that specifies how a default
     * letter tile avatar should be drawn.
     */
    CARAPI LoadDirectoryPhoto(
        /* [in] */ IImageView* view,
        /* [in] */ IUri* photoUri,
        /* [in] */ Boolean darkTheme,
        /* [in] */ Boolean isCircular,
        /* [in] */ IContactPhotoManagerDefaultImageRequest* defaultImageRequest);

protected:
    static CARAPI_(AutoPtr<DefaultImageRequest>) GetDefaultImageRequestFromUri(
        /* [in] */ IUri* uri);

    CARAPI_(Boolean) IsDefaultImageUri(
        /* [in] */ IUri* uri);

private:
    static CARAPI_(AutoPtr<IContactPhotoManagerDefaultImageProvider>) InitDefaultAvatar();

    static CARAPI_(AutoPtr<IContactPhotoManagerDefaultImageProvider>) InitDefaultBlank();

    static CARAPI_(AutoPtr<IUri>) InitDefaultImageUri();

public:
    /** Contact type constants used for default letter images */
    static const Int32 TYPE_PERSON;
    static const Int32 TYPE_BUSINESS;
    static const Int32 TYPE_VOICEMAIL;
    static const Int32 TYPE_DEFAULT;

    /** Scale and offset default constants used for default letter images */
    static const Float SCALE_DEFAULT = 1.0f;
    static const Float OFFSET_DEFAULT = 0.0f;

    static const Boolean IS_CIRCULAR_DEFAULT = FALSE;
    static const String CONTACT_PHOTO_SERVICE;

    static const AutoPtr<IContactPhotoManagerDefaultImageProvider> DEFAULT_AVATAR;

    static const AutoPtr<IContactPhotoManagerDefaultImageProvider> DEFAULT_BLANK;

protected:
    static const String TAG;
    static const Boolean DEBUG = FALSE; // Don't submit with true
    static const Boolean DEBUG_SIZES = FALSE; // Don't submit with true

private:
    /** Uri-related constants used for default letter images */
    static const String DISPLAY_NAME_PARAM_KEY;
    static const String IDENTIFIER_PARAM_KEY;
    static const String CONTACT_TYPE_PARAM_KEY;
    static const String SCALE_PARAM_KEY;
    static const String OFFSET_PARAM_KEY;
    static const String IS_CIRCULAR_PARAM_KEY;
    static const String DEFAULT_IMAGE_URI_SCHEME;
    static const AutoPtr<IUri> DEFAULT_IMAGE_URI;

    // Static field used to cache the default letter avatar drawable that is created
    // using a null {@link DefaultImageRequest}
    static AutoPtr<IDrawable> sDefaultLetterAvatar;
    static Object sLock;

    friend class DefaultImageRequest;
};

class ContactPhotoManagerImpl
    : public ContactPhotoManager
{
private:
    class InnerHandlerCallback
        : public Object
        , public IHandlerCallback
    {
    public:
        InnerHandlerCallback(
            /* [in] */ ContactPhotoManagerImpl* host)
            : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

    private:
        ContactPhotoManagerImpl* mHost;
    };

    class BitmapHolderCache;

    /**
     * Maintains the state of a particular photo.
     */
    class BitmapHolder : public Object
    {
    public:
        BitmapHolder(
            /* [in] */ ArrayOf<Byte>* bytes,
            /* [in] */ Int32 originalSmallerExtent)
            : mBytes(bytes)
            , mOriginalSmallerExtent(originalSmallerExtent)
            , mFresh(TRUE)
            , mDecodedSampleSize(0)
        {}

    private:
        AutoPtr<ArrayOf<Byte> > mBytes;
        Int32 mOriginalSmallerExtent;

        Boolean mFresh;
        AutoPtr<IBitmap> mBitmap;
        AutoPtr<IBitmap> mBitmapRef;
        Int32 mDecodedSampleSize;

        friend class BitmapHolderCache;
        friend class ContactPhotoManagerImpl;
    };

    class Request;

    /**
     * The thread that performs loading of photos from the database.
     */
    class LoaderThread
        : public HandlerThread
    {
    private:
        class InnerCallback
            : public Object
            , public IHandlerCallback
        {
        public:
            InnerCallback(
                /* [in] */ LoaderThread* host)
                : mHost(host)
            {}

            CAR_INTERFACE_DECL()

            CARAPI HandleMessage(
                /* [in] */ IMessage* msg,
                /* [out] */ Boolean* result);

        private:
            LoaderThread* mHost;
        };

    public:
        LoaderThread(
            /* [in] */ ContactPhotoManagerImpl* host);

        CARAPI constructor(
            /* [in] */ IContentResolver* resolver);

        CARAPI_(void) EnsureHandler();

        /**
         * Kicks off preloading of the next batch of photos on the background thread.
         * Preloading will happen after a delay: we want to yield to the UI thread
         * as much as possible.
         * <p>
         * If preloading is already complete, does nothing.
         */
        CARAPI_(void) RequestPreloading();

        /**
         * Receives the above message, loads photos and then sends a message
         * to the main thread to process them.
         */
        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        /**
         * Sends a message to this thread to load requested photos.  Cancels a preloading
         * request, if any: we don't want preloading to impede loading of the photos
         * we need to display now.
         */
        CARAPI_(void) RequestLoading();

    private:
        /**
         * The first time it is called, figures out which photos need to be preloaded.
         * Each subsequent call preloads the next batch of photos and requests
         * another cycle of preloading after a delay.  The whole process ends when
         * we either run out of photos to preload or fill up cache.
         */
        CARAPI_(void) PreloadPhotosInBackground();

        CARAPI_(void) QueryPhotosForPreload();

        CARAPI_(void) LoadPhotosInBackground();

        /** Loads thumbnail photos with ids */
        CARAPI_(void) LoadThumbnails(
            /* [in] */ Boolean preloading);

        /**
         * Loads photos referenced with Uris. Those can be remote thumbnails
         * (from directory searches), display photos etc
         */
        CARAPI_(void) LoadUriBasedPhotos();

    private:
        static const Int32 BUFFER_SIZE = 1024*16;
        static const Int32 MESSAGE_PRELOAD_PHOTOS = 0;
        static const Int32 MESSAGE_LOAD_PHOTOS = 1;

        /**
         * A pause between preload batches that yields to the UI thread.
         */
        static const Int32 PHOTO_PRELOAD_DELAY = 1000;

        /**
         * Number of photos to preload per batch.
         */
        static const Int32 PRELOAD_BATCH = 25;

        /**
         * Maximum number of photos to preload.  If the cache size is 2Mb and
         * the expected average size of a photo is 4kb, then this number should be 2Mb/4kb = 500.
         */
        static const Int32 MAX_PHOTOS_TO_PRELOAD = 100;

        static const Int32 PRELOAD_STATUS_NOT_STARTED = 0;
        static const Int32 PRELOAD_STATUS_IN_PROGRESS = 1;
        static const Int32 PRELOAD_STATUS_DONE = 2;

        AutoPtr<IContentResolver> mResolver;
        StringBuilder mStringBuilder;
        Set<Int64> mPhotoIds;
        Set<String> mPhotoIdsAsStrings;
        Set<AutoPtr<Request> > mPhotoUris;
        Elastos::Utility::Etl::List<Int64> mPreloadPhotoIds;

        AutoPtr<IHandler> mLoaderThreadHandler;
        AutoPtr<ArrayOf<Byte> > mBuffer;

        Int32 mPreloadStatus;
        ContactPhotoManagerImpl* mHost;
    };

    /**
     * A holder for either a Uri or an id and a flag whether this was requested for the dark or
     * light theme
     */
    class Request : public Object
    {
    public:
        static CARAPI_(AutoPtr<Request>) CreateFromThumbnailId(
            /* [in] */ Int64 id,
            /* [in] */ Boolean darkTheme,
            /* [in] */ Boolean isCircular,
            /* [in] */ IContactPhotoManagerDefaultImageProvider* defaultProvider);

        static CARAPI_(AutoPtr<Request>) CreateFromUri(
            /* [in] */ IUri* uri,
            /* [in] */ Int32 requestedExtent,
            /* [in] */ Boolean darkTheme,
            /* [in] */ Boolean isCircular,
            /* [in] */ IContactPhotoManagerDefaultImageProvider* defaultProvider);

        CARAPI_(Boolean) IsUriRequest();

        CARAPI_(AutoPtr<IUri>) GetUri();

        CARAPI_(Int64) GetId();

        CARAPI_(Int32) GetRequestedExtent();

        // @Override
        CARAPI GetHashCode(
            /* [out] */ Int32* hashCode);

        // @Override
        CARAPI Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result);

        CARAPI_(AutoPtr<IInterface>) GetKey();

        /**
         * Applies the default image to the current view. If the request is URI-based, looks for
         * the contact type encoded fragment to determine if this is a request for a business photo,
         * in which case we will load the default business photo.
         *
         * @param view The current image view to apply the image to.
         * @param isCircular Whether the image is circular or not.
         */
        CARAPI_(void) ApplyDefaultImage(
            /* [in] */ IImageView* view,
            /* [in] */ Boolean isCircular);

    private:
        Request(
            /* [in] */ Int64 id,
            /* [in] */ IUri* uri,
            /* [in] */ Int32 requestedExtent,
            /* [in] */ Boolean darkTheme,
            /* [in] */ Boolean isCircular,
            /* [in] */ IContactPhotoManagerDefaultImageProvider* defaultProvider);

    private:
        Int64 mId;
        AutoPtr<IUri> mUri;
        Boolean mDarkTheme;
        Int32 mRequestedExtent;
        AutoPtr<IContactPhotoManagerDefaultImageProvider> mDefaultProvider;
        /**
         * Whether or not the contact photo is to be displayed as a circle
         */
        Boolean mIsCircular;

        friend class ContactPhotoManagerImpl;
    };

    class BitmapCache
        : public LruCache
    {
    public:
        BitmapCache(
            /* [in] */ ContactPhotoManagerImpl* host)
            : mHost(host)
        {}

    protected:
        // @Override
        CARAPI_(Int32) SizeOf(
            /* [in] */ IInterface* key,
            /* [in] */ IInterface* value);

        // @Override
        CARAPI EntryRemoved(
            /* [in] */ Boolean evicted,
            /* [in] */ IInterface* key,
            /* [in] */ IInterface* oldValue,
            /* [in] */ IInterface* newValue);

    private:
        ContactPhotoManagerImpl* mHost;
    };

    class BitmapHolderCache
        : public LruCache
    {
    public:
        BitmapHolderCache(
            /* [in] */ ContactPhotoManagerImpl* host)
            : mHost(host)
        {}

    protected:
        // @Override
        CARAPI_(Int32) SizeOf(
            /* [in] */ IInterface* key,
            /* [in] */ IInterface* value);

        // @Override
        CARAPI EntryRemoved(
            /* [in] */ Boolean evicted,
            /* [in] */ IInterface* key,
            /* [in] */ IInterface* oldValue,
            /* [in] */ IInterface* newValue);

    private:
        ContactPhotoManagerImpl* mHost;
    };

public:
    ContactPhotoManagerImpl(
        /* [in] */ IContext* context);

    // @Override
    CARAPI OnTrimMemory(
        /* [in] */ Int32 level);

    // @Override
    CARAPI PreloadPhotosInBackground();

    using ContactPhotoManager::LoadThumbnail;

    // @Override
    CARAPI LoadThumbnail(
        /* [in] */ IImageView* view,
        /* [in] */ Int64 photoId,
        /* [in] */ Boolean darkTheme,
        /* [in] */ Boolean isCircular,
        /* [in] */ IContactPhotoManagerDefaultImageRequest* defaultImageRequest,
        /* [in] */ IContactPhotoManagerDefaultImageProvider* defaultProvider);

    using ContactPhotoManager::LoadPhoto;

    // @Override
    CARAPI LoadPhoto(
        /* [in] */ IImageView* view,
        /* [in] */ IUri* photoUri,
        /* [in] */ Int32 requestedExtent,
        /* [in] */ Boolean darkTheme,
        /* [in] */ Boolean isCircular,
        /* [in] */ IContactPhotoManagerDefaultImageRequest* defaultImageRequest,
        /* [in] */ IContactPhotoManagerDefaultImageProvider* defaultProvider);

    // @Override
    CARAPI RemovePhoto(
        /* [in] */ IImageView* view);

    /**
     * Cancels pending requests to load photos asynchronously for views inside
     * {@param fragmentRootView}. If {@param fragmentRootView} is null, cancels all requests.
     */
    // @Override
    CARAPI CancelPendingRequests(
        /* [in] */ IView* fragmentRootView);

    // @Override
    CARAPI RefreshCache();

    CARAPI_(void) Clear();

    // @Override
    CARAPI Pause();

    // @Override
    CARAPI Resume();

    /**
     * Processes requests on the main thread.
     */
    // @Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg,
        /* [out] */ Boolean* result);

    CARAPI_(void) EnsureLoaderThread();

    // @Override
    CARAPI CacheBitmap(
        /* [in] */ IUri* photoUri,
        /* [in] */ IBitmap* bitmap,
        /* [in] */ ArrayOf<Byte>* photoBytes);

private:
    static CARAPI_(String) Btk(
        /* [in] */ Int32 bytes);

    static CARAPI_(Int32) SafeDiv(
        /* [in] */ Int32 dividend,
        /* [in] */ Int32 divisor);

    /**
     * Dump cache stats on logcat.
     */
    CARAPI_(void) DumpStats();

    CARAPI_(void) CreateAndApplyDefaultImageForUri(
        /* [in] */ IImageView* view,
        /* [in] */ IUri* uri,
        /* [in] */ Int32 requestedExtent,
        /* [in] */ Boolean darkTheme,
        /* [in] */ Boolean isCircular,
        /* [in] */ IContactPhotoManagerDefaultImageProvider* defaultProvider);

    CARAPI_(void) LoadPhotoByIdOrUri(
        /* [in] */ IImageView* view,
        /* [in] */ Request* request);

    static CARAPI_(Boolean) IsChildView(
        /* [in] */ IView* parent,
        /* [in] */ IView* potentialChild);

    /**
     * Checks if the photo is present in cache.  If so, sets the photo on the view.
     *
     * @return false if the photo needs to be (re)loaded from the provider.
     */
    CARAPI_(Boolean) LoadCachedPhoto(
        /* [in] */ IImageView* view,
        /* [in] */ Request* request,
        /* [in] */ Boolean fadeIn);

    /**
     * Given a bitmap, returns a drawable that is configured to display the bitmap based on the
     * specified request.
     */
    CARAPI_(AutoPtr<IDrawable>) GetDrawableForBitmap(
        /* [in] */ IResources* resources,
        /* [in] */ IBitmap* bitmap,
        /* [in] */ Request* request);

    /**
     * If necessary, decodes bytes stored in the holder to Bitmap.  As long as the
     * bitmap is held either by {@link #mBitmapCache} or by a soft reference in
     * the holder, it will not be necessary to decode the bitmap.
     */
    static CARAPI_(void) InflateBitmap(
        /* [in] */ BitmapHolder* holder,
        /* [in] */ Int32 requestedExtent);

    /**
     * Sends a message to this thread itself to start loading images.  If the current
     * view contains multiple image views, all of those image views will get a chance
     * to request their respective photos before any of those requests are executed.
     * This allows us to load images in bulk.
     */
    CARAPI_(void) RequestLoading();

    /**
     * Goes over pending loading requests and displays loaded photos.  If some of the
     * photos still haven't been loaded, sends another request for image loading.
     */
    CARAPI_(void) ProcessLoadedImages();

    /**
     * Removes strong references to loaded bitmaps to allow them to be garbage collected
     * if needed.  Some of the bitmaps will still be retained by {@link #mBitmapCache}.
     */
    CARAPI_(void) SoftenCache();

    /**
     * Stores the supplied bitmap in cache.
     */
    CARAPI_(void) CacheBitmap(
        /* [in] */ IInterface* key,
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ Boolean preloading,
        /* [in] */ Int32 requestedExtent);

    /**
     * Populates an array of photo IDs that need to be loaded. Also decodes bitmaps that we have
     * already loaded
     */
    CARAPI_(void) ObtainPhotoIdsAndUrisToLoad(
        /* [in] */ Set<Int64>& photoIds,
        /* [in] */ Set<String>& photoIdsAsStrings,
        /* [in] */ Set<AutoPtr<Request> >& uris);

private:
    static const String LOADER_THREAD_NAME;

    static const Int32 FADE_TRANSITION_DURATION = 200;

    /**
     * Type of message sent by the UI thread to itself to indicate that some photos
     * need to be loaded.
     */
    static const Int32 MESSAGE_REQUEST_LOADING = 1;

    /**
     * Type of message sent by the loader thread to indicate that some photos have
     * been loaded.
     */
    static const Int32 MESSAGE_PHOTOS_LOADED = 2;

    static const AutoPtr<ArrayOf<String> > EMPTY_STRING_ARRAY;

    static const AutoPtr<ArrayOf<String> > COLUMNS;

    /** Cache size for {@link #mBitmapHolderCache} for devices with "large" RAM. */
    static const Int32 HOLDER_CACHE_SIZE = 2000000;

    /** Cache size for {@link #mBitmapCache} for devices with "large" RAM. */
    static const Int32 BITMAP_CACHE_SIZE = 36864 * 48; // 1728K

    AutoPtr<IContext> mContext;

    /**
     * An LRU cache for bitmap holders. The cache contains bytes for photos just
     * as they come from the database. Each holder has a soft reference to the
     * actual bitmap.
     */
    AutoPtr<ILruCache> mBitmapHolderCache;

    /**
     * {@code true} if ALL entries in {@link #mBitmapHolderCache} are NOT fresh.
     */
    Boolean mBitmapHolderCacheAllUnfresh;

    /**
     * Cache size threshold at which bitmaps will not be preloaded.
     */
    Int32 mBitmapHolderCacheRedZoneBytes;

    /**
     * Level 2 LRU cache for bitmaps. This is a smaller cache that holds
     * the most recently used bitmaps to save time on decoding
     * them from bytes (the bytes are stored in {@link #mBitmapHolderCache}.
     */
    AutoPtr<ILruCache> mBitmapCache;

    /**
     * A map from ImageView to the corresponding photo ID or uri, encapsulated in a request.
     * The request may swapped out before the photo loading request is started.
     */
    AutoPtr<IConcurrentHashMap> mPendingRequests;

    /**
     * Handler for messages sent to the UI thread.
     */
    AutoPtr<IHandler> mMainThreadHandler;

    /**
     * Thread responsible for loading photos from the database. Created upon
     * the first request.
     */
    AutoPtr<LoaderThread> mLoaderThread;

    /**
     * A gate to make sure we only send one instance of MESSAGE_PHOTOS_NEEDED at a time.
     */
    Boolean mLoadingRequested;

    /**
     * Flag indicating if the image loading is paused.
     */
    Boolean mPaused;

    /** Height/width of a thumbnail image */
    static Int32 sThumbnailSize;

    /** For debug: How many times we had to reload cached photo for a stale entry */
    AutoPtr<IAtomicInteger32> mStaleCacheOverwrite;

    /** For debug: How many times we had to reload cached photo for a fresh entry.  Should be 0. */
    AutoPtr<IAtomicInteger32> mFreshCacheOverwrite;

    friend class LoaderThread;
};

} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_CONTACTPHOTOMANAGER_H__
