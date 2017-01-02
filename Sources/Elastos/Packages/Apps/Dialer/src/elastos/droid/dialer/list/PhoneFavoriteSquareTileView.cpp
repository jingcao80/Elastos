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

#include "elastos/droid/dialer/list/PhoneFavoriteSquareTileView.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/view/View.h"
#include "R.h"

using Elastos::Droid::Dialer::List::EIID_IPhoneFavoriteSquareTileView;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Provider::IContactsContractQuickContact;
using Elastos::Droid::Provider::CContactsContractQuickContact;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::View;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {


//=================================================================
// PhoneFavoriteSquareTileView::SecondaryButtonOnClickListener
//=================================================================
CAR_INTERFACE_IMPL(PhoneFavoriteSquareTileView::SecondaryButtonOnClickListener, Object, IViewOnClickListener);

PhoneFavoriteSquareTileView::SecondaryButtonOnClickListener::SecondaryButtonOnClickListener(
    /* [in] */ PhoneFavoriteSquareTileView* host)
    : mHost(host)
{}

ECode PhoneFavoriteSquareTileView::SecondaryButtonOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    mHost->LaunchQuickContact();
    return NOERROR;
}


//=================================================================
// PhoneFavoriteSquareTileView
//=================================================================
CAR_INTERFACE_IMPL(PhoneFavoriteSquareTileView, PhoneFavoriteTileView, IPhoneFavoriteSquareTileView);

const String PhoneFavoriteSquareTileView::TAG("PhoneFavoriteSquareTileView");

ECode PhoneFavoriteSquareTileView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    PhoneFavoriteTileView::constructor(context, attrs);

    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    resources->GetFraction(Elastos::Droid::Dialer::R::dimen::contact_tile_height_to_width_ratio,
            1, 1, &mHeightToWidthRatio);

    return NOERROR;
}

ECode PhoneFavoriteSquareTileView::OnFinishInflate()
{
    PhoneFavoriteTileView::OnFinishInflate();
    AutoPtr<IView> nameView;
    FindViewById(Elastos::Droid::Dialer::R::id::contact_tile_name, (IView**)&nameView);
    ITextView::Probe(nameView)->SetElegantTextHeight(FALSE);
    AutoPtr<IView> phoneTypeView;
    FindViewById(Elastos::Droid::Dialer::R::id::contact_tile_phone_type, (IView**)&phoneTypeView);
    ITextView::Probe(phoneTypeView)->SetElegantTextHeight(FALSE);

    AutoPtr<IView> button;
    FindViewById(Elastos::Droid::Dialer::R::id::contact_tile_secondary_button, (IView**)&button);
    mSecondaryButton = IImageButton::Probe(button);

    return NOERROR;
}

Int32 PhoneFavoriteSquareTileView::GetApproximateImageSize()
{
    // The picture is the full size of the tile (minus some padding, but we can be generous)
    Int32 width;
    GetWidth(&width);
    return width;
}

void PhoneFavoriteSquareTileView::LaunchQuickContact()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IUri> uri;
    GetLookupUri((IUri**)&uri);

    AutoPtr<IContactsContractQuickContact> contact;
    CContactsContractQuickContact::AcquireSingleton(
            (IContactsContractQuickContact**)&contact);
    contact->ShowQuickContact(context, this,
                uri, IContactsContractQuickContact::MODE_LARGE, NULL);
}

ECode PhoneFavoriteSquareTileView::LoadFromContact(
    /* [in] */ IInterface* entry)
{
    PhoneFavoriteTileView::LoadFromContact(entry);
    if (entry != NULL) {
        AutoPtr<IViewOnClickListener> listener = (IViewOnClickListener*)new SecondaryButtonOnClickListener(this);
        IView::Probe(mSecondaryButton)->SetOnClickListener(listener);
    }
    mContactEntry = (ContactEntry*)(IObject*)entry;

    return NOERROR;
}

ECode PhoneFavoriteSquareTileView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 width = Elastos::Droid::View::View::MeasureSpec::GetSize(widthMeasureSpec);
    Int32 height = (Int32)(mHeightToWidthRatio * width);
    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        child->Measure(
                View::MeasureSpec::MakeMeasureSpec(width, View::MeasureSpec::EXACTLY),
                View::MeasureSpec::MakeMeasureSpec(height, View::MeasureSpec::EXACTLY));
    }
    SetMeasuredDimension(width, height);

    return NOERROR;
}

ECode PhoneFavoriteSquareTileView::GetContactEntry(
    /* [out] */ IInterface** entry)
{
    VALIDATE_NOT_NULL(entry)
    *entry = (IObject*)mContactEntry;
    REFCOUNT_ADD(*entry)
    return NOERROR;
}

} // List
} // Dialer
} // Droid
} // Elastos
