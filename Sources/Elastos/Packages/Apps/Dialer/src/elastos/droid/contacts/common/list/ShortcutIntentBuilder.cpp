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
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telecom.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/contacts/common/list/ShortcutIntentBuilder.h"
#include "elastos/droid/contacts/common/ContactPhotoManager.h"
#include "elastos/droid/contacts/common/ContactsUtils.h"
#include <elastos/droid/text/TextUtils.h>
#include <elastos/core/StringUtils.h>
#include "R.h"

using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::Contacts::Common::ContactsUtils;
using Elastos::Droid::Contacts::Common::ContactPhotoManager;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::PaintStyle_FILL;
using Elastos::Droid::Graphics::IPaintFontMetricsInt;
using Elastos::Droid::Graphics::Drawable::CBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Provider::IContactsContractContactsColumns;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsCommonColumns;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsPhone;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsPhoto;
using Elastos::Droid::Provider::IContactsContractData;
using Elastos::Droid::Provider::CContactsContractData;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::IContactsContractQuickContact;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsPhone;
using Elastos::Droid::Provider::CContactsContractCommonDataKindsPhone;
using Elastos::Droid::Telecom::IPhoneAccount;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::ITextPaint;
using Elastos::Droid::Text::CTextPaint;
using Elastos::Droid::Text::TextUtilsTruncateAt_END;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Core::StringUtils;
using Elastos::Core::CString;
using Elastos::IO::ICloseable;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

//=================================================================
// ShortcutIntentBuilder::LoadingAsyncTask
//=================================================================
ShortcutIntentBuilder::LoadingAsyncTask::LoadingAsyncTask(
    /* [in] */ ShortcutIntentBuilder* host,
    /* [in] */ IUri* uri)
    : mUri(uri)
    , mPhotoId(0)
    , mHost(host)
{}

ECode ShortcutIntentBuilder::LoadingAsyncTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IContentResolver> cr;
    mHost->mContext->GetContentResolver((IContentResolver**)&cr);
    cr->GetType(mUri, &mContentType);
    LoadData();
    LoadPhoto();
    *result = NULL;
    return NOERROR;
}

void ShortcutIntentBuilder::LoadingAsyncTask::LoadPhoto()
{
    if (mPhotoId == 0) {
        return;
    }

    AutoPtr<IContentResolver> resolver;
    mHost->mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<IContactsContractData> data;
    CContactsContractData::AcquireSingleton((IContactsContractData**)&data);
    AutoPtr<IUri> uri;
    data->GetCONTENT_URI((IUri**)&uri);
    AutoPtr<ArrayOf<String> > attrs = ArrayOf<String>::Alloc(1);
    (*attrs)[0] = StringUtils::ToString(mPhotoId);
    AutoPtr<ICursor> cursor;
    resolver->Query(uri, PHOTO_COLUMNS, PHOTO_SELECTION, attrs, String(NULL), (ICursor**)&cursor);
    if (cursor != NULL) {
        // try {
        Boolean result;
        if (cursor->MoveToFirst(&result), result) {
            cursor->GetBlob(PHOTO_PHOTO_COLUMN_INDEX, (ArrayOf<Byte>**)&mBitmapData);
        }
        // } finally {
        //     cursor.close();
        // }
        ICloseable::Probe(cursor)->Close();
    }
}


//=================================================================
// ShortcutIntentBuilder::ContactLoadingAsyncTask
//=================================================================
ShortcutIntentBuilder::ContactLoadingAsyncTask::ContactLoadingAsyncTask(
    /* [in] */ ShortcutIntentBuilder* host,
    /* [in] */ IUri* uri)
    : LoadingAsyncTask(host, uri)
{}

void ShortcutIntentBuilder::ContactLoadingAsyncTask::LoadData()
{
    AutoPtr<IContentResolver> resolver;
    mHost->mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ICursor> cursor;
    resolver->Query(mUri, CONTACT_COLUMNS, String(NULL), NULL, String(NULL), (ICursor**)&cursor);
    if (cursor != NULL) {
        // try {
        Boolean result;
        if (cursor->MoveToFirst(&result), result) {
            cursor->GetString(CONTACT_DISPLAY_NAME_COLUMN_INDEX, &mDisplayName);
            mPhotoId = cursor->GetInt64(CONTACT_PHOTO_ID_COLUMN_INDEX, &mPhotoId);
            cursor->GetString(CONTACT_LOOKUP_KEY_COLUMN_INDEX, &mLookupKey);
        }
        // } finally {
        //     cursor.close();
        // }
        ICloseable::Probe(cursor)->Close();
    }
}

ECode ShortcutIntentBuilder::ContactLoadingAsyncTask::OnPostExecute(
    /* [in] */ IInterface* result)
{
    mHost->CreateContactShortcutIntent(mUri, mContentType, mDisplayName, mLookupKey, mBitmapData);
    return NOERROR;
}


//=================================================================
// ShortcutIntentBuilder::PhoneNumberLoadingAsyncTask
//=================================================================
ShortcutIntentBuilder::PhoneNumberLoadingAsyncTask::PhoneNumberLoadingAsyncTask(
    /* [in] */ ShortcutIntentBuilder* host,
    /* [in] */ IUri* uri,
    /* [in] */ const String& shortcutAction)
    : LoadingAsyncTask(host, uri)
    , mShortcutAction(shortcutAction)
    , mPhoneType(0)
{}

void ShortcutIntentBuilder::PhoneNumberLoadingAsyncTask::LoadData()
{
    AutoPtr<IContentResolver> resolver;
    mHost->mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ICursor> cursor;
    resolver->Query(mUri, PHONE_COLUMNS, String(NULL), NULL, String(NULL), (ICursor**)&cursor);
    if (cursor != NULL) {
        // try {
        Boolean result;
        if (cursor->MoveToFirst(&result), result) {
            cursor->GetString(PHONE_DISPLAY_NAME_COLUMN_INDEX, &mDisplayName);
            cursor->GetInt64(PHONE_PHOTO_ID_COLUMN_INDEX, &mPhotoId);
            cursor->GetString(PHONE_NUMBER_COLUMN_INDEX, &mPhoneNumber);
            cursor->GetInt32(PHONE_TYPE_COLUMN_INDEX, &mPhoneType);
            cursor->GetString(PHONE_LABEL_COLUMN_INDEX, &mPhoneLabel);
            cursor->GetString(PHONE_LOOKUP_KEY_COLUMN_INDEX, &mLookupKey);
        }
        // } finally {
        //     cursor.close();
        // }
        ICloseable::Probe(cursor)->Close();
    }
}

ECode ShortcutIntentBuilder::PhoneNumberLoadingAsyncTask::OnPostExecute(
    /* [in] */ IInterface* result)
{
    mHost->CreatePhoneNumberShortcutIntent(mUri, mDisplayName, mLookupKey, mBitmapData,
            mPhoneNumber, mPhoneType, mPhoneLabel, mShortcutAction);
    return NOERROR;
}


//=================================================================
// ShortcutIntentBuilder
//=================================================================
const String ShortcutIntentBuilder::INTENT_EXTRA_IGNORE_LAUNCH_ANIMATION("com.android.launcher.intent.extra.shortcut.INGORE_LAUNCH_ANIMATION");

static AutoPtr<ArrayOf<String> > InitContactColumns()
{
    AutoPtr<ArrayOf<String> > columns = ArrayOf<String>::Alloc(3);
    (*columns)[0] = IContactsContractContactsColumns::DISPLAY_NAME;
    (*columns)[1] = IContactsContractContactsColumns::PHOTO_ID;
    (*columns)[2] = IContactsContractContactsColumns::LOOKUP_KEY;
    return columns;
}
const AutoPtr<ArrayOf<String> > ShortcutIntentBuilder::CONTACT_COLUMNS = InitContactColumns();
const Int32 ShortcutIntentBuilder::CONTACT_DISPLAY_NAME_COLUMN_INDEX;
const Int32 ShortcutIntentBuilder::CONTACT_PHOTO_ID_COLUMN_INDEX;
const Int32 ShortcutIntentBuilder::CONTACT_LOOKUP_KEY_COLUMN_INDEX;

static AutoPtr<ArrayOf<String> > InitPhoneColumns()
{
    AutoPtr<ArrayOf<String> > columns = ArrayOf<String>::Alloc(6);
    (*columns)[0] = IContactsContractContactsColumns::DISPLAY_NAME;
    (*columns)[1] = IContactsContractContactsColumns::PHOTO_ID;
    (*columns)[2] = IContactsContractCommonDataKindsPhone::NUMBER;
    (*columns)[3] = IContactsContractCommonDataKindsCommonColumns::TYPE;
    (*columns)[4] = IContactsContractCommonDataKindsCommonColumns::LABEL;
    (*columns)[5] = IContactsContractContactsColumns::LOOKUP_KEY;
    return columns;
}
const AutoPtr<ArrayOf<String> > ShortcutIntentBuilder::PHONE_COLUMNS = InitPhoneColumns();
const Int32 ShortcutIntentBuilder::PHONE_DISPLAY_NAME_COLUMN_INDEX;
const Int32 ShortcutIntentBuilder::PHONE_PHOTO_ID_COLUMN_INDEX;
const Int32 ShortcutIntentBuilder::PHONE_NUMBER_COLUMN_INDEX;
const Int32 ShortcutIntentBuilder::PHONE_TYPE_COLUMN_INDEX;
const Int32 ShortcutIntentBuilder::PHONE_LABEL_COLUMN_INDEX;
const Int32 ShortcutIntentBuilder::PHONE_LOOKUP_KEY_COLUMN_INDEX;

static AutoPtr<ArrayOf<String> > InitPhotoColumns()
{
    AutoPtr<ArrayOf<String> > columns = ArrayOf<String>::Alloc(1);
    (*columns)[0] = IContactsContractCommonDataKindsPhoto::PHOTO;
    return columns;
}
const AutoPtr<ArrayOf<String> > ShortcutIntentBuilder::PHOTO_COLUMNS = InitPhotoColumns();
const Int32 ShortcutIntentBuilder::PHOTO_PHOTO_COLUMN_INDEX;
const String ShortcutIntentBuilder::PHOTO_SELECTION(IBaseColumns::ID + "=?");

ShortcutIntentBuilder::ShortcutIntentBuilder(
    /* [in] */ IContext* context,
    /* [in] */ IOnShortcutIntentCreatedListener* listener)
    : mListener(listener)
    , mContext(context)
    , mIconSize(0)
    , mIconDensity(0)
    , mOverlayTextBackgroundColor(0)
{
    context->GetResources((IResources**)&mResources);
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&service);
    AutoPtr<IActivityManager> am = IActivityManager::Probe(service);
    mResources->GetDimensionPixelSize(Elastos::Droid::Dialer::R::dimen::shortcut_icon_size, &mIconSize);
    if (mIconSize == 0) {
        am->GetLauncherLargeIconSize(&mIconSize);
    }
    am->GetLauncherLargeIconDensity(&mIconDensity);
    mResources->GetColor(Elastos::Droid::Dialer::R::color::shortcut_overlay_text_background, &mOverlayTextBackgroundColor);
}

void ShortcutIntentBuilder::CreateContactShortcutIntent(
    /* [in] */ IUri* contactUri)
{
    AutoPtr<ContactLoadingAsyncTask> task = new ContactLoadingAsyncTask(this, contactUri);
    AutoPtr<ArrayOf<IInterface*> > agrs = ArrayOf<IInterface*>::Alloc(0);
    task->Execute(agrs);
}

void ShortcutIntentBuilder::CreatePhoneNumberShortcutIntent(
    /* [in] */ IUri* contactUri,
    /* [in] */ const String& shortcutAction)
{
    AutoPtr<PhoneNumberLoadingAsyncTask> task = new PhoneNumberLoadingAsyncTask(this, contactUri, shortcutAction);
    AutoPtr<ArrayOf<IInterface*> > agrs = ArrayOf<IInterface*>::Alloc(0);
    task->Execute(agrs);
}

AutoPtr<IDrawable> ShortcutIntentBuilder::GetPhotoDrawable(
    /* [in] */ ArrayOf<Byte>* bitmapData,
    /* [in] */ const String& displayName,
    /* [in] */ const String& lookupKey)
{
    if (bitmapData != NULL) {
        AutoPtr<IBitmapFactory> factory;
        CBitmapFactory::AcquireSingleton((IBitmapFactory**)&factory);
        AutoPtr<IBitmap> bitmap;
        factory->DecodeByteArray(bitmapData, 0, bitmapData->GetLength(), NULL, (IBitmap**)&bitmap);
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        AutoPtr<IDrawable> drawable;
        CBitmapDrawable::New(res, bitmap, (IDrawable**)&drawable);
        return drawable;
    }
    else {
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        AutoPtr<ContactPhotoManager::DefaultImageRequest> request
                = new ContactPhotoManager::DefaultImageRequest(displayName, lookupKey, FALSE);
        return ContactPhotoManager::GetDefaultAvatarDrawableForContact(res, FALSE, request);
    }
}

void ShortcutIntentBuilder::CreateContactShortcutIntent(
    /* [in] */ IUri* contactUri,
    /* [in] */ const String& contentType,
    /* [in] */ const String& displayName,
    /* [in] */ const String& lookupKey,
    /* [in] */ ArrayOf<Byte>* bitmapData)
{
    AutoPtr<IDrawable> drawable = GetPhotoDrawable(bitmapData, displayName, lookupKey);

    AutoPtr<IIntent> shortcutIntent;
    CIntent::New(IContactsContractQuickContact::ACTION_QUICK_CONTACT, (IIntent**)&shortcutIntent);

    // When starting from the launcher, start in a new, cleared task.
    // CLEAR_WHEN_TASK_RESET cannot reset the root of a task, so we
    // clear the whole thing preemptively here since QuickContactActivity will
    // finish itself when launching other detail activities. We need to use
    // IIntent::FLAG_ACTIVITY_NO_ANIMATION since not all versions of launcher will respect
    // the INTENT_EXTRA_IGNORE_LAUNCH_ANIMATION intent extra.
    shortcutIntent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK | IIntent::FLAG_ACTIVITY_CLEAR_TASK
            | IIntent::FLAG_ACTIVITY_NO_ANIMATION);

    // Tell the launcher to not do its animation, because we are doing our own
    shortcutIntent->PutExtra(INTENT_EXTRA_IGNORE_LAUNCH_ANIMATION, TRUE);

    shortcutIntent->SetDataAndType(contactUri, contentType);
    shortcutIntent->PutExtra(IContactsContractQuickContact::EXTRA_EXCLUDE_MIMES, (ArrayOf<String>*)NULL);

    AutoPtr<IBitmap> icon = GenerateQuickContactIcon(drawable);

    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->PutExtra(IIntent::EXTRA_SHORTCUT_ICON, IParcelable::Probe(icon));
    intent->PutExtra(IIntent::EXTRA_SHORTCUT_INTENT, IParcelable::Probe(shortcutIntent));
    if (TextUtils::IsEmpty(displayName)) {
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        String missingName;
        res->GetString(Elastos::Droid::Dialer::R::string::missing_name, &missingName);
        intent->PutExtra(IIntent::EXTRA_SHORTCUT_NAME, missingName);
    }
    else {
        intent->PutExtra(IIntent::EXTRA_SHORTCUT_NAME, displayName);
    }

    mListener->OnShortcutIntentCreated(contactUri, intent);
}

void ShortcutIntentBuilder::CreatePhoneNumberShortcutIntent(
    /* [in] */ IUri* uri,
    /* [in] */ const String& displayName,
    /* [in] */ const String& lookupKey,
    /* [in] */ ArrayOf<Byte>* bitmapData,
    /* [in] */ const String& phoneNumber,
    /* [in] */ Int32 phoneType,
    /* [in] */ const String& phoneLabel,
    /* [in] */ const String& shortcutAction)
{
    AutoPtr<IDrawable> drawable = GetPhotoDrawable(bitmapData, displayName, lookupKey);

    AutoPtr<IBitmap> bitmap;
    AutoPtr<IUri> phoneUri;
    if (IIntent::ACTION_CALL.Equals(shortcutAction)) {
        // Make the URI a direct tel: URI so that it will always continue to work
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        helper->FromParts(IPhoneAccount::SCHEME_TEL, phoneNumber, String(NULL), (IUri**)&phoneUri);
        bitmap = GeneratePhoneNumberIcon(drawable, phoneType, phoneLabel,
                Elastos::Droid::Dialer::R::drawable::badge_action_call);
    }
    else {
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        helper->FromParts(ContactsUtils::SCHEME_SMSTO, phoneNumber, String(NULL), (IUri**)&phoneUri);
        bitmap = GeneratePhoneNumberIcon(drawable, phoneType, phoneLabel,
                Elastos::Droid::Dialer::R::drawable::badge_action_sms);
    }

    AutoPtr<IIntent> shortcutIntent;
    CIntent::New(shortcutAction, phoneUri, (IIntent**)&shortcutIntent);
    shortcutIntent->SetFlags(IIntent::FLAG_ACTIVITY_CLEAR_TOP);

    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->PutExtra(IIntent::EXTRA_SHORTCUT_ICON, IParcelable::Probe(bitmap));
    intent->PutExtra(IIntent::EXTRA_SHORTCUT_INTENT, IParcelable::Probe(shortcutIntent));
    intent->PutExtra(IIntent::EXTRA_SHORTCUT_NAME, displayName);

    mListener->OnShortcutIntentCreated(uri, intent);
}

AutoPtr<IBitmap> ShortcutIntentBuilder::GenerateQuickContactIcon(
    /* [in] */ IDrawable* photo)
{
    // Setup the drawing classes
    AutoPtr<IBitmapHelper> helper;
    CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
    AutoPtr<IBitmap> bitmap;
    helper->CreateBitmap(mIconSize, mIconSize, BitmapConfig_ARGB_8888, (IBitmap**)&bitmap);
    AutoPtr<ICanvas> canvas;
    CCanvas::New(bitmap, (ICanvas**)&canvas);

    // Copy in the photo
    AutoPtr<IRect> dst;
    CRect::New(0,0, mIconSize, mIconSize, (IRect**)&dst);
    photo->SetBounds(dst);
    photo->Draw(canvas);

    // Draw the icon with a rounded border
    assert(0);
    // RoundedBitmapDrawable roundedDrawable =
    //         RoundedBitmapDrawableFactory.create(mResources, bitmap);
    // roundedDrawable.setAntiAlias(true);
    // roundedDrawable.setCornerRadius(mIconSize / 2);
    AutoPtr<IBitmap> roundedBitmap;
    helper->CreateBitmap(mIconSize, mIconSize, BitmapConfig_ARGB_8888, (IBitmap**)&roundedBitmap);
    canvas->SetBitmap(roundedBitmap);
    // roundedDrawable.setBounds(dst);
    // roundedDrawable.draw(canvas);
    canvas->SetBitmap(NULL);

    return roundedBitmap;
}

AutoPtr<IBitmap> ShortcutIntentBuilder::GeneratePhoneNumberIcon(
    /* [in] */ IDrawable* photo,
    /* [in] */ Int32 phoneType,
    /* [in] */ const String& phoneLabel,
    /* [in] */ Int32 actionResId)
{
    AutoPtr<IResources> r;
    mContext->GetResources((IResources**)&r);
    AutoPtr<IDisplayMetrics> dm;
    r->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Float density;
    dm->GetDensity(&density);

    AutoPtr<IDrawable> drawable;
    r->GetDrawableForDensity(actionResId, mIconDensity, (IDrawable**)&drawable);
    AutoPtr<IBitmap> phoneIcon;
    IBitmapDrawable::Probe(drawable)->GetBitmap((IBitmap**)&phoneIcon);

    AutoPtr<IBitmap> icon = GenerateQuickContactIcon(photo);
    AutoPtr<ICanvas> canvas;
    CCanvas::New(icon, (ICanvas**)&canvas);

    // Copy in the photo
    AutoPtr<IPaint> photoPaint;
    CPaint::New((IPaint**)&photoPaint);
    photoPaint->SetDither(TRUE);
    photoPaint->SetFilterBitmap(TRUE);
    AutoPtr<IRect> dst;
    CRect::New(0, 0, mIconSize, mIconSize, (IRect**)&dst);

    // Create an overlay for the phone number type
    AutoPtr<IContactsContractCommonDataKindsPhone> phone;
    CContactsContractCommonDataKindsPhone::AcquireSingleton((IContactsContractCommonDataKindsPhone**)&phone);
    AutoPtr<ICharSequence> overlay;
    AutoPtr<ICharSequence> cs;
    CString::New(phoneLabel, (ICharSequence**)&cs);
    phone->GetTypeLabel(r, phoneType, cs, (ICharSequence**)&overlay);

    if (overlay != NULL) {
        AutoPtr<IPaint> textPaint;
        CTextPaint::New(IPaint::ANTI_ALIAS_FLAG | IPaint::DEV_KERN_TEXT_FLAG, (IPaint**)&textPaint);
        Float dimension;
        r->GetDimension(Elastos::Droid::Dialer::R::dimen::shortcut_overlay_text_size, &dimension);
        textPaint->SetTextSize(dimension);
        Int32 color;
        r->GetColor(Elastos::Droid::Dialer::R::color::textColorIconOverlay, &color);
        textPaint->SetColor(color);
        r->GetColor(Elastos::Droid::Dialer::R::color::textColorIconOverlayShadow, &color);
        textPaint->SetShadowLayer(4.0, 0, 2.0, color);

        AutoPtr<IPaintFontMetricsInt> fmi;
        textPaint->GetFontMetricsInt((IPaintFontMetricsInt**)&fmi);

        // First fill in a darker background around the text to be drawn
        AutoPtr<IPaint> workPaint;
        CPaint::New((IPaint**)&workPaint);
        workPaint->SetColor(mOverlayTextBackgroundColor);
        workPaint->SetStyle(PaintStyle_FILL);
        Int32 textPadding;
        r->GetDimensionPixelOffset(Elastos::Droid::Dialer::R::dimen::shortcut_overlay_text_background_padding, &textPadding);
        Int32 descent, ascent;
        fmi->GetDescent(&descent);
        fmi->GetAscent(&ascent);
        Int32 textBandHeight = (descent - ascent) + textPadding * 2;
        dst->Set(0, mIconSize - textBandHeight, mIconSize, mIconSize);
        canvas->DrawRect(dst, workPaint);

        overlay = TextUtils::Ellipsize(overlay, ITextPaint::Probe(textPaint), mIconSize, TextUtilsTruncateAt_END);
        Float textWidth;
        Int32 len;
        overlay->GetLength(&len);
        textPaint->MeasureText(overlay, 0, len, &textWidth);
        canvas->DrawText(overlay, 0, len, (mIconSize - textWidth) / 2,
                mIconSize - descent - textPadding, textPaint);
    }

    // Draw the phone action icon as an overlay
    Int32 width, height;
    phoneIcon->GetWidth(&width);
    phoneIcon->GetHeight(&height);
    AutoPtr<IRect> src;
    CRect::New(0, 0, width, height, (IRect**)&src);
    Int32 iconWidth;
    icon->GetWidth(&iconWidth);
    dst->Set(iconWidth - ((Int32)(20 * density)), -1, iconWidth, ((Int32)(19 * density)));
    canvas->DrawBitmap(phoneIcon, src, dst, photoPaint);

    canvas->SetBitmap(NULL);

    return icon;
}

} // List
} // Common
} // Contacts
} // Droid
} // Elastos
