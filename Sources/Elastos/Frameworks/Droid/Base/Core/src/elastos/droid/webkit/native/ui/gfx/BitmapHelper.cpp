
#include "elastos/droid/content/res/CResourcesHelper.h"
#include "elastos/droid/graphics/CBitmapFactory.h"
#include "elastos/droid/graphics/CBitmapFactoryOptions.h"
#include "elastos/droid/graphics/CBitmapHelper.h"
#include "elastos/droid/webkit/native/ui/gfx/BitmapHelper.h"
#include "elastos/droid/webkit/native/ui/gfx/BitmapFormat.h"
#include "elastos/droid/webkit/native/ui/api/BitmapHelper_dec.h"
#include "elastos/core/Math.h"

using Elastos::Droid::Content::Res::CResourcesHelper;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesHelper;
using Elastos::Droid::Graphics::BitmapConfig_ALPHA_8;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_4444;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::BitmapConfig_RGB_565;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::CBitmapFactoryOptions;
using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::IBitmapFactoryOptions;
using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Webkit::Ui::Gfx::BitmapFormat;
using Elastos::Core::Math;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Ui {
namespace Gfx {

//=====================================================================
//                             BitmapHelper
//=====================================================================
AutoPtr<IInterface> BitmapHelper::CreateBitmap(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 bitmapFormatValue)
{
    // ==================before translated======================
    // Bitmap.Config bitmapConfig = getBitmapConfigForFormat(bitmapFormatValue);
    // return Bitmap.createBitmap(width, height, bitmapConfig);

    BitmapConfig bitmapConfig = GetBitmapConfigForFormat(bitmapFormatValue);
    AutoPtr<IBitmapHelper> helper;
    CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
    AutoPtr<IBitmap> bitmap;
    helper->CreateBitmap(width, height, bitmapConfig, (IBitmap**)&bitmap);
    return TO_IINTERFACE(bitmap);
}

AutoPtr<IInterface> BitmapHelper::DecodeDrawableResource(
    /* [in] */ const String& name,
    /* [in] */ Int32 reqWidth,
    /* [in] */ Int32 reqHeight)
{
    // ==================before translated======================
    // Resources res = Resources.getSystem();
    // int resId = res.getIdentifier(name, null, null);
    // if (resId == 0) return null;
    //
    // final BitmapFactory.Options options = new BitmapFactory.Options();
    // options.inJustDecodeBounds = true;
    // BitmapFactory.decodeResource(res, resId, options);
    //
    // options.inSampleSize = calculateInSampleSize(options, reqWidth, reqHeight);
    // options.inJustDecodeBounds = false;
    // options.inPreferredConfig = Bitmap.Config.ARGB_8888;
    // return BitmapFactory.decodeResource(res, resId, options);

    AutoPtr<IResources> res;
    AutoPtr<IResourcesHelper> helper;
    CResourcesHelper::AcquireSingleton((IResourcesHelper**)&helper);
    helper->GetSystem((IResources**)&res);

    Int32 resId;
    res->GetIdentifier(name, String(""), String(""), &resId);
    if (resId == 0)
        return NULL;

    AutoPtr<IBitmapFactoryOptions> options;
    CBitmapFactoryOptions::New((IBitmapFactoryOptions**)&options);

    options->SetInJustDecodeBounds(TRUE);
    AutoPtr<IBitmapFactory> bitmapFactory;
    CBitmapFactory::AcquireSingleton((IBitmapFactory**)&bitmapFactory);

    AutoPtr<IBitmap> bitmap;
    bitmapFactory->DecodeResource(res, resId, options, (IBitmap**)&bitmap);

    options->SetInSampleSize(CalculateInSampleSize(options, reqWidth, reqHeight));
    options->SetInJustDecodeBounds(FALSE);
    options->SetInPreferredConfig(BitmapConfig_ARGB_8888);

    AutoPtr<IBitmap> result;
    bitmapFactory->DecodeResource(res, resId, options, (IBitmap**)&result);
    return TO_IINTERFACE(result);
}

Int32 BitmapHelper::CalculateInSampleSize(
    /* [in] */ IBitmapFactoryOptions* options,
    /* [in] */ Int32 reqWidth,
    /* [in] */ Int32 reqHeight)
{
    // ==================before translated======================
    // // Raw height and width of image
    // final int height = options.outHeight;
    // final int width = options.outWidth;
    // int inSampleSize = 1;
    //
    // if (height > reqHeight || width > reqWidth) {
    //
    //     // Calculate ratios of height and width to requested height and width
    //     final int heightRatio = Math.round((float) height / (float) reqHeight);
    //     final int widthRatio = Math.round((float) width / (float) reqWidth);
    //
    //     // Choose the smallest ratio as inSampleSize value, this will guarantee
    //     // a final image with both dimensions larger than or equal to the
    //     // requested height and width.
    //     inSampleSize = heightRatio < widthRatio ? heightRatio : widthRatio;
    // }
    //
    // return inSampleSize;

    assert(0);
    Int32 height = 0;
    options->GetOutHeight(&height);
    Int32 width = 0;
    options->GetOutWidth(&width);

    Int32 inSampleSize = 1;
    if (height > reqHeight || width > reqWidth) {
        const Int32 heightRatio = Elastos::Core::Math::Round((float)((float) height / (float) reqHeight));
        const Int32 widthRatio = Elastos::Core::Math::Round((float)((float) width / (float) reqWidth));
        inSampleSize = heightRatio < widthRatio ? heightRatio : widthRatio;
    }
    return inSampleSize;
}

Int32 BitmapHelper::GetBitmapFormatForConfig(
    /* [in] */ BitmapConfig bitmapConfig)
{
    // ==================before translated======================
    // switch (bitmapConfig) {
    //     case ALPHA_8:
    //         return BitmapFormat.FORMAT_ALPHA_8;
    //     case ARGB_4444:
    //         return BitmapFormat.FORMAT_ARGB_4444;
    //     case ARGB_8888:
    //         return BitmapFormat.FORMAT_ARGB_8888;
    //     case RGB_565:
    //         return BitmapFormat.FORMAT_RGB_565;
    //     default:
    //         return BitmapFormat.FORMAT_NO_CONFIG;
    // }

    switch (bitmapConfig) {
        case BitmapConfig_ALPHA_8:
            return BitmapFormat::FORMAT_ALPHA_8;
        case BitmapConfig_ARGB_4444:
            return BitmapFormat::FORMAT_ARGB_4444;
        case BitmapConfig_ARGB_8888:
            return BitmapFormat::FORMAT_ARGB_8888;
        case BitmapConfig_RGB_565:
            return BitmapFormat::FORMAT_RGB_565;
        default:
            return BitmapFormat::FORMAT_NO_CONFIG;
    }
}

BitmapConfig BitmapHelper::GetBitmapConfigForFormat(
    /* [in] */ Int32 bitmapFormatValue)
{
    // ==================before translated======================
    // switch (bitmapFormatValue) {
    //     case BitmapFormat.FORMAT_ALPHA_8:
    //         return Bitmap.Config.ALPHA_8;
    //     case BitmapFormat.FORMAT_ARGB_4444:
    //         return Bitmap.Config.ARGB_4444;
    //     case BitmapFormat.FORMAT_RGB_565:
    //         return Bitmap.Config.RGB_565;
    //     case BitmapFormat.FORMAT_ARGB_8888:
    //     default:
    //         return Bitmap.Config.ARGB_8888;
    // }

    switch (bitmapFormatValue) {
        case BitmapFormat::FORMAT_ALPHA_8:
            return BitmapConfig_ALPHA_8;
        case BitmapFormat::FORMAT_ARGB_4444:
            return BitmapConfig_ARGB_4444;
        case BitmapFormat::FORMAT_RGB_565:
            return BitmapConfig_RGB_565;
        case BitmapFormat::FORMAT_ARGB_8888:
        default:
            return BitmapConfig_ARGB_8888;
    }
}

} // namespace Gfx
} // namespace Ui
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


