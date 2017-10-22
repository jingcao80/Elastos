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

#ifndef __ELASTOS_DROID_WIDGET_QUICKCONTACTBADGE_H__
#define __ELASTOS_DROID_WIDGET_QUICKCONTACTBADGE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/ImageView.h"
#include "elastos/droid/content/AsyncQueryHandler.h"

using Elastos::Droid::Content::AsyncQueryHandler;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::View::IViewOnClickListener;

namespace Elastos {
namespace Droid {
namespace Widget {

class QuickContactBadge
    : public ImageView
    , public IQuickContactBadge
    , public IViewOnClickListener
{
private:
    class QueryHandler : public AsyncQueryHandler
    {
    public:
        QueryHandler(
            /* [in] */ IContentResolver* cr,
            /* [in] */ QuickContactBadge* host);

    protected:
        CARAPI OnQueryComplete(
            /* [in] */ Int32 token,
            /* [in] */ IInterface* cookie,
            /* [in] */ ICursor* cursor);

    private:
        QuickContactBadge* mHost;
    };

    class QuickContactBadgeClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        QuickContactBadgeClickListener(
            /* [in] */ QuickContactBadge* host);

        virtual CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        QuickContactBadge* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    virtual CARAPI SetMode(
        /* [in] */ Int32 size);

    virtual CARAPI SetImageToDefault();

    virtual CARAPI AssignContactUri(
        /* [in] */ IUri* contactUri);

    virtual CARAPI AssignContactFromEmail(
        /* [in] */ const String& emailAddress,
        /* [in] */ Boolean lazyLookup);

    /**
     * Assign a contact based on an email address. This should only be used when
     * the contact's URI is not available, as an extra query will have to be
     * performed to lookup the URI based on the email.

     @param emailAddress The email address of the contact.
     @param lazyLookup If this is true, the lookup query will not be performed
     until this view is clicked.
     @param extras A bundle of extras to populate the contact edit page with if the contact
     is not found and the user chooses to add the email address to an existing contact or
     create a new contact. Uses the same string constants as those found in
     {@link android.provider.ContactsContract.Intents.Insert}
    */
    CARAPI AssignContactFromEmail(
        /* [in] */ const String& emailAddress,
        /* [in] */ Boolean lazyLookup,
        /* [in] */ IBundle* extras);

    virtual CARAPI AssignContactFromPhone(
        /* [in] */ const String& phoneNumber,
        /* [in] */ Boolean lazyLookup);

    /**
     * Assign a contact based on a phone number. This should only be used when
     * the contact's URI is not available, as an extra query will have to be
     * performed to lookup the URI based on the phone number.
     *
     * @param phoneNumber The phone number of the contact.
     * @param lazyLookup If this is true, the lookup query will not be performed
     * until this view is clicked.
     * @param extras A bundle of extras to populate the contact edit page with if the contact
     * is not found and the user chooses to add the phone number to an existing contact or
     * create a new contact. Uses the same string constants as those found in
     * {@link android.provider.ContactsContract.Intents.Insert}
     */
    CARAPI AssignContactFromPhone(
        /* [in] */ const String& phoneNumber,
        /* [in] */ Boolean lazyLookup,
        /* [in] */ IBundle* extras);

    /**
     * Assigns the drawable that is to be drawn on top of the assigned contact photo.
     *
     * @param overlay Drawable to be drawn over the assigned contact photo. Must have a non-zero
     *         instrinsic width and height.
     */
    CARAPI SetOverlay(
        /* [in] */ IDrawable* overlay);

    virtual CARAPI OnClick(
        /* [in] */ IView* v);

    virtual CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    virtual CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    virtual CARAPI SetExcludeMimes(
        /* [in] */ ArrayOf<String>* excludeMimes);

    // @Override
    CARAPI DrawableHotspotChanged(
        /* [in] */ Float x,
        /* [in] */ Float y);

protected:
    CARAPI DrawableStateChanged();

    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

private:
    CARAPI_(Boolean) IsAssigned();

    CARAPI_(void) OnContactUriChanged();

    static CARAPI_(Boolean) InitStatic();

private:
    AutoPtr<IUri> mContactUri;
    String mContactEmail;
    String mContactPhone;
    AutoPtr<IDrawable> mOverlay;
    AutoPtr<QueryHandler> mQueryHandler;
    AutoPtr<IDrawable> mDefaultAvatar;
    AutoPtr<IBundle> mExtras;

protected:
    AutoPtr<ArrayOf<String> > mExcludeMimes;

public:
    static const Int32 TOKEN_EMAIL_LOOKUP;
    static const Int32 TOKEN_PHONE_LOOKUP;
    static const Int32 TOKEN_EMAIL_LOOKUP_AND_TRIGGER;
    static const Int32 TOKEN_PHONE_LOOKUP_AND_TRIGGER;
    static const String EXTRA_URI_CONTENT;

    static Boolean sInit;
    static AutoPtr<ArrayOf<String> > EMAIL_LOOKUP_PROJECTION;
    static const Int32 EMAIL_ID_COLUMN_INDEX;
    static const Int32 EMAIL_LOOKUP_STRING_COLUMN_INDEX;

    static AutoPtr<ArrayOf<String> > PHONE_LOOKUP_PROJECTION;
    static const Int32 PHONE_ID_COLUMN_INDEX;
    static const Int32 PHONE_LOOKUP_STRING_COLUMN_INDEX;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_QUICKCONTACTBADGE_H__
