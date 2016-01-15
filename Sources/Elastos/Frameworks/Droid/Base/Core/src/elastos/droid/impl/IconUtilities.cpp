#include "elastos/droid/impl/IconUtilities.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/CCanvas.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CColorMatrix.h"
#include "elastos/droid/graphics/CBlurMaskFilter.h"
#include "elastos/droid/graphics/CBlurMaskFilter.h"
#include "elastos/droid/graphics/CPaintFlagsDrawFilter.h"
#include "elastos/droid/graphics/CTableMaskFilterHelper.h"
#include "elastos/droid/graphics/drawable/CBitmapDrawable.h"
#include "elastos/droid/graphics/drawable/CStateListDrawable.h"
#include "elastos/droid/graphics/drawable/CPaintDrawable.h"
#include "elastos/droid/utility/CTypedValue.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IColorMatrix;
using Elastos::Droid::Graphics::CColorMatrix;
using Elastos::Droid::Graphics::Drawable::CBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::IStateListDrawable;
using Elastos::Droid::Graphics::Drawable::CStateListDrawable;
using Elastos::Droid::Graphics::Drawable::IPaintDrawable;
using Elastos::Droid::Graphics::Drawable::CPaintDrawable;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::PorterDuffMode_CLEAR;
using Elastos::Droid::Graphics::BlurMaskFilterBlur_NORMAL;
using Elastos::Droid::Graphics::ITableMaskFilter;
using Elastos::Droid::Graphics::ITableMaskFilterHelper;
using Elastos::Droid::Graphics::CTableMaskFilterHelper;
using Elastos::Droid::Graphics::IBlurMaskFilter;
using Elastos::Droid::Graphics::CBlurMaskFilter;
using Elastos::Droid::Graphics::IPaintFlagsDrawFilter;
using Elastos::Droid::Graphics::CPaintFlagsDrawFilter;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesTheme;


namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {


const String IconUtilities::TAG("IconUtilities");
const Int32 IconUtilities::sColors[3] = { 0xffff0000, 0xff00ff00, 0xff0000ff };

IconUtilities::IconUtilities(
    /* [in] */ IContext* context)
    : mIconWidth(-1)
    , mIconHeight(-1)
    , mIconTextureWidth(-1)
    , mIconTextureHeight(-1)
    , mColorIndex(0)
{
    CPaint::New((IPaint**)&mPaint);
    CPaint::New((IPaint**)&mBlurPaint);
    CPaint::New((IPaint**)&mGlowColorPressedPaint);
    CPaint::New((IPaint**)&mGlowColorFocusedPaint);
    CRect::New((IRect**)&mOldBounds);
    CCanvas::New((ICanvas**)&mCanvas);

    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> metrics;
    resources->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    mDisplayMetrics = metrics;
    Float density, blurPx;
    metrics->GetDensity(&density);
    blurPx = 5 * density;

    Float ftmp;
    resources->GetDimension(R::dimen::app_icon_size, &ftmp);
    mIconWidth = mIconHeight = (Int32) ftmp;
    mIconTextureWidth = mIconTextureHeight = mIconWidth + (Int32)(blurPx*2);

    AutoPtr<IBlurMaskFilter> bmf;
    CBlurMaskFilter::New(blurPx, BlurMaskFilterBlur_NORMAL, (IBlurMaskFilter**)&bmf);
    mBlurPaint->SetMaskFilter(bmf);

    AutoPtr<IResourcesTheme> theme;
    context->GetTheme((IResourcesTheme**)&theme);

    AutoPtr<ITypedValue> value;
    CTypedValue::New((ITypedValue**)&value);
    Boolean pressed, focused;
    Int32 data;

    AutoPtr<ITableMaskFilterHelper> helper;
    CTableMaskFilterHelper::AcquireSingleton((ITableMaskFilterHelper**)&helper);
    AutoPtr<ITableMaskFilter> filter;
    helper->CreateClipTable(0, 30, (ITableMaskFilter**)&filter);

    theme->ResolveAttribute(R::attr::colorPressedHighlight, value, TRUE, &pressed);
    value->GetData(&data);
    mGlowColorPressedPaint->SetColor(pressed ? data: 0xffffc300);
    mGlowColorPressedPaint->SetMaskFilter(filter);

    theme->ResolveAttribute(R::attr::colorFocusedHighlight, value, TRUE, &focused);
    value->GetData(&data);
    mGlowColorFocusedPaint->SetColor(focused ? data : 0xffff8e00);
    mGlowColorFocusedPaint->SetMaskFilter(filter);

    AutoPtr<IColorMatrix> cm;
    CColorMatrix::New((IColorMatrix**)&cm);
    cm->SetSaturation(0.2f);

    AutoPtr<IPaintFlagsDrawFilter> pfdFilter;
    CPaintFlagsDrawFilter::New(IPaint::DITHER_FLAG,
        IPaint::FILTER_BITMAP_FLAG, (IPaintFlagsDrawFilter**)&pfdFilter);
    mCanvas->SetDrawFilter(pfdFilter);
}

AutoPtr<IDrawable> IconUtilities::CreateIconDrawable(
    /* [in] */ IDrawable* src)
{
    AutoPtr<IBitmap> scaled = CreateIconBitmap(src);

    AutoPtr<IStateListDrawable> result;
    CStateListDrawable::New((IStateListDrawable**)&result);

    AutoPtr<ArrayOf<Int32> > focusedArray = ArrayOf<Int32>::Alloc(1);
    focusedArray->Set(0, R::attr::state_focused);
    AutoPtr<ArrayOf<Int32> > pressedArray = ArrayOf<Int32>::Alloc(1);
    pressedArray->Set(0, R::attr::state_pressed);
    AutoPtr<ArrayOf<Int32> > scaledArray = ArrayOf<Int32>::Alloc(0);

    AutoPtr<IBitmap> pressedSB = CreateSelectedBitmap(scaled, FALSE);
    AutoPtr<IBitmapDrawable> pressedBD;
    CBitmapDrawable::New(pressedSB, (IBitmapDrawable**)&pressedBD);
    AutoPtr<IBitmap> focusedSB = CreateSelectedBitmap(scaled, TRUE);
    AutoPtr<IBitmapDrawable> focusedBD;
    CBitmapDrawable::New(focusedSB, (IBitmapDrawable**)&focusedBD);
    AutoPtr<IBitmapDrawable> scaledBD;
    CBitmapDrawable::New(scaled, (IBitmapDrawable**)&scaledBD);

    result->AddState(*focusedArray, pressedBD);
    result->AddState(*pressedArray, focusedBD);
    result->AddState(*scaledArray, scaledBD);

    result->SetBounds(0, 0, mIconTextureWidth, mIconTextureHeight);
    return result;
}

AutoPtr<IBitmap> IconUtilities::CreateIconBitmap(
    /* [in] */ IDrawable* icon)
{
    Int32 width = mIconWidth;
    Int32 height = mIconHeight;

    if (IPaintDrawable::Probe(icon) != NULL) {
        AutoPtr<IPaintDrawable> painter = IPaintDrawable::Probe(icon);
        painter->SetIntrinsicWidth(width);
        painter->SetIntrinsicHeight(height);
    }
    else if (IBitmapDrawable::Probe(icon)) {
        // Ensure the bitmap has a density.
        AutoPtr<IBitmapDrawable> bitmapDrawable = IBitmapDrawable::Probe(icon);
        AutoPtr<IBitmap> bitmap;
        bitmapDrawable->GetBitmap((IBitmap**)&bitmap);
        Int32 density;
        bitmap->GetDensity(&density);
        if (density == IBitmap::DENSITY_NONE) {
            bitmapDrawable->SetTargetDensity(mDisplayMetrics);
        }
    }

    Int32 sourceWidth, sourceHeight;
    icon->GetIntrinsicWidth(&sourceWidth);
    icon->GetIntrinsicHeight(&sourceHeight);

    if (sourceWidth > 0 && sourceHeight > 0) {
        // There are intrinsic sizes.
        if (width < sourceWidth || height < sourceHeight) {
            // It's too big, scale it down.
            Float ratio = (Float) sourceWidth / sourceHeight;
            if (sourceWidth > sourceHeight) {
                height = (Int32) (width / ratio);
            }
            else if (sourceHeight > sourceWidth) {
                width = (Int32) (height * ratio);
            }
        }
        else if (sourceWidth < width && sourceHeight < height) {
            // It's small, use the size they gave us.
            width = sourceWidth;
            height = sourceHeight;
        }
    }

    // no intrinsic size --> use default size
    Int32 textureWidth = mIconTextureWidth;
    Int32 textureHeight = mIconTextureHeight;

    AutoPtr<IBitmap> bitmap;
    CBitmap::CreateBitmap(textureWidth, textureHeight,
            BitmapConfig_ARGB_8888, (IBitmap**)&bitmap);
    AutoPtr<ICanvas> canvas = mCanvas;
    canvas->SetBitmap(bitmap);

    Int32 left = (textureWidth - width) / 2;
    Int32 top = (textureHeight - height) / 2;

    if (FALSE) {
        // draw a big box for the icon for debugging
        canvas->DrawColor(sColors[mColorIndex]);
        if (++mColorIndex >= 3) {
            mColorIndex = 0;
        }
        AutoPtr<IPaint> debugPaint;
        CPaint::New((IPaint**)&debugPaint);
        debugPaint->SetColor(0xffcccc00);
        canvas->DrawRect(left, top, left + width, top + height, debugPaint);
    }

    AutoPtr<IRect> bounds;
    icon->GetBounds((IRect**)&bounds);
    mOldBounds->Set(bounds);
    icon->SetBounds(left, top, left + width, top+height);
    icon->Draw(canvas);
    icon->SetBounds(mOldBounds);

    return bitmap;
}

AutoPtr<IBitmap> IconUtilities::CreateSelectedBitmap(
    /* [in] */ IBitmap* src,
    /* [in] */ Boolean pressed)
{
    assert(src != NULL);

    AutoPtr<IBitmap> result;
    CBitmap::CreateBitmap(mIconTextureWidth, mIconTextureHeight,
        BitmapConfig_ARGB_8888, (IBitmap**)&result);
    AutoPtr<ICanvas> dest;
    CCanvas::New(result, (ICanvas**)&dest);

    dest->DrawColor(0, PorterDuffMode_CLEAR);

    AutoPtr<ArrayOf<Int32> > xy = ArrayOf<Int32>::Alloc(2);
    xy->Set(0, 0);
    xy->Set(1, 0);

    AutoPtr<IBitmap> mask;
    src->ExtractAlpha(mBlurPaint, xy, (IBitmap**)&mask);

    dest->DrawBitmap(mask, (*xy)[0], (*xy)[1],
        pressed ? mGlowColorPressedPaint : mGlowColorFocusedPaint);

    mask->Recycle();

    dest->DrawBitmap(src, 0, 0, mPaint);
    dest->SetBitmap(NULL);

    return result;
}

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namepsace Droid
} // namespace Elastos
