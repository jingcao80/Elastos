
#include "elastos/droid/server/wm/Watermark.h"
#include "elastos/droid/server/wm/CWindowManagerService.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::ITypefaceHelper;
using Elastos::Droid::Graphics::CTypefaceHelper;
using Elastos::Droid::Graphics::ITypeface;
using Elastos::Droid::Graphics::IPaintFontMetricsInt;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::View::CSurface;
using Elastos::Droid::View::CSurfaceControl;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

Watermark::Watermark(
    /* [in] */ IDisplay* display,
    /* [in] */ IDisplayMetrics* dm,
    /* [in] */ ISurfaceSession* session,
    /* [in] */ ArrayOf<String>* tokens)
    : mDisplay(display)
    , mTextWidth(0)
    , mTextHeight(0)
    , mDeltaX(0)
    , mDeltaY(0)
    , mLastDW(0)
    , mLastDH(0)
    , mDrawNeeded(FALSE)
{
    CSurface::New((ISurface**)&mSurface);
    // if (false) {
    //     Log.i(WindowManagerService.TAG, "*********************** WATERMARK");
    //     for (Int32 i=0; i<tokens.length; i++) {
    //         Log.i(WindowManagerService.TAG, "  TOKEN #" + i + ": " + tokens[i]);
    //     }
    // }

    mTokens = tokens;

    AutoPtr<StringBuilder> builder = new StringBuilder(32);
    AutoPtr<ArrayOf<Char32> > charArray = (*mTokens)[0].GetChars();
    Int32 len = charArray->GetLength();
    len = len & ~1;
    Int32 c1, c2;
    for (Int32 i = 0; i < len; i += 2) {
        c1 = (*charArray)[i];
        c2 = (*charArray)[i + 1];
        if (c1 >= 'a' && c1 <= 'f') c1 = c1 - 'a' + 10;
        else if (c1 >= 'A' && c1 <= 'F') c1 = c1 - 'A' + 10;
        else c1 -= '0';
        if (c2 >= 'a' && c2 <= 'f') c2 = c2 - 'a' + 10;
        else if (c2 >= 'A' && c2 <= 'F') c2 = c2 - 'A' + 10;
        else c2 -= '0';
        builder->AppendChar((char)(255 - ((c1 * 16) + c2)));
    }
    builder->ToString(&mText);
    // if (false) {
    //     Log.i(WindowManagerService.TAG, "Final text: " + mText);
    // }

    Int32 fontSize = CWindowManagerService::GetPropertyInt(tokens, 1,
            ITypedValue::COMPLEX_UNIT_DIP, 20, dm);

    ASSERT_SUCCEEDED(CPaint::New(IPaint::ANTI_ALIAS_FLAG, (IPaint**)&mTextPaint));
    mTextPaint->SetTextSize(fontSize);
    AutoPtr<ITypefaceHelper> tHelper;
    CTypefaceHelper::AcquireSingleton((ITypefaceHelper**)&tHelper);
    AutoPtr<ITypeface> t;
    assert(0);
    // todo: should get Typeface::SANS_SERIF via CTypefaceHelper
    // tHelper->Create(ITypeface::SANS_SERIF, ITypeface::BOLD, (ITypeface**)&t);
    mTextPaint->SetTypeface(t);

    AutoPtr<IPaintFontMetricsInt> fm;
    mTextPaint->GetFontMetricsInt((IPaintFontMetricsInt**)&fm);
    mTextPaint->MeasureText(mText, (Float*)&mTextWidth);
    Int32 descent, ascent;
    fm->GetDescent(&descent);
    fm->GetAscent(&ascent);
    mTextHeight = descent - ascent;

    mDeltaX = CWindowManagerService::GetPropertyInt(tokens, 2,
            ITypedValue::COMPLEX_UNIT_PX, mTextWidth * 2, dm);
    mDeltaY = CWindowManagerService::GetPropertyInt(tokens, 3,
            ITypedValue::COMPLEX_UNIT_PX, mTextHeight*3, dm);
    Int32 shadowColor = CWindowManagerService::GetPropertyInt(tokens, 4,
            ITypedValue::COMPLEX_UNIT_PX, 0xb0000000, dm);
    Int32 color = CWindowManagerService::GetPropertyInt(tokens, 5,
            ITypedValue::COMPLEX_UNIT_PX, 0x60ffffff, dm);
    Int32 shadowRadius = CWindowManagerService::GetPropertyInt(tokens, 6,
            ITypedValue::COMPLEX_UNIT_PX, 7, dm);
    Int32 shadowDx = CWindowManagerService::GetPropertyInt(tokens, 8,
            ITypedValue::COMPLEX_UNIT_PX, 0, dm);
    Int32 shadowDy = CWindowManagerService::GetPropertyInt(tokens, 9,
            ITypedValue::COMPLEX_UNIT_PX, 0, dm);

    mTextPaint->SetColor(color);
    mTextPaint->SetShadowLayer(shadowRadius, shadowDx, shadowDy, shadowColor);

    AutoPtr<ISurfaceControl> ctrl;
    // try {
    CSurfaceControl::New(session, String("WatermarkSurface"),
            1, 1, IPixelFormat::TRANSLUCENT, ISurfaceControl::HIDDEN, (ISurfaceControl**)&ctrl);
    Int32 stack;
    mDisplay->GetLayerStack(&stack);
    ctrl->SetLayerStack(stack);
    ctrl->SetLayer(CWindowManagerService::TYPE_LAYER_MULTIPLIER * 100);
    ctrl->SetPosition(0, 0);
    ctrl->Show();
    mSurface->CopyFrom(ctrl);
    // } catch (OutOfResourcesException e) {
    // }
    mSurfaceControl = ctrl;
}

void Watermark::PositionSurface(
    /* [in] */ Int32 dw,
    /* [in] */ Int32 dh)
{
    if (mLastDW != dw || mLastDH != dh) {
        mLastDW = dw;
        mLastDH = dh;
        mSurfaceControl->SetSize(dw, dh);
        mDrawNeeded = TRUE;
    }
}

void Watermark::DrawIfNeeded()
{
    if (mDrawNeeded) {
        Int32 dw = mLastDW;
        Int32 dh = mLastDH;

        mDrawNeeded = FALSE;
        AutoPtr<IRect> dirty;
        CRect::New(0, 0, dw, dh, (IRect**)&dirty);
        AutoPtr<ICanvas> c;
        // try {
        mSurface->LockCanvas(dirty, (ICanvas**)&c);
        // } catch (IllegalArgumentException e) {
        // } catch (OutOfResourcesException e) {
        // }
        if (c != NULL) {
            c->DrawColor(0, Elastos::Droid::Graphics::PorterDuffMode_CLEAR);

            Int32 deltaX = mDeltaX;
            Int32 deltaY = mDeltaY;

            // deltaX shouldn't be close to a round fraction of our
            // x step, or else things will line up too much.
            Int32 div = (dw + mTextWidth) / deltaX;
            Int32 rem = (dw + mTextWidth) - (div * deltaX);
            Int32 qdelta = deltaX / 4;
            if (rem < qdelta || rem > (deltaX - qdelta)) {
                deltaX += deltaX / 3;
            }

            Int32 y = -mTextHeight;
            Int32 x = -mTextWidth;
            while (y < (dh + mTextHeight)) {
                c->DrawText(mText, x, y, mTextPaint);
                x += deltaX;
                if (x >= dw) {
                    x -= (dw + mTextWidth);
                    y += deltaY;
                }
            }
            mSurface->UnlockCanvasAndPost(c);
        }
    }
}


} // Wm
} // Server
} // Droid
} // Elastos
