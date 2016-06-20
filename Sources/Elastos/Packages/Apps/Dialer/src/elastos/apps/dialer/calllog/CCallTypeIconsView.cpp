
#include "elastos/apps/dialer/calllog/CCallTypeIconsView.h"
#include "R.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/core/CoreUtils.h"
#include "elastos/core/Math.h"

using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::CBitmapDrawable;
using Elastos::Droid::Provider::ICalls;
using Elastos::Core::CoreUtils;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace CallLog {

//=================================================================
// CCallTypeIconsView::Resources
//=================================================================
CCallTypeIconsView::Resources::Resources(
    /* [in] */ IContext* context)
{
    AutoPtr<IResources> r;
    context->GetResources((IResources**)&r);

    r->GetDrawable(R::drawable::ic_call_arrow, (IDrawable**)&mIncoming);
    Int32 color;
    r->GetColor(R::color::answered_call, &color);
    mIncoming->SetColorFilter(color, Elastos::Droid::Graphics::PorterDuffMode_MULTIPLY);

    // Create a rotated instance of the call arrow for outgoing ICalls::
    assert(0 && "TODO");
    // mOutgoing = BitmapUtil::GetRotatedDrawable(r, R::drawable::ic_call_arrow, 180);
    mOutgoing->SetColorFilter(color, Elastos::Droid::Graphics::PorterDuffMode_MULTIPLY);

    // Need to make a copy of the arrow drawable, otherwise the same instance colored
    // above will be recolored here.
    r->GetDrawable(R::drawable::ic_call_arrow, (IDrawable**)&mMissed);
    mMissed->Mutate();
    r->GetColor(R::color::missed_call, &color);
    mMissed->SetColorFilter(color, Elastos::Droid::Graphics::PorterDuffMode_MULTIPLY);

    r->GetDrawable(R::drawable::ic_call_voicemail_holo_dark, (IDrawable**)&mVoicemail);

    // Get the video call icon, scaled to match the height of the call arrows.
    // We want the video call icon to be the same height as the call arrows, while keeping
    // the same width aspect ratio.
    AutoPtr<IBitmapFactory> bitmapFactory;
    CBitmapFactory::AcquireSingleton((IBitmapFactory**)&bitmapFactory);
    AutoPtr<IBitmap> videoIcon;
    bitmapFactory->DecodeResource(r,
            R::drawable::ic_videocam_wht_24dp, (IBitmap**)&videoIcon);
    Int32 scaledHeight;
    mMissed->GetIntrinsicHeight(&scaledHeight);
    Int32 width, height;
    videoIcon->GetWidth(&width);
    videoIcon->GetHeight(&height);
    Int32 scaledWidth = (Int32) ((Float) width *
            ((Float) scaledHeight / (Float) height));

    AutoPtr<IBitmapHelper> bitmapHelper;
    CBitmapHelper::AcquireSingleton((IBitmapHelper**)&bitmapHelper);
    AutoPtr<IBitmap> scaled;
    bitmapHelper->CreateScaledBitmap(videoIcon,
            scaledWidth, scaledHeight, FALSE, (IBitmap**)&scaled);
    AutoPtr<IBitmapDrawable> bitmapDrawable;
    CBitmapDrawable::New(r, scaled, (IBitmapDrawable**)&bitmapDrawable);
    mVideoCall = IDrawable::Probe(bitmapDrawable);
    r->GetColor(R::color::dialtacts_secondary_text_color, &color);
    mVideoCall->SetColorFilter(color, Elastos::Droid::Graphics::PorterDuffMode_MULTIPLY);

    r->GetDimensionPixelSize(R::dimen::call_log_icon_margin, &mIconMargin);
}

//=================================================================
// CCallTypeIconsView
//=================================================================
CAR_INTERFACE_IMPL(CCallTypeIconsView, View, ICallTypeIconsView);

CAR_OBJECT_IMPL(CCallTypeIconsView);

CCallTypeIconsView::CCallTypeIconsView()
    : mShowVideo(FALSE)
{
    AutoPtr<IArrayList> list;
    CArrayList::New(3, (IArrayList**)&list);
    mCallTypes = IList::Probe(list);
}

ECode CCallTypeIconsView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CCallTypeIconsView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    View::constructor(context, attrs);
    mResources = new Resources(context);
    return NOERROR;
}

ECode CCallTypeIconsView::Clear()
{
    mCallTypes->Clear();
    mWidth = 0;
    mHeight = 0;
    Invalidate();

    return NOERROR;
}

ECode CCallTypeIconsView::Add(
    /* [in] */ Int32 callType)
{
    mCallTypes->Add(CoreUtils::Convert(callType));

    AutoPtr<IDrawable> drawable = GetCallTypeDrawable(callType);
    Int32 width;
    drawable->GetIntrinsicWidth(&width);
    mWidth += width + mResources->mIconMargin;
    Int32 height;
    drawable->GetIntrinsicHeight(&height);
    mHeight = Elastos::Core::Math::Max(mHeight, height);
    Invalidate();

    return NOERROR;
}

ECode CCallTypeIconsView::SetShowVideo(
    /* [in] */ Boolean showVideo)
{
    mShowVideo = showVideo;
    if (showVideo) {
        Int32 width;
        mResources->mVideoCall->GetIntrinsicWidth(&width);
        mWidth += width;
        Int32 height;
        mResources->mVideoCall->GetIntrinsicHeight(&height);
        mHeight = Elastos::Core::Math::Max(mHeight, height);
        Invalidate();
    }

    return NOERROR;
}

ECode CCallTypeIconsView::IsVideoShown(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mShowVideo;
    return NOERROR;
}

ECode CCallTypeIconsView::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    mCallTypes->GetSize(count);
    return NOERROR;
}

ECode CCallTypeIconsView::GetCallType(
    /* [in] */ Int32 index,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> type;
    mCallTypes->Get(index, (IInterface**)&type);
    *result = CoreUtils::Unbox(IInteger32::Probe(type));
    return NOERROR;
}

AutoPtr<IDrawable> CCallTypeIconsView::GetCallTypeDrawable(
    /* [in] */ Int32 callType)
{
    switch (callType) {
        case ICalls::INCOMING_TYPE:
            return mResources->mIncoming;
        case ICalls::OUTGOING_TYPE:
            return mResources->mOutgoing;
        case ICalls::MISSED_TYPE:
            return mResources->mMissed;
        case ICalls::VOICEMAIL_TYPE:
            return mResources->mVoicemail;
        default:
            // It is possible for users to end up with calls with unknown call types in their
            // call history, possibly due to 3rd party call log implementations (e.g. to
            // distinguish between rejected and missed calls). Instead of crashing, just
            // assume that all unknown call types are missed ICalls::
            return mResources->mMissed;
    }
}

void CCallTypeIconsView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    SetMeasuredDimension(mWidth, mHeight);
}

void CCallTypeIconsView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    Int32 left = 0;
    AutoPtr<IIterator> it;
    mCallTypes->GetIterator((IIterator**)&it);
    Boolean hasNext = FALSE;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> callType;
        it->GetNext((IInterface**)&callType);
        AutoPtr<IDrawable> drawable = GetCallTypeDrawable(
                CoreUtils::Unbox(IInteger32::Probe(callType)));
        Int32 width, height;
        drawable->GetIntrinsicWidth(&width);
        Int32 right = left + width;
        drawable->GetIntrinsicHeight(&height);
        drawable->SetBounds(left, 0, right, height);
        drawable->Draw(canvas);
        left = right + mResources->mIconMargin;
    }

    // If showing the video call icon, draw it scaled appropriately.
    if (mShowVideo) {
        AutoPtr<IDrawable> drawable = mResources->mVideoCall;
        Int32 width, height;
        mResources->mVideoCall->GetIntrinsicWidth(&width);
        Int32 right = left + width;
        mResources->mVideoCall->GetIntrinsicHeight(&height);
        drawable->SetBounds(left, 0, right, height);
        drawable->Draw(canvas);
    }
}

} // CallLog
} // Dialer
} // Apps
} // Elastos
