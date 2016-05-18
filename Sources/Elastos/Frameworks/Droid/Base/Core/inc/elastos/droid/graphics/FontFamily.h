
#ifndef __ELASTOS_DROID_GRAPHICS_FONTFAMILY_H__
#define __ELASTOS_DROID_GRAPHICS_FONTFAMILY_H__

#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::Res::IAssetManager;

namespace Elastos {
namespace Droid {
namespace Graphics {

/**
 * A family of typefaces with different styles.
 *
 * @hide
 */
class FontFamily
    : public Object
    , public IFontFamily
{
public:
    CAR_INTERFACE_DECL()

    FontFamily();

    FontFamily(
        /* [in] */ const String& lang,
        /* [in] */ const String& variant);

    ~FontFamily();

    CARAPI AddFont(
        /* [in] */ const String& path,
        /* [out] */ Boolean* result);

    CARAPI AddFontWeightStyle(
        /* [in] */ const String& path,
        /* [in] */ Int32 weight,
        /* [in] */ Boolean style,
        /* [out] */ Boolean* result);

    CARAPI AddFontFromAsset(
        /* [in] */ IAssetManager* mgr,
        /* [in] */ const String& path,
        /* [out] */ Boolean* result);

private:
    static CARAPI_(Int64) NativeCreateFamily(
        /* [in] */ const String& lang,
        /* [in] */ Int32 variant);

    static CARAPI_(void) NativeUnrefFamily(
        /* [in] */ Int64 nativePtr);

    static CARAPI_(Boolean) NativeAddFont(
        /* [in] */ Int64 nativeFamily,
        /* [in] */ const String& path);

    static CARAPI_(Boolean) NativeAddFontWeightStyle(
        /* [in] */ Int64 nativeFamily,
        /* [in] */ const String& path,
        /* [in] */ Int32 weight,
        /* [in] */ Boolean isItalic);

    static CARAPI_(Boolean) NativeAddFontFromAsset(
        /* [in] */ Int64 nativeFamily,
        /* [in] */ IAssetManager* mgr,
        /* [in] */ const String& path);

public:
    /**
     * @hide
     */
    Int64 mNativePtr;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_FONTFAMILY_H__
