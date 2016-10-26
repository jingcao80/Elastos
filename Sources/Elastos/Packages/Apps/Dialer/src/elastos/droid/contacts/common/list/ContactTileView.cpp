
#include "elastos/droid/contacts/common/list/ContactTileView.h"
#include "elastos/droid/contacts/common/list/ContactEntry.h"
#include "elastos/droid/contacts/common/MoreContactUtils.h"
#include "elastos/droid/contacts/common/ContactPhotoManager.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::Contacts::Common::MoreContactUtils;
using Elastos::Droid::Contacts::Common::ContactPhotoManager;
using Elastos::Droid::Contacts::Common::List::EIID_IContactTileView;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

//=================================================================
// ContactTileView::MyOnClickListener
//=================================================================
CAR_INTERFACE_IMPL(ContactTileView::MyOnClickListener, Object, IViewOnClickListener)

ECode ContactTileView::MyOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    if (mHost->mListener == NULL) return NOERROR;
    AutoPtr<IUri> uri;
    mHost->GetLookupUri((IUri**)&uri);
    return mHost->mListener->OnContactSelected(
            uri, MoreContactUtils::GetTargetRectFromView(IView::Probe(mHost)));
}


//=================================================================
// ContactTileView
//=================================================================
const String ContactTileView::TAG("ContactTileView");

CAR_INTERFACE_IMPL(ContactTileView, FrameLayout, IContactTileView)

ECode ContactTileView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return FrameLayout::constructor(context, attrs);
}

ECode ContactTileView::OnFinishInflate()
{
    FrameLayout::OnFinishInflate();
    AutoPtr<IView> name;
    FindViewById(Elastos::Droid::Dialer::R::id::contact_tile_name, (IView**)&name);
    mName = ITextView::Probe(name);

    AutoPtr<IView> contact;
    FindViewById(Elastos::Droid::Dialer::R::id::contact_tile_quick, (IView**)&contact);
    mQuickContact = IQuickContactBadge::Probe(contact);
    AutoPtr<IView> photo;
    FindViewById(Elastos::Droid::Dialer::R::id::contact_tile_image, (IView**)&photo);
    mPhoto = IImageView::Probe(photo);
    AutoPtr<IView> status;
    FindViewById(Elastos::Droid::Dialer::R::id::contact_tile_status, (IView**)&status);
    mStatus = ITextView::Probe(status);
    AutoPtr<IView> label;
    FindViewById(Elastos::Droid::Dialer::R::id::contact_tile_phone_type, (IView**)&label);
    mPhoneLabel = ITextView::Probe(label);
    AutoPtr<IView> number;
    FindViewById(Elastos::Droid::Dialer::R::id::contact_tile_phone_number, (IView**)&number);
    mPhoneNumber = ITextView::Probe(number);
    mPushState = NULL;
    FindViewById(Elastos::Droid::Dialer::R::id::contact_tile_push_state, (IView**)&mPushState);
    mHorizontalDivider = NULL;
    FindViewById(Elastos::Droid::Dialer::R::id::contact_tile_horizontal_divider, (IView**)&mHorizontalDivider);

    AutoPtr<IViewOnClickListener> listener = CreateClickListener();
    SetOnClickListener(listener);

    return NOERROR;
}

AutoPtr<IViewOnClickListener> ContactTileView::CreateClickListener()
{
    AutoPtr<IViewOnClickListener> listener = (IViewOnClickListener*)new MyOnClickListener(this);
    return listener;
}

ECode ContactTileView::SetPhotoManager(
    /* [in] */ IContactPhotoManager* photoManager)
{
    mPhotoManager = photoManager;
    return NOERROR;
}

ECode ContactTileView::LoadFromContact(
    /* [in] */ IInterface* entry)
{
    if (entry != NULL) {
        AutoPtr<ContactEntry> entryObj = (ContactEntry*)(IObject*)entry;
        AutoPtr<ICharSequence> name;
        CString::New(GetNameForView(entryObj->mName), (ICharSequence**)&name);
        mName->SetText(name);
        mLookupUri = entryObj->mLookupUri;

        if (mStatus != NULL) {
            if (entryObj->mStatus.IsNull()) {
                IView::Probe(mStatus)->SetVisibility(IView::GONE);
            }
            else {
                AutoPtr<ICharSequence> cs;
                CString::New(entryObj->mStatus, (ICharSequence**)&cs);
                mStatus->SetText(cs);
                mStatus->SetCompoundDrawablesWithIntrinsicBounds(entryObj->mPresenceIcon,
                        NULL, NULL, NULL);
                IView::Probe(mStatus)->SetVisibility(IView::VISIBLE);
            }
        }

        if (mPhoneLabel != NULL) {
            if (TextUtils::IsEmpty(entryObj->mPhoneLabel)) {
                IView::Probe(mPhoneLabel)->SetVisibility(IView::GONE);
            }
            else {
                IView::Probe(mPhoneLabel)->SetVisibility(IView::VISIBLE);
                AutoPtr<ICharSequence> cs;
                CString::New(entryObj->mPhoneLabel, (ICharSequence**)&cs);
                mPhoneLabel->SetText(cs);
            }
        }

        if (mPhoneNumber != NULL) {
            // TODO: Format number correctly
            AutoPtr<ICharSequence> cs;
            CString::New(entryObj->mPhoneNumber, (ICharSequence**)&cs);
            mPhoneNumber->SetText(cs);
        }

        SetVisibility(IView::VISIBLE);

        if (mPhotoManager != NULL) {
            AutoPtr<IContactPhotoManagerDefaultImageRequest> request
                    = GetDefaultImageRequest(entryObj->mName, entryObj->mLookupKey);
            ConfigureViewForImage(entryObj->mPhotoUri == NULL);
            if (mPhoto != NULL) {
                mPhotoManager->LoadPhoto(mPhoto, entryObj->mPhotoUri, GetApproximateImageSize(),
                        IsDarkTheme(), IsContactPhotoCircular(), request);

                if (mQuickContact != NULL) {
                    mQuickContact->AssignContactUri(mLookupUri);
                }
            }
            else if (mQuickContact != NULL) {
                mQuickContact->AssignContactUri(mLookupUri);
                mPhotoManager->LoadPhoto(IImageView::Probe(mQuickContact), entryObj->mPhotoUri,
                        GetApproximateImageSize(), IsDarkTheme(), IsContactPhotoCircular(),
                        request);
            }
        }
        else {
            Logger::W(TAG, "contactPhotoManager not set");
        }

        if (mPushState != NULL) {
            AutoPtr<ICharSequence> cs;
            CString::New(entryObj->mName, (ICharSequence**)&cs);
            mPushState->SetContentDescription(cs);
        }
        else if (mQuickContact != NULL) {
            AutoPtr<ICharSequence> cs;
            CString::New(entryObj->mName, (ICharSequence**)&cs);
            IView::Probe(mQuickContact)->SetContentDescription(cs);
        }
    }
    else {
        SetVisibility(IView::INVISIBLE);
    }

    return NOERROR;
}

ECode ContactTileView::SetListener(
    /* [in] */ IContactTileViewListener* listener)
{
    mListener = listener;
    return NOERROR;
}

ECode ContactTileView::SetHorizontalDividerVisibility(
    /* [in] */ Int32 visibility)
{
    if (mHorizontalDivider != NULL)
        IView::Probe(mHorizontalDivider)->SetVisibility(visibility);
    return NOERROR;
}

ECode ContactTileView::GetLookupUri(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri)
    *uri = mLookupUri;
    REFCOUNT_ADD(*uri)
    return NOERROR;
}

AutoPtr<IQuickContactBadge> ContactTileView::GetQuickContact()
{
    return mQuickContact;
}

AutoPtr<IView> ContactTileView::GetPhotoView()
{
    return IView::Probe(mPhoto);
}

String ContactTileView::GetNameForView(
    /* [in] */ const String& name)
{
    return name;
}

void ContactTileView::ConfigureViewForImage(
    /* [in] */ Boolean isDefaultImage)
{
    // No-op by default.
}

AutoPtr<IContactPhotoManagerDefaultImageRequest> ContactTileView::GetDefaultImageRequest(
    /* [in] */ const String& displayName,
    /* [in] */ const String& lookupKey)
{
    AutoPtr<ContactPhotoManager::DefaultImageRequest> request
            = new ContactPhotoManager::DefaultImageRequest(displayName, lookupKey, IsContactPhotoCircular());
    return (IContactPhotoManagerDefaultImageRequest*)request;
}

Boolean ContactTileView::IsContactPhotoCircular()
{
    return TRUE;
}

} // List
} // Common
} // Contacts
} // Droid
} // Elastos
