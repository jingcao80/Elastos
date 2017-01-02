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

#ifndef __ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTTILEVIEW_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTTILEVIEW_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Widget.h"
#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/widget/FrameLayout.h"

using Elastos::Droid::Contacts::Common::IContactPhotoManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IQuickContactBadge;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

/**
 * A ContactTile displays a contact's picture and name
 */
class ContactTileView
    : public FrameLayout
    , public IContactTileView
{
private:
    class MyOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        MyOnClickListener(
            /* [in] */ ContactTileView* host)
            : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        ContactTileView* mHost;
    };

public:
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CAR_INTERFACE_DECL()

    CARAPI SetPhotoManager(
        /* [in] */ IContactPhotoManager* photoManager);

    /**
     * Populates the data members to be displayed from the
     * fields in {@link com.android.contacts.common.list.ContactEntry}
     */
    CARAPI LoadFromContact(
        /* [in] */ IInterface* entry);

    CARAPI SetListener(
        /* [in] */ IContactTileViewListener* listener);

    CARAPI SetHorizontalDividerVisibility(
        /* [in] */ Int32 visibility);

    CARAPI GetLookupUri(
        /* [out] */ IUri** uri);

protected:
    CARAPI OnFinishInflate();

    CARAPI_(AutoPtr<IViewOnClickListener>) CreateClickListener();

    CARAPI_(AutoPtr<IQuickContactBadge>) GetQuickContact();

    CARAPI_(AutoPtr<IView>) GetPhotoView();

    /**
     * Returns the string that should actually be displayed as the contact's name. Subclasses
     * can override this to return formatted versions of the name - i.e. first name only.
     */
    CARAPI_(String) GetNameForView(
        /* [in] */ const String& name);

    /**
     * Implemented by subclasses to estimate the size of the picture. This can return -1 if only
     * a thumbnail is shown anyway
     */
    virtual CARAPI_(Int32) GetApproximateImageSize() = 0;

    virtual CARAPI_(Boolean) IsDarkTheme() = 0;

    /**
     * Implemented by subclasses to reconfigure the view's layout and subviews, based on whether
     * or not the contact has a user-defined photo.
     *
     * @param isDefaultImage True if the contact does not have a user-defined contact photo
     * (which means a default contact image will be applied by the {@link ContactPhotoManager}
     */
    CARAPI_(void) ConfigureViewForImage(
        /* [in] */ Boolean isDefaultImage);

    /**
     * Implemented by subclasses to allow them to return a {@link DefaultImageRequest} with the
     * various image parameters defined to match their own layouts.
     *
     * @param displayName The display name of the contact
     * @param lookupKey The lookup key of the contact
     * @return A {@link DefaultImageRequest} object with each field configured by the subclass
     * as desired, or {@code null}.
     */
    CARAPI_(AutoPtr<IContactPhotoManagerDefaultImageRequest>) GetDefaultImageRequest(
        /* [in] */ const String& displayName,
        /* [in] */ const String& lookupKey);

    /**
     * Whether contact photo should be displayed as a circular image. Implemented by subclasses
     * so they can change which drawables to fetch.
     */
    CARAPI_(Boolean) IsContactPhotoCircular();

protected:
    AutoPtr<IContactTileViewListener> mListener;

private:
    static const String TAG;

    AutoPtr<IUri> mLookupUri;
    AutoPtr<IImageView> mPhoto;
    AutoPtr<IQuickContactBadge> mQuickContact;
    AutoPtr<ITextView> mName;
    AutoPtr<ITextView> mStatus;
    AutoPtr<ITextView> mPhoneLabel;
    AutoPtr<ITextView> mPhoneNumber;
    AutoPtr<IContactPhotoManager> mPhotoManager;
    AutoPtr<IView> mPushState;
    AutoPtr<IView> mHorizontalDivider;
};

} // List
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTTILEVIEW_H__
