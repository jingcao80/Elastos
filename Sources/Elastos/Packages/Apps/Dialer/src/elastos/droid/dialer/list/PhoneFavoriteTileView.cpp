
#include "elastos/droid/dialer/list/PhoneFavoriteTileView.h"
#include <elastos/droid/text/TextUtils.h>
#include "R.h"

using Elastos::Droid::Content::IClipDataHelper;
using Elastos::Droid::Content::CClipDataHelper;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::IDragShadowBuilder;
using Elastos::Droid::View::CDragShadowBuilder;

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
    VALIDATE_NOT_NULL(result);

    AutoPtr<IPhoneFavoriteTileView> view = IPhoneFavoriteTileView::Probe(v);
    // NOTE The drag shadow is handled in the ListView.
    AutoPtr<IDragShadowBuilder> builder;
    CDragShadowBuilder::New((IDragShadowBuilder**)&builder);
    view->StartDrag(EMPTY_CLIP_DATA, builder, DRAG_PHONE_FAVORITE_TILE, 0);
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
        return;
    }
    if (TextUtils::IsEmpty(mHost->mPhoneNumberString)) {
        // Copy "superclass" implementation
        AutoPtr<IUri> uri;
        GetLookupUri((IUri**)&uri);
        assert(0 && "TODO");
        // mHost->mListener->OnContactSelected(uri,
        //         MoreContactUtils::GetTargetRectFromView(this));
    }
    else {
        // When you tap a frequently-called contact, you want to
        // call them at the number that you usually talk to them
        // at (i.e. the one displayed in the UI), regardless of
        // whether that's their default number.
        mListener->OnCallNumberDirectly(mPhoneNumberString);
    }

    return NOERROR;
}

//=================================================================
// PhoneFavoriteTileView
//=================================================================
AutoPtr<IClipData> CreateClipData()
{
    AutoPtr<IClipDataHelper> helper;
    CClipDataHelper::AcquireSingleton((IClipDataHelper**)&helper);
    AutoPtr<IClipData> result;
    helper->NewPlainText(String(""), String(""), (IClipData**)&result);
    return result;
}

const String PhoneFavoriteTileView::DRAG_PHONE_FAVORITE_TILE("PHONE_FAVORITE_TILE");

const String PhoneFavoriteTileView::TAG("PhoneFavoriteTileView");
const Boolean PhoneFavoriteTileView::DEBUG = FALSE;

const Float PhoneFavoriteTileView::DEFAULT_IMAGE_LETTER_OFFSET = -0.12;
const Float PhoneFavoriteTileView::DEFAULT_IMAGE_LETTER_SCALE = 0.70;

const AutoPtr<IClipData> PhoneFavoriteTileView::EMPTY_CLIP_DATA = CreateClipData();

CAR_INTERFACE_IMPL(PhoneFavoriteTileView, /*ContactTileView*/FrameLayout, IPhoneFavoriteTileView)

ECode PhoneFavoriteTileView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    assert(0 && "TODO");
    return NOERROR;
    // return ContactTileView::constructor(context, attrs);
}

ECode PhoneFavoriteTileView::OnFinishInflate()
{
    assert(0 && "TODO");
    // ContactTileView::OnFinishInflate();
    FindViewById(R::id::shadow_overlay, (IView**)&mShadowOverlay);

    SetOnLongClickListener(
            (IViewOnLongClickListener*)new MyLongClickListener(this));
    return NOERROR;
}

// ECode PhoneFavoriteTileView::LoadFromContact(
//     /* [in] */ IContactEntry* entry)
// {
//     ContactTileView::LoadFromContact(entry);
//     // Set phone number to null in case we're reusing the view.
//     mPhoneNumberString = NULL;
//     if (entry != NULL) {
//         // Grab the phone-number to call directly. See {@link onClick()}.
//         mPhoneNumberString = ((CContactEntry*)entry)->mPhoneNumber;

//         // If this is a blank entry, don't show anything.
//         // TODO krelease: Just hide the view for now. For this to truly look like an empty row
//         // the entire ContactTileRow needs to be hidden.
//         if (entry == ContactEntry::BLANK_ENTRY) {
//             SetVisibility(IView::INVISIBLE);
//         }
//         else {
//             AutoPtr<IView> starIcon;
//             FindViewById(R::id::contact_star_icon, (IView**)&starIcon);
//             starIcon->SetVisibility(
//                     ((CContactEntry*)entry)->mIsFavorite ? IView::VISIBLE : IView::GONE);
//             SetVisibility(IView::VISIBLE);
//         }
//     }
//     return NOERROR;
// }

ECode PhoneFavoriteTileView::IsDarkTheme(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode PhoneFavoriteTileView::CreateClickListener(
    /* [out] */ IViewOnClickListener** listener)
{
    VALIDATE_NOT_NULL(listener);
    AutoPtr<MyClickListener> clickListener = new MyClickListener(this);
    *listener = (IViewOnClickListener*)clickListener.Get();
    REFCOUNT_ADD(*listener)
    return NOERROR;
}

// ECode PhoneFavoriteTileView:: GetDefaultImageRequest(
//     /* [in] */ const String& displayName,
//     /* [in] */ const String& lookupKey,
//     /* [out] */ IDefaultImageRequest** request)
// {
//     VALIDATE_NOT_NULL(request);
//     return CDefaultImageRequest::New(displayName, lookupKey,
//                 IContactPhotoManager::TYPE_DEFAULT, DEFAULT_IMAGE_LETTER_SCALE,
//                 DEFAULT_IMAGE_LETTER_OFFSET, FALSE, request);
// }

ECode PhoneFavoriteTileView::ConfigureViewForImage(
    /* [in] */ Boolean isDefaultImage)
{
    // Hide the shadow overlay if the image is a default image (i.e. colored letter tile)
    if (mShadowOverlay != NULL) {
        mShadowOverlay->SetVisibility(isDefaultImage ? IView::GONE : IView::VISIBLE);
    }

    return NOERROR;
}

ECode PhoneFavoriteTileView::IsContactPhotoCircular(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // Unlike Contacts' tiles, the Dialer's favorites tiles are square.
    *result = FALSE;
    return NOERROR;
}

} // List
} // Dialer
} // Droid
} // Elastos
