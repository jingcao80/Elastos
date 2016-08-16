
#include "Elastos.Droid.View.h"
#include "elastos/droid/contacts/common/ContactPhotoManager.h"
#include "elastos/droid/contacts/common/lettertiles/LetterTileDrawable.h"
#include "elastos/droid/text/TextUtils.h"
#include "R.h"

using Elastos::Droid::Contacts::Common::EIID_IContactPhotoManagerDefaultImageRequest;
using Elastos::Droid::Contacts::Common::EIID_IContactPhotoManagerDefaultImageProvider;
using Elastos::Droid::Contacts::Common::EIID_IContactPhotoManager;
using Elastos::Droid::Contacts::Common::Lettertiles::LetterTileDrawable;
using Elastos::Droid::Content::EIID_IComponentCallbacks2;
using Elastos::Droid::Graphics::Drawable::CColorDrawable;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::View::IView;
using Elastos::Droid::Text::TextUtils;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {

//=================================================================
// ContactPhotoManager::DefaultImageRequest
//=================================================================
AutoPtr<ContactPhotoManager::DefaultImageRequest> ContactPhotoManager::DefaultImageRequest::InitDefaultImageRequest()
{
    AutoPtr<ContactPhotoManager::DefaultImageRequest> request = new ContactPhotoManager::DefaultImageRequest();
    return request;
}
AutoPtr<ContactPhotoManager::DefaultImageRequest> ContactPhotoManager::DefaultImageRequest::EMPTY_DEFAULT_IMAGE_REQUEST
        = ContactPhotoManager::DefaultImageRequest::InitDefaultImageRequest();

AutoPtr<ContactPhotoManager::DefaultImageRequest> ContactPhotoManager::DefaultImageRequest::InitDefaultBusinessImageRequest()
{
    AutoPtr<ContactPhotoManager::DefaultImageRequest> request
            = new ContactPhotoManager::DefaultImageRequest(String(NULL), String(NULL), ContactPhotoManager::TYPE_BUSINESS, FALSE);
    return request;
}
AutoPtr<ContactPhotoManager::DefaultImageRequest> ContactPhotoManager::DefaultImageRequest::EMPTY_DEFAULT_BUSINESS_IMAGE_REQUEST
        = ContactPhotoManager::DefaultImageRequest::InitDefaultBusinessImageRequest();

AutoPtr<ContactPhotoManager::DefaultImageRequest> ContactPhotoManager::DefaultImageRequest::InitCircularDefaultImageRequest()
{
    AutoPtr<ContactPhotoManager::DefaultImageRequest> request
            = new ContactPhotoManager::DefaultImageRequest(String(NULL), String(NULL), TRUE);
    return request;
}
AutoPtr<ContactPhotoManager::DefaultImageRequest> ContactPhotoManager::DefaultImageRequest::EMPTY_CIRCULAR_DEFAULT_IMAGE_REQUEST
        = ContactPhotoManager::DefaultImageRequest::InitCircularBusinessImageRequest();

AutoPtr<ContactPhotoManager::DefaultImageRequest> ContactPhotoManager::DefaultImageRequest::InitCircularBusinessImageRequest()
{
    AutoPtr<ContactPhotoManager::DefaultImageRequest> request
            = new ContactPhotoManager::DefaultImageRequest(String(NULL), String(NULL), ContactPhotoManager::TYPE_BUSINESS, TRUE);
    return request;
}
AutoPtr<ContactPhotoManager::DefaultImageRequest> ContactPhotoManager::DefaultImageRequest::EMPTY_CIRCULAR_BUSINESS_IMAGE_REQUEST
        = ContactPhotoManager::DefaultImageRequest::InitCircularBusinessImageRequest();

ContactPhotoManager::DefaultImageRequest::DefaultImageRequest()
    : mContactType(TYPE_DEFAULT)
    , mScale(SCALE_DEFAULT)
    , mOffset(OFFSET_DEFAULT)
    , mIsCircular(FALSE)
{}

ContactPhotoManager::DefaultImageRequest::DefaultImageRequest(
    /* [in] */ const String& displayName,
    /* [in] */ const String& identifier,
    /* [in] */ Boolean isCircular)
{
    Init(displayName, identifier, TYPE_DEFAULT, SCALE_DEFAULT, OFFSET_DEFAULT, isCircular);
}

ContactPhotoManager::DefaultImageRequest::DefaultImageRequest(
    /* [in] */ const String& displayName,
    /* [in] */ const String& identifier,
    /* [in] */ Int32 contactType,
    /* [in] */ Boolean isCircular)
{
    Init(displayName, identifier, contactType, SCALE_DEFAULT, OFFSET_DEFAULT, isCircular);
}

ContactPhotoManager::DefaultImageRequest::DefaultImageRequest(
    /* [in] */ const String& displayName,
    /* [in] */ const String& identifier,
    /* [in] */ Int32 contactType,
    /* [in] */ Float scale,
    /* [in] */ Float offset,
    /* [in] */ Boolean isCircular)
{
    Init(displayName, identifier, contactType, scale, offset, isCircular);
}

CAR_INTERFACE_IMPL(ContactPhotoManager::DefaultImageRequest, Object, IContactPhotoManagerDefaultImageRequest)

void ContactPhotoManager::DefaultImageRequest::Init(
    /* [in] */ const String& displayName,
    /* [in] */ const String& identifier,
    /* [in] */ Int32 contactType,
    /* [in] */ Float scale,
    /* [in] */ Float offset,
    /* [in] */ Boolean isCircular)
{
    mDisplayName = displayName;
    mIdentifier = identifier;
    mContactType = contactType;
    mScale = scale;
    mOffset = offset;
    mIsCircular = isCircular;
}


//=================================================================
// ContactPhotoManager::LetterTileDefaultImageProvider
//=================================================================
CAR_INTERFACE_IMPL(ContactPhotoManager::LetterTileDefaultImageProvider, Object, IContactPhotoManagerDefaultImageProvider)

ECode ContactPhotoManager::LetterTileDefaultImageProvider::ApplyDefaultImage(
    /* [in] */ IImageView* view,
    /* [in] */ Int32 extent,
    /* [in] */ Boolean darkTheme,
    /* [in] */ IContactPhotoManagerDefaultImageRequest* defaultImageRequest)
{
    AutoPtr<IResources> res;
    IView::Probe(view)->GetResources((IResources**)&res);
    AutoPtr<IDrawable> drawable = GetDefaultImageForContact(res, (DefaultImageRequest*)defaultImageRequest);
    view->SetImageDrawable(drawable);
    return NOERROR;
}

AutoPtr<IDrawable> ContactPhotoManager::LetterTileDefaultImageProvider::GetDefaultImageForContact(
    /* [in] */ IResources* resources,
    /* [in] */ DefaultImageRequest* defaultImageRequest)
{
//    AutoPtr<LetterTileDrawable> drawable = new LetterTileDrawable(resources);
//    if (defaultImageRequest != NULL) {
//        // If the contact identifier is null or empty, fallback to the
//        // displayName. In that case, use {@code null} for the contact's
//        // display name so that a default bitmap will be used instead of a
//        // letter
//        if (TextUtils::IsEmpty(defaultImageRequest->mIdentifier)) {
//            drawable->SetContactDetails(String(NULL), defaultImageRequest->mDisplayName);
//        }
//        else {
//            drawable->SetContactDetails(defaultImageRequest->mDisplayName,
//                    defaultImageRequest->mIdentifier);
//        }
//        drawable->SetContactType(defaultImageRequest->mContactType);
//        drawable->SetScale(defaultImageRequest->mScale);
//        drawable->SetOffset(defaultImageRequest->mOffset);
//        drawable->SetIsCircular(defaultImageRequest->mIsCircular);
//    }
//    return drawable;
    return NULL;
}


//=================================================================
// ContactPhotoManager::BlankDefaultImageProvider
//=================================================================
AutoPtr<IDrawable> ContactPhotoManager::BlankDefaultImageProvider::sDrawable;

CAR_INTERFACE_IMPL(ContactPhotoManager::BlankDefaultImageProvider, Object, IContactPhotoManagerDefaultImageProvider)

ECode ContactPhotoManager::BlankDefaultImageProvider::ApplyDefaultImage(
    /* [in] */ IImageView* view,
    /* [in] */ Int32 extent,
    /* [in] */ Boolean darkTheme,
    /* [in] */ IContactPhotoManagerDefaultImageRequest* defaultImageRequest)
{
    if (sDrawable == NULL) {
        AutoPtr<IContext> context;
        IView::Probe(view)->GetContext((IContext**)&context);
        AutoPtr<IResources> res;
        context->GetResources((IResources**)&res);
        Int32 color;
        res->GetColor(Elastos::Droid::Dialer::R::color::image_placeholder, &color);
        CColorDrawable::New(color, (IDrawable**)&sDrawable);
    }
    view->SetImageDrawable(sDrawable);
    return NOERROR;
}


//=================================================================
// ContactPhotoManager
//=================================================================
const Boolean ContactPhotoManager::IS_CIRCULAR_DEFAULT;
const String ContactPhotoManager::CONTACT_PHOTO_SERVICE("contactPhotos");

AutoPtr<IContactPhotoManagerDefaultImageProvider> ContactPhotoManager::InitDefaultAvatar()
{
    AutoPtr<IContactPhotoManagerDefaultImageProvider> provider
            = (IContactPhotoManagerDefaultImageProvider*)new ContactPhotoManager::LetterTileDefaultImageProvider();
    return provider;
}
const AutoPtr<IContactPhotoManagerDefaultImageProvider> ContactPhotoManager::DEFAULT_AVATAR
        = ContactPhotoManager::InitDefaultAvatar();

AutoPtr<IContactPhotoManagerDefaultImageProvider> ContactPhotoManager::InitDefaultBlank()
{
    AutoPtr<IContactPhotoManagerDefaultImageProvider> provider
            = (IContactPhotoManagerDefaultImageProvider*)new ContactPhotoManager::BlankDefaultImageProvider();
    return provider;
}
const AutoPtr<IContactPhotoManagerDefaultImageProvider> ContactPhotoManager::DEFAULT_BLANK
        = ContactPhotoManager::InitDefaultBlank();
const String ContactPhotoManager::TAG("ContactPhotoManager");
const Boolean ContactPhotoManager::DEBUG; // Don't submit with true
const Boolean ContactPhotoManager::DEBUG_SIZES; // Don't submit with true
const Int32 ContactPhotoManager::TYPE_PERSON = 0; //LetterTileDrawable::TYPE_PERSON;
const Int32 ContactPhotoManager::TYPE_BUSINESS = 0; //LetterTileDrawable::TYPE_BUSINESS;
const Int32 ContactPhotoManager::TYPE_VOICEMAIL = 0; //LetterTileDrawable::TYPE_VOICEMAIL;
const Int32 ContactPhotoManager::TYPE_DEFAULT = 0; //LetterTileDrawable::TYPE_DEFAULT;
const Float ContactPhotoManager::SCALE_DEFAULT;
const Float ContactPhotoManager::OFFSET_DEFAULT;
const String ContactPhotoManager::DISPLAY_NAME_PARAM_KEY("display_name");
const String ContactPhotoManager::IDENTIFIER_PARAM_KEY("identifier");
const String ContactPhotoManager::CONTACT_TYPE_PARAM_KEY("contact_type");
const String ContactPhotoManager::SCALE_PARAM_KEY("scale");
const String ContactPhotoManager::OFFSET_PARAM_KEY("offset");
const String ContactPhotoManager::IS_CIRCULAR_PARAM_KEY("is_circular");
const String ContactPhotoManager::DEFAULT_IMAGE_URI_SCHEME("defaultimage");

AutoPtr<IUri> ContactPhotoManager::InitDefaultImageUri()
{
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    AutoPtr<IUri> uri;
    helper->Parse(ContactPhotoManager::DEFAULT_IMAGE_URI_SCHEME + "://", (IUri**)&uri);
    return uri;
}
const AutoPtr<IUri> ContactPhotoManager::DEFAULT_IMAGE_URI = ContactPhotoManager::InitDefaultImageUri();
AutoPtr<IDrawable> ContactPhotoManager::sDefaultLetterAvatar;

CAR_INTERFACE_IMPL_2(ContactPhotoManager, Object, IContactPhotoManager, IComponentCallbacks2)

AutoPtr<IDrawable> ContactPhotoManager::GetDefaultAvatarDrawableForContact(
    /* [in] */ IResources* resources,
    /* [in] */ Boolean hires,
    /* [in] */ DefaultImageRequest* defaultImageRequest)
{
    assert(0);
    return NULL;
}

AutoPtr<IUri> ContactPhotoManager::GetDefaultAvatarUriForContact(
    /* [in] */ DefaultImageRequest* request)
{
    assert(0);
    return NULL;
}

String ContactPhotoManager::AppendBusinessContactType(
    /* [in] */ const String& photoUrl)
{
    assert(0);
    return String(NULL);
}

AutoPtr<IUri> ContactPhotoManager::RemoveContactType(
    /* [in] */ IUri* photoUri)
{
    assert(0);
    return NULL;
}

Boolean ContactPhotoManager::IsBusinessContactUri(
    /* [in] */ IUri* photoUri)
{
    assert(0);
    return FALSE;
}

AutoPtr<IContactPhotoManager> ContactPhotoManager::GetInstance(
    /* [in] */ IContext* context)
{
    assert(0);
    return NULL;
}

AutoPtr<IContactPhotoManager> ContactPhotoManager::CreateContactPhotoManager(
    /* [in] */ IContext* context)
{
    assert(0);
    return NULL;
}

ECode ContactPhotoManager::LoadThumbnail(
    /* [in] */ IImageView* view,
    /* [in] */ Int64 photoId,
    /* [in] */ Boolean darkTheme,
    /* [in] */ Boolean isCircular,
    /* [in] */ IContactPhotoManagerDefaultImageRequest* defaultImageRequest)
{
    assert(0);
    return NOERROR;
}

ECode ContactPhotoManager::LoadPhoto(
    /* [in] */ IImageView* view,
    /* [in] */ IUri* photoUri,
    /* [in] */ Int32 requestedExtent,
    /* [in] */ Boolean darkTheme,
    /* [in] */ Boolean isCircular,
    /* [in] */ IContactPhotoManagerDefaultImageRequest* defaultImageRequest)
{
    assert(0);
    return NOERROR;
}

ECode ContactPhotoManager::LoadDirectoryPhoto(
    /* [in] */ IImageView* view,
    /* [in] */ IUri* photoUri,
    /* [in] */ Int32 requestedExtent,
    /* [in] */ Boolean darkTheme,
    /* [in] */ Boolean isCircular,
    /* [in] */ IContactPhotoManagerDefaultImageRequest* defaultImageRequest)
{
    assert(0);
    return NOERROR;
}

AutoPtr<ContactPhotoManager::DefaultImageRequest> ContactPhotoManager::GetDefaultImageRequestFromUri(
    /* [in] */ IUri* uri)
{
    assert(0);
    return NOERROR;
}

Boolean ContactPhotoManager::IsDefaultImageUri(
    /* [in] */ IUri* uri)
{
    assert(0);
    return FALSE;
}

} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos
