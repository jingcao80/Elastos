
#ifndef __ELASTOS_DROID_GRAPHICS_TYPEFACE_H__
#define __ELASTOS_DROID_GRAPHICS_TYPEFACE_H__

#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/FontListParser.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Content::Res::IAssetManager;
using Elastos::IO::IFile;
using Elastos::Utility::IMap;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Graphics {

/**
 * The Typeface class specifies the typeface and intrinsic style of a font.
 * This is used in the paint, along with optionally Paint settings like
 * textSize, textSkewX, textScaleX to specify
 * how text appears when drawn (and measured).
 */
class Typeface
    : public Object
    , public ITypeface
{
private:
    class StaticInitializer
    {
    public:
        StaticInitializer();
    };

public:
    CAR_INTERFACE_DECL();

    // don't allow clients to call this directly
    CARAPI constructor(
        /* [in] */ Int64 ni);

    /** Returns the typeface's intrinsic style attributes */
    virtual CARAPI GetStyle(
        /* [out] */ Int32* style);

    /** Returns true if getStyle() has the BOLD bit set. */
    CARAPI IsBold(
        /* [out] */ Boolean* result);

    /** Returns true if getStyle() has the ITALIC bit set. */
    CARAPI IsItalic(
        /* [out] */ Boolean* result);

    // @Override
    virtual CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* e);

    // @Override
    virtual CARAPI GetHashCode(
        /* [out] */ Int32* code);

    /**
     * Create a typeface object given a family name, and option style information.
     * If null is passed for the name, then the "default" font will be chosen.
     * The resulting typeface object can be queried (getStyle()) to discover what
     * its "real" style characteristics are.
     *
     * @param familyName May be null. The name of the font family.
     * @param style  The style (normal, bold, italic) of the typeface.
     *               e.g. NORMAL, BOLD, ITALIC, BOLD_ITALIC
     * @return The best matching typeface.
     */
    static CARAPI Create(
        /* [in] */ const String& familyName,
        /* [in] */ Int32 style,
        /* [out] */ ITypeface** typeface);

    /**
     * Create a typeface object that best matches the specified existing
     * typeface and the specified Style. Use this call if you want to pick a new
     * style from the same family of an existing typeface object. If family is
     * null, this selects from the default font's family.
     *
     * @param family May be null. The name of the existing type face.
     * @param style  The style (normal, bold, italic) of the typeface.
     *               e.g. NORMAL, BOLD, ITALIC, BOLD_ITALIC
     * @return The best matching typeface.
     */
    static CARAPI Create(
        /* [in] */ ITypeface* family,
        /* [in] */ Int32 style,
        /* [out] */ ITypeface** typeface);

    /**
     * Returns one of the default typeface objects, based on the specified style
     *
     * @return the default typeface that corresponds to the style
     */
    static CARAPI DefaultFromStyle(
        /* [in] */ Int32 style,
        /* [out] */ ITypeface** typeface);

    /**
     * Create a new typeface from the specified font data.
     * @param mgr The application's asset manager
     * @param path  The file name of the font data in the assets directory
     * @return The new typeface.
     */
    static CARAPI CreateFromAsset(
        /* [in] */ IAssetManager* mgr,
        /* [in] */ const String& path,
        /* [out] */ ITypeface** typeface);

    /**
     * Create a new typeface from the specified font file.
     *
     * @param path The path to the font data.
     * @return The new typeface.
     */
    static CARAPI CreateFromFile(
        /* [in] */ IFile* path,
        /* [out] */ ITypeface** typeface);

    /**
     * Create a new typeface from the specified font file.
     *
     * @param path The full path to the font data.
     * @return The new typeface.
     */
    static CARAPI CreateFromFile(
        /* [in]*/ const String& path,
        /* [out] */ ITypeface** typeface);

    /**
     * Create a new typeface from an array of font families.
     *
     * @param families array of font families
     * @hide
     */
    static CARAPI CreateFromFamilies(
        /* [in]*/ ArrayOf<IFontFamily*>* families,
        /* [out]*/ ITypeface** typeface);

    /**
     * Create a new typeface from an array of font families, including
     * also the font families in the fallback list.
     *
     * @param families array of font families
     * @hide
     */
    static CARAPI CreateFromFamiliesWithDefault(
        /* [in]*/ ArrayOf<IFontFamily*>* families,
        /* [out]*/ ITypeface** typeface);

protected:
    Typeface();

    virtual ~Typeface();

private:
    static CARAPI_(void) SetDefault(
        /* [in] */ ITypeface* t);

    static CARAPI_(Int64) NativeCreateFromTypeface(
        /* [in] */ Int64 native_instance,
        /* [in] */ Int32 style);

    static CARAPI_(Int64) NativeCreateWeightAlias(
        /* [in] */ Int64 native_instance,
        /* [in] */ Int32 weight);

    static CARAPI_(void) NativeUnref(
        /* [in] */ Int64 native_instance);

    static CARAPI_(Int32) NativeGetStyle(
        /* [in] */ Int64 native_instance);

    static CARAPI_(Int64) NativeCreateFromArray(
        /* [in] */ ArrayOf<Int64>* familyArray);

    static CARAPI_(void) NativeSetDefault(
        /* [in] */ Int64 native_instance);

    static CARAPI_(AutoPtr<IFontFamily>) MakeFamilyFromParsed(
        /* [in] */ FontListParser::Family* family);

    /*
     * (non-Javadoc)
     *
     * This should only be called once, from the static class initializer block.
     */
    static CARAPI_(void) Init();

    static CARAPI_(AutoPtr<IFile>) GetSystemFontConfigLocation();

public:
    static const String TAG;

    /** The default NORMAL typeface object */
    static AutoPtr<ITypeface> DEFAULT;
    /**
     * The default BOLD typeface object. Note: this may be not actually be
     * bold, depending on what fonts are installed. Call getStyle() to know
     * for sure.
     */
    static AutoPtr<ITypeface> DEFAULT_BOLD;
    /** The NORMAL style of the default sans serif typeface. */
    static AutoPtr<ITypeface> SANS_SERIF;
    /** The NORMAL style of the default serif typeface. */
    static AutoPtr<ITypeface> SERIF;
    /** The NORMAL style of the default monospace typeface. */
    static AutoPtr<ITypeface> MONOSPACE;

    static AutoPtr< ArrayOf<ITypeface*> > sDefaults;

    typedef HashMap<Int32, AutoPtr<ITypeface> > TypefaceMap;
    static HashMap<Int32, AutoPtr<TypefaceMap> > sTypefaceCache;

    static AutoPtr<ITypeface> sDefaultTypeface;
    static HashMap<String, AutoPtr<ITypeface> > sSystemFontMap;
    static AutoPtr<ArrayOf<IFontFamily*> > sFallbackFonts;

    static const String FONTS_CONFIG;

    /* package */
    Int64 mNativeInstance;

private:
    Int32 mStyle;

    static const StaticInitializer sInitializer;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_TYPEFACE_H__
