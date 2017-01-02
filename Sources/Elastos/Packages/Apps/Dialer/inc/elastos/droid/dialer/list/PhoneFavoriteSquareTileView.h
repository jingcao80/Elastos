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

#ifndef __ELASTOS_DROID_DIALER_LIST_PHONEFAVORITESQUARETITLEVIEW_H__
#define __ELASTOS_DROID_DIALER_LIST_PHONEFAVORITESQUARETITLEVIEW_H__

#include "elastos/droid/dialer/list/PhoneFavoriteTileView.h"
#include "elastos/droid/contacts/common/list/ContactEntry.h"

using Elastos::Droid::Contacts::Common::List::ContactEntry;
using Elastos::Droid::Widget::IImageButton;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

/**
 * Displays the contact's picture overlaid with their name and number type in a tile.
 */
class PhoneFavoriteSquareTileView
    : public PhoneFavoriteTileView
    , public IPhoneFavoriteSquareTileView
{
private:
    class SecondaryButtonOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        SecondaryButtonOnClickListener(
            /* [in] */ PhoneFavoriteSquareTileView* host);

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        PhoneFavoriteSquareTileView* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    // @Override
    CARAPI LoadFromContact(
        /* [in] */ IInterface* entry);

    CARAPI GetContactEntry(
        /* [out] */ IInterface** entry);

protected:
    // @Override
    CARAPI OnFinishInflate();

    // @Override
    CARAPI_(Int32) GetApproximateImageSize();

    // @Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);
private:
    CARAPI_(void) LaunchQuickContact();

private:
    static const String TAG;

    Float mHeightToWidthRatio;

    AutoPtr<IImageButton> mSecondaryButton;

    AutoPtr<ContactEntry> mContactEntry;
};

} // List
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_LIST_PHONEFAVORITESQUARETITLEVIEW_H__
