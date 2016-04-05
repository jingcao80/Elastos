#include "elastos/droid/launcher2/Utilities.h"
#include "Elastos.Droid.Service.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/AutoLock.h>
#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "R.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::PorterDuffMode_CLEAR;
using Elastos::Droid::Graphics::BlurMaskFilterBlur_NORMAL;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CColorMatrix;
using Elastos::Droid::Graphics::IColorMatrix;
using Elastos::Droid::Graphics::IColorFilter;
using Elastos::Droid::Graphics::IMaskFilter;
using Elastos::Droid::Graphics::IDrawFilter;
using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Graphics::IBlurMaskFilter;
using Elastos::Droid::Graphics::CBlurMaskFilter;
using Elastos::Droid::Graphics::CPaintFlagsDrawFilter;
using Elastos::Droid::Graphics::IPaintFlagsDrawFilter;
using Elastos::Droid::Graphics::IColorMatrixColorFilter;
using Elastos::Droid::Graphics::CColorMatrixColorFilter;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::CBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::IPaintDrawable;
using Elastos::Droid::Graphics::Drawable::IShapeDrawable;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::IRandom;
using Elastos::Utility::CRandom;


namespace Elastos {
namespace Droid {
namespace Launcher2 {

const String Utilities::TAG("Launcher.Utilities");

Int32 Utilities::sIconWidth = -1;
Int32 Utilities::sIconHeight = -1;
Int32 Utilities::sIconTextureWidth = -1;
Int32 Utilities::sIconTextureHeight = -1;

AutoPtr<IPaint> Utilities::sBlurPaint;
AutoPtr<IPaint> Utilities::sGlowColorPressedPaint;
AutoPtr<IPaint> Utilities::sGlowColorFocusedPaint;
AutoPtr<IPaint> Utilities::sDisabledPaint;
AutoPtr<IRect> Utilities::sOldBounds;
AutoPtr<ICanvas> Utilities::sCanvas;
Object Utilities::sCanvasLock;

AutoPtr<ArrayOf<Int32> > Utilities::sColors;

Boolean Utilities::InitStaticBlock()
{
    CPaint::New((IPaint**)&sBlurPaint);
    CPaint::New((IPaint**)&sGlowColorPressedPaint);
    CPaint::New((IPaint**)&sGlowColorFocusedPaint);
    CPaint::New((IPaint**)&sDisabledPaint);
    CRect::New((IRect**)&sOldBounds);
    CCanvas::New((ICanvas**)&sCanvas);

    AutoPtr<IPaintFlagsDrawFilter> filter;
    CPaintFlagsDrawFilter::New(IPaint::DITHER_FLAG, IPaint::FILTER_BITMAP_FLAG, (IPaintFlagsDrawFilter**)&filter);
    sCanvas->SetDrawFilter(IDrawFilter::Probe(filter));

    sColors = ArrayOf<Int32>::Alloc(3);
    (*sColors)[0] = 0xffff0000;
    (*sColors)[1] = 0xff00ff00;
    (*sColors)[2] = 0xff0000ff;

    return TRUE;
}

Boolean Utilities::initStaticBlock = InitStaticBlock();

Int32 Utilities::sColorIndex = 0;


AutoPtr<IBitmap> Utilities::CreateIconBitmap(
    /* [in] */ IBitmap* icon,
    /* [in] */ IContext* context)
{
    Int32 textureWidth = sIconTextureWidth;
    Int32 textureHeight = sIconTextureHeight;
    Int32 sourceWidth;
    icon->GetWidth(&sourceWidth);
    Int32 sourceHeight;
    icon->GetHeight(&sourceHeight);
    if (sourceWidth > textureWidth && sourceHeight > textureHeight) {
        // Icon is bigger than it should be; clip it (solves the GB->ICS migration case)
        AutoPtr<IBitmap> bitmap;
        AutoPtr<IBitmapHelper> helper;
        CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
        helper->CreateBitmap(icon,
                (sourceWidth - textureWidth) / 2,
                (sourceHeight - textureHeight) / 2,
                textureWidth, textureHeight, (IBitmap**)&bitmap);
        return bitmap;
    }
    else if (sourceWidth == textureWidth && sourceHeight == textureHeight) {
        // Icon is the right size, no need to change it
        return icon;
    }
    else {
        // Icon is too small, render to a larger bitmap
        AutoPtr<IResources> resources;
        context->GetResources((IResources**)&resources);
        AutoPtr<IBitmapDrawable> drawable;
        CBitmapDrawable::New(resources, icon, (IBitmapDrawable**)&drawable);
        return CreateIconBitmap(IDrawable::Probe(drawable), context);
    }
    return NULL;
}

AutoPtr<IBitmap> Utilities::CreateIconBitmap(
    /* [in] */ IDrawable* icon,
    /* [in] */ IContext* context)
{
    synchronized(sCanvasLock) { // we share the statics :-(
        if (sIconWidth == -1) {
            InitStatics(context);
        }

        Int32 width = sIconWidth;
        Int32 height = sIconHeight;

        if (IPaintDrawable::Probe(icon) != NULL) {
            AutoPtr<IPaintDrawable> painter = IPaintDrawable::Probe(icon);
            IShapeDrawable::Probe(painter)->SetIntrinsicWidth(width);
            IShapeDrawable::Probe(painter)->SetIntrinsicHeight(height);
        }
        else if (IBitmapDrawable::Probe(icon) != NULL) {
            // Ensure the bitmap has a density.
            AutoPtr<IBitmapDrawable> bitmapDrawable = IBitmapDrawable::Probe(icon);
            AutoPtr<IBitmap> bitmap;
            bitmapDrawable->GetBitmap((IBitmap**)&bitmap);
            Int32 density;
            bitmap->GetDensity(&density);
            if (density == IBitmap::DENSITY_NONE) {
                AutoPtr<IResources> resources;
                context->GetResources((IResources**)&resources);
                AutoPtr<IDisplayMetrics> metrics;
                resources->GetDisplayMetrics((IDisplayMetrics**)&metrics);
                bitmapDrawable->SetTargetDensity(metrics);
            }
        }
        Int32 sourceWidth;
        icon->GetIntrinsicWidth(&sourceWidth);
        Int32 sourceHeight;
        icon->GetIntrinsicHeight(&sourceHeight);
        if (sourceWidth > 0 && sourceHeight > 0) {
            // There are intrinsic sizes.
            if (width < sourceWidth || height < sourceHeight) {
                // It's too big, scale it down.
                const Float ratio = (Float) sourceWidth / sourceHeight;
                if (sourceWidth > sourceHeight) {
                    height = (Int32) (width / ratio);
                }
                else if (sourceHeight > sourceWidth) {
                    width = (Int32) (height * ratio);
                }
            }
            else if (sourceWidth < width && sourceHeight < height) {
                // Don't scale up the icon
                width = sourceWidth;
                height = sourceHeight;
            }
        }

        // no intrinsic size --> use default size
        Int32 textureWidth = sIconTextureWidth;
        Int32 textureHeight = sIconTextureHeight;

        AutoPtr<IBitmap> bitmap;
        AutoPtr<IBitmapHelper> helper;
        CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
        helper->CreateBitmap(textureWidth, textureHeight,
                BitmapConfig_ARGB_8888, (IBitmap**)&bitmap);
        const AutoPtr<ICanvas> canvas = sCanvas;
        canvas->SetBitmap(bitmap);

        const Int32 left = (textureWidth-width) / 2;
        const Int32 top = (textureHeight-height) / 2;

        //@SuppressWarnings("all") // suppress dead code warning
        const Boolean debug = FALSE;
        if (debug) {
            // draw a big box for the icon for debugging
            canvas->DrawColor((*sColors)[sColorIndex]);
            if (++sColorIndex >= sColors->GetLength()) sColorIndex = 0;
            AutoPtr<IPaint> debugPaint;
            CPaint::New((IPaint**)&debugPaint);
            debugPaint->SetColor(0xffcccc00);
            canvas->DrawRect(left, top, left+width, top+height, debugPaint);
        }

        AutoPtr<IRect> bounds;
        icon->GetBounds((IRect**)&bounds);
        sOldBounds->Set(bounds);
        icon->SetBounds(left, top, left+width, top+height);
        icon->Draw(canvas);
        icon->SetBounds(sOldBounds);
        canvas->SetBitmap(NULL);

        return bitmap;
    }
    return NULL;
}

ECode Utilities::DrawSelectedAllAppsBitmap(
    /* [in] */ ICanvas* dest,
    /* [in] */ Int32 destWidth,
    /* [in] */ Int32 destHeight,
    /* [in] */ Boolean pressed,
    /* [in] */ IBitmap* src)
{
    synchronized(sCanvasLock) { // we share the statics :-(
        if (sIconWidth == -1) {
            // We can't have gotten to here without src being initialized, which
            // comes from this file already.  So just assert.
            //initStatics(context);
            //throw new RuntimeException("Assertion failed: Utilities not initialized");
            Slogger::E(TAG, "Assertion failed: Utilities not initialized");
            return E_RUNTIME_EXCEPTION;
        }

        dest->DrawColor(0, PorterDuffMode_CLEAR);

        AutoPtr<ArrayOf<Int32> >  xy = ArrayOf<Int32>::Alloc(2);
        AutoPtr<IBitmap> mask;
        src->ExtractAlpha(sBlurPaint, xy, (IBitmap**)&mask);

        Int32 width;
        src->GetWidth(&width);
        Int32 height;
        src->GetHeight(&height);
        Float px = (destWidth - width) / 2;
        Float py = (destHeight - height) / 2;
        dest->DrawBitmap(mask, px + (*xy)[0], py + (*xy)[1],
                pressed ? sGlowColorPressedPaint : sGlowColorFocusedPaint);

        return mask->Recycle();
    }
    return NOERROR;
}

AutoPtr<IBitmap> Utilities::ResampleIconBitmap(
    /* [in] */ IBitmap* bitmap,
    /* [in] */ IContext* context)
{
    synchronized(sCanvasLock) { // we share the statics :-(
        if (sIconWidth == -1) {
            InitStatics(context);
        }

        Int32 width;
        bitmap->GetWidth(&width);
        Int32 height;
        bitmap->GetHeight(&height);
        if (width == sIconWidth && height == sIconHeight) {
            return bitmap;
        }
        else {
            AutoPtr<IResources> resources;
            context->GetResources((IResources**)&resources);
            AutoPtr<IBitmapDrawable> drawble;
            CBitmapDrawable::New(resources, bitmap, (IBitmapDrawable**)&drawble);
            return CreateIconBitmap(IBitmap::Probe(drawble), context);
        }
    }
    return NULL;
}

AutoPtr<IBitmap> Utilities::DrawDisabledBitmap(
    /* [in] */ IBitmap* bitmap,
    /* [in] */ IContext* context)
{
    synchronized (sCanvasLock) { // we share the statics :-(
        if (sIconWidth == -1) {
            InitStatics(context);
        }

        Int32 width;
        bitmap->GetWidth(&width);
        Int32 height;
        bitmap->GetHeight(&height);
        AutoPtr<IBitmap> disabled;
        AutoPtr<IBitmapHelper> helper;
        CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
        helper->CreateBitmap(width, height, BitmapConfig_ARGB_8888,
                (IBitmap**)&disabled);
        AutoPtr<ICanvas> canvas = sCanvas;
        canvas->SetBitmap(disabled);
        canvas->DrawBitmap(bitmap, 0.0f, 0.0f, sDisabledPaint);
        canvas->SetBitmap(NULL);
        return disabled;
    }
    return NULL;
}

void Utilities::InitStatics(
    /* [in] */ IContext* context)
{
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> metrics;
    resources->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    Float density;
    metrics->GetDensity(&density);

    Float dim;
    resources->GetDimension(Elastos::Droid::Launcher2::R::dimen::app_icon_size, &dim);
    sIconWidth = sIconHeight = (Int32)dim;
    sIconTextureWidth = sIconTextureHeight = sIconWidth;

    AutoPtr<IBlurMaskFilter> filter;
    CBlurMaskFilter::New(5 * density, BlurMaskFilterBlur_NORMAL, (IBlurMaskFilter**)&filter);
    sBlurPaint->SetMaskFilter(IMaskFilter::Probe(filter));
    sGlowColorPressedPaint->SetColor(0xffffc300);
    sGlowColorFocusedPaint->SetColor(0xffff8e00);

    AutoPtr<IColorMatrix> cm;
    CColorMatrix::New((IColorMatrix**)&cm);
    cm->SetSaturation(0.2f);
    AutoPtr<IColorMatrixColorFilter> colorFilter;
    CColorMatrixColorFilter::New(cm, (IColorMatrixColorFilter**)&colorFilter);
    sDisabledPaint->SetColorFilter(IColorFilter::Probe(colorFilter));
    sDisabledPaint->SetAlpha(0x88);
    return;
}

Int32 Utilities::RoundToPow2(
    /* [in] */ Int32 n)
{
    Int32 orig = n;
    n >>= 1;
    Int32 mask = 0x8000000;
    while (mask != 0 && (n & mask) == 0) {
        mask >>= 1;
    }
    while (mask != 0) {
        n |= mask;
        mask >>= 1;
    }
    n += 1;
    if (n != orig) {
        n <<= 1;
    }
    return n;
}

Int32 Utilities::GenerateRandomId()
{
    Int64 currentTimeMillis;
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&currentTimeMillis);

    AutoPtr<IRandom> ran;
    CRandom::New(currentTimeMillis, (IRandom**)&ran);
    Int32 id;
    ran->NextInt32(1 << 24, &id);
    return id;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos