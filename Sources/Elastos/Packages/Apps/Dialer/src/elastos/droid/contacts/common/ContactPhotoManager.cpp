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

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Net.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "elastos/droid/contacts/common/ContactPhotoManager.h"
#include "elastos/droid/contacts/common/lettertiles/LetterTileDrawable.h"
#include "elastos/droid/contacts/common/util/UriUtils.h"
#include "elastos/droid/contacts/common/util/BitmapUtil.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::Contacts::Common::EIID_IContactPhotoManagerDefaultImageRequest;
using Elastos::Droid::Contacts::Common::EIID_IContactPhotoManagerDefaultImageProvider;
using Elastos::Droid::Contacts::Common::EIID_IContactPhotoManager;
using Elastos::Droid::Contacts::Common::Lettertiles::LetterTileDrawable;
using Elastos::Droid::Contacts::Common::Util::UriUtils;
using Elastos::Droid::Contacts::Common::Util::BitmapUtil;
using Elastos::Droid::Content::EIID_IComponentCallbacks2;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::PaintStyle_FILL;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::BitmapConfig;
using Elastos::Droid::Graphics::Drawable::CColorDrawable;
using Elastos::Droid::Graphics::Drawable::ITransitionDrawable;
using Elastos::Droid::Graphics::Drawable::CTransitionDrawable;
using Elastos::Droid::Graphics::Drawable::ILayerDrawable;
using Elastos::Droid::Graphics::Drawable::CBitmapDrawable;
using Elastos::Droid::Media::IThumbnailUtils;
using Elastos::Droid::Media::CThumbnailUtils;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriBuilder;
using Elastos::Droid::Os::EIID_IHandlerCallback;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Provider::IContactsContract;
using Elastos::Droid::Provider::CContactsContract;
using Elastos::Droid::Provider::CContactsContractContacts;
using Elastos::Droid::Provider::IContactsContractContacts;
using Elastos::Droid::Provider::IContactsContractDirectory;
using Elastos::Droid::Provider::IContactsContractContactsColumns;
using Elastos::Droid::Provider::IContactsContractContactOptionsColumns;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::IContactsContractData;
using Elastos::Droid::Provider::CContactsContractData;
using Elastos::Droid::Provider::IContactsContractContactsPhoto;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::AutoLock;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::IInteger64;
using Elastos::Core::CInteger64;
using Elastos::IO::ICloseable;
using Elastos::IO::IInputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::IOutputStream;
using Elastos::Net::IURL;
using Elastos::Net::CURL;
using Elastos::Utility::ISet;
using Elastos::Utility::IMap;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::Concurrent::CConcurrentHashMap;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;
using Elastos::Utility::Logging::Logger;

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

CAR_INTERFACE_IMPL(ContactPhotoManager::DefaultImageRequest, Object, IContactPhotoManagerDefaultImageRequest);

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
   AutoPtr<LetterTileDrawable> drawable = new LetterTileDrawable(resources);
   if (defaultImageRequest != NULL) {
       // If the contact identifier is null or empty, fallback to the
       // displayName. In that case, use {@code null} for the contact's
       // display name so that a default bitmap will be used instead of a
       // letter
       if (TextUtils::IsEmpty(defaultImageRequest->mIdentifier)) {
           drawable->SetContactDetails(String(NULL), defaultImageRequest->mDisplayName);
       }
       else {
           drawable->SetContactDetails(defaultImageRequest->mDisplayName,
                    defaultImageRequest->mIdentifier);
       }
       drawable->SetContactType(defaultImageRequest->mContactType);
       drawable->SetScale(defaultImageRequest->mScale);
       drawable->SetOffset(defaultImageRequest->mOffset);
       drawable->SetIsCircular(defaultImageRequest->mIsCircular);
   }
   return drawable;
}


//=================================================================
// ContactPhotoManager::BlankDefaultImageProvider
//=================================================================
AutoPtr<IDrawable> ContactPhotoManager::BlankDefaultImageProvider::sDrawable;

CAR_INTERFACE_IMPL(ContactPhotoManager::BlankDefaultImageProvider, Object, IContactPhotoManagerDefaultImageProvider);

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
const Int32 ContactPhotoManager::TYPE_PERSON = LetterTileDrawable::TYPE_PERSON;
const Int32 ContactPhotoManager::TYPE_BUSINESS = LetterTileDrawable::TYPE_BUSINESS;
const Int32 ContactPhotoManager::TYPE_VOICEMAIL = LetterTileDrawable::TYPE_VOICEMAIL;
const Int32 ContactPhotoManager::TYPE_DEFAULT = LetterTileDrawable::TYPE_DEFAULT;
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
Object ContactPhotoManager::sLock;

CAR_INTERFACE_IMPL_2(ContactPhotoManager, Object, IContactPhotoManager, IComponentCallbacks2);

AutoPtr<IDrawable> ContactPhotoManager::GetDefaultAvatarDrawableForContact(
    /* [in] */ IResources* resources,
    /* [in] */ Boolean hires,
    /* [in] */ DefaultImageRequest* defaultImageRequest)
{
    if (defaultImageRequest == NULL) {
        if (sDefaultLetterAvatar == NULL) {
            // Cache and return the letter tile drawable that is created by a null request,
            // so that it doesn't have to be recreated every time it is requested again.
            sDefaultLetterAvatar = LetterTileDefaultImageProvider::GetDefaultImageForContact(
                    resources, NULL);
        }
        return sDefaultLetterAvatar;
    }
    return LetterTileDefaultImageProvider::GetDefaultImageForContact(resources,
            defaultImageRequest);
}

AutoPtr<IUri> ContactPhotoManager::GetDefaultAvatarUriForContact(
    /* [in] */ DefaultImageRequest* request)
{
    AutoPtr<IUriBuilder> builder;
    DEFAULT_IMAGE_URI->BuildUpon((IUriBuilder**)&builder);
    if (request != NULL) {
        if (!TextUtils::IsEmpty(request->mDisplayName)) {
            builder->AppendQueryParameter(DISPLAY_NAME_PARAM_KEY, request->mDisplayName);
        }
        if (!TextUtils::IsEmpty(request->mIdentifier)) {
            builder->AppendQueryParameter(IDENTIFIER_PARAM_KEY, request->mIdentifier);
        }
        if (request->mContactType != TYPE_DEFAULT) {
            builder->AppendQueryParameter(CONTACT_TYPE_PARAM_KEY, StringUtils::ToString(request->mContactType));
        }
        if (request->mScale != SCALE_DEFAULT) {
            builder->AppendQueryParameter(SCALE_PARAM_KEY, StringUtils::ToString(request->mScale));
        }
        if (request->mOffset != OFFSET_DEFAULT) {
            builder->AppendQueryParameter(OFFSET_PARAM_KEY, StringUtils::ToString(request->mOffset));
        }
        if (request->mIsCircular != IS_CIRCULAR_DEFAULT) {
            builder->AppendQueryParameter(IS_CIRCULAR_PARAM_KEY, StringUtils::ToString(request->mIsCircular));
        }

    }
    AutoPtr<IUri> uri;
    builder->Build((IUri**)&uri);
    return uri;
}

String ContactPhotoManager::AppendBusinessContactType(
    /* [in] */ const String& photoUrl)
{
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    AutoPtr<IUri> uri;
    helper->Parse(photoUrl, (IUri**)&uri);
    AutoPtr<IUriBuilder> builder;
    uri->BuildUpon((IUriBuilder**)&builder);
    builder->EncodedFragment(StringUtils::ToString(TYPE_BUSINESS));
    AutoPtr<IUri> newUri;
    builder->Build((IUri**)&newUri);
    String str;
    IObject::Probe(uri)->ToString(&str);
    return str;
}

AutoPtr<IUri> ContactPhotoManager::RemoveContactType(
    /* [in] */ IUri* photoUri)
{
    String encodedFragment;
    photoUri->GetEncodedFragment(&encodedFragment);
    if (!TextUtils::IsEmpty(encodedFragment)) {
        AutoPtr<IUriBuilder> builder;
        photoUri->BuildUpon((IUriBuilder**)&builder);
        builder->EncodedFragment(String(NULL));
        AutoPtr<IUri> newUri;
        builder->Build((IUri**)&newUri);
        return newUri;
    }
    return photoUri;
}

Boolean ContactPhotoManager::IsBusinessContactUri(
    /* [in] */ IUri* photoUri)
{
    if (photoUri == NULL) {
        return FALSE;
    }

    String encodedFragment;
    photoUri->GetEncodedFragment(&encodedFragment);
    return !TextUtils::IsEmpty(encodedFragment)
            && encodedFragment.Equals(StringUtils::ToString(TYPE_BUSINESS));
}

AutoPtr<ContactPhotoManager::DefaultImageRequest> ContactPhotoManager::GetDefaultImageRequestFromUri(
    /* [in] */ IUri* uri)
{
    String name, identifier;
    uri->GetQueryParameter(DISPLAY_NAME_PARAM_KEY, &name);
    uri->GetQueryParameter(IDENTIFIER_PARAM_KEY, &identifier);
    AutoPtr<DefaultImageRequest> request = new DefaultImageRequest(name, identifier, FALSE);
    // try {
    String contactType;
    uri->GetQueryParameter(CONTACT_TYPE_PARAM_KEY, &contactType);
    if (!TextUtils::IsEmpty(contactType)) {
        request->mContactType = StringUtils::ParseInt32(contactType);
    }

    String scale;
    uri->GetQueryParameter(SCALE_PARAM_KEY, &scale);
    if (!TextUtils::IsEmpty(scale)) {
        request->mScale = StringUtils::ParseFloat(scale);
    }

    String offset;
    uri->GetQueryParameter(OFFSET_PARAM_KEY, &offset);
    if (!TextUtils::IsEmpty(offset)) {
        request->mOffset = StringUtils::ParseFloat(offset);
    }

    String isCircular;
    uri->GetQueryParameter(IS_CIRCULAR_PARAM_KEY, &isCircular);
    if (!TextUtils::IsEmpty(isCircular)) {
        request->mIsCircular = StringUtils::ParseBoolean(isCircular);
    }
    // } catch (NumberFormatException e) {
    //     Log.w(TAG, "Invalid DefaultImageRequest image parameters provided, ignoring and using "
    //             + "defaults.");
    // }

    return request;
}

Boolean ContactPhotoManager::IsDefaultImageUri(
    /* [in] */ IUri* uri)
{
    String scheme;
    uri->GetScheme(&scheme);
    return DEFAULT_IMAGE_URI_SCHEME.Equals(scheme);
}

AutoPtr<IContactPhotoManager> ContactPhotoManager::GetInstance(
    /* [in] */ IContext* context)
{
    AutoPtr<IContext> applicationContext;
    context->GetApplicationContext((IContext**)&applicationContext);
    AutoPtr<IInterface> s;
    applicationContext->GetSystemService(CONTACT_PHOTO_SERVICE, (IInterface**)&s);
    AutoPtr<IContactPhotoManager> service = IContactPhotoManager::Probe(s);
    if (service == NULL) {
        service = CreateContactPhotoManager(applicationContext);
        Logger::E(TAG, "No contact photo service in context: %s", TO_CSTR(applicationContext));
    }
    return service;
}

AutoPtr<IContactPhotoManager> ContactPhotoManager::CreateContactPhotoManager(
    /* [in] */ IContext* context)
{
    AutoLock lock(sLock);
    AutoPtr<IContactPhotoManager> manager = (IContactPhotoManager*)new ContactPhotoManagerImpl(context);
    return manager;
}

ECode ContactPhotoManager::LoadThumbnail(
    /* [in] */ IImageView* view,
    /* [in] */ Int64 photoId,
    /* [in] */ Boolean darkTheme,
    /* [in] */ Boolean isCircular,
    /* [in] */ IContactPhotoManagerDefaultImageRequest* defaultImageRequest)
{
    return LoadThumbnail(view, photoId, darkTheme, isCircular,
            defaultImageRequest, DEFAULT_AVATAR);
}

ECode ContactPhotoManager::LoadPhoto(
    /* [in] */ IImageView* view,
    /* [in] */ IUri* photoUri,
    /* [in] */ Int32 requestedExtent,
    /* [in] */ Boolean darkTheme,
    /* [in] */ Boolean isCircular,
    /* [in] */ IContactPhotoManagerDefaultImageRequest* defaultImageRequest)
{
    return LoadPhoto(view, photoUri, requestedExtent, darkTheme, isCircular,
            defaultImageRequest, DEFAULT_AVATAR);
}

ECode ContactPhotoManager::LoadDirectoryPhoto(
    /* [in] */ IImageView* view,
    /* [in] */ IUri* photoUri,
    /* [in] */ Boolean darkTheme,
    /* [in] */ Boolean isCircular,
    /* [in] */ IContactPhotoManagerDefaultImageRequest* defaultImageRequest)
{
    return LoadPhoto(view, photoUri, -1, darkTheme, isCircular, defaultImageRequest, DEFAULT_AVATAR);
}


//=================================================================
// ContactPhotoManagerImpl::LoaderThread::InnerCallback
//=================================================================
CAR_INTERFACE_IMPL(ContactPhotoManagerImpl::LoaderThread::InnerCallback, Object, IHandlerCallback);

ECode ContactPhotoManagerImpl::LoaderThread::InnerCallback::HandleMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    return mHost->HandleMessage(msg, result);
}

//=================================================================
// ContactPhotoManagerImpl::LoaderThread
//=================================================================
const Int32 ContactPhotoManagerImpl::LoaderThread::BUFFER_SIZE;
const Int32 ContactPhotoManagerImpl::LoaderThread::MESSAGE_PRELOAD_PHOTOS;
const Int32 ContactPhotoManagerImpl::LoaderThread::MESSAGE_LOAD_PHOTOS;
const Int32 ContactPhotoManagerImpl::LoaderThread::PHOTO_PRELOAD_DELAY;
const Int32 ContactPhotoManagerImpl::LoaderThread::PRELOAD_BATCH;
const Int32 ContactPhotoManagerImpl::LoaderThread::MAX_PHOTOS_TO_PRELOAD;
const Int32 ContactPhotoManagerImpl::LoaderThread::PRELOAD_STATUS_NOT_STARTED;
const Int32 ContactPhotoManagerImpl::LoaderThread::PRELOAD_STATUS_IN_PROGRESS;
const Int32 ContactPhotoManagerImpl::LoaderThread::PRELOAD_STATUS_DONE;

ContactPhotoManagerImpl::LoaderThread::LoaderThread(
    /* [in] */ ContactPhotoManagerImpl* host)
    : mPreloadStatus(PRELOAD_STATUS_NOT_STARTED)
    , mHost(host)
{}

ECode ContactPhotoManagerImpl::LoaderThread::constructor(
    /* [in] */ IContentResolver* resolver)
{
    HandlerThread::constructor(LOADER_THREAD_NAME);
    mResolver = resolver;
    return NOERROR;
}

void ContactPhotoManagerImpl::LoaderThread::EnsureHandler()
{
    if (mLoaderThreadHandler == NULL) {
        AutoPtr<ILooper> looper;
        GetLooper((ILooper**)&looper);
        AutoPtr<IHandlerCallback> cb = (IHandlerCallback*)new InnerCallback(this);
        CHandler::New(looper, cb, FALSE, (IHandler**)&mLoaderThreadHandler);
    }
}

void ContactPhotoManagerImpl::LoaderThread::RequestPreloading()
{
    if (mPreloadStatus == PRELOAD_STATUS_DONE) {
        return;
    }

    EnsureHandler();
    Boolean hasMessages;
    if (mLoaderThreadHandler->HasMessages(MESSAGE_LOAD_PHOTOS, &hasMessages), hasMessages) {
        return;
    }

    Boolean result;
    mLoaderThreadHandler->SendEmptyMessageDelayed(MESSAGE_PRELOAD_PHOTOS, PHOTO_PRELOAD_DELAY, &result);
}

void ContactPhotoManagerImpl::LoaderThread::RequestLoading()
{
    EnsureHandler();
    mLoaderThreadHandler->RemoveMessages(MESSAGE_PRELOAD_PHOTOS);
    Boolean result;
    mLoaderThreadHandler->SendEmptyMessage(MESSAGE_LOAD_PHOTOS, &result);
}

ECode ContactPhotoManagerImpl::LoaderThread::HandleMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MESSAGE_PRELOAD_PHOTOS:
            PreloadPhotosInBackground();
            break;
        case MESSAGE_LOAD_PHOTOS:
            LoadPhotosInBackground();
            break;
    }
    *result = TRUE;
    return NOERROR;
}

void ContactPhotoManagerImpl::LoaderThread::PreloadPhotosInBackground()
{
    if (mPreloadStatus == PRELOAD_STATUS_DONE) {
        return;
    }

    if (mPreloadStatus == PRELOAD_STATUS_NOT_STARTED) {
        QueryPhotosForPreload();
        if (mPreloadPhotoIds.Begin() == mPreloadPhotoIds.End()) {
            mPreloadStatus = PRELOAD_STATUS_DONE;
        }
        else {
            mPreloadStatus = PRELOAD_STATUS_IN_PROGRESS;
        }
        RequestPreloading();
        return;
    }

    Int32 size;
    if (mHost->mBitmapHolderCache->GetSize(&size), size > mHost->mBitmapHolderCacheRedZoneBytes) {
        mPreloadStatus = PRELOAD_STATUS_DONE;
        return;
    }

    mPhotoIds.Clear();
    mPhotoIdsAsStrings.Clear();

    Int32 count = 0;
    Int32 preloadSize = mPreloadPhotoIds.GetSize();
    while(preloadSize > 0 && mPhotoIds.GetSize() < PRELOAD_BATCH) {
        preloadSize--;
        count++;
        Int64 photoId = mPreloadPhotoIds[preloadSize];
        mPhotoIds.Insert(photoId);
        mPhotoIdsAsStrings.Insert(StringUtils::ToString(photoId));
        mPreloadPhotoIds.Remove(photoId);
    }

    LoadThumbnails(TRUE);

    if (preloadSize == 0) {
        mPreloadStatus = PRELOAD_STATUS_DONE;
    }

    mHost->mBitmapHolderCache->GetSize(&size);
    Logger::V(TAG, "Preloaded %d photos.  Cached bytes: %d", count, size);

    RequestPreloading();
}

void ContactPhotoManagerImpl::LoaderThread::QueryPhotosForPreload()
{
    AutoPtr<ICursor> cursor;
    // try {
    AutoPtr<IContactsContractContacts> contacts;
    CContactsContractContacts::AcquireSingleton((IContactsContractContacts**)&contacts);
    AutoPtr<IUri> contentUri;
    contacts->GetCONTENT_URI((IUri**)&contentUri);
    AutoPtr<IUriBuilder> builder;
    contentUri->BuildUpon((IUriBuilder**)&builder);
    builder->AppendQueryParameter(
            IContactsContract::DIRECTORY_PARAM_KEY, StringUtils::ToString(IContactsContractDirectory::DEFAULT));
    builder->AppendQueryParameter(IContactsContract::LIMIT_PARAM_KEY,
            StringUtils::ToString(MAX_PHOTOS_TO_PRELOAD));
    AutoPtr<IUri> uri;
    builder->Build((IUri**)&uri);
    AutoPtr<ArrayOf<String> > attrs = ArrayOf<String>::Alloc(1);
    (*attrs)[0] = IContactsContractContactsColumns::PHOTO_ID;
    StringBuilder sb(IContactsContractContactsColumns::PHOTO_ID);
    sb.Append(" NOT NULL AND ");
    sb.Append(IContactsContractContactsColumns::PHOTO_ID);
    sb.Append("!=0");
    StringBuilder sb1(IContactsContractContactOptionsColumns::STARRED);
    sb1.Append(" DESC, ");
    sb1.Append(IContactsContractContactOptionsColumns::LAST_TIME_CONTACTED);
    sb1.Append(" DESC");
    mResolver->Query(uri, attrs, sb.ToString(), NULL, sb1.ToString(), (ICursor**)&cursor);

    if (cursor != NULL) {
        Boolean next;
        while (cursor->MoveToNext(&next), next) {
            // Insert them in reverse order, because we will be taking
            // them from the end of the list for loading.
            Int64 value;
            cursor->GetInt64(0, &value);
            mPreloadPhotoIds.Insert(mPreloadPhotoIds.Begin(), value);
        }
    }
    // } finally {
    //     if (cursor != null) {
    //         cursor.close();
    //     }
    // }
    if (cursor != NULL) {
        ICloseable::Probe(cursor)->Close();
    }
}

void ContactPhotoManagerImpl::LoaderThread::LoadPhotosInBackground()
{
    mHost->ObtainPhotoIdsAndUrisToLoad(mPhotoIds, mPhotoIdsAsStrings, mPhotoUris);
    LoadThumbnails(FALSE);
    LoadUriBasedPhotos();
    RequestPreloading();
}

void ContactPhotoManagerImpl::LoaderThread::LoadThumbnails(
    /* [in] */ Boolean preloading)
{
    if (mPhotoIds.Begin() == mPhotoIds.End()) {
        return;
    }

    // Remove loaded photos from the preload queue: we don't want
    // the preloading process to load them again.
    if (!preloading && mPreloadStatus == PRELOAD_STATUS_IN_PROGRESS) {
        Set<Int64>::Iterator it = mPhotoIds.Begin();
        for (; it != mPhotoIds.End(); ++it) {
            Int64 id = *it;
            mPreloadPhotoIds.Remove(id);
        }
        if (mPreloadPhotoIds.Begin() == mPreloadPhotoIds.End()) {
            mPreloadStatus = PRELOAD_STATUS_DONE;
        }
    }

    mStringBuilder.SetLength(0);
    mStringBuilder.Append(IBaseColumns::ID);
    mStringBuilder.Append(" IN(");
    Set<Int64>::Iterator it = mPhotoIds.Begin();
    for (; it != mPhotoIds.End(); ++it) {
        if (it != mPhotoIds.Begin()) {
            mStringBuilder.AppendChar(',');
        }
        mStringBuilder.AppendChar('?');
    }
    mStringBuilder.AppendChar(')');

    AutoPtr<ICursor> cursor;
    // try {
    // if (DEBUG) Logger::D(TAG, "Loading " + TextUtils.join(",", mPhotoIdsAsStrings));
    AutoPtr<IContactsContractData> data;
    CContactsContractData::AcquireSingleton((IContactsContractData**)&data);
    AutoPtr<IUri> contentUri;
    data->GetCONTENT_URI((IUri**)&contentUri);
    AutoPtr<ArrayOf<String> > strs = ArrayOf<String>::Alloc(mPhotoIdsAsStrings.GetSize());
    Set<String>::Iterator strIt = mPhotoIdsAsStrings.Begin();
    for (Int32 i = 0; strIt != mPhotoIdsAsStrings.End(); ++strIt, ++i) {
        (*strs)[i] = *strIt;
    }
    mResolver->Query(contentUri, COLUMNS, mStringBuilder.ToString(), strs, String(NULL), (ICursor**)&cursor);

    if (cursor != NULL) {
        Boolean next;
        while (cursor->MoveToNext(&next), next) {
            Int64 id;
            cursor->GetInt64(0, &id);
            AutoPtr<IInteger64> integer;
            CInteger64::New(id, (IInteger64**)&integer);
            AutoPtr<ArrayOf<Byte> > bytes;
            cursor->GetBlob(1, (ArrayOf<Byte>**)&bytes);
            mHost->CacheBitmap(integer, bytes, preloading, -1);
            mPhotoIds.Erase(id);
        }
    }
    // } finally {
    //     if (cursor != null) {
    //         cursor.close();
    //     }
    // }
    if (cursor != NULL) {
        ICloseable::Probe(cursor)->Close();
    }

    // Remaining photos were not found in the contacts database (but might be in profile).
    it = mPhotoIds.Begin();
    for (; it != mPhotoIds.End(); ++it) {
        Int64 id = *it;
        AutoPtr<IContactsContract> contract;
        CContactsContract::AcquireSingleton((IContactsContract**)&contract);
        Boolean isProfileId;
        if (contract->IsProfileId(id, &isProfileId), isProfileId) {
            AutoPtr<ICursor> profileCursor;
            // try {
            AutoPtr<IContentUris> uris;
            CContentUris::AcquireSingleton((IContentUris**)&uris);
            AutoPtr<IUri> uri;
            uris->WithAppendedId(contentUri, id, (IUri**)&uri);
            mResolver->Query(uri, COLUMNS, String(NULL), NULL, String(NULL), (ICursor**)&profileCursor);
            Boolean first;
            if (profileCursor != NULL && (profileCursor->MoveToFirst(&first), first)) {
                Int64 profileId;
                profileCursor->GetInt64(0, &profileId);
                AutoPtr<IInteger64> integer;
                CInteger64::New(profileId, (IInteger64**)&integer);
                AutoPtr<ArrayOf<Byte> > bytes;
                profileCursor->GetBlob(1, (ArrayOf<Byte>**)&bytes);
                mHost->CacheBitmap(integer, bytes, preloading, -1);
            }
            else {
                // Couldn't load a photo this way either.
                AutoPtr<IInteger64> integer;
                CInteger64::New(id, (IInteger64**)&integer);
                mHost->CacheBitmap(integer, NULL, preloading, -1);
            }
            // } finally {
            //     if (profileCursor != null) {
            //         profileCursor.close();
            //     }
            // }
            if (profileCursor != NULL) {
                ICloseable::Probe(profileCursor)->Close();
            }
        }
        else {
            // Not a profile photo and not found - mark the cache accordingly
            AutoPtr<IInteger64> integer;
            CInteger64::New(id, (IInteger64**)&integer);
            mHost->CacheBitmap(integer, NULL, preloading, -1);
        }
    }

    Boolean result;
    mHost->mMainThreadHandler->SendEmptyMessage(MESSAGE_PHOTOS_LOADED, &result);
}

void ContactPhotoManagerImpl::LoaderThread::LoadUriBasedPhotos()
{
    Set<AutoPtr<Request> >::Iterator it = mPhotoUris.Begin();
    for (; it != mPhotoUris.End(); ++it) {
        AutoPtr<Request> uriRequest = *it;
        // Keep the original URI and use this to key into the cache.  Failure to do so will
        // result in an image being continually reloaded into cache if the original URI
        // has a contact type encodedFragment (eg nearby places business photo URLs).
        AutoPtr<IUri> originalUri = uriRequest->GetUri();

        // Strip off the "contact type" we added to the URI to ensure it was identifiable as
        // a business photo -- there is no need to pass this on to the server.
        AutoPtr<IUri> uri = ContactPhotoManager::RemoveContactType(originalUri);

        if (mBuffer == NULL) {
            mBuffer = ArrayOf<Byte>::Alloc(BUFFER_SIZE);
        }
        // try {
        if (DEBUG) Logger::D(TAG, "Loading %s", TO_CSTR(uri));
        String scheme;
        uri->GetScheme(&scheme);
        AutoPtr<IInputStream> is;
        if (scheme.Equals("http") || scheme.Equals("https")) {
            String str;
            IObject::Probe(uri)->ToString(&str);
            AutoPtr<IURL> url;
            CURL::New(str, (IURL**)&url);
            url->OpenStream((IInputStream**)&is);
        }
        else {
            mResolver->OpenInputStream(uri, (IInputStream**)&is);
        }
        if (is != NULL) {
            AutoPtr<IByteArrayOutputStream> baos;
            CByteArrayOutputStream::New((IByteArrayOutputStream**)&baos);
            // try {
            Int32 size;
            while (is->Read(mBuffer, &size), size != -1) {
                IOutputStream::Probe(baos)->Write(mBuffer, 0, size);
            }
            // } finally {
            //     is.close();
            // }
            ICloseable::Probe(is)->Close();
            AutoPtr<ArrayOf<Byte> > bytes;
            baos->ToByteArray((ArrayOf<Byte>**)&bytes);
            mHost->CacheBitmap(originalUri, bytes, FALSE, uriRequest->GetRequestedExtent());
            Boolean result;
            mHost->mMainThreadHandler->SendEmptyMessage(MESSAGE_PHOTOS_LOADED, &result);
        }
        else {
            Logger::V(TAG, "Cannot load photo %s", TO_CSTR(uri));
            mHost->CacheBitmap(originalUri, NULL, FALSE, uriRequest->GetRequestedExtent());
        }
        // } catch (final Exception | OutOfMemoryError ex) {
        //     Log.v(TAG, "Cannot load photo " + uri, ex);
        //     cacheBitmap(originalUri, null, false, uriRequest.getRequestedExtent());
        // }
    }
}


//=================================================================
// ContactPhotoManagerImpl::Request
//=================================================================
ContactPhotoManagerImpl::Request::Request(
    /* [in] */ Int64 id,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 requestedExtent,
    /* [in] */ Boolean darkTheme,
    /* [in] */ Boolean isCircular,
    /* [in] */ IContactPhotoManagerDefaultImageProvider* defaultProvider)
    : mId(id)
    , mUri(uri)
    , mDarkTheme(darkTheme)
    , mRequestedExtent(requestedExtent)
    , mDefaultProvider(defaultProvider)
    , mIsCircular(isCircular)
{}

AutoPtr<ContactPhotoManagerImpl::Request> ContactPhotoManagerImpl::Request::CreateFromThumbnailId(
    /* [in] */ Int64 id,
    /* [in] */ Boolean darkTheme,
    /* [in] */ Boolean isCircular,
    /* [in] */ IContactPhotoManagerDefaultImageProvider* defaultProvider)
{
    return new Request(id, NULL /* no URI */, -1, darkTheme, isCircular, defaultProvider);
}

AutoPtr<ContactPhotoManagerImpl::Request> ContactPhotoManagerImpl::Request::CreateFromUri(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 requestedExtent,
    /* [in] */ Boolean darkTheme,
    /* [in] */ Boolean isCircular,
    /* [in] */ IContactPhotoManagerDefaultImageProvider* defaultProvider)
{
    return new Request(0 /* no ID */, uri, requestedExtent, darkTheme, isCircular, defaultProvider);
}

Boolean ContactPhotoManagerImpl::Request::IsUriRequest()
{
    return mUri != NULL;
}

AutoPtr<IUri> ContactPhotoManagerImpl::Request::GetUri()
{
    return mUri;
}

Int64 ContactPhotoManagerImpl::Request::GetId()
{
    return mId;
}

Int32 ContactPhotoManagerImpl::Request::GetRequestedExtent()
{
    return mRequestedExtent;
}

ECode ContactPhotoManagerImpl::Request::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    Int32 prime = 31;
    Int32 result = 1;
    result = prime * result + (Int32)(mId ^ (((UInt64)mId) >> 32));
    result = prime * result + mRequestedExtent;
    Int32 uriCode;
    result = prime * result + ((mUri == NULL) ? 0 : (IObject::Probe(mUri)->GetHashCode(&uriCode), uriCode));
    *hashCode = result;
    return NOERROR;
}

ECode ContactPhotoManagerImpl::Request::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (TO_IINTERFACE(this) == obj) {
        *result = TRUE;
        return NOERROR;
    }
    if (obj == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    // if (getClass() != obj.getClass()) return false;
    AutoPtr<Request> that = (Request*)(IObject*)obj;
    if (mId != that->mId) {
        *result = FALSE;
        return NOERROR;
    }
    if (mRequestedExtent != that->mRequestedExtent) {
        *result = FALSE;
        return NOERROR;
    }
    if (!UriUtils::AreEqual(mUri, that->mUri)) {
        *result = FALSE;
        return NOERROR;
    }
    // Don't compare equality of mDarkTheme because it is only used in the default contact
    // photo case. When the contact does have a photo, the contact photo is the same
    // regardless of mDarkTheme, so we shouldn't need to put the photo request on the queue
    // twice.
    *result = TRUE;
    return NOERROR;
}

AutoPtr<IInterface> ContactPhotoManagerImpl::Request::GetKey()
{
    AutoPtr<IInteger64> integer;
    CInteger64::New(mId, (IInteger64**)&integer);
    return mUri == NULL ? TO_IINTERFACE(integer) : TO_IINTERFACE(mUri);
}

void ContactPhotoManagerImpl::Request::ApplyDefaultImage(
    /* [in] */ IImageView* view,
    /* [in] */ Boolean isCircular)
{
    AutoPtr<IContactPhotoManagerDefaultImageRequest> request;

    if (isCircular) {
        request = ContactPhotoManager::IsBusinessContactUri(mUri)
                ? (IContactPhotoManagerDefaultImageRequest*)ContactPhotoManager::DefaultImageRequest::EMPTY_CIRCULAR_BUSINESS_IMAGE_REQUEST
                : (IContactPhotoManagerDefaultImageRequest*)ContactPhotoManager::DefaultImageRequest::EMPTY_CIRCULAR_DEFAULT_IMAGE_REQUEST;
    }
    else {
        request = ContactPhotoManager::IsBusinessContactUri(mUri)
                ? (IContactPhotoManagerDefaultImageRequest*)ContactPhotoManager::DefaultImageRequest::EMPTY_DEFAULT_BUSINESS_IMAGE_REQUEST
                : (IContactPhotoManagerDefaultImageRequest*)ContactPhotoManager::DefaultImageRequest::EMPTY_DEFAULT_IMAGE_REQUEST;
    }
    mDefaultProvider->ApplyDefaultImage(view, mRequestedExtent, mDarkTheme, request);
}


//=================================================================
// ContactPhotoManagerImpl::BitmapCache
//=================================================================
Int32 ContactPhotoManagerImpl::BitmapCache::SizeOf(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value)
{
    Int32 count;
    IBitmap::Probe(value)->GetByteCount(&count);
    return count;
}

ECode ContactPhotoManagerImpl::BitmapCache::EntryRemoved(
    /* [in] */ Boolean evicted,
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* oldValue,
    /* [in] */ IInterface* newValue)
{
    if (DEBUG) mHost->DumpStats();
    return NOERROR;
}


//=================================================================
// ContactPhotoManagerImpl::BitmapHolderCache
//=================================================================
Int32 ContactPhotoManagerImpl::BitmapHolderCache::SizeOf(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value)
{
    AutoPtr<BitmapHolder> holder = (BitmapHolder*)(IObject*)value;
    return holder->mBytes != NULL ? holder->mBytes->GetLength() : 0;
}

ECode ContactPhotoManagerImpl::BitmapHolderCache::EntryRemoved(
    /* [in] */ Boolean evicted,
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* oldValue,
    /* [in] */ IInterface* newValue)
{
    if (DEBUG) mHost->DumpStats();
    return NOERROR;
}


//=================================================================
// ContactPhotoManagerImpl::InnerHandlerCallback
//=================================================================
CAR_INTERFACE_IMPL(ContactPhotoManagerImpl::InnerHandlerCallback, Object, IHandlerCallback);

ECode ContactPhotoManagerImpl::InnerHandlerCallback::HandleMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    return mHost->HandleMessage(msg, result);
}


//=================================================================
// ContactPhotoManagerImpl
//=================================================================
const String ContactPhotoManagerImpl::LOADER_THREAD_NAME("ContactPhotoLoader");
const Int32 ContactPhotoManagerImpl::FADE_TRANSITION_DURATION;
const Int32 ContactPhotoManagerImpl::MESSAGE_REQUEST_LOADING;
const Int32 ContactPhotoManagerImpl::MESSAGE_PHOTOS_LOADED;

static AutoPtr<ArrayOf<String> > InitEmptyArray()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(0);
    return array;
}
const AutoPtr<ArrayOf<String> > ContactPhotoManagerImpl::EMPTY_STRING_ARRAY = InitEmptyArray();

static AutoPtr<ArrayOf<String> > InitColumns()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(2);
    (*array)[0] = IBaseColumns::ID;
    (*array)[1] = IContactsContractContactsPhoto::PHOTO;
    return array;
}
const AutoPtr<ArrayOf<String> > ContactPhotoManagerImpl::COLUMNS = InitColumns();
const Int32 ContactPhotoManagerImpl::HOLDER_CACHE_SIZE;
const Int32 ContactPhotoManagerImpl::BITMAP_CACHE_SIZE;
Int32 ContactPhotoManagerImpl::sThumbnailSize;

ContactPhotoManagerImpl::ContactPhotoManagerImpl(
    /* [in] */ IContext* context)
    : mBitmapHolderCacheAllUnfresh(TRUE)
    , mBitmapHolderCacheRedZoneBytes(0)
    , mLoadingRequested(FALSE)
    , mPaused(FALSE)
{
    CConcurrentHashMap::New((IConcurrentHashMap**)&mPendingRequests);
    AutoPtr<IHandlerCallback> cb = (IHandlerCallback*)new InnerHandlerCallback(this);
    CHandler::New(cb, FALSE, (IHandler**)&mMainThreadHandler);
    CAtomicInteger32::New((IAtomicInteger32**)&mStaleCacheOverwrite);
    CAtomicInteger32::New((IAtomicInteger32**)&mFreshCacheOverwrite);

    mContext = context;

    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&service);
    AutoPtr<IActivityManager> am = IActivityManager::Probe(service);

    Boolean isLowRamDevice;
    am->IsLowRamDevice(&isLowRamDevice);
    Float cacheSizeAdjustment = isLowRamDevice ? 0.5f : 1.0f;

    Int32 bitmapCacheSize = (Int32)(cacheSizeAdjustment * BITMAP_CACHE_SIZE);
    AutoPtr<BitmapCache> bitmapCache = new BitmapCache(this);
    bitmapCache->constructor(bitmapCacheSize);
    mBitmapCache = (ILruCache*)bitmapCache;

    Int32 holderCacheSize = (Int32)(cacheSizeAdjustment * HOLDER_CACHE_SIZE);
    AutoPtr<BitmapHolderCache> bitmapHolderCache = new BitmapHolderCache(this);
    bitmapHolderCache->constructor(holderCacheSize);
    mBitmapHolderCache = (ILruCache*)bitmapHolderCache;

    mBitmapHolderCacheRedZoneBytes = (Int32)(holderCacheSize * 0.75);
    Logger::I(TAG, "Cache adj: %d", cacheSizeAdjustment);
    if (DEBUG) {
        Int32 size1, size2;
        mBitmapHolderCache->GetMaxSize(&size1);
        mBitmapCache->GetMaxSize(&size2);
        Logger::D(TAG, "Cache size: %s + %s", Btk(size1).string(), Btk(size2).string());
    }

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetDimensionPixelSize(
            Elastos::Droid::Dialer::R::dimen::contact_browser_list_item_photo_size, &sThumbnailSize);
}

String ContactPhotoManagerImpl::Btk(
    /* [in] */ Int32 bytes)
{
    return StringUtils::ToString((bytes + 1023) / 1024) + "K";
}

Int32 ContactPhotoManagerImpl::SafeDiv(
    /* [in] */ Int32 dividend,
    /* [in] */ Int32 divisor)
{
    return (divisor == 0) ? 0 : (dividend / divisor);
}

void ContactPhotoManagerImpl::DumpStats()
{
    if (!DEBUG) return;
    // {
    //     Int32 numHolders = 0;
    //     Int32 rawBytes = 0;
    //     Int32 bitmapBytes = 0;
    //     Int32 numBitmaps = 0;
    //     for (BitmapHolder h : mBitmapHolderCache.snapshot().values()) {
    //         numHolders++;
    //         if (h.bytes != null) {
    //             rawBytes += h.bytes.length;
    //         }
    //         Bitmap b = h.bitmapRef != null ? h.bitmapRef.get() : null;
    //         if (b != null) {
    //             numBitmaps++;
    //             bitmapBytes += b.getByteCount();
    //         }
    //     }
    //     Log.d(TAG, "L1: " + btk(rawBytes) + " + " + btk(bitmapBytes) + " = "
    //             + btk(rawBytes + bitmapBytes) + ", " + numHolders + " holders, "
    //             + numBitmaps + " bitmaps, avg: "
    //             + btk(safeDiv(rawBytes, numHolders))
    //             + "," + btk(safeDiv(bitmapBytes,numBitmaps)));
    //     Log.d(TAG, "L1 Stats: " + mBitmapHolderCache.toString()
    //             + ", overwrite: fresh=" + mFreshCacheOverwrite.get()
    //             + " stale=" + mStaleCacheOverwrite.get());
    // }

    // {
    //     int numBitmaps = 0;
    //     int bitmapBytes = 0;
    //     for (Bitmap b : mBitmapCache.snapshot().values()) {
    //         numBitmaps++;
    //         bitmapBytes += b.getByteCount();
    //     }
    //     Log.d(TAG, "L2: " + btk(bitmapBytes) + ", " + numBitmaps + " bitmaps"
    //             + ", avg: " + btk(safeDiv(bitmapBytes, numBitmaps)));
    //     // We don't get from L2 cache, so L2 stats is meaningless.
    // }
}

ECode ContactPhotoManagerImpl::OnTrimMemory(
    /* [in] */ Int32 level)
{
    if (DEBUG) Logger::D(TAG, "onTrimMemory: %d", level);
    if (level >= IComponentCallbacks2::TRIM_MEMORY_MODERATE) {
        // Clear the caches.  Note all pending requests will be removed too.
        Clear();
    }
    return NOERROR;
}

ECode ContactPhotoManagerImpl::PreloadPhotosInBackground()
{
    EnsureLoaderThread();
    mLoaderThread->RequestPreloading();
    return NOERROR;
}

ECode ContactPhotoManagerImpl::LoadThumbnail(
    /* [in] */ IImageView* view,
    /* [in] */ Int64 photoId,
    /* [in] */ Boolean darkTheme,
    /* [in] */ Boolean isCircular,
    /* [in] */ IContactPhotoManagerDefaultImageRequest* defaultImageRequest,
    /* [in] */ IContactPhotoManagerDefaultImageProvider* defaultProvider)
{
    if (photoId == 0) {
        // No photo is needed
        defaultProvider->ApplyDefaultImage(view, -1, darkTheme, defaultImageRequest);
        mPendingRequests->Remove(view);
    }
    else {
        if (DEBUG) Logger::D(TAG, "loadPhoto request: %d", photoId);
        AutoPtr<Request> request = Request::CreateFromThumbnailId(
                photoId, darkTheme, isCircular, defaultProvider);
        LoadPhotoByIdOrUri(view, request);
    }
    return NOERROR;
}

ECode ContactPhotoManagerImpl::LoadPhoto(
    /* [in] */ IImageView* view,
    /* [in] */ IUri* photoUri,
    /* [in] */ Int32 requestedExtent,
    /* [in] */ Boolean darkTheme,
    /* [in] */ Boolean isCircular,
    /* [in] */ IContactPhotoManagerDefaultImageRequest* defaultImageRequest,
    /* [in] */ IContactPhotoManagerDefaultImageProvider* defaultProvider)
{
    if (photoUri == NULL) {
        // No photo is needed
        defaultProvider->ApplyDefaultImage(view, requestedExtent, darkTheme,
                defaultImageRequest);
        mPendingRequests->Remove(view);
    }
    else {
        if (DEBUG) Logger::D(TAG, "loadPhoto request: %s", TO_CSTR(photoUri));
        if (IsDefaultImageUri(photoUri)) {
            CreateAndApplyDefaultImageForUri(view, photoUri, requestedExtent, darkTheme,
                    isCircular, defaultProvider);
        }
        else {
            AutoPtr<Request> request = Request::CreateFromUri(
                    photoUri, requestedExtent, darkTheme, isCircular, defaultProvider);
            LoadPhotoByIdOrUri(view, request);
        }
    }
    return NOERROR;
}

void ContactPhotoManagerImpl::CreateAndApplyDefaultImageForUri(
    /* [in] */ IImageView* view,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 requestedExtent,
    /* [in] */ Boolean darkTheme,
    /* [in] */ Boolean isCircular,
    /* [in] */ IContactPhotoManagerDefaultImageProvider* defaultProvider)
{
    AutoPtr<DefaultImageRequest> request = GetDefaultImageRequestFromUri(uri);
    request->mIsCircular = isCircular;
    defaultProvider->ApplyDefaultImage(view, requestedExtent, darkTheme,
            (IContactPhotoManagerDefaultImageRequest*)request.Get());
}

void ContactPhotoManagerImpl::LoadPhotoByIdOrUri(
    /* [in] */ IImageView* view,
    /* [in] */ Request* request)
{
    Boolean loaded = LoadCachedPhoto(view, request, FALSE);
    if (loaded) {
        mPendingRequests->Remove(view);
    }
    else {
        mPendingRequests->Put(view, (IObject*)request);
        if (!mPaused) {
            // Send a request to start loading photos
            RequestLoading();
        }
    }
}

ECode ContactPhotoManagerImpl::RemovePhoto(
    /* [in] */ IImageView* view)
{
    view->SetImageDrawable(NULL);
    mPendingRequests->Remove(view);
    return NOERROR;
}

ECode ContactPhotoManagerImpl::CancelPendingRequests(
    /* [in] */ IView* fragmentRootView)
{
    if (fragmentRootView == NULL) {
        mPendingRequests->Clear();
        return NOERROR;
    }
    AutoPtr<ISet> keySet;
    mPendingRequests->GetKeySet((ISet**)&keySet);
    Int32 size;
    mPendingRequests->GetSize(&size);
    AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(size);
    AutoPtr<ArrayOf<IInterface*> > requestSetCopy;
    keySet->ToArray(array, (ArrayOf<IInterface*>**)&requestSetCopy);
    for (Int32 i = 0; i < requestSetCopy->GetLength(); ++i) {
        AutoPtr<IImageView> imageView = IImageView::Probe((*requestSetCopy)[i]);
        // If an ImageView is orphaned (currently scrap) or a child of fragmentRootView, then
        // we can safely remove its request.
        AutoPtr<IViewParent> parent;
        if ((IView::Probe(imageView)->GetParent((IViewParent**)&parent), parent == NULL)
                || IsChildView(fragmentRootView, IView::Probe(imageView))) {
            mPendingRequests->Remove(imageView);
        }
    }
    return NOERROR;
}

Boolean ContactPhotoManagerImpl::IsChildView(
    /* [in] */ IView* parent,
    /* [in] */ IView* potentialChild)
{
    AutoPtr<IViewParent> p;
    return (potentialChild->GetParent((IViewParent**)&p), p != NULL) && (IView::Probe(p) == parent || (
            IViewGroup::Probe(p) != NULL && IsChildView(parent, IView::Probe(p))));
}

ECode ContactPhotoManagerImpl::RefreshCache()
{
    if (mBitmapHolderCacheAllUnfresh) {
        if (DEBUG) Logger::D(TAG, "refreshCache -- no fresh entries.");
        return NOERROR;
    }
    if (DEBUG) Logger::D(TAG, "refreshCache");
    mBitmapHolderCacheAllUnfresh = TRUE;
    AutoPtr<IMap> map;
    mBitmapHolderCache->Snapshot((IMap**)&map);
    AutoPtr<ICollection> values;
    map->GetValues((ICollection**)&values);
    AutoPtr<IIterator> it;
    values->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> next;
        it->GetNext((IInterface**)&next);
        AutoPtr<BitmapHolder> holder = (BitmapHolder*)(IObject*)next.Get();
        holder->mFresh = FALSE;
    }
    return NOERROR;
}

Boolean ContactPhotoManagerImpl::LoadCachedPhoto(
    /* [in] */ IImageView* view,
    /* [in] */ Request* request,
    /* [in] */ Boolean fadeIn)
{
    AutoPtr<IInterface> key = request->GetKey();
    AutoPtr<IInterface> value;
    mBitmapHolderCache->Get(key, (IInterface**)&value);
    if (value == NULL) {
        // The bitmap has not been loaded ==> show default avatar
        request->ApplyDefaultImage(view, request->mIsCircular);
        return FALSE;
    }

    AutoPtr<BitmapHolder> holder = (BitmapHolder*)(IObject*)value.Get();
    if (holder->mBytes == NULL) {
        request->ApplyDefaultImage(view, request->mIsCircular);
        return holder->mFresh;
    }

    AutoPtr<IBitmap> cachedBitmap = holder->mBitmapRef == NULL ? NULL : holder->mBitmapRef;
    if (cachedBitmap == NULL) {
        if (holder->mBytes->GetLength() < 8 * 1024) {
            // Small thumbnails are usually quick to inflate. Let's do that on the UI thread
            InflateBitmap(holder, request->GetRequestedExtent());
            cachedBitmap = holder->mBitmap;
            if (cachedBitmap == NULL) return FALSE;
        }
        else {
            // This is bigger data. Let's send that back to the Loader so that we can
            // inflate this in the background
            request->ApplyDefaultImage(view, request->mIsCircular);
            return FALSE;
        }
    }

    AutoPtr<IDrawable> previousDrawable;
    view->GetDrawable((IDrawable**)&previousDrawable);
    if (fadeIn && previousDrawable != NULL) {
        AutoPtr<ArrayOf<IDrawable*> > layers = ArrayOf<IDrawable*>::Alloc(2);
        // Prevent cascade of TransitionDrawables.
        AutoPtr<ITransitionDrawable> previousTransitionDrawable = ITransitionDrawable::Probe(previousDrawable);
        if (previousTransitionDrawable != NULL) {
            AutoPtr<ILayerDrawable> layerDrawable = ILayerDrawable::Probe(previousTransitionDrawable);
            Int32 number;
            layerDrawable->GetNumberOfLayers(&number);
            AutoPtr<IDrawable> d;
            layerDrawable->GetDrawable(number - 1, (IDrawable**)&d);
            layers->Set(0, d);
        }
        else {
            layers->Set(0, previousDrawable);
        }
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        AutoPtr<IDrawable> d = GetDrawableForBitmap(res, cachedBitmap, request);
        layers->Set(1, d);
        AutoPtr<ITransitionDrawable> drawable;
        CTransitionDrawable::New(layers, (ITransitionDrawable**)&drawable);
        view->SetImageDrawable(IDrawable::Probe(drawable));
        drawable->StartTransition(FADE_TRANSITION_DURATION);
    }
    else {
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        AutoPtr<IDrawable> d = GetDrawableForBitmap(res, cachedBitmap, request);
        view->SetImageDrawable(d);
    }

    // Put the bitmap in the LRU cache. But only do this for images that are small enough
    // (we require that at least six of those can be cached at the same time)
    Int32 count;
    cachedBitmap->GetByteCount(&count);
    Int32 maxSize;
    mBitmapCache->GetMaxSize(&maxSize);
    if (count < maxSize / 6) {
        AutoPtr<IInterface> key = request->GetKey();
        AutoPtr<IInterface> oldValue;
        mBitmapCache->Put(key, cachedBitmap, (IInterface**)&oldValue);
    }

    // Soften the reference
    holder->mBitmap = NULL;

    return holder->mFresh;
}

AutoPtr<IDrawable> ContactPhotoManagerImpl::GetDrawableForBitmap(
    /* [in] */ IResources* resources,
    /* [in] */ IBitmap* bitmap,
    /* [in] */ Request* request)
{
    if (request->mIsCircular) {
        assert(0);
        // AutoPtr<IRoundedBitmapDrawable> drawable =
        //         RoundedBitmapDrawableFactory.create(resources, bitmap);
        // drawable.setAntiAlias(true);
        // drawable.setCornerRadius(bitmap.getHeight() / 2);
        // return drawable;
    }
    else {
        AutoPtr<IDrawable> drawable;
        CBitmapDrawable::New(resources, bitmap, (IDrawable**)&drawable);
        return drawable;
    }
}

void ContactPhotoManagerImpl::InflateBitmap(
    /* [in] */ BitmapHolder* holder,
    /* [in] */ Int32 requestedExtent)
{
    Int32 sampleSize =
            BitmapUtil::FindOptimalSampleSize(holder->mOriginalSmallerExtent, requestedExtent);
    AutoPtr<ArrayOf<Byte> > bytes = holder->mBytes;
    if (bytes == NULL || bytes->GetLength() == 0) {
        return;
    }

    if (sampleSize == holder->mDecodedSampleSize) {
        // Check the soft reference.  If will be retained if the bitmap is also
        // in the LRU cache, so we don't need to check the LRU cache explicitly.
        if (holder->mBitmapRef != NULL) {
            holder->mBitmap = holder->mBitmapRef;
            if (holder->mBitmap != NULL) {
                return;
            }
        }
    }

    // try {
    AutoPtr<IBitmap> bitmap = BitmapUtil::DecodeBitmapFromBytes(bytes, sampleSize);

    // TODO: As a temporary workaround while framework support is being added to
    // clip non-square bitmaps into a perfect circle, manually crop the bitmap into
    // into a square if it will be displayed as a thumbnail so that it can be cropped
    // into a circle.
    Int32 height, width;
    bitmap->GetHeight(&height);
    bitmap->GetWidth(&width);

    // The smaller dimension of a scaled bitmap can range from anywhere from 0 to just
    // below twice the length of a thumbnail image due to the way we calculate the optimal
    // sample size.
    if (height != width && Elastos::Core::Math::Min(height, width) <= sThumbnailSize * 2) {
        Int32 dimension = Elastos::Core::Math::Min(height, width);
        AutoPtr<IThumbnailUtils> thumbnailUtils;
        CThumbnailUtils::AcquireSingleton((IThumbnailUtils**)&thumbnailUtils);
        AutoPtr<IBitmap> temp;
        thumbnailUtils->ExtractThumbnail(bitmap, dimension, dimension, (IBitmap**)&temp);
        bitmap = temp;
    }
    // make bitmap mutable and draw size onto it
    if (DEBUG_SIZES) {
        AutoPtr<IBitmap> original = bitmap;
        BitmapConfig config;
        bitmap->GetConfig(&config);
        AutoPtr<IBitmap> temp;
        bitmap->Copy(config, TRUE, (IBitmap**)&temp);
        bitmap = temp;
        original->Recycle();
        AutoPtr<ICanvas> canvas;
        CCanvas::New(bitmap, (ICanvas**)&canvas);
        AutoPtr<IPaint> paint;
        CPaint::New((IPaint**)&paint);
        paint->SetTextSize(16);
        paint->SetColor(IColor::BLUE);
        paint->SetStyle(PaintStyle_FILL);
        canvas->DrawRect(0.0f, 0.0f, 50.0f, 20.0f, paint);
        paint->SetColor(IColor::WHITE);
        paint->SetAntiAlias(TRUE);
        bitmap->GetWidth(&width);
        canvas->DrawText(StringUtils::ToString(width) + "/" + StringUtils::ToString(sampleSize), 0, 15, paint);
    }

    holder->mDecodedSampleSize = sampleSize;
    holder->mBitmap = bitmap;
    holder->mBitmapRef = bitmap;
    if (DEBUG) {
        bitmap->GetWidth(&width);
        bitmap->GetHeight(&height);
        Int32 count;
        bitmap->GetByteCount(&count);
        Logger::D(TAG, "inflateBitmap %s -> %dx%d, %s", Btk(bytes->GetLength()).string(),
                width, height, Btk(count).string());
    }
    // } catch (OutOfMemoryError e) {
    //     // Do nothing - the photo will appear to be missing
    // }
}

void ContactPhotoManagerImpl::Clear()
{
    if (DEBUG) Logger::D(TAG, "clear");
    mPendingRequests->Clear();
    mBitmapHolderCache->EvictAll();
    mBitmapCache->EvictAll();
}

ECode ContactPhotoManagerImpl::Pause()
{
    mPaused = TRUE;
    return NOERROR;
}

ECode ContactPhotoManagerImpl::Resume()
{
    mPaused = FALSE;
    if (DEBUG) DumpStats();
    Boolean isEmpty;
    if (mPendingRequests->IsEmpty(&isEmpty), !isEmpty) {
        RequestLoading();
    }
    return NOERROR;
}

void ContactPhotoManagerImpl::RequestLoading()
{
    if (!mLoadingRequested) {
        mLoadingRequested = TRUE;
        Boolean result;
        mMainThreadHandler->SendEmptyMessage(MESSAGE_REQUEST_LOADING, &result);
    }
}

ECode ContactPhotoManagerImpl::HandleMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MESSAGE_REQUEST_LOADING: {
            mLoadingRequested = FALSE;
            if (!mPaused) {
                EnsureLoaderThread();
                mLoaderThread->RequestLoading();
            }
            *result = TRUE;
            return NOERROR;
        }

        case MESSAGE_PHOTOS_LOADED: {
            if (!mPaused) {
                ProcessLoadedImages();
            }
            if (DEBUG) DumpStats();
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

void ContactPhotoManagerImpl::EnsureLoaderThread()
{
    if (mLoaderThread == NULL) {
        AutoPtr<IContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);
        mLoaderThread = new LoaderThread(this);
        mLoaderThread->constructor(cr);
        mLoaderThread->Start();
    }
}

void ContactPhotoManagerImpl::ProcessLoadedImages()
{
    AutoPtr<ISet> keySet;
    mPendingRequests->GetKeySet((ISet**)&keySet);
    AutoPtr<IIterator> iterator;
    keySet->GetIterator((IIterator**)&iterator);
    Boolean hasNext;
    while (iterator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> next;
        iterator->GetNext((IInterface**)&next);
        AutoPtr<IImageView> view = IImageView::Probe(next);
        AutoPtr<IInterface> temp;
        mPendingRequests->Get(view, (IInterface**)&temp);
        AutoPtr<Request> key = (Request*)(IObject*)temp.Get();
        // TODO: Temporarily disable contact photo fading in, until issues with
        // RoundedBitmapDrawables overlapping the default image drawables are resolved.
        Boolean loaded = LoadCachedPhoto(view, key, FALSE);
        if (loaded) {
            iterator->Remove();
        }
    }

    SoftenCache();

    Boolean isEmpty;
    if (mPendingRequests->IsEmpty(&isEmpty), !isEmpty) {
        RequestLoading();
    }
}

void ContactPhotoManagerImpl::SoftenCache()
{
    AutoPtr<IMap> map;
    mBitmapHolderCache->Snapshot((IMap**)&map);
    AutoPtr<ICollection> collection;
    map->GetValues((ICollection**)&collection);
    AutoPtr<IIterator> it;
    collection->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> next;
        it->GetNext((IInterface**)&next);
        AutoPtr<BitmapHolder> holder = (BitmapHolder*)(IObject*)next.Get();
        holder->mBitmap = NULL;
    }
}

void ContactPhotoManagerImpl::CacheBitmap(
    /* [in] */ IInterface* key,
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Boolean preloading,
    /* [in] */ Int32 requestedExtent)
{
    if (DEBUG) {
        AutoPtr<IInterface> value;
        mBitmapHolderCache->Get(key, (IInterface**)&value);
        AutoPtr<BitmapHolder> prev = (BitmapHolder*)(IObject*)value.Get();
        if (prev != NULL && prev->mBytes != NULL) {
            Logger::D(TAG, "Overwriting cache: key=%s %s", TO_CSTR(key), (prev->mFresh ? "FRESH" : "stale"));
            if (prev->mFresh) {
                Int32 value;
                mFreshCacheOverwrite->IncrementAndGet(&value);
            }
            else {
                Int32 value;
                mStaleCacheOverwrite->IncrementAndGet(&value);
            }
        }
        Logger::D(TAG, "Caching data: key=%s, %s", TO_CSTR(key),
                (bytes == NULL ? "<null>" : Btk(bytes->GetLength()).string()));
    }
    AutoPtr<BitmapHolder> holder = new BitmapHolder(bytes,
            bytes == NULL ? -1 : BitmapUtil::GetSmallerExtentFromBytes(bytes));

    // Unless this image is being preloaded, decode it right away while
    // we are still on the background thread.
    if (!preloading) {
        InflateBitmap(holder, requestedExtent);
    }

    AutoPtr<IInterface> oldValue;
    mBitmapHolderCache->Put(key, (IObject*)holder, (IInterface**)&oldValue);
    mBitmapHolderCacheAllUnfresh = FALSE;
}

ECode ContactPhotoManagerImpl::CacheBitmap(
    /* [in] */ IUri* photoUri,
    /* [in] */ IBitmap* bitmap,
    /* [in] */ ArrayOf<Byte>* photoBytes)
{
    Int32 width, height;
    bitmap->GetWidth(&width);
    bitmap->GetHeight(&height);
    Int32 smallerExtent = Elastos::Core::Math::Min(width, height);
    // We can pretend here that the extent of the photo was the size that we originally
    // requested
    AutoPtr<Request> request = Request::CreateFromUri(photoUri, smallerExtent, FALSE /* darkTheme */,
            FALSE /* isCircular */ , DEFAULT_AVATAR);
    AutoPtr<BitmapHolder> holder = new BitmapHolder(photoBytes, smallerExtent);
    holder->mBitmapRef = bitmap;
    AutoPtr<IInterface> oldValue;
    mBitmapHolderCache->Put(request->GetKey(), (IObject*)holder, (IInterface**)&oldValue);
    mBitmapHolderCacheAllUnfresh = FALSE;
    oldValue = NULL;
    mBitmapCache->Put(request->GetKey(), bitmap, (IInterface**)&oldValue);
    return NOERROR;
}

void ContactPhotoManagerImpl::ObtainPhotoIdsAndUrisToLoad(
    /* [in] */ Set<Int64>& photoIds,
    /* [in] */ Set<String>& photoIdsAsStrings,
    /* [in] */ Set<AutoPtr<Request> >& uris)
{
    photoIds.Clear();
    photoIdsAsStrings.Clear();
    uris.Clear();

    Boolean jpegsDecoded = FALSE;

    /*
     * Since the call is made from the loader thread, the map could be
     * changing during the iteration. That's not really a problem:
     * ConcurrentHashMap will allow those changes to happen without throwing
     * exceptions. Since we may miss some requests in the situation of
     * concurrent change, we will need to check the map again once loading
     * is complete.
     */
    AutoPtr<ICollection> collection;
    mPendingRequests->GetValues((ICollection**)&collection);
    AutoPtr<IIterator> iterator;
    collection->GetIterator((IIterator**)&iterator);
    Boolean hasNext;
    while (iterator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> next;
        iterator->GetNext((IInterface**)&next);
        AutoPtr<Request> request = (Request*)(IObject*)next.Get();
        AutoPtr<IInterface> value;
        mBitmapHolderCache->Get(request->GetKey(), (IInterface**)&value);
        if (value != NULL) {
            AutoPtr<BitmapHolder> holder = (BitmapHolder*)(IObject*)value.Get();
            if (holder->mBytes != NULL && holder->mFresh && holder->mBitmapRef == NULL/* || holder.bitmapRef.get() == null*/) {
                // This was previously loaded but we don't currently have the inflated Bitmap
                InflateBitmap(holder, request->GetRequestedExtent());
                jpegsDecoded = TRUE;
            }
            else {
                if (!holder->mFresh) {
                    if (request->IsUriRequest()) {
                        uris.Insert(request);
                    }
                    else {
                        photoIds.Insert(request->GetId());
                        photoIdsAsStrings.Insert(StringUtils::ToString(request->mId));
                    }
                }
            }
        }
        else {
            if (request->IsUriRequest()) {
                uris.Insert(request);
            }
            else {
                photoIds.Insert(request->GetId());
                photoIdsAsStrings.Insert(StringUtils::ToString(request->mId));
            }
        }
    }

    if (jpegsDecoded) {
        Boolean result;
        mMainThreadHandler->SendEmptyMessage(MESSAGE_PHOTOS_LOADED, &result);
    }
}

} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos
