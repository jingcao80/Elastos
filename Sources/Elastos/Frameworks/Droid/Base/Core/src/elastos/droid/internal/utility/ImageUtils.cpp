
#include "elastos/droid/internal/utility/ImageUtils.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/CCanvas.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/CMatrix.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_4444;
using Elastos::Droid::Graphics::PorterDuffMode_SRC;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

const Int32 ImageUtils::TOLERANCE = 20;
const Int32 ImageUtils::ALPHA_TOLERANCE = 50;
const Int32 ImageUtils::COMPACT_BITMAP_SIZE = 64; // pixels

AutoPtr<ArrayOf<Int32> > mTempBuffer;
AutoPtr<IBitmap> mTempCompactBitmap;
AutoPtr<ICanvas> mTempCompactBitmapCanvas;
AutoPtr<IPaint> mTempCompactBitmapPaint;
AutoPtr<IMatrix> mTempMatrix;

ImageUtils::ImageUtils()
{
    CMatrix::New((IMatrix**)&mTempMatrix);
}

Boolean ImageUtils::IsGrayscale(
    /* [in] */ IBitmap* bitmap)
{
    Int32 height;
    bitmap->GetHeight(&height);
    Int32 width;
    bitmap->GetWidth(&width);

    // shrink to a more manageable (yet hopefully no more or less colorful) size
    if (height > COMPACT_BITMAP_SIZE || width > COMPACT_BITMAP_SIZE) {
        if (mTempCompactBitmap == NULL) {
            CBitmap::CreateBitmap(COMPACT_BITMAP_SIZE, COMPACT_BITMAP_SIZE,
                BitmapConfig_ARGB_4444, (IBitmap**)&mTempCompactBitmap);
            CCanvas::New(mTempCompactBitmap, (ICanvas**)&mTempCompactBitmapCanvas);
            CPaint::New(IPaint::ANTI_ALIAS_FLAG, (IPaint**)&mTempCompactBitmapPaint);
            mTempCompactBitmapPaint->SetFilterBitmap(TRUE);
        }
        mTempMatrix->Reset();
        mTempMatrix->SetScale((Float)COMPACT_BITMAP_SIZE / width,
                (Float)COMPACT_BITMAP_SIZE / height, 0, 0);
        mTempCompactBitmapCanvas->DrawColor(0, PorterDuffMode_SRC); // select all, erase
        mTempCompactBitmapCanvas->DrawBitmap(bitmap, mTempMatrix, mTempCompactBitmapPaint);
        bitmap = mTempCompactBitmap;
        width = height = COMPACT_BITMAP_SIZE;
    }

    Int32 size = height * width;
    EnsureBufferSize(size);
    bitmap->GetPixels(mTempBuffer, 0, width, 0, 0, width, height);
    for (Int32 i = 0; i < size; i++) {
        if (!IsGrayscale((*mTempBuffer)[i])) {
            return FALSE;
        }
    }
    return TRUE;
}

void ImageUtils::EnsureBufferSize(
    /* [in] */ Int32 size)
{
    if (mTempBuffer == NULL || mTempBuffer->GetLength() < size) {
        mTempBuffer = ArrayOf<Int32>::Alloc(size);
    }
}

Boolean ImageUtils::IsGrayscale(
    /* [in] */ Int32 color)
{
    Int32 alpha = 0xFF & (color >> 24);
    if (alpha < ALPHA_TOLERANCE) {
        return TRUE;
    }

    Int32 r = 0xFF & (color >> 16);
    Int32 g = 0xFF & (color >> 8);
    Int32 b = 0xFF & color;

    return Elastos::Core::Math::Abs(r - g) < TOLERANCE
        && Elastos::Core::Math::Abs(r - b) < TOLERANCE
        && Elastos::Core::Math::Abs(g - b) < TOLERANCE;
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
