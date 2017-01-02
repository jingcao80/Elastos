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

#include "elastos/droid/dialer/list/PhoneFavoriteTileView.h"
#include "elastos/droid/contacts/common/list/ContactEntry.h"
#include "elastos/droid/contacts/common/ContactPhotoManager.h"
#include "elastos/droid/contacts/common/MoreContactUtils.h"
#include <elastos/droid/text/TextUtils.h>
#include "R.h"

using Elastos::Droid::Contacts::Common::MoreContactUtils;
using Elastos::Droid::Contacts::Common::ContactPhotoManager;
using Elastos::Droid::Contacts::Common::List::ContactEntry;
using Elastos::Droid::Content::IClipDataHelper;
using Elastos::Droid::Content::CClipDataHelper;
using Elastos::Droid::Dialer::List::EIID_IPhoneFavoriteTileView;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::EIID_IViewOnLongClickListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::IDragShadowBuilder;
using Elastos::Droid::View::CDragShadowBuilder;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

//=================================================================
// PhoneFavoriteTileView::MyLongClickListener
//=================================================================
CAR_INTERFACE_IMPL(PhoneFavoriteTileView::MyLongClickListener, Object, IViewOnLongClickListener);

PhoneFavoriteTileView::MyLongClickListener::MyLongClickListener(
    /* [in] */ PhoneFavoriteTileView* host)
    : mHost(host)
{}

ECode PhoneFavoriteTileView::MyLongClickListener::OnLongClick(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IPhoneFavoriteTileView> view = IPhoneFavoriteTileView::Probe(v);
    // NOTE The drag shadow is handled in the ListView.
    AutoPtr<IDragShadowBuilder> builder;
    CDragShadowBuilder::New((IDragShadowBuilder**)&builder);
    AutoPtr<ICharSequence> cs;
    CString::New(DRAG_PHONE_FAVORITE_TILE, (ICharSequence**)&cs);
    Boolean ret;
    IView::Probe(view)->StartDrag(EMPTY_CLIP_DATA, builder, cs, 0, &ret);
    *result = TRUE;
    return NOERROR;
}


//=================================================================
// PhoneFavoriteTileView::MyClickListener
//=================================================================
CAR_INTERFACE_IMPL(PhoneFavoriteTileView::MyClickListener, Object, IViewOnClickListener);

PhoneFavoriteTileView::MyClickListener::MyClickListener(
    /* [in] */ PhoneFavoriteTileView* host)
    : mHost(host)
{}

ECode PhoneFavoriteTileView::MyClickListener::OnClick(
    /* [in] */ IView* v)
{
    if (mHost->mListener == NULL) {
        return NOERROR;
    }
    if (TextUtils::IsEmpty(mHost->mPhoneNumberString)) {
        // Copy "superclass" implementation
        AutoPtr<IUri> uri;
        mHost->GetLookupUri((IUri**)&uri);
        mHost->mListener->OnContactSelected(uri,
                MoreContactUtils::GetTargetRectFromView(IView::Probe(mHost)));
    }
    else {
        // When you tap a frequently-called contact, you want to
        // call them at the number that you usually talk to them
        // at (i.e. the one displayed in the UI), regardless of
        // whether that's their default number.
        mHost->mListener->OnCallNumberDirectly(mHost->mPhoneNumberString);
    }
    return NOERROR;
}

//=================================================================
// PhoneFavoriteTileView
//=================================================================
const String PhoneFavoriteTileView::DRAG_PHONE_FAVORITE_TILE("PHONE_FAVORITE_TILE");
const String PhoneFavoriteTileView::TAG("PhoneFavoriteTileView");
const Boolean PhoneFavoriteTileView::DEBUG;
const Float PhoneFavoriteTileView::DEFAULT_IMAGE_LETTER_OFFSET;
const Float PhoneFavoriteTileView::DEFAULT_IMAGE_LETTER_SCALE;

static AutoPtr<IClipData> CreateClipData()
{
    AutoPtr<IClipDataHelper> helper;
    CClipDataHelper::AcquireSingleton((IClipDataHelper**)&helper);
    AutoPtr<ICharSequence> cs1, cs2;;
    CString::New(String(""), (ICharSequence**)&cs1);
    CString::New(String(""), (ICharSequence**)&cs2);
    AutoPtr<IClipData> result;
    helper->NewPlainText(cs1, cs2, (IClipData**)&result);
    return result;
}
const AutoPtr<IClipData> PhoneFavoriteTileView::EMPTY_CLIP_DATA = CreateClipData();

CAR_INTERFACE_IMPL(PhoneFavoriteTileView, ContactTileView, IPhoneFavoriteTileView)

ECode PhoneFavoriteTileView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return ContactTileView::constructor(context, attrs);
}

ECode PhoneFavoriteTileView::OnFinishInflate()
{
    ContactTileView::OnFinishInflate();
    mShadowOverlay = NULL;
    FindViewById(R::id::shadow_overlay, (IView**)&mShadowOverlay);
    AutoPtr<IViewOnLongClickListener> listener = (IViewOnLongClickListener*)new MyLongClickListener(this);
    SetOnLongClickListener(listener);
    return NOERROR;
}

ECode PhoneFavoriteTileView::LoadFromContact(
    /* [in] */ IInterface* entry)
{
    ContactTileView::LoadFromContact(entry);
    // Set phone number to null in case we're reusing the view.
    mPhoneNumberString = NULL;
    if (entry != NULL) {
        // Grab the phone-number to call directly. See {@link onClick()}.
        AutoPtr<ContactEntry> entryObj = (ContactEntry*)(IObject*)entry;
        mPhoneNumberString = entryObj->mPhoneNumber;

        // If this is a blank entry, don't show anything.
        // TODO krelease: Just hide the view for now. For this to truly look like an empty row
        // the entire ContactTileRow needs to be hidden.
        if (entry == (IObject*)ContactEntry::BLANK_ENTRY) {
            SetVisibility(IView::INVISIBLE);
        }
        else {
            AutoPtr<IView> starIcon;
            FindViewById(R::id::contact_star_icon, (IView**)&starIcon);
            starIcon->SetVisibility(
                    entryObj->mIsFavorite ? IView::VISIBLE : IView::GONE);
            SetVisibility(IView::VISIBLE);
        }
    }
    return NOERROR;
}

Boolean PhoneFavoriteTileView::IsDarkTheme()
{
    return FALSE;
}

AutoPtr<IViewOnClickListener> PhoneFavoriteTileView::CreateClickListener()
{
    AutoPtr<IViewOnClickListener> listener = (IViewOnClickListener*)new MyClickListener(this);
    return listener;
}

AutoPtr<IContactPhotoManagerDefaultImageProvider> PhoneFavoriteTileView:: GetDefaultImageRequest(
    /* [in] */ const String& displayName,
    /* [in] */ const String& lookupKey)
{
    AutoPtr<IContactPhotoManagerDefaultImageProvider> request
            = (IContactPhotoManagerDefaultImageProvider*)new ContactPhotoManager::DefaultImageRequest(
                    displayName, lookupKey,
                    ContactPhotoManager::TYPE_DEFAULT, DEFAULT_IMAGE_LETTER_SCALE,
                    DEFAULT_IMAGE_LETTER_OFFSET, FALSE);
    return request;
}

void PhoneFavoriteTileView::ConfigureViewForImage(
    /* [in] */ Boolean isDefaultImage)
{
    // Hide the shadow overlay if the image is a default image (i.e. colored letter tile)
    if (mShadowOverlay != NULL) {
        mShadowOverlay->SetVisibility(isDefaultImage ? IView::GONE : IView::VISIBLE);
    }
}

Boolean PhoneFavoriteTileView::IsContactPhotoCircular()
{
    // Unlike Contacts' tiles, the Dialer's favorites tiles are square.
    return FALSE;
}

} // List
} // Dialer
} // Droid
} // Elastos
