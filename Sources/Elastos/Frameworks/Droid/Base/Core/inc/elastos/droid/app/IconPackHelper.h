
#ifndef __ELASTOS_DROID_APP_ICONPACKHELPER_H__
#define __ELASTOS_DROID_APP_ICONPACKHELPER_H__

#include "_Elastos.Droid.App.h"
#include "_Elastos.Droid.Content.h"
#include "_Elastos.Droid.Graphics.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IIThemeService;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IColorMatrix;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Core::Object;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;
using Elastos::Utility::IRandom;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace App {

class IconPackHelper
    : public Object
    , public IIconPackHelper
{
public:
    class IconCustomizer
    {
    public:
        static CARAPI GetComposedIconDrawable(
            /* [in] */ IDrawable* icon,
            /* [in] */ IContext* context,
            /* [in] */ IComposedIconInfo* iconInfo,
            /* [out] */ IDrawable** drawable);

        static CARAPI GetComposedIconDrawable(
            /* [in] */ IDrawable* icon,
            /* [in] */ IResources* res,
            /* [in] */ IComposedIconInfo* iconInfo,
            /* [out] */ IDrawable** drawable);

        static CARAPI GetValue(
            /* [in] */ IResources* res,
            /* [in] */ Int32 resId,
            /* [in] */ ITypedValue* outValue,
            /* [in] */ IDrawable* baseIcon);

    private:
        static CARAPI_(AutoPtr<IBitmap>) CreateIconBitmap(
            /* [in] */ IDrawable* icon,
            /* [in] */ IResources* res,
            /* [in] */ Int32 iconBack,
            /* [in] */ Int32 iconMask,
            /* [in] */ Int32 iconUpon,
            /* [in] */ Float scale,
            /* [in] */ Int32 iconSize,
            /* [in] */ ArrayOf<Float>* colorFilter);

        static CARAPI_(Boolean) CacheComposedIcon(
            /* [in] */ IBitmap* bmp,
            /* [in] */ const String& path);

        static CARAPI_(String) GetCachedIconPath(
            /* [in] */ const String& pkgName,
            /* [in] */ Int32 resId,
            /* [in] */ Int32 density);

        static CARAPI_(String) GetCachedIconName(
            /* [in] */ const String& pkgName,
            /* [in] */ Int32 resId,
            /* [in] */ Int32 density);

        static CARAPI_(AutoPtr<IRandom>) Init_sRandom();

        static CARAPI_(AutoPtr<IIThemeService>) Init_sThemeService();

    private:
        static const AutoPtr<IRandom> sRandom;
        static const AutoPtr<IIThemeService> sThemeService;
    };

    class ColorFilterUtils
    {
    public:
        class Builder
            : public Object
            , public IColorFilterUtilsBuilder
        {
        public:
            CAR_INTERFACE_DECL()

            CARAPI constructor();

            CARAPI Hue(
                /* [in] */ Float value);

            CARAPI Saturate(
                /* [in] */ Float saturation);

            CARAPI Brightness(
                /* [in] */ Float brightness);

            CARAPI Contrast(
                /* [in] */ Float contrast);

            CARAPI Alpha(
                /* [in] */ Float alpha);

            CARAPI InvertColors();

            CARAPI Tint(
                /* [in] */ Int32 color);

            CARAPI Build(
                /* [out] */ IColorMatrix** result);

        private:
            AutoPtr<IList> mMatrixList;
        };

    public:
        static CARAPI ParseIconFilter(
            /* [in] */ IXmlPullParser* parser,
            /* [in] */ IColorFilterUtilsBuilder* builder,
            /* [out] */ Boolean* result);

        /**
         * See the following links for reference
         * http://groups.google.com/group/android-developers/browse_thread/thread/9e215c83c3819953
         * http://gskinner.com/blog/archives/2007/12/colormatrix_cla.html
         * @param value
         */
        static CARAPI AdjustHue(
            /* [in] */ Float value,
            /* [out] */ IColorMatrix** result);

        static CARAPI AdjustSaturation(
            /* [in] */ Float saturation,
            /* [out] */ IColorMatrix** result);

        static CARAPI InvertColors(
            /* [out] */ IColorMatrix** result);

        static CARAPI AdjustBrightness(
            /* [in] */ Float brightness,
            /* [out] */ IColorMatrix** result);

        static CARAPI AdjustContrast(
            /* [in] */ Float contrast,
            /* [out] */ IColorMatrix** result);

        static CARAPI AdjustAlpha(
            /* [in] */ Float alpha,
            /* [out] */ IColorMatrix** result);

        static CARAPI ApplyTint(
            /* [in] */ Int32 color,
            /* [out] */ IColorMatrix** result);

    private:
        static CARAPI_(Int32) GetInt32(
            /* [in] */ const String& value,
            /* [in] */ Int32 defaultValue);

        static CARAPI_(Int32) ClampValue(
            /* [in] */ Int32 value,
            /* [in] */ Int32 min,
            /* [in] */ Int32 max);

    private:
        static const String TAG_FILTER;
        static const String FILTER_HUE;
        static const String FILTER_SATURATION;
        static const String FILTER_INVERT;
        static const String FILTER_BRIGHTNESS;
        static const String FILTER_CONTRAST;
        static const String FILTER_ALPHA;
        static const String FILTER_TINT;

        static const Int32 MIN_HUE;
        static const Int32 MAX_HUE;
        static const Int32 MIN_SATURATION;
        static const Int32 MAX_SATURATION;
        static const Int32 MIN_BRIGHTNESS;
        static const Int32 MAX_BRIGHTNESS;
        static const Int32 MIN_CONTRAST;
        static const Int32 MAX_CONTRAST;
        static const Int32 MIN_ALPHA;
        static const Int32 MAX_ALPHA;
    };

private:
    class StaticInitializer
    {
    public:
        StaticInitializer();
    };

public:
    IconPackHelper();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI LoadIconPack(
        /* [in] */ const String& packageName);

    CARAPI GetComposedIconInfo(
        /* [out] */ IComposedIconInfo** iconInfo);

    static CARAPI CreateIconResource(
        /* [in] */ IContext* context,
        /* [in] */ const String& packageName,
        /* [out] */ IResources** iconRes);

    CARAPI GetIconResMapFromXml(
        /* [in] */ IResources* res,
        /* [in] */ const String& packageName,
        /* [out] */ IMap** iconResMap);

    CARAPI IsIconPackLoaded(
        /* [out] */ Boolean* isLoaded);

    CARAPI GetResourceIdForActivityIcon(
        /* [in] */ IActivityInfo* info,
        /* [out] */ Int32* resId);

    CARAPI GetResourceIdForApp(
        /* [in] */ const String& pkgName,
        /* [out] */ Int32* resId);

    CARAPI GetDrawableForActivity(
        /* [in] */ IActivityInfo* info,
        /* [out] */ IDrawable** drawable);

    CARAPI GetDrawableForActivityWithDensity(
        /* [in] */ IActivityInfo* info,
        /* [in] */ Int32 density,
        /* [out] */ IDrawable** drawable);

    static CARAPI ShouldComposeIcon(
        /* [in] */ IComposedIconInfo* iconInfo,
        /* [out] */ Boolean* result);

private:
    CARAPI LoadResourcesFromXmlParser(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IMap* iconPackResources);

    CARAPI_(Boolean) IsComposedIconComponent(
        /* [in] */ const String& tag);

    CARAPI_(Boolean) ParseComposedIconComponent(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IMap* iconPackResources);

    CARAPI_(void) LoadComposedIconComponents();

    CARAPI_(Int32) GetResourceIdForName(
        /* [in] */ IComponentName* component);

    CARAPI_(Int32) GetResourceIdForDrawable(
        /* [in] */ const String& resource);

private:
    static const String TAG;
    static const String ICON_MASK_TAG;
    static const String ICON_BACK_TAG;
    static const String ICON_UPON_TAG;
    static const String ICON_SCALE_TAG;
    static const String ICON_BACK_FORMAT;

    static AutoPtr<IComponentName> ICON_BACK_COMPONENT;
    static AutoPtr<IComponentName> ICON_MASK_COMPONENT;
    static AutoPtr<IComponentName> ICON_UPON_COMPONENT;
    static AutoPtr<IComponentName> ICON_SCALE_COMPONENT;

    static const Float DEFAULT_SCALE;
    static const Int32 COMPOSED_ICON_COOKIE;

    AutoPtr<IContext> mContext;
    AutoPtr<IMap> mIconPackResourceMap;
    String mLoadedIconPackName;
    AutoPtr<IResources> mLoadedIconPackResource;
    AutoPtr<IComposedIconInfo> mComposedIconInfo;
    Int32 mIconBackCount;
    AutoPtr<IColorFilterUtilsBuilder> mFilterBuilder;
    static StaticInitializer sInitializer;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_ICONPACKHELPER_H__
