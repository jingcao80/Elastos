
#include "elastos/droid/launcher2/HolographicViewHelper.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

HolographicViewHelper::HolographicViewHelper(
    /* [in] */ IContext* context)
{
    CCanvas::New((ICanvas**)&mTempCanvas);

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    mHighlightColor = res->GetColor(android.R.color.white);
}

void HolographicViewHelper::GeneratePressedFocusedStates(
    /* [in] */ IImageView* v)
{
    if (!mStatesUpdated && v != NULL) {
        mStatesUpdated = TRUE;
        AutoPtr<IBitmap> original = CreateOriginalImage(v, mTempCanvas);
        AutoPtr<IBitmap> outline = CreatePressImage(v, mTempCanvas);
        AutoPtr<FastBitmapDrawable> originalD = new FastBitmapDrawable(original);
        AutoPtr<FastBitmapDrawable> outlineD = new FastBitmapDrawable(outline);

        AutoPtr<IStateListDrawable> states;
        CStateListDrawable::New((IStateListDrawable**)&states);
        AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(1);
        (*array)[0] = android.R.attr.state_pressed;
        states->AddState(array, outlineD);
        AutoPtr<ArrayOf<Int32> > array2 = ArrayOf<Int32>::Alloc(1);
        (*array2)[0] = android.R.attr.state_focused;
        states->AddState(array2, outlineD);
        AutoPtr<ArrayOf<Int32> > array3 = ArrayOf<Int32>::Alloc(0);
        states->AddState(array3, originalD);

        v->SetImageDrawable(states);
    }
}

void HolographicViewHelper::InvalidatePressedFocusedStates(
    /* [in] */ IImageView* v)
{
    mStatesUpdated = FALSE;
    if (v != NULL) {
        v->Invalidate();
    }
}

AutoPtr<IBitmap> HolographicViewHelper::CreateOriginalImage(
    /* [in] */ IImageView* v,
    /* [in] */ ICanvas* canvas)
{
    AutoPtr<IDrawable> d;
    v->GetDrawable((IDrawable**)&d);

    AutoPtr<IBitmap> b;
    Int32 width;
    d->GetIntrinsicWidth(&width);
    Int32 height;
    d->GetIntrinsicHeight(&height);
    Bitmap::CreateBitmap(width, height, Bitmap::Config::ARGB_8888, (IBitmap**)&b);

    canvas->SetBitmap(b);
    canvas->Save();
    d->Draw(canvas);
    canvas->Restore();
    canvas->SetBitmap(NULL);
    return b;
}

AutoPtr<IBitmap> HolographicViewHelper::CreatePressImage(
    /* [in] */ IImageView* v,
    /* [in] */ ICanvas* canvas)
{
    AutoPtr<IDrawable> d;
    v->GetDrawable((IDrawable**)&d);

    AutoPtr<IBitmap> b;
    Int32 width;
    d->GetIntrinsicWidth(&width);
    Int32 height;
    d->GetIntrinsicHeight(&height);
    Bitmap::CreateBitmap(width height, Bitmap::Config::ARGB_8888, (IBitmap**)&b);

    canvas->SetBitmap(b);
    canvas->Save();
    d->Draw(canvas);
    canvas->Restore();
    canvas->DrawColor(mHighlightColor, PorterDuff.Mode.SRC_IN);
    canvas->SetBitmap(NULL);

    return b;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos