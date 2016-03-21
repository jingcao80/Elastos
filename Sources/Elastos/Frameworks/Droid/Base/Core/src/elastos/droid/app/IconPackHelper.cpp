
#include "_Elastos.Droid.Utility.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.External.h"
#include "elastos/droid/app/CColorFilterUtilsBuilder.h"
#include "elastos/droid/app/CComposedIconInfo.h"
#include "elastos/droid/app/IconPackHelper.h"
#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/content/pm/CActivityInfo.h"
#include "elastos/droid/content/pm/ThemeUtils.h"
#include "elastos/droid/content/res/CAssetManager.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/graphics/CBitmapHelper.h"
#include "elastos/droid/graphics/CCanvas.h"
#include "elastos/droid/graphics/CPaintFlagsDrawFilter.h"
#include "elastos/droid/graphics/Color.h"
#include "elastos/droid/graphics/CColorMatrix.h"
#include "elastos/droid/graphics/CColorMatrixColorFilter.h"
#include "elastos/droid/graphics/CPorterDuffXfermode.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/drawable/CBitmapDrawable.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/utility/CTypedValue.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::Pm::CActivityInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IThemeUtils;
using Elastos::Droid::Content::Pm::ThemeUtils;
using Elastos::Droid::Content::Res::IAssetManager;
using Elastos::Droid::Content::Res::CAssetManager;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::CPaintFlagsDrawFilter;
using Elastos::Droid::Graphics::Color;
using Elastos::Droid::Graphics::CColorMatrix;
using Elastos::Droid::Graphics::CColorMatrixColorFilter;
using Elastos::Droid::Graphics::CPorterDuffXfermode;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IColorFilter;
using Elastos::Droid::Graphics::IDrawFilter;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::PorterDuffMode_DST_OUT;
using Elastos::Droid::Graphics::PorterDuffMode_DST_OVER;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IXfermode;
using Elastos::Droid::Graphics::Drawable::CBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::IPaintDrawable;
using Elastos::Droid::Graphics::Drawable::IShapeDrawable;
using Elastos::Droid::Graphics::Drawable::IVectorDrawable;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::IO::CFile;
using Elastos::IO::IFile;
using Elastos::IO::IInputStream;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CRandom;
using Elastos::Utility::Logging::Logger;
using Org::Xmlpull::V1::IXmlPullParserFactory;
using Org::Xmlpull::V1::IXmlPullParserFactoryHelper;
using Org::Xmlpull::V1::CXmlPullParserFactoryHelper;

namespace Elastos {
namespace Droid {
namespace App {

//==========================================================
// IconPackHelper::IconCustomizer
//==========================================================
const AutoPtr<IRandom> IconPackHelper::IconCustomizer::sRandom = Init_sRandom();
AutoPtr<IIThemeService> IconPackHelper::IconCustomizer::sThemeService;

AutoPtr<IRandom> IconPackHelper::IconCustomizer::Init_sRandom()
{
    AutoPtr<IRandom> r;
    CRandom::New((IRandom**)&r);
    return r;
}

ECode IconPackHelper::IconCustomizer::GetComposedIconDrawable(
    /* [in] */ IDrawable* icon,
    /* [in] */ IContext* context,
    /* [in] */ IComposedIconInfo* iconInfo,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    return GetComposedIconDrawable(icon, res, iconInfo, drawable);
}

ECode IconPackHelper::IconCustomizer::GetComposedIconDrawable(
    /* [in] */ IDrawable* icon,
    /* [in] */ IResources* res,
    /* [in] */ IComposedIconInfo* iconInfo,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    if (iconInfo == NULL) {
        *drawable = icon;
        REFCOUNT_ADD(*drawable);
        return NOERROR;
    }
    Int32 back = 0;
    AutoPtr< ArrayOf<Int32> > iconBacks;
    iconInfo->GetIconBacks((ArrayOf<Int32>**)&iconBacks);
    if (iconBacks != NULL && iconBacks->GetLength() > 0) {
        Int32 index;
        sRandom->NextInt32(iconBacks->GetLength(), &index);
        back = (*iconBacks)[index];
    }
    Int32 iconMask, iconUpon, iconSize;
    Float iconScale;
    iconInfo->GetIconMask(&iconMask);
    iconInfo->GetIconUpon(&iconUpon);
    iconInfo->GetIconScale(&iconScale);
    iconInfo->GetIconSize(&iconSize);
    AutoPtr< ArrayOf<Float> > colorFilter;
    iconInfo->GetColorFilter((ArrayOf<Float>**)&colorFilter);
    AutoPtr<IBitmap> bmp = CreateIconBitmap(icon, res, back, iconMask, iconUpon,
            iconScale, iconSize, colorFilter);
    if (bmp == NULL) {
        *drawable = NULL;
        return NOERROR;
    }
    else {
        return CBitmapDrawable::New(res, bmp, drawable);
    }
}

ECode IconPackHelper::IconCustomizer::GetValue(
    /* [in] */ IResources* res,
    /* [in] */ Int32 resId,
    /* [in] */ ITypedValue* outValue,
    /* [in] */ IDrawable* baseIcon)
{
    AutoPtr<IAssetManager> am;
    res->GetAssets((IAssetManager**)&am);
    String pkgName;
    am->GetAppName(&pkgName);
    AutoPtr<ITypedValue> tempValue;
    CTypedValue::New((ITypedValue**)&tempValue);
    tempValue->SetTo(outValue);
    outValue->SetAssetCookie(COMPOSED_ICON_COOKIE);
    outValue->SetData(resId & (COMPOSED_ICON_COOKIE << 24 | 0x00ffffff));
    Int32 density;
    outValue->GetDensity(&density);
    outValue->SetString(CoreUtils::Convert(GetCachedIconPath(pkgName, resId, density)));
    AutoPtr<ICharSequence> ovString;
    outValue->GetString((ICharSequence**)&ovString);
    String str;
    ovString->ToString(&str);
    AutoPtr<IFile> f;
    CFile::New(str, (IFile**)&f);
    Boolean existed;
    if (f->Exists(&existed), !existed) {
        // compose the icon and cache it
        AutoPtr<IComposedIconInfo> iconInfo;
        res->GetComposedIconInfo((IComposedIconInfo**)&iconInfo);
        Int32 back = 0;
        AutoPtr< ArrayOf<Int32> > iconBacks;
        iconInfo->GetIconBacks((ArrayOf<Int32>**)&iconBacks);
        if (iconBacks != NULL && iconBacks->GetLength() > 0) {
            Int32 index = (str.GetHashCode() & 0x7fffffff)
                    % iconBacks->GetLength();
            back = (*iconBacks)[index];
        }

        Int32 iconMask, iconUpon, iconSize;
        Float iconScale;
        iconInfo->GetIconMask(&iconMask);
        iconInfo->GetIconUpon(&iconUpon);
        iconInfo->GetIconScale(&iconScale);
        iconInfo->GetIconSize(&iconSize);
        AutoPtr< ArrayOf<Float> > colorFilter;
        iconInfo->GetColorFilter((ArrayOf<Float>**)&colorFilter);
        AutoPtr<IBitmap> bmp = CreateIconBitmap(baseIcon, res, back, iconMask, iconUpon,
                iconScale, iconSize, colorFilter);
        if (!CacheComposedIcon(bmp, GetCachedIconName(pkgName, resId, density))) {
            Logger::W(TAG, "Unable to cache icon %s", str.string());
            // restore the original TypedValue
            outValue->SetTo(tempValue);
        }
    }
    return NOERROR;
}

AutoPtr<IBitmap> IconPackHelper::IconCustomizer::CreateIconBitmap(
    /* [in] */ IDrawable* icon,
    /* [in] */ IResources* res,
    /* [in] */ Int32 iconBack,
    /* [in] */ Int32 iconMask,
    /* [in] */ Int32 iconUpon,
    /* [in] */ Float scale,
    /* [in] */ Int32 iconSize,
    /* [in] */ ArrayOf<Float>* colorFilter)
{
    if (iconSize <= 0) return NULL;

    AutoPtr<ICanvas> canvas;
    CCanvas::New((ICanvas**)&canvas);
    AutoPtr<IDrawFilter> drawFilter;
    CPaintFlagsDrawFilter::New(IPaint::ANTI_ALIAS_FLAG,
            IPaint::FILTER_BITMAP_FLAG, (IDrawFilter**)&drawFilter);
    canvas->SetDrawFilter(drawFilter);

    Int32 width = 0, height = 0;
    if (IPaintDrawable::Probe(icon)) {
        IShapeDrawable* painter = IShapeDrawable::Probe(icon);
        painter->SetIntrinsicWidth(iconSize);
        painter->SetIntrinsicHeight(iconSize);

        // A PaintDrawable does not have an exact size
        width = iconSize;
        height = iconSize;
    }
    else if (IBitmapDrawable::Probe(icon)) {
        // Ensure the bitmap has a density.
        IBitmapDrawable* bitmapDrawable = IBitmapDrawable::Probe(icon);
        AutoPtr<IBitmap> bitmap;
        bitmapDrawable->GetBitmap((IBitmap**)&bitmap);
        Int32 density;
        bitmap->GetDensity(&density);
        if (density == IBitmap::DENSITY_NONE) {
            AutoPtr<IDisplayMetrics> dm;
            bitmapDrawable->SetTargetDensity(dm);
        }
        canvas->SetDensity(density);

        // If the original size of the icon isn't greater
        // than twice the size of recommended large icons
        // respect the original size of the icon
        // otherwise enormous icons can easily create
        // OOM situations.
        Int32 bmWidth, bmHeight;
        bitmap->GetWidth(&bmWidth);
        bitmap->GetHeight(&bmHeight);
        if ((bmWidth < (iconSize * 2))
                && (bmHeight < (iconSize * 2))) {
            width = bmWidth;
            height = bmHeight;
        }
        else {
            width = iconSize;
            height = iconSize;
        }
    }
    else if (IVectorDrawable::Probe(icon)) {
        width = height = iconSize;
    }

    if (width <= 0 || height <= 0) return NULL;

    AutoPtr<IBitmapHelper> helper;
    CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
    AutoPtr<IBitmap> bitmap;
    helper->CreateBitmap(width, height, BitmapConfig_ARGB_8888, (IBitmap**)&bitmap);
    canvas->SetBitmap(bitmap);

    // Scale the original
    AutoPtr<IRect> oldBounds;
    CRect::New((IRect**)&oldBounds);

    AutoPtr<IRect> bounds;
    icon->GetBounds((IRect**)&bounds);
    oldBounds->Set(bounds);
    icon->SetBounds(0, 0, width, height);
    Int32 result;
    canvas->Save(&result);
    canvas->Scale(scale, scale, width / 2, height / 2);
    if (colorFilter != NULL) {
        AutoPtr<IPaint> p;
        if (IBitmapDrawable::Probe(icon)) {
            IBitmapDrawable::Probe(icon)->GetPaint((IPaint**)&p);
        }
        else if (IPaintDrawable::Probe(icon)) {
            IShapeDrawable::Probe(icon)->GetPaint((IPaint**)&p);
        }
        if (p != NULL) {
            AutoPtr<IColorFilter> cf;
            CColorMatrixColorFilter::New(*colorFilter, (IColorFilter**)&cf);
            p->SetColorFilter(cf);
        }
    }
    icon->Draw(canvas);
    canvas->Restore();

    // Mask off the original if iconMask is not null
    if (iconMask != 0) {
        AutoPtr<IDrawable> mask;
        res->GetDrawable(iconMask, (IDrawable**)&mask);
        if (mask != NULL) {
            bounds = NULL;
            icon->GetBounds((IRect**)&bounds);
            mask->SetBounds(bounds);
            AutoPtr<IXfermode> mode;
            CPorterDuffXfermode::New(PorterDuffMode_DST_OUT, (IXfermode**)&mode);
            AutoPtr<IPaint> p;
            IBitmapDrawable::Probe(mask)->GetPaint((IPaint**)&p);
            p->SetXfermode(mode);
            mask->Draw(canvas);
        }
    }
    // Draw the iconBacks if not null and then the original (scaled and masked) icon on top
    if (iconBack != 0) {
        AutoPtr<IDrawable> back;
        res->GetDrawable(iconBack, (IDrawable**)&back);
        if (back != NULL) {
            bounds = NULL;
            icon->GetBounds((IRect**)&bounds);
            back->SetBounds(bounds);
            AutoPtr<IXfermode> mode;
            CPorterDuffXfermode::New(PorterDuffMode_DST_OVER, (IXfermode**)&mode);
            AutoPtr<IPaint> p;
            IBitmapDrawable::Probe(back)->GetPaint((IPaint**)&p);
            p->SetXfermode(mode);
            back->Draw(canvas);
        }
    }
    // Finally draw the foreground if one was supplied
    if (iconUpon != 0) {
        AutoPtr<IDrawable> upon;
        res->GetDrawable(iconUpon, (IDrawable**)&upon);
        if (upon != NULL) {
            bounds = NULL;
            icon->GetBounds((IRect**)&bounds);
            upon->SetBounds(bounds);
            upon->Draw(canvas);
        }
    }
    icon->SetBounds(oldBounds);
    Int32 density;
    canvas->GetDensity(&density);
    bitmap->SetDensity(density);

    return bitmap;
}

Boolean IconPackHelper::IconCustomizer::CacheComposedIcon(
    /* [in] */ IBitmap* bmp,
    /* [in] */ const String& path)
{
    // try {
    if (sThemeService == NULL) {
        AutoPtr<IInterface> obj = ServiceManager::GetService(IContext::THEME_SERVICE);
        sThemeService = IIThemeService::Probe(obj);
    }
    Boolean result = FALSE;
    sThemeService->CacheComposedIcon(bmp, path, &result);
    return result;
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Unable to cache icon.", e);
    // }
}

String IconPackHelper::IconCustomizer::GetCachedIconPath(
    /* [in] */ const String& pkgName,
    /* [in] */ Int32 resId,
    /* [in] */ Int32 density)
{
    String str("");
    str.AppendFormat("%s/%s", IThemeUtils::SYSTEM_THEME_ICON_CACHE_DIR.string(),
            GetCachedIconName(pkgName, resId, density).string());
    return str;
}

String IconPackHelper::IconCustomizer::GetCachedIconName(
    /* [in] */ const String& pkgName,
    /* [in] */ Int32 resId,
    /* [in] */ Int32 density)
{
    String str("");
    str.AppendFormat("%s_%08x_%d.png", pkgName.string(), resId, density);
    return str;
}

//==========================================================
// IconPackHelper::ColorFilterUtils::Builder
//==========================================================
CAR_INTERFACE_IMPL(IconPackHelper::ColorFilterUtils::Builder, Object, IColorFilterUtilsBuilder)

ECode IconPackHelper::ColorFilterUtils::Builder::constructor()
{
    CArrayList::New((IList**)&mMatrixList);
    return NOERROR;
}

ECode IconPackHelper::ColorFilterUtils::Builder::Hue(
    /* [in] */ Float value)
{
    AutoPtr<IColorMatrix> matrix;
    ColorFilterUtils::AdjustHue(value, (IColorMatrix**)&matrix);
    mMatrixList->Add(matrix);
    return NOERROR;
}

ECode IconPackHelper::ColorFilterUtils::Builder::Saturate(
    /* [in] */ Float saturation)
{
    AutoPtr<IColorMatrix> matrix;
    ColorFilterUtils::AdjustSaturation(saturation, (IColorMatrix**)&matrix);
    mMatrixList->Add(matrix);
    return NOERROR;
}

ECode IconPackHelper::ColorFilterUtils::Builder::Brightness(
    /* [in] */ Float brightness)
{
    AutoPtr<IColorMatrix> matrix;
    ColorFilterUtils::AdjustBrightness(brightness, (IColorMatrix**)&matrix);
    mMatrixList->Add(matrix);
    return NOERROR;
}

ECode IconPackHelper::ColorFilterUtils::Builder::Contrast(
    /* [in] */ Float contrast)
{
    AutoPtr<IColorMatrix> matrix;
    ColorFilterUtils::AdjustContrast(contrast, (IColorMatrix**)&matrix);
    mMatrixList->Add(matrix);
    return NOERROR;
}

ECode IconPackHelper::ColorFilterUtils::Builder::Alpha(
    /* [in] */ Float alpha)
{
    AutoPtr<IColorMatrix> matrix;
    ColorFilterUtils::AdjustAlpha(alpha, (IColorMatrix**)&matrix);
    mMatrixList->Add(matrix);
    return NOERROR;
}

ECode IconPackHelper::ColorFilterUtils::Builder::InvertColors()
{
    AutoPtr<IColorMatrix> matrix;
    ColorFilterUtils::InvertColors((IColorMatrix**)&matrix);
    mMatrixList->Add(matrix);
    return NOERROR;
}

ECode IconPackHelper::ColorFilterUtils::Builder::Tint(
    /* [in] */ Int32 color)
{
    AutoPtr<IColorMatrix> matrix;
    ColorFilterUtils::ApplyTint(color, (IColorMatrix**)&matrix);
    mMatrixList->Add(matrix);
    return NOERROR;
}

ECode IconPackHelper::ColorFilterUtils::Builder::Build(
    /* [out] */ IColorMatrix** result)
{
    VALIDATE_NOT_NULL(result);
    Int32 N;
    if (mMatrixList == NULL || (mMatrixList->GetSize(&N), N == 0)) {
        *result = NULL;
        return NOERROR;
    }

    AutoPtr<IColorMatrix> colorMatrix;
    CColorMatrix::New((IColorMatrix**)&colorMatrix);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mMatrixList->Get(i, (IInterface**)&obj);
        IColorMatrix* cm = IColorMatrix::Probe(obj);
        colorMatrix->PostConcat(cm);
    }
    *result = colorMatrix;
    REFCOUNT_ADD(*result);
    return NOERROR;
}


//==========================================================
// IconPackHelper::ColorFilterUtils
//==========================================================
const String IconPackHelper::ColorFilterUtils::TAG_FILTER("filter");
const String IconPackHelper::ColorFilterUtils::FILTER_HUE("hue");
const String IconPackHelper::ColorFilterUtils::FILTER_SATURATION("saturation");
const String IconPackHelper::ColorFilterUtils::FILTER_INVERT("invert");
const String IconPackHelper::ColorFilterUtils::FILTER_BRIGHTNESS("brightness");
const String IconPackHelper::ColorFilterUtils::FILTER_CONTRAST("contrast");
const String IconPackHelper::ColorFilterUtils::FILTER_ALPHA("alpha");
const String IconPackHelper::ColorFilterUtils::FILTER_TINT("tint");

const Int32 IconPackHelper::ColorFilterUtils::MIN_HUE = -180;
const Int32 IconPackHelper::ColorFilterUtils::MAX_HUE = 180;
const Int32 IconPackHelper::ColorFilterUtils::MIN_SATURATION = 0;
const Int32 IconPackHelper::ColorFilterUtils::MAX_SATURATION = 200;
const Int32 IconPackHelper::ColorFilterUtils::MIN_BRIGHTNESS = 0;
const Int32 IconPackHelper::ColorFilterUtils::MAX_BRIGHTNESS = 200;
const Int32 IconPackHelper::ColorFilterUtils::MIN_CONTRAST = -100;
const Int32 IconPackHelper::ColorFilterUtils::MAX_CONTRAST = 100;
const Int32 IconPackHelper::ColorFilterUtils::MIN_ALPHA = 0;
const Int32 IconPackHelper::ColorFilterUtils::MAX_ALPHA = 100;

ECode IconPackHelper::ColorFilterUtils::ParseIconFilter(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IColorFilterUtilsBuilder* builder,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    String tag;
    parser->GetName(&tag);
    if (!TAG_FILTER.Equals(tag)) return NOERROR;

    Int32 attrCount;
    parser->GetAttributeCount(&attrCount);
    String attrName;
    String attr;
    Int32 intValue;
    while (attrCount-- > 0) {
        parser->GetAttributeName(attrCount, &attrName);
        if (attrName.Equals("name")) {
            parser->GetAttributeValue(attrCount, &attr);
        }
    }
    String content;
    FAIL_RETURN(parser->NextText(&content));
    if (!attr.IsNull() && !content.IsNullOrEmpty()) {
        content = content.Trim();
        if (FILTER_HUE.EqualsIgnoreCase(attr)) {
            intValue = ClampValue(GetInt32(content, 0), MIN_HUE, MAX_HUE);
            builder->Hue(intValue);
        }
        else if (FILTER_SATURATION.EqualsIgnoreCase(attr)) {
            intValue = ClampValue(GetInt32(content, 100),
                    MIN_SATURATION, MAX_SATURATION);
            builder->Saturate(intValue);
        }
        else if (FILTER_INVERT.EqualsIgnoreCase(attr)) {
            if (String("true").EqualsIgnoreCase(content)) {
                builder->InvertColors();
            }
        }
        else if (FILTER_BRIGHTNESS.EqualsIgnoreCase(attr)) {
            intValue = ClampValue(GetInt32(content, 100),
                    MIN_BRIGHTNESS, MAX_BRIGHTNESS);
            builder->Brightness(intValue);
        }
        else if (FILTER_CONTRAST.EqualsIgnoreCase(attr)) {
            intValue = ClampValue(GetInt32(content, 0),
                    MIN_CONTRAST, MAX_CONTRAST);
            builder->Contrast(intValue);
        }
        else if (FILTER_ALPHA.EqualsIgnoreCase(attr)) {
            intValue = ClampValue(GetInt32(content, 100), MIN_ALPHA, MAX_ALPHA);
            builder->Alpha(intValue);
        }
        else if (FILTER_TINT.EqualsIgnoreCase(attr)) {
            // try {
            ECode ec = Color::ParseColor(content, &intValue);
            if (SUCCEEDED(ec)) builder->Tint(intValue);
            // } catch (IllegalArgumentException e) {
            //     Log.w(TAG, "Cannot apply tint, invalid argument: " + content);
            // }
        }
    }
    *result = TRUE;
    return NOERROR;
}

Int32 IconPackHelper::ColorFilterUtils::GetInt32(
    /* [in] */ const String& value,
    /* [in] */ Int32 defaultValue)
{
    // try {
    Int32 result;
    ECode ec = StringUtils::Parse(value, &result);
    if (FAILED(ec)) result = defaultValue;
    return result;
    // } catch (NumberFormatException e) {
    //     return defaultValue;
    // }
}

Int32 IconPackHelper::ColorFilterUtils::ClampValue(
    /* [in] */ Int32 value,
    /* [in] */ Int32 min,
    /* [in] */ Int32 max)
{
    return Elastos::Core::Math::Min(max, Elastos::Core::Math::Max(min, value));
}

ECode IconPackHelper::ColorFilterUtils::AdjustHue(
    /* [in] */ Float value,
    /* [out] */ IColorMatrix** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IColorMatrix> cm;
    CColorMatrix::New((IColorMatrix**)&cm);
    value = value / 180 * (Float)Elastos::Core::Math::PI;
    if (value != 0) {
        Float cosVal = (Float)Elastos::Core::Math::Cos(value);
        Float sinVal = (Float)Elastos::Core::Math::Sin(value);
        Float lumR = 0.213f;
        Float lumG = 0.715f;
        Float lumB = 0.072f;
        AutoPtr< ArrayOf<Float> > mat = ArrayOf<Float>::Alloc(25);
        (*mat)[0] = lumR + cosVal * (1 - lumR) + sinVal * (-lumR);
        (*mat)[1] = lumG + cosVal * (-lumG) + sinVal * (-lumG);
        (*mat)[2] = lumB + cosVal * (-lumB) + sinVal * (1 - lumB);
        (*mat)[3] = 0;
        (*mat)[4] = 0;
        (*mat)[5] = lumR + cosVal * (-lumR) + sinVal * (0.143f);
        (*mat)[6] = lumG + cosVal * (1 - lumG) + sinVal * (0.140f);
        (*mat)[7] = lumB + cosVal * (-lumB) + sinVal * (-0.283f);
        (*mat)[8] = 0;
        (*mat)[9] = 0;
        (*mat)[10] = lumR + cosVal * (-lumR) + sinVal * (-(1 - lumR));
        (*mat)[11] = lumG + cosVal * (-lumG) + sinVal * (lumG);
        (*mat)[12] = lumB + cosVal * (1 - lumB) + sinVal * (lumB);
        (*mat)[13] = 0;
        (*mat)[14] = 0;
        (*mat)[15] = 0;
        (*mat)[16] = 0;
        (*mat)[17] = 0;
        (*mat)[18] = 1;
        (*mat)[19] = 0;
        (*mat)[20] = 0;
        (*mat)[21] = 0;
        (*mat)[22] = 0;
        (*mat)[23] = 0;
        (*mat)[24] = 1;
        cm->Set(mat);
    }
    *result = cm;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode IconPackHelper::ColorFilterUtils::AdjustSaturation(
    /* [in] */ Float saturation,
    /* [out] */ IColorMatrix** result)
{
    VALIDATE_NOT_NULL(result);
    saturation = saturation / 100;
    AutoPtr<IColorMatrix> cm;
    CColorMatrix::New((IColorMatrix**)&cm);
    cm->SetSaturation(saturation);

    *result = cm;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode IconPackHelper::ColorFilterUtils::InvertColors(
    /* [out] */ IColorMatrix** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr< ArrayOf<Float> > matrix = ArrayOf<Float>::Alloc(20);
    (*matrix)[0] = -1;
    (*matrix)[1] = 0;
    (*matrix)[2] = 0;
    (*matrix)[3] = 0;
    (*matrix)[4] = 255; //red
    (*matrix)[5] = 0;
    (*matrix)[6] = -1;
    (*matrix)[7] = 0;
    (*matrix)[8] = 0;
    (*matrix)[9] = 255; //green
    (*matrix)[10] = 0;
    (*matrix)[11] = 0;
    (*matrix)[12] = -1;
    (*matrix)[13] = 0;
    (*matrix)[14] = 255; //blue
    (*matrix)[15] = 0;
    (*matrix)[16] = 0;
    (*matrix)[17] = 0;
    (*matrix)[18] = 1;
    (*matrix)[19] = 0; //alpha

    return CColorMatrix::New(*matrix, result);
}

ECode IconPackHelper::ColorFilterUtils::AdjustBrightness(
    /* [in] */ Float brightness,
    /* [out] */ IColorMatrix** result)
{
    VALIDATE_NOT_NULL(result);
    brightness = brightness / 100;
    AutoPtr<IColorMatrix> cm;
    CColorMatrix::New((IColorMatrix**)&cm);
    cm->SetScale(brightness, brightness, brightness, 1);

    *result = cm;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode IconPackHelper::ColorFilterUtils::AdjustContrast(
    /* [in] */ Float contrast,
    /* [out] */ IColorMatrix** result)
{
    VALIDATE_NOT_NULL(result);
    contrast = contrast / 100 + 1;
    Float o = (-0.5f * contrast + 0.5f) * 255;
    AutoPtr< ArrayOf<Float> > matrix = ArrayOf<Float>::Alloc(20);
    (*matrix)[0] = contrast;
    (*matrix)[1] = 0;
    (*matrix)[2] = 0;
    (*matrix)[3] = 0;
    (*matrix)[4] = o; //red
    (*matrix)[5] = 0;
    (*matrix)[6] = contrast;
    (*matrix)[7] = 0;
    (*matrix)[8] = 0;
    (*matrix)[9] = o; //green
    (*matrix)[10] = 0;
    (*matrix)[11] = 0;
    (*matrix)[12] = contrast;
    (*matrix)[13] = 0;
    (*matrix)[14] = o; //blue
    (*matrix)[15] = 0;
    (*matrix)[16] = 0;
    (*matrix)[17] = 0;
    (*matrix)[18] = 1;
    (*matrix)[19] = 0; //alpha

    return CColorMatrix::New(*matrix, result);
}

ECode IconPackHelper::ColorFilterUtils::AdjustAlpha(
    /* [in] */ Float alpha,
    /* [out] */ IColorMatrix** result)
{
    VALIDATE_NOT_NULL(result);
    alpha = alpha / 100;
    AutoPtr<IColorMatrix> cm;
    CColorMatrix::New((IColorMatrix**)&cm);
    cm->SetScale(1, 1, 1, alpha);

    *result = cm;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode IconPackHelper::ColorFilterUtils::ApplyTint(
    /* [in] */ Int32 color,
    /* [out] */ IColorMatrix** result)
{
    Float alpha = Color::Alpha(color) / 255.0;
    Float red = Color::Red(color) * alpha;
    Float green = Color::Green(color) * alpha;
    Float blue = Color::Blue(color) * alpha;

    AutoPtr< ArrayOf<Float> > matrix = ArrayOf<Float>::Alloc(20);
    (*matrix)[0] = 1;
    (*matrix)[1] = 0;
    (*matrix)[2] = 0;
    (*matrix)[3] = 0;
    (*matrix)[4] = red; //red
    (*matrix)[5] = 0;
    (*matrix)[6] = 1;
    (*matrix)[7] = 0;
    (*matrix)[8] = 0;
    (*matrix)[9] = green; //green
    (*matrix)[10] = 0;
    (*matrix)[11] = 0;
    (*matrix)[12] = 1;
    (*matrix)[13] = 0;
    (*matrix)[14] = blue; //blue
    (*matrix)[15] = 0;
    (*matrix)[16] = 0;
    (*matrix)[17] = 0;
    (*matrix)[18] = 1;
    (*matrix)[19] = 0; //alpha

    return CColorMatrix::New(*matrix, result);
}


//==========================================================
// IconPackHelper::StaticInitializer
//==========================================================
IconPackHelper::StaticInitializer::StaticInitializer()
{
    CComponentName::New(ICON_BACK_TAG, String(""), (IComponentName**)&ICON_BACK_COMPONENT);
    CComponentName::New(ICON_MASK_TAG, String(""), (IComponentName**)&ICON_MASK_COMPONENT);
    CComponentName::New(ICON_UPON_TAG, String(""), (IComponentName**)&ICON_UPON_COMPONENT);
    CComponentName::New(ICON_SCALE_TAG, String(""), (IComponentName**)&ICON_SCALE_COMPONENT);
}

//==========================================================
// IconPackHelper
//==========================================================
const String IconPackHelper::TAG("IconPackHelper");
const String IconPackHelper::ICON_MASK_TAG("iconmask");
const String IconPackHelper::ICON_BACK_TAG("iconback");
const String IconPackHelper::ICON_UPON_TAG("iconupon");
const String IconPackHelper::ICON_SCALE_TAG("scale");
const String IconPackHelper::ICON_BACK_FORMAT("iconback%d");

AutoPtr<IComponentName> IconPackHelper::ICON_BACK_COMPONENT;
AutoPtr<IComponentName> IconPackHelper::ICON_MASK_COMPONENT;
AutoPtr<IComponentName> IconPackHelper::ICON_UPON_COMPONENT;
AutoPtr<IComponentName> IconPackHelper::ICON_SCALE_COMPONENT;

const Float IconPackHelper::DEFAULT_SCALE = 1.0f;
const Int32 IconPackHelper::COMPOSED_ICON_COOKIE = 128;
IconPackHelper::StaticInitializer IconPackHelper::sInitializer;

IconPackHelper::IconPackHelper()
    : mIconBackCount(0)
{}

ECode IconPackHelper::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    CHashMap::New((IMap**)&mIconPackResourceMap);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&obj);
    IActivityManager* am = IActivityManager::Probe(obj);
    CComposedIconInfo::New((IComposedIconInfo**)&mComposedIconInfo);
    Int32 size, density;
    am->GetLauncherLargeIconSize(&size);
    am->GetLauncherLargeIconDensity(&density);
    mComposedIconInfo->SetIconSize(size);
    mComposedIconInfo->SetIconDensity(density);
    CColorFilterUtilsBuilder::New((IColorFilterUtilsBuilder**)&mFilterBuilder);
    return NOERROR;
}

ECode IconPackHelper::LoadResourcesFromXmlParser(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IMap* iconPackResources)
{
    mIconBackCount = 0;
    Int32 eventType;
    parser->GetEventType(&eventType);
    do {
        if (eventType != IXmlPullParser::START_TAG) {
            continue;
        }

        if (ParseComposedIconComponent(parser, iconPackResources)) {
            continue;
        }

        Boolean result;
        FAIL_RETURN(ColorFilterUtils::ParseIconFilter(parser, mFilterBuilder, &result));
        if (result) {
            continue;
        }

        String strName;
        if (parser->GetName(&strName), strName.EqualsIgnoreCase(ICON_SCALE_TAG)) {
            String factor;
            parser->GetAttributeValue(String(NULL), String("factor"), &factor);
            if (factor.IsNull()) {
                Int32 count;
                if (parser->GetAttributeCount(&count), count == 1) {
                    parser->GetAttributeValue(0, &factor);
                }
            }
            iconPackResources->Put(ICON_SCALE_COMPONENT, CoreUtils::Convert(factor));
            continue;
        }

        if (parser->GetName(&strName), !strName.EqualsIgnoreCase("item")) {
            continue;
        }

        String component;
        parser->GetAttributeValue(String(NULL), String("component"), &component);
        String drawable;
        parser->GetAttributeValue(String(NULL), String("drawable"), &drawable);

        // Validate component/drawable exist
        if (TextUtils::IsEmpty(component) || TextUtils::IsEmpty(drawable)) {
            continue;
        }

        // Validate format/length of component
        if (!component.StartWith("ComponentInfo{") || !component.EndWith("}")
                || component.GetLength() < 16 || drawable.GetLength() == 0) {
            continue;
        }

        // Sanitize stored value
        component = component.Substring(14, component.GetLength() - 1).ToLowerCase();

        AutoPtr<IComponentName> name;
        if (!component.Contains("/")) {
            // Package icon reference
            CComponentName::New(component.ToLowerCase(), String(""), (IComponentName**)&name);
        }
        else {
            CComponentName::UnflattenFromString(component, (IComponentName**)&name);
        }

        if (name != NULL) {
            iconPackResources->Put(name, CoreUtils::Convert(drawable));
        }
    } while (parser->Next(&eventType), eventType != IXmlPullParser::END_DOCUMENT);
    return NOERROR;
}

Boolean IconPackHelper::IsComposedIconComponent(
    /* [in] */ const String& tag)
{
    return tag.EqualsIgnoreCase(ICON_MASK_TAG) ||
            tag.EqualsIgnoreCase(ICON_BACK_TAG) ||
            tag.EqualsIgnoreCase(ICON_UPON_TAG);
}

Boolean IconPackHelper::ParseComposedIconComponent(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IMap* iconPackResources)
{
    String icon;
    String tag;
    parser->GetName(&tag);
    if (!IsComposedIconComponent(tag)) {
        return FALSE;
    }

    Int32 count;
    if (parser->GetAttributeCount(&count), count >= 1) {
        if (tag.EqualsIgnoreCase(ICON_BACK_TAG)) {
            parser->GetAttributeCount(&mIconBackCount);
            for (Int32 i = 0; i < mIconBackCount; i++) {
                tag = "";
                tag.AppendFormat(ICON_BACK_FORMAT.string(), i);
                parser->GetAttributeValue(i, &icon);
                AutoPtr<IComponentName> component;
                CComponentName::New(tag, String(""), (IComponentName**)&component);
                iconPackResources->Put(component, CoreUtils::Convert(icon));
            }
        }
        else {
            parser->GetAttributeValue(0, &icon);
            AutoPtr<IComponentName> component;
            CComponentName::New(tag, String(""), (IComponentName**)&component);
            iconPackResources->Put(component, CoreUtils::Convert(icon));
        }
        return TRUE;
    }

    return FALSE;
}

ECode IconPackHelper::LoadIconPack(
    /* [in] */ const String& packageName)
{
    if (packageName.IsNull()) {
        mLoadedIconPackResource = NULL;
        mLoadedIconPackName = NULL;
        mComposedIconInfo->SetIconBacks(NULL);
        mComposedIconInfo->SetIconMask(0);
        mComposedIconInfo->SetIconUpon(0);
        mComposedIconInfo->SetIconScale(0);
        mComposedIconInfo->SetColorFilter(NULL);
    }
    else {
        mIconBackCount = 0;
        AutoPtr<IResources> res;
        FAIL_RETURN(CreateIconResource(mContext, packageName, (IResources**)&res));
        FAIL_RETURN(GetIconResMapFromXml(res, packageName, (IMap**)&mIconPackResourceMap));
        mLoadedIconPackResource = res;
        mLoadedIconPackName = packageName;
        LoadComposedIconComponents();
        AutoPtr<IColorMatrix> cm;
        mFilterBuilder->Build((IColorMatrix**)&cm);
        if (cm != NULL) {
            AutoPtr< ArrayOf<Float> > arr;
            cm->GetArray((ArrayOf<Float>**)&arr);
            AutoPtr< ArrayOf<Float> > arrClone = arr->Clone();
            mComposedIconInfo->SetColorFilter(arrClone);
        }
    }
    return NOERROR;
}

ECode IconPackHelper::GetComposedIconInfo(
    /* [out] */ IComposedIconInfo** iconInfo)
{
    VALIDATE_NOT_NULL(iconInfo);
    *iconInfo = mComposedIconInfo;
    REFCOUNT_ADD(*iconInfo);
    return NOERROR;
}

void IconPackHelper::LoadComposedIconComponents()
{
    mComposedIconInfo->SetIconMask(GetResourceIdForName(ICON_MASK_COMPONENT));
    mComposedIconInfo->SetIconUpon(GetResourceIdForName(ICON_UPON_COMPONENT));

    // Take care of loading iconback which can have multiple images
    if (mIconBackCount > 0) {
        AutoPtr< ArrayOf<Int32> > iconBacks = ArrayOf<Int32>::Alloc(mIconBackCount);
        for (int i = 0; i < mIconBackCount; i++) {
            String str("");
            str.AppendFormat(ICON_BACK_FORMAT.string(), i);
            AutoPtr<IComponentName> component;
            CComponentName::New(str, String(""), (IComponentName**)&component);
            (*iconBacks)[i] = GetResourceIdForName(component);
        }
        mComposedIconInfo->SetIconBacks(iconBacks);
    }

    // Get the icon scale from this pack
    AutoPtr<IInterface> obj;
    mIconPackResourceMap->Get(ICON_SCALE_COMPONENT, (IInterface**)&obj);
    String scale = CoreUtils::Unbox(ICharSequence::Probe(obj));
    if (!scale.IsNull()) {
        // try {
        Float scaleValue;
        ECode ec = StringUtils::Parse(scale, &scaleValue);
        if (FAILED(ec)) scaleValue = DEFAULT_SCALE;
        // } catch (NumberFormatException e) {
        //     mComposedIconInfo.iconScale = DEFAULT_SCALE;
        // }
        mComposedIconInfo->SetIconScale(scaleValue);
    }
    else {
        mComposedIconInfo->SetIconScale(DEFAULT_SCALE);
    }
}

Int32 IconPackHelper::GetResourceIdForName(
    /* [in] */ IComponentName* component)
{
    AutoPtr<IInterface> obj;
    mIconPackResourceMap->Get(component, (IInterface**)&obj);
    String item = CoreUtils::Unbox(ICharSequence::Probe(obj));
    if (!TextUtils::IsEmpty(item)) {
        return GetResourceIdForDrawable(item);
    }
    return 0;
}

ECode IconPackHelper::CreateIconResource(
    /* [in] */ IContext* context,
    /* [in] */ const String& packageName,
    /* [out] */ IResources** iconRes)
{
    VALIDATE_NOT_NULL(iconRes);
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IPackageInfo> info;
    pm->GetPackageInfo(packageName, 0, (IPackageInfo**)&info);
    AutoPtr<IApplicationInfo> appInfo;
    info->GetApplicationInfo((IApplicationInfo**)&appInfo);
    String themeApk;
    appInfo->GetPublicSourceDir(&themeApk);

    String prefixPath;
    String iconPkgPath;
    String iconResPath;
    Boolean result;
    if (info->GetIsLegacyIconPackPkg(&result), result) {
        iconResPath = "";
        iconPkgPath = "";
        prefixPath = "";
    }
    else {
        prefixPath = IThemeUtils::ICONS_PATH; //path inside APK
        iconPkgPath = ThemeUtils::GetIconPackPkgPath(packageName);
        iconResPath = ThemeUtils::GetIconPackResPath(packageName);
    }

    AutoPtr<IAssetManager> assets;
    CAssetManager::New((IAssetManager**)&assets);
    Int32 cookie;
    assets->AddIconPath(themeApk, iconPkgPath,
            prefixPath, IResources::THEME_ICON_PKG_ID, &cookie);

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    AutoPtr<IConfiguration> config;
    res->GetConfiguration((IConfiguration**)&config);
    return CResources::New(assets, dm, config, iconRes);
}

ECode IconPackHelper::GetIconResMapFromXml(
    /* [in] */ IResources* res,
    /* [in] */ const String& packageName,
    /* [out] */ IMap** iconResMap)
{
    VALIDATE_NOT_NULL(iconResMap);
    *iconResMap = NULL;

    AutoPtr<IXmlPullParser> parser;
    AutoPtr<IInputStream> inputStream;
    AutoPtr<IMap> iconPackResources;
    CHashMap::New((IMap**)&iconPackResources);

    // try {
    AutoPtr<IAssetManager> am;
    res->GetAssets((IAssetManager**)&am);
    ECode ec = am->Open(String("appfilter.xml"), (IInputStream**)&inputStream);
    if (SUCCEEDED(ec)) {
        AutoPtr<IXmlPullParserFactoryHelper> helper;
        CXmlPullParserFactoryHelper::AcquireSingleton((IXmlPullParserFactoryHelper**)&helper);
        AutoPtr<IXmlPullParserFactory> factory;
        helper->NewInstance((IXmlPullParserFactory**)&factory);
        factory->NewPullParser((IXmlPullParser**)&parser);
        ec = parser->SetInput(inputStream, String("UTF-8"));
    }
    if (FAILED(ec)) {
        // Catch any exception since we want to fall back to parsing the xml/
        // resource in all cases
        Int32 resId;
        res->GetIdentifier(String("appfilter"), String("xml"), packageName, &resId);
        if (resId != 0) {
            AutoPtr<IXmlResourceParser> resParser;
            res->GetXml(resId, (IXmlResourceParser**)&resParser);
            parser = IXmlPullParser::Probe(resParser);
        }
    }
    // } catch (Exception e) {
    //     // Catch any exception since we want to fall back to parsing the xml/
    //     // resource in all cases
    //     int resId = res.getIdentifier("appfilter", "xml", packageName);
    //     if (resId != 0) {
    //         parser = res.getXml(resId);
    //     }
    // }

    if (parser != NULL) {
        // try {
        ECode ec = LoadResourcesFromXmlParser(parser, iconPackResources);
        if (IXmlResourceParser::Probe(parser) != NULL) {
            IXmlResourceParser::Probe(parser)->Close();
        }
        else if (inputStream != NULL) {
            // try {
            inputStream->Close();
            // } catch (IOException e) {
            // }
        }
        if (SUCCEEDED(ec)) {
            *iconResMap = iconPackResources;
            REFCOUNT_ADD(*iconResMap);
            return NOERROR;
        }
        // } catch (XmlPullParserException e) {
        //     e.printStackTrace();
        // } catch (IOException e) {
        //     e.printStackTrace();
        // } finally {
        //     // Cleanup resources
        //     if (parser instanceof XmlResourceParser) {
        //         ((XmlResourceParser) parser).close();
        //     } else if (inputStream != null) {
        //         try {
        //             inputStream.close();
        //         } catch (IOException e) {
        //         }
        //     }
        // }
    }

    // Application uses a different theme format (most likely launcher pro)
    Int32 arrayId;
    res->GetIdentifier(String("theme_iconpack"), String("array"), packageName, &arrayId);
    if (arrayId == 0) {
        res->GetIdentifier(String("icon_pack"), String("array"), packageName, &arrayId);
    }

    if (arrayId != 0) {
        AutoPtr< ArrayOf<String> > iconPack;
        res->GetStringArray(arrayId, (ArrayOf<String>**)&iconPack);
        for (Int32 i = 0; i < iconPack->GetLength(); i++) {
            String entry = (*iconPack)[i];

            if (TextUtils::IsEmpty(entry)) {
                continue;
            }

            String icon = entry;
            StringUtils::ReplaceAll(String(entry), String("_"), String("."), &entry);

            AutoPtr<IComponentName> compName;
            CComponentName::New(entry.ToLowerCase(), String(""), (IComponentName**)&compName);
            iconPackResources->Put(compName, CoreUtils::Convert(icon));

            Int32 activityIndex = entry.LastIndexOf(".");
            if (activityIndex <= 0 || activityIndex == entry.GetLength() - 1) {
                continue;
            }

            String iconPackage = entry.Substring(0, activityIndex);
            if (TextUtils::IsEmpty(iconPackage)) {
                continue;
            }

            String iconActivity = entry.Substring(activityIndex + 1);
            if (TextUtils::IsEmpty(iconActivity)) {
                continue;
            }

            // Store entries as lower case to ensure match
            iconPackage = iconPackage.ToLowerCase();
            iconActivity = iconActivity.ToLowerCase();

            iconActivity = iconPackage + "." + iconActivity;
            compName = NULL;
            CComponentName::New(iconPackage, iconActivity, (IComponentName**)&compName);
            iconPackResources->Put(compName, CoreUtils::Convert(icon));
        }
    }
    *iconResMap = iconPackResources;
    REFCOUNT_ADD(*iconResMap);
    return NOERROR;
}

ECode IconPackHelper::IsIconPackLoaded(
    /* [out] */ Boolean* isLoaded)
{
    VALIDATE_NOT_NULL(isLoaded);
    *isLoaded = mLoadedIconPackResource != NULL &&
            !mLoadedIconPackName.IsNull() &&
            mIconPackResourceMap != NULL;
    return NOERROR;
}

Int32 IconPackHelper::GetResourceIdForDrawable(
    /* [in] */ const String& resource)
{
    Int32 resId;
    mLoadedIconPackResource->GetIdentifier(resource, String("drawable"), mLoadedIconPackName, &resId);
    return resId;
}

ECode IconPackHelper::GetResourceIdForActivityIcon(
    /* [in] */ IActivityInfo* info,
    /* [out] */ Int32* resId)
{
    VALIDATE_NOT_NULL(resId);
    Boolean isLoaded;
    if (IsIconPackLoaded(&isLoaded), !isLoaded) {
        *resId = 0;
        return NOERROR;
    }
    String pkgName, name;
    IPackageItemInfo::Probe(info)->GetPackageName(&pkgName);
    IPackageItemInfo::Probe(info)->GetName(&name);
    AutoPtr<IComponentName> compName;
    CComponentName::New(pkgName.ToLowerCase(), name.ToLowerCase(),
            (IComponentName**)&compName);
    AutoPtr<IInterface> obj;
    mIconPackResourceMap->Get(compName, (IInterface**)&obj);
    String drawable = CoreUtils::Unbox(ICharSequence::Probe(obj));
    if (!drawable.IsNull()) {
        Int32 id = GetResourceIdForDrawable(drawable);
        if (id != 0) {
            *resId = id;
            return NOERROR;
        }
    }

    // Icon pack doesn't have an icon for the activity, fallback to package icon
    compName = NULL;
    CComponentName::New(pkgName.ToLowerCase(), String(""), (IComponentName**)&compName);
    obj = NULL;
    mIconPackResourceMap->Get(compName, (IInterface**)&obj);
    drawable = CoreUtils::Unbox(ICharSequence::Probe(obj));
    if (drawable.IsNull()) {
        *resId = 0;
        return NOERROR;
    }
    *resId = GetResourceIdForDrawable(drawable);
    return NOERROR;
}

ECode IconPackHelper::GetResourceIdForApp(
    /* [in] */ const String& pkgName,
    /* [out] */ Int32* resId)
{
    VALIDATE_NOT_NULL(resId);
    AutoPtr<IActivityInfo> info;
    CActivityInfo::New((IActivityInfo**)&info);
    IPackageItemInfo::Probe(info)->SetPackageName(pkgName);
    IPackageItemInfo::Probe(info)->SetName(String(""));
    return GetResourceIdForActivityIcon(info, resId);
}

ECode IconPackHelper::GetDrawableForActivity(
    /* [in] */ IActivityInfo* info,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    Int32 id;
    GetResourceIdForActivityIcon(info, &id);
    if (id == 0) {
        *drawable = NULL;
        return NOERROR;
    }
    return mLoadedIconPackResource->GetDrawable(id, NULL, FALSE, drawable);
}

ECode IconPackHelper::GetDrawableForActivityWithDensity(
    /* [in] */ IActivityInfo* info,
    /* [in] */ Int32 density,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    Int32 id;
    GetResourceIdForActivityIcon(info, &id);
    if (id == 0) {
        *drawable = NULL;
        return NOERROR;
    }
    return mLoadedIconPackResource->GetDrawableForDensity(id, density, NULL, FALSE, drawable);
}

ECode IconPackHelper::ShouldComposeIcon(
    /* [in] */ IComposedIconInfo* iconInfo,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (iconInfo == NULL) return NOERROR;
    AutoPtr< ArrayOf<Int32> > iconBacks;
    iconInfo->GetIconBacks((ArrayOf<Int32>**)&iconBacks);
    if (iconBacks != NULL) {
        *result = TRUE;
        return NOERROR;
    }
    Int32 iconMask;
    iconInfo->GetIconMask(&iconMask);
    if (iconMask != 0) {
        *result = TRUE;
        return NOERROR;
    }
    Int32 iconUpon;
    iconInfo->GetIconUpon(&iconUpon);
    if (iconUpon != 0) {
        *result = TRUE;
        return NOERROR;
    }
    AutoPtr< ArrayOf<Float> > colorFilter;
    iconInfo->GetColorFilter((ArrayOf<Float>**)&colorFilter);
    if (colorFilter != NULL) {
        *result = TRUE;
        return NOERROR;
    }
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
