
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_CONTACTPHOTOMANAGER_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_CONTACTPHOTOMANAGER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Widget.h"
#include "_Elastos.Droid.Dialer.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IComponentCallbacks2;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Widget::IImageView;
using Elastos::Core::Object;

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
        /* [in] */ Int32 requestedExtent,
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

} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_CONTACTPHOTOMANAGER_H__
