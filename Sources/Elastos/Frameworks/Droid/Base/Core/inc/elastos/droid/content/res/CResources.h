
#ifndef __ELASTOS_DROID_CONTENT_RES_CRESOURCES_H__
#define __ELASTOS_DROID_CONTENT_RES_CRESOURCES_H__

#include "_Elastos_Droid_Content_Res_CResources.h"
#include "elastos/droid/content/res/CAssetManager.h"
#include "elastos/droid/content/res/XmlBlock.h"
#include "elastos/droid/content/res/CConfiguration.h"
#include "elastos/droid/content/res/CCompatibilityInfo.h"
#include "elastos/droid/content/res/CTypedArray.h"
#include "elastos/droid/utility/CDisplayMetrics.h"
#include "elastos/droid/utility/CTypedValue.h"
#include "elastos/droid/utility/Pools.h"
#include <elastos/utility/etl/List.h>

using Elastos::Droid::App::IComposedIconInfo;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Graphics::IMovie;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::Drawable::IDrawableConstantState;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Utility::IInt64SparseArray;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Droid::Utility::CDisplayMetrics;
using Elastos::Droid::Utility::Pools;
using Elastos::Utility::Etl::List;
using Elastos::Utility::IHashMap;
using Libcore::ICU::INativePluralRules;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

/**
 * Class for accessing an application's resources.  This sits on top of the
 * asset manager of the application (accessible through {@link #getAssets}) and
 * provides a high-level API for getting typed data from the assets.
 *
 * <p>The Android resource system keeps track of all non-code assets associated with an
 * application. You can use this class to access your application's resources. You can generally
 * acquire the {@link android.content.res.Resources} instance associated with your application
 * with {@link android.content.Context#getResources getResources()}.</p>
 *
 * <p>The Android SDK tools compile your application's resources into the application binary
 * at build time.  To use a resource, you must install it correctly in the source tree (inside
 * your project's {@code res/} directory) and build your application.  As part of the build
 * process, the SDK tools generate symbols for each resource, which you can use in your application
 * code to access the resources.</p>
 *
 * <p>Using application resources makes it easy to update various characteristics of your
 * application without modifying code, and&mdash;by providing sets of alternative
 * resources&mdash;enables you to optimize your application for a variety of device configurations
 * (such as for different languages and screen sizes). This is an important aspect of developing
 * Android applications that are compatible on different types of devices.</p>
 *
 * <p>For more information about using resources, see the documentation about <a
 * href="{@docRoot}guide/topics/resources/index.html">Application Resources</a>.</p>
 */
CarClass(CResources)
    , public Object
    , public IResources
{
public:
    // Information about preloaded resources.  Note that they are not
    // protected by a lock, because while preloading in zygote we are all
    // single-threaded, and after that these are immutable.
    // private final ArrayMap<String, LongSparseArray<WeakReference<ConstantState>>> mDrawableCache
    // private final ArrayMap<String, LongSparseArray<WeakReference<ConstantState>>> mColorStateListCache
    // private final LongSparseArray<WeakReference<ColorStateList>> mColorDrawableCache
    typedef HashMap<String, AutoPtr<IInt64SparseArray> > DrawableMap;
    typedef typename DrawableMap::Iterator DrawableMapIterator;

    typedef HashMap<Int64, AutoPtr<IWeakReference> > ColorStateMap;
    typedef typename HashMap<Int64, AutoPtr<IWeakReference> >::Iterator ColorStateIterator;

public:
    class Theme
        : public Object
        , public IResourcesTheme
    {
    public:
        CAR_INTERFACE_DECL()

        Theme(
            /* [in] */ CResources* host);

        virtual ~Theme();

        CARAPI ApplyStyle(
            /* [in] */ Int32 resid,
            /* [in] */ Boolean force);

        CARAPI SetTo(
            /* [in] */ IResourcesTheme* other);

        CARAPI ObtainStyledAttributes(
            /* [in] */ ArrayOf<Int32>* attrs,
            /* [out] */ ITypedArray** styles);

        CARAPI ObtainStyledAttributes(
            /* [in] */ Int32 resid,
            /* [in] */ ArrayOf<Int32>* attrs,
            /* [out] */ ITypedArray** styles);

        CARAPI ObtainStyledAttributes(
            /* [in] */ IAttributeSet* set,
            /* [in] */ ArrayOf<Int32>* attrs,
            /* [in] */ Int32 defStyleAttr,
            /* [in] */ Int32 defStyleRes,
            /* [out] */ ITypedArray** styles);

       /**
         * Retrieve the values for a set of attributes in the Theme. The
         * contents of the typed array are ultimately filled in by
         * {@link Resources#getValue}.
         *
         * @param values The base set of attribute values, must be equal
         *               in length to {@code attrs} or {@code null}. All values
         *               must be of type {@link TypedValue#TYPE_ATTRIBUTE}.
         * @param attrs The desired attributes to be retrieved.
         * @return Returns a TypedArray holding an array of the attribute
         *         values. Be sure to call {@link TypedArray#recycle()}
         *         when done with it.
         * @hide
         */
        CARAPI ResolveAttribute(
            /* [in] */ ArrayOf<Int32>* values,
            /* [in] */ ArrayOf<Int32>* attrs,
            /* [out] */ ITypedArray** isFounded);

        CARAPI ResolveAttribute(
            /* [in] */ Int32 resid,
            /* [in, out] */ ITypedValue* outValue,
            /* [in] */ Boolean resolveRefs,
            /* [out] */ Boolean* isFounded);

       /**
         * Gets all of the attribute ids associated with this {@link Theme}. For debugging only.
         *
         * @return The int array containing attribute ids associated with this {@link Theme}.
         * @hide
         */
        CARAPI GetAllAttributes(
            /* [out, callee] */ ArrayOf<Int32>** attrs);

        /**
         * Returns the resources to which this theme belongs.
         *
         * @return Resources to which this theme belongs.
         */
        CARAPI GetResources(
            /* [out] */ IResources** res);

        /**
         * Return a drawable object associated with a particular resource ID
         * and styled for the Theme.
         *
         * @param id The desired resource identifier, as generated by the aapt
         *           tool. This integer encodes the package, type, and resource
         *           entry. The value 0 is an invalid identifier.
         * @return Drawable An object that can be used to draw this resource.
         * @throws NotFoundException Throws NotFoundException if the given ID
         *         does not exist.
         */
         CARAPI GetDrawable(
            /* [in] */ Int32 id,
            /* [out] */ IDrawable** drawable);

        CARAPI Dump(
            /* [in] */ Int32 priority,
            /* [in] */ const String& tag,
            /* [in] */ const String& prefix);

        // Needed by layoutlib.
        /*package*/ Int64 GetNativeTheme();

        /*package*/ Int32 GetAppliedStyleResId();

        /*package*/ String GetKey();

        String GetResourceNameFromHexString(
            /* [in] */ const String& hexString);

        CARAPI GetTheme(
            /* [out, callee] */ ArrayOf<String>** themes);

    private:
        friend class CResources;

        AutoPtr<CResources> mHost;
        AutoPtr<CAssetManager> mAssets;
        Int64 mTheme;

        /** Resource identifier for the theme. */
        Int32 mThemeResId;

        /** Unique key for the series of styles applied to this theme. */
        String mKey;
    };

private:
    class StaticInitializer
    {
    public:
        StaticInitializer();
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CResources();

    virtual ~CResources();

    CARAPI constructor();

    /**
     * Create a new Resources object on top of an existing set of assets in an
     * AssetManager.
     *
     * @param assets Previously created AssetManager.
     * @param metrics Current display metrics to consider when
     *                selecting/computing resource values.
     * @param config Desired device configuration to consider when
     *               selecting/computing resource values (optional).
     */
    CARAPI constructor(
        /* [in] */ IAssetManager* assets,
        /* [in] */ IDisplayMetrics* metrics,
        /* [in] */ IConfiguration* config);

    /**
     * Creates a new Resources object with CompatibilityInfo.
     *
     * @param assets Previously created AssetManager.
     * @param metrics Current display metrics to consider when
     *                selecting/computing resource values.
     * @param config Desired device configuration to consider when
     *               selecting/computing resource values (optional).
     * @param compatInfo this resource's compatibility info. Must not be null.
     * @param token The Activity token for determining stack affiliation. Usually null.
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IAssetManager* assets,
        /* [in] */ IDisplayMetrics* metrics,
        /* [in] */ IConfiguration* config,
        /* [in] */ ICompatibilityInfo* compInfo,
        /* [in] */ IBinder* token);

    /**
     * Returns the most appropriate default theme for the specified target SDK version.
     * <ul>
     * <li>Below API 11: Gingerbread
     * <li>APIs 11 thru 14: Holo
     * <li>APIs 14 thru XX: Device default dark
     * <li>API XX and above: Device default light with dark action bar
     * </ul>
     *
     * @param curTheme The current theme, or 0 if not specified.
     * @param targetSdkVersion The target SDK version.
     * @return A theme resource identifier
     * @hide
     */
    static CARAPI_(Int32) SelectDefaultTheme(
        /* [in] */ Int32 curTheme,
        /* [in] */ Int32 targetSdkVersion);

    static CARAPI_(Int32) SelectSystemTheme(
        /* [in] */ Int32 curTheme,
        /* [in] */ Int32 targetSdkVersion,
        /* [in] */ Int32 orig,
        /* [in] */ Int32 holo,
        /* [in] */ Int32 dark,
        /* [in] */ Int32 deviceDefault);

    /**
     * Return a global shared Resources object that provides access to only
     * system resources (no application resources), and is not configured for
     * the current screen (can not use dimension units, does not change based
     * on orientation, etc).
     */
    static CARAPI_(AutoPtr<IResources>) GetSystem();

    /**
     * Return the string value associated with a particular resource ID.  The
     * returned object will be a String if this is a plain string; it will be
     * some other type of CharSequence if it is styled.
     * {@more}
     *
     * @param id The desired resource identifier, as generated by the aapt
     *           tool. This integer encodes the package, type, and resource
     *           entry. The value 0 is an invalid identifier.
     *
     * @throws NotFoundException Throws NotFoundException if the given ID does not exist.
     *
     * @return CharSequence The string data associated with the resource, plus
     *         possibly styled text information.
     */
    CARAPI GetText(
        /* [in] */ Int32 id,
        /* [out] */ ICharSequence** seq);

    /**
     * Return the character sequence associated with a particular resource ID for a particular
     * numerical quantity.
     *
     * <p>See <a href="{@docRoot}guide/topics/resources/string-resource.html#Plurals">String
     * Resources</a> for more on quantity strings.
     *
     * @param id The desired resource identifier, as generated by the aapt
     *           tool. This integer encodes the package, type, and resource
     *           entry. The value 0 is an invalid identifier.
     * @param quantity The number used to get the correct string for the current language's
     *           plural rules.
     *
     * @throws NotFoundException Throws NotFoundException if the given ID does not exist.
     *
     * @return CharSequence The string data associated with the resource, plus
     *         possibly styled text information.
     */
    CARAPI GetQuantityText(
        /* [in] */ Int32 id,
        /* [in] */ Int32 quantity,
        /* [out] */ ICharSequence** seq);

    /**
     * Return the string value associated with a particular resource ID.  It
     * will be stripped of any styled text information.
     * {@more}
     *
     * @param id The desired resource identifier, as generated by the aapt
     *           tool. This integer encodes the package, type, and resource
     *           entry. The value 0 is an invalid identifier.
     *
     * @throws NotFoundException Throws NotFoundException if the given ID does not exist.
     *
     * @return String The string data associated with the resource,
     * stripped of styled text information.
     */
    CARAPI GetString(
        /* [in] */ Int32 id,
        /* [out] */ String* str);

    /**
     * Return the string value associated with a particular resource ID,
     * substituting the format arguments as defined in {@link java.util.Formatter}
     * and {@link java.lang.String#format}. It will be stripped of any styled text
     * information.
     * {@more}
     *
     * @param id The desired resource identifier, as generated by the aapt
     *           tool. This integer encodes the package, type, and resource
     *           entry. The value 0 is an invalid identifier.
     *
     * @param formatArgs The format arguments that will be used for substitution.
     *
     * @throws NotFoundException Throws NotFoundException if the given ID does not exist.
     *
     * @return String The string data associated with the resource,
     * stripped of styled text information.
     */
    CARAPI GetString(
        /* [in] */ Int32 id,
        /* [in] */ ArrayOf<IInterface*>* formatArgs,
        /* [out] */ String* str);

    /**
     * Return the string value associated with a particular resource ID for a particular
     * numerical quantity, substituting the format arguments as defined in
     * {@link java.util.Formatter} and {@link java.lang.String#format}. It will be
     * stripped of any styled text information.
     * {@more}
     *
     * <p>See <a href="{@docRoot}guide/topics/resources/string-resource.html#Plurals">String
     * Resources</a> for more on quantity strings.
     *
     * @param id The desired resource identifier, as generated by the aapt
     *           tool. This integer encodes the package, type, and resource
     *           entry. The value 0 is an invalid identifier.
     * @param quantity The number used to get the correct string for the current language's
     *           plural rules.
     * @param formatArgs The format arguments that will be used for substitution.
     *
     * @throws NotFoundException Throws NotFoundException if the given ID does not exist.
     *
     * @return String The string data associated with the resource,
     * stripped of styled text information.
     */
    CARAPI GetQuantityString(
        /* [in] */ Int32 id,
        /* [in] */ Int32 quantity,
        /* [in] */ ArrayOf<IInterface*>* formatArgs,
        /* [out] */ String* str);

    /**
     * Return the string value associated with a particular resource ID for a particular
     * numerical quantity.
     *
     * <p>See <a href="{@docRoot}guide/topics/resources/string-resource.html#Plurals">String
     * Resources</a> for more on quantity strings.
     *
     * @param id The desired resource identifier, as generated by the aapt
     *           tool. This integer encodes the package, type, and resource
     *           entry. The value 0 is an invalid identifier.
     * @param quantity The number used to get the correct string for the current language's
     *           plural rules.
     *
     * @throws NotFoundException Throws NotFoundException if the given ID does not exist.
     *
     * @return String The string data associated with the resource,
     * stripped of styled text information.
     */
    CARAPI GetQuantityString(
        /* [in] */ Int32 id,
        /* [in] */ Int32 quantity,
        /* [out] */ String* str);

    /**
     * Return the string value associated with a particular resource ID.  The
     * returned object will be a String if this is a plain string; it will be
     * some other type of CharSequence if it is styled.
     *
     * @param id The desired resource identifier, as generated by the aapt
     *           tool. This integer encodes the package, type, and resource
     *           entry. The value 0 is an invalid identifier.
     *
     * @param def The default CharSequence to return.
     *
     * @return CharSequence The string data associated with the resource, plus
     *         possibly styled text information, or def if id is 0 or not found.
     */
    CARAPI GetText(
        /* [in] */ Int32 id,
        /* [in] */ ICharSequence* def,
        /* [out] */ ICharSequence** seq);

    /**
     * Return the styled text array associated with a particular resource ID.
     *
     * @param id The desired resource identifier, as generated by the aapt
     *           tool. This integer encodes the package, type, and resource
     *           entry. The value 0 is an invalid identifier.
     *
     * @throws NotFoundException Throws NotFoundException if the given ID does not exist.
     *
     * @return The styled text array associated with the resource.
     */
    CARAPI GetTextArray(
        /* [in] */ Int32 id,
        /* [out, callee] */ ArrayOf<ICharSequence*>** seqs);

    /**
     * Return the string array associated with a particular resource ID.
     *
     * @param id The desired resource identifier, as generated by the aapt
     *           tool. This integer encodes the package, type, and resource
     *           entry. The value 0 is an invalid identifier.
     *
     * @throws NotFoundException Throws NotFoundException if the given ID does not exist.
     *
     * @return The string array associated with the resource.
     */
    CARAPI GetStringArray(
        /* [in] */ Int32 id,
        /* [out, callee] */ ArrayOf<String>** strs);

    /**
     * Return the int array associated with a particular resource ID.
     *
     * @param id The desired resource identifier, as generated by the aapt
     *           tool. This integer encodes the package, type, and resource
     *           entry. The value 0 is an invalid identifier.
     *
     * @throws NotFoundException Throws NotFoundException if the given ID does not exist.
     *
     * @return The int array associated with the resource.
     */
    CARAPI GetInt32Array(
        /* [in] */ Int32 id,
        /* [out, callee] */ ArrayOf<Int32>** ints);

    /**
     * Return an array of heterogeneous values.
     *
     * @param id The desired resource identifier, as generated by the aapt
     *           tool. This integer encodes the package, type, and resource
     *           entry. The value 0 is an invalid identifier.
     *
     * @throws NotFoundException Throws NotFoundException if the given ID does not exist.
     *
     * @return Returns a TypedArray holding an array of the array values.
     * Be sure to call {@link TypedArray#recycle() TypedArray.recycle()}
     * when done with it.
     */
    CARAPI ObtainTypedArray(
        /* [in] */ Int32 id,
        /* [out] */ ITypedArray** array);

    /**
     * Retrieve a dimensional for a particular resource ID.  Unit
     * conversions are based on the current {@link DisplayMetrics} associated
     * with the resources.
     *
     * @param id The desired resource identifier, as generated by the aapt
     *           tool. This integer encodes the package, type, and resource
     *           entry. The value 0 is an invalid identifier.
     *
     * @return Resource dimension value multiplied by the appropriate
     * metric.
     *
     * @throws NotFoundException Throws NotFoundException if the given ID does not exist.
     *
     * @see #getDimensionPixelOffset
     * @see #getDimensionPixelSize
     */
    CARAPI GetDimension(
        /* [in] */ Int32 id,
        /* [out] */ Float* dim);

    /**
     * Retrieve a dimensional for a particular resource ID for use
     * as an offset in raw pixels.  This is the same as
     * {@link #getDimension}, except the returned value is converted to
     * integer pixels for you.  An offset conversion involves simply
     * truncating the base value to an integer.
     *
     * @param id The desired resource identifier, as generated by the aapt
     *           tool. This integer encodes the package, type, and resource
     *           entry. The value 0 is an invalid identifier.
     *
     * @return Resource dimension value multiplied by the appropriate
     * metric and truncated to integer pixels.
     *
     * @throws NotFoundException Throws NotFoundException if the given ID does not exist.
     *
     * @see #getDimension
     * @see #getDimensionPixelSize
     */
    CARAPI GetDimensionPixelOffset(
        /* [in] */ Int32 id,
        /* [out] */ Int32* offset);

    /**
     * Retrieve a dimensional for a particular resource ID for use
     * as a size in raw pixels.  This is the same as
     * {@link #getDimension}, except the returned value is converted to
     * integer pixels for use as a size.  A size conversion involves
     * rounding the base value, and ensuring that a non-zero base value
     * is at least one pixel in size.
     *
     * @param id The desired resource identifier, as generated by the aapt
     *           tool. This integer encodes the package, type, and resource
     *           entry. The value 0 is an invalid identifier.
     *
     * @return Resource dimension value multiplied by the appropriate
     * metric and truncated to integer pixels.
     *
     * @throws NotFoundException Throws NotFoundException if the given ID does not exist.
     *
     * @see #getDimension
     * @see #getDimensionPixelOffset
     */
    CARAPI GetDimensionPixelSize(
        /* [in] */ Int32 id,
        /* [out] */ Int32* size);

    /**
     * Retrieve a fractional unit for a particular resource ID.
     *
     * @param id The desired resource identifier, as generated by the aapt
     *           tool. This integer encodes the package, type, and resource
     *           entry. The value 0 is an invalid identifier.
     * @param base The base value of this fraction.  In other words, a
     *             standard fraction is multiplied by this value.
     * @param pbase The parent base value of this fraction.  In other
     *             words, a parent fraction (nn%p) is multiplied by this
     *             value.
     *
     * @return Attribute fractional value multiplied by the appropriate
     * base value.
     *
     * @throws NotFoundException Throws NotFoundException if the given ID does not exist.
     */
    CARAPI GetFraction(
        /* [in] */ Int32 id,
        /* [in] */ Int32 base,
        /* [in] */ Int32 pbase,
        /* [out] */ Float* fraction);

    /**
     * Return a drawable object associated with a particular resource ID.
     * Various types of objects will be returned depending on the underlying
     * resource -- for example, a solid color, PNG image, scalable image, etc.
     * The Drawable API hides these implementation details.
     *
     * <p class="note"><strong>Note:</strong> Prior to
     * {@link android.os.Build.VERSION_CODES#JELLY_BEAN}, this function
     * would not correctly retrieve the final configuration density when
     * the resource ID passed here is an alias to another Drawable resource.
     * This means that if the density configuration of the alias resource
     * is different than the actual resource, the density of the returned
     * Drawable would be incorrect, resulting in bad scaling.  To work
     * around this, you can instead retrieve the Drawable through
     * {@link TypedArray#getDrawable TypedArray.getDrawable}.  Use
     * {@link android.content.Context#obtainStyledAttributes(int[])
     * Context.obtainStyledAttributes} with
     * an array containing the resource ID of interest to create the TypedArray.</p>
     *
     * @param id The desired resource identifier, as generated by the aapt
     *           tool. This integer encodes the package, type, and resource
     *           entry. The value 0 is an invalid identifier.
     *
     * @throws NotFoundException Throws NotFoundException if the given ID does not exist.
     *
     * @return Drawable An object that can be used to draw this resource.
     */
    CARAPI GetDrawable(
        /* [in] */ Int32 id,
        /* [out] */ IDrawable** drawable);

    /**
     * Return a drawable object associated with a particular resource ID and
     * styled for the specified theme. Various types of objects will be
     * returned depending on the underlying resource -- for example, a solid
     * color, PNG image, scalable image, etc.
     *
     * @param id The desired resource identifier, as generated by the aapt
     *           tool. This integer encodes the package, type, and resource
     *           entry. The value 0 is an invalid identifier.
     * @param theme The theme used to style the drawable attributes, may be {@code null}.
     * @return Drawable An object that can be used to draw this resource.
     * @throws NotFoundException Throws NotFoundException if the given ID does
     *         not exist.
     */
    CARAPI GetDrawable(
        /* [in] */ Int32 index,
        /* [in] */ IResourcesTheme* theme,
        /* [out] */ IDrawable** drawable);

    CARAPI GetDrawable(
        /* [in] */ Int32 id,
        /* [in] */ IResourcesTheme* theme,
        /* [in] */ Boolean supportComposedIcons,
        /* [out] */ IDrawable** drawable);

    /**
     * Return a drawable object associated with a particular resource ID for the
     * given screen density in DPI. This will set the drawable's density to be
     * the device's density multiplied by the ratio of actual drawable density
     * to requested density. This allows the drawable to be scaled up to the
     * correct size if needed. Various types of objects will be returned
     * depending on the underlying resource -- for example, a solid color, PNG
     * image, scalable image, etc. The Drawable API hides these implementation
     * details.
     *
     * @param id The desired resource identifier, as generated by the aapt tool.
     *            This integer encodes the package, type, and resource entry.
     *            The value 0 is an invalid identifier.
     * @param density the desired screen density indicated by the resource as
     *            found in {@link DisplayMetrics}.
     * @throws NotFoundException Throws NotFoundException if the given ID does
     *             not exist.
     * @return Drawable An object that can be used to draw this resource.
     */
    CARAPI GetDrawableForDensity(
        /* [in] */ Int32 id,
        /* [in] */ Int32 density,
        /* [out] */ IDrawable** drawable);

    /**
     * Return a drawable object associated with a particular resource ID for the
     * given screen density in DPI and styled for the specified theme.
     *
     * @param id The desired resource identifier, as generated by the aapt tool.
     *            This integer encodes the package, type, and resource entry.
     *            The value 0 is an invalid identifier.
     * @param density The desired screen density indicated by the resource as
     *            found in {@link DisplayMetrics}.
     * @param theme The theme used to style the drawable attributes, may be {@code null}.
     * @return Drawable An object that can be used to draw this resource.
     * @throws NotFoundException Throws NotFoundException if the given ID does
     *             not exist.
     */
    CARAPI GetDrawableForDensity(
        /* [in] */ Int32 id,
        /* [in] */ Int32 density,
        /* [in] */ IResourcesTheme* theme,
        /* [out] */ IDrawable** drawable);

    CARAPI GetDrawableForDensity(
        /* [in] */ Int32 id,
        /* [in] */ Int32 density,
        /* [in] */ IResourcesTheme* theme,
        /* [in] */ Boolean supportComposedIcons,
        /* [out] */ IDrawable** drawable);

    /**
     * Return a movie object associated with the particular resource ID.
     * @param id The desired resource identifier, as generated by the aapt
     *           tool. This integer encodes the package, type, and resource
     *           entry. The value 0 is an invalid identifier.
     * @throws NotFoundException Throws NotFoundException if the given ID does not exist.
     *
     */
    CARAPI GetMovie(
        /* [in] */ Int32 id,
        /* [out] */ IMovie** movie);

    /**
     * Return a color integer associated with a particular resource ID.
     * If the resource holds a complex
     * {@link android.content.res.ColorStateList}, then the default color from
     * the set is returned.
     *
     * @param id The desired resource identifier, as generated by the aapt
     *           tool. This integer encodes the package, type, and resource
     *           entry. The value 0 is an invalid identifier.
     *
     * @throws NotFoundException Throws NotFoundException if the given ID does not exist.
     *
     * @return Returns a single color value in the form 0xAARRGGBB.
     */
    CARAPI GetColor(
        /* [in] */ Int32 id,
        /* [out] */ Int32* color);

    /**
     * Return a color state list associated with a particular resource ID.  The
     * resource may contain either a single raw color value, or a complex
     * {@link android.content.res.ColorStateList} holding multiple possible colors.
     *
     * @param id The desired resource identifier of a {@link ColorStateList},
     *        as generated by the aapt tool. This integer encodes the package, type, and resource
     *        entry. The value 0 is an invalid identifier.
     *
     * @throws NotFoundException Throws NotFoundException if the given ID does not exist.
     *
     * @return Returns a ColorStateList object containing either a single
     * solid color or multiple colors that can be selected based on a state.
     */
    CARAPI GetColorStateList(
        /* [in] */ Int32 id,
        /* [out] */ IColorStateList** list);

    /**
     * Return a boolean associated with a particular resource ID.  This can be
     * used with any integral resource value, and will return true if it is
     * non-zero.
     *
     * @param id The desired resource identifier, as generated by the aapt
     *           tool. This integer encodes the package, type, and resource
     *           entry. The value 0 is an invalid identifier.
     *
     * @throws NotFoundException Throws NotFoundException if the given ID does not exist.
     *
     * @return Returns the boolean value contained in the resource.
     */
    CARAPI GetBoolean(
        /* [in] */ Int32 id,
        /* [out] */ Boolean* b);

    /**
     * Return an integer associated with a particular resource ID.
     *
     * @param id The desired resource identifier, as generated by the aapt
     *           tool. This integer encodes the package, type, and resource
     *           entry. The value 0 is an invalid identifier.
     *
     * @throws NotFoundException Throws NotFoundException if the given ID does not exist.
     *
     * @return Returns the integer value contained in the resource.
     */
    CARAPI GetInteger(
        /* [in] */ Int32 id,
        /* [out] */ Int32* i);

    /**
     * Retrieve a floating-point value for a particular resource ID.
     *
     * @param id The desired resource identifier, as generated by the aapt
     *           tool. This integer encodes the package, type, and resource
     *           entry. The value 0 is an invalid identifier.
     *
     * @return Returns the floating-point value contained in the resource.
     *
     * @throws NotFoundException Throws NotFoundException if the given ID does
     *         not exist or is not a floating-point value.
     * @hide Pending API council approval.
     */
    CARAPI GetFloat(
        /* [in] */ Int32 id,
        /* [out] */ Float* f);

    /**
     * Return an XmlResourceParser through which you can read a view layout
     * description for the given resource ID.  This parser has limited
     * functionality -- in particular, you can't change its input, and only
     * the high-level events are available.
     *
     * <p>This function is really a simple wrapper for calling
     * {@link #getXml} with a layout resource.
     *
     * @param id The desired resource identifier, as generated by the aapt
     *           tool. This integer encodes the package, type, and resource
     *           entry. The value 0 is an invalid identifier.
     *
     * @throws NotFoundException Throws NotFoundException if the given ID does not exist.
     *
     * @return A new parser object through which you can read
     *         the XML data.
     *
     * @see #getXml
     */
    CARAPI GetLayout(
        /* [in] */ Int32 id,
        /* [out] */ IXmlResourceParser** parser);

    /**
     * Return an XmlResourceParser through which you can read an animation
     * description for the given resource ID.  This parser has limited
     * functionality -- in particular, you can't change its input, and only
     * the high-level events are available.
     *
     * <p>This function is really a simple wrapper for calling
     * {@link #getXml} with an animation resource.
     *
     * @param id The desired resource identifier, as generated by the aapt
     *           tool. This integer encodes the package, type, and resource
     *           entry. The value 0 is an invalid identifier.
     *
     * @throws NotFoundException Throws NotFoundException if the given ID does not exist.
     *
     * @return A new parser object through which you can read
     *         the XML data.
     *
     * @see #getXml
     */
    CARAPI GetAnimation(
        /* [in] */ Int32 id,
        /* [out] */ IXmlResourceParser** animation);

    /**
     * Return an XmlResourceParser through which you can read a generic XML
     * resource for the given resource ID.
     *
     * <p>The XmlPullParser implementation returned here has some limited
     * functionality.  In particular, you can't change its input, and only
     * high-level parsing events are available (since the document was
     * pre-parsed for you at build time, which involved merging text and
     * stripping comments).
     *
     * @param id The desired resource identifier, as generated by the aapt
     *           tool. This integer encodes the package, type, and resource
     *           entry. The value 0 is an invalid identifier.
     *
     * @throws NotFoundException Throws NotFoundException if the given ID does not exist.
     *
     * @return A new parser object through which you can read
     *         the XML data.
     *
     * @see android.util.AttributeSet
     */
    CARAPI GetXml(
        /* [in] */ Int32 id,
        /* [out] */ IXmlResourceParser** xml);

    /**
     * Open a data stream for reading a raw resource.  This can only be used
     * with resources whose value is the name of an asset files -- that is, it can be
     * used to open drawable, sound, and raw resources; it will fail on string
     * and color resources.
     *
     * @param id The resource identifier to open, as generated by the appt
     *           tool.
     *
     * @return InputStream Access to the resource data.
     *
     * @throws NotFoundException Throws NotFoundException if the given ID does not exist.
     *
     */
    CARAPI OpenRawResource(
        /* [in] */ Int32 id,
        /* [out] */ IInputStream** res);

    /**
     * Open a data stream for reading a raw resource.  This can only be used
     * with resources whose value is the name of an asset file -- that is, it can be
     * used to open drawable, sound, and raw resources; it will fail on string
     * and color resources.
     *
     * @param id The resource identifier to open, as generated by the appt tool.
     * @param value The TypedValue object to hold the resource information.
     *
     * @return InputStream Access to the resource data.
     *
     * @throws NotFoundException Throws NotFoundException if the given ID does not exist.
     */
    CARAPI OpenRawResource(
        /* [in] */ Int32 id,
        /* [in] */ ITypedValue* value,
        /* [out] */ IInputStream** res);

    /**
     * Open a file descriptor for reading a raw resource.  This can only be used
     * with resources whose value is the name of an asset files -- that is, it can be
     * used to open drawable, sound, and raw resources; it will fail on string
     * and color resources.
     *
     * <p>This function only works for resources that are stored in the package
     * as uncompressed data, which typically includes things like mp3 files
     * and png images.
     *
     * @param id The resource identifier to open, as generated by the appt
     *           tool.
     *
     * @return AssetFileDescriptor A new file descriptor you can use to read
     * the resource.  This includes the file descriptor itself, as well as the
     * offset and length of data where the resource appears in the file.  A
     * null is returned if the file exists but is compressed.
     *
     * @throws NotFoundException Throws NotFoundException if the given ID does not exist.
     *
     */
    CARAPI OpenRawResourceFd(
        /* [in] */ Int32 id,
        /* [out] */ IAssetFileDescriptor** des);

    /**
     * Return the raw data associated with a particular resource ID.
     *
     * @param id The desired resource identifier, as generated by the aapt
     *           tool. This integer encodes the package, type, and resource
     *           entry. The value 0 is an invalid identifier.
     * @param outValue Object in which to place the resource data.
     * @param resolveRefs If true, a resource that is a reference to another
     *                    resource will be followed so that you receive the
     *                    actual final resource data.  If false, the TypedValue
     *                    will be filled in with the reference itself.
     *
     * @throws NotFoundException Throws NotFoundException if the given ID does not exist.
     *
     */
    CARAPI GetValue(
        /* [in] */ Int32 id,
        /* [in, out] */ ITypedValue* outValue,
        /* [in] */ Boolean resolveRefs);

    /**
     * Get the raw value associated with a resource with associated density.
     *
     * @param id resource identifier
     * @param density density in DPI
     * @param resolveRefs If true, a resource that is a reference to another
     *            resource will be followed so that you receive the actual final
     *            resource data. If false, the TypedValue will be filled in with
     *            the reference itself.
     * @throws NotFoundException Throws NotFoundException if the given ID does
     *             not exist.
     * @see #getValue(String, TypedValue, boolean)
     */
    CARAPI GetValueForDensity(
        /* [in] */ Int32 id,
        /* [in] */ Int32 density,
        /* [in] */ ITypedValue* outValue,
        /* [in] */ Boolean resolveRefs);

    /**
     * Return the raw data associated with a particular resource ID.
     * See getIdentifier() for information on how names are mapped to resource
     * IDs, and getString(int) for information on how string resources are
     * retrieved.
     *
     * <p>Note: use of this function is discouraged.  It is much more
     * efficient to retrieve resources by identifier than by name.
     *
     * @param name The name of the desired resource.  This is passed to
     *             getIdentifier() with a default type of "string".
     * @param outValue Object in which to place the resource data.
     * @param resolveRefs If true, a resource that is a reference to another
     *                    resource will be followed so that you receive the
     *                    actual final resource data.  If false, the TypedValue
     *                    will be filled in with the reference itself.
     *
     * @throws NotFoundException Throws NotFoundException if the given ID does not exist.
     *
     */
    CARAPI GetValue(
        /* [in] */ const String& name,
        /* [in, out] */ ITypedValue* outValue,
        /* [in] */ Boolean resolveRefs);

    /**
     * Generate a new Theme object for this set of Resources.  It initially
     * starts out empty.
     *
     * @return Theme The newly created Theme container.
     */
    CARAPI NewTheme(
        /* [out] */ IResourcesTheme** theme);

    /**
     * Retrieve a set of basic attribute values from an AttributeSet, not
     * performing styling of them using a theme and/or style resources.
     *
     * @param set The current attribute values to retrieve.
     * @param attrs The specific attributes to be retrieved.
     * @return Returns a TypedArray holding an array of the attribute values.
     * Be sure to call {@link TypedArray#recycle() TypedArray.recycle()}
     * when done with it.
     *
     * @see Theme#obtainStyledAttributes(AttributeSet, int[], int, int)
     */
    CARAPI ObtainAttributes(
        /* [in] */ IAttributeSet* set,
        /* [in] */ ArrayOf<Int32>* attrs,
        /* [out] */ ITypedArray** array);

    /**
     * Store the newly updated configuration.
     */
    CARAPI UpdateConfiguration(
        /* [in] */ IConfiguration* config,
        /* [in] */ IDisplayMetrics* metrics);

    /**
     * @hide
     */
    CARAPI UpdateConfiguration(
        /* [in] */ IConfiguration* config,
        /* [in] */ IDisplayMetrics* metrics,
        /* [in] */ ICompatibilityInfo* compat);

    /**
     * Update the system resources configuration if they have previously
     * been initialized.
     *
     * @hide
     */
    static CARAPI_(void) UpdateSystemConfiguration(
        /* [in] */ IConfiguration* config,
        /* [in] */ IDisplayMetrics* metrics,
        /* [in] */ ICompatibilityInfo* compat);

    /**
     * Return the current display metrics that are in effect for this resource
     * object.  The returned object should be treated as read-only.
     *
     * @return The resource's current display metrics.
     */
    CARAPI GetDisplayMetrics(
        /* [out] */ IDisplayMetrics** metrics);

    /**
     * Return the current configuration that is in effect for this resource
     * object.  The returned object should be treated as read-only.
     *
     * @return The resource's current configuration.
     */
    CARAPI GetConfiguration(
        /* [out] */ IConfiguration** config);

    /**
     * Return the compatibility mode information for the application.
     * The returned object should be treated as read-only.
     *
     * @return compatibility info.
     * @hide
     */
    CARAPI GetCompatibilityInfo(
        /* [out] */ ICompatibilityInfo** ci);

    /**
     * This is just for testing.
     * @hide
     */
    CARAPI SetCompatibilityInfo(
        /* [in] */ ICompatibilityInfo* ci);

    /**
     * Return a resource identifier for the given resource name.  A fully
     * qualified resource name is of the form "package:type/entry".  The first
     * two components (package and type) are optional if defType and
     * defPackage, respectively, are specified here.
     *
     * <p>Note: use of this function is discouraged.  It is much more
     * efficient to retrieve resources by identifier than by name.
     *
     * @param name The name of the desired resource.
     * @param defType Optional default resource type to find, if "type/" is
     *                not included in the name.  Can be null to require an
     *                explicit type.
     * @param defPackage Optional default package to find, if "package:" is
     *                   not included in the name.  Can be null to require an
     *                   explicit package.
     *
     * @return int The associated resource identifier.  Returns 0 if no such
     *         resource was found.  (0 is not a valid resource ID.)
     */
    CARAPI GetIdentifier(
        /* [in] */ const String& name,
        /* [in] */ const String& defType,
        /* [in] */ const String& defCapsule,
        /* [out] */ Int32* id);

    /**
     * Return true if given resource identifier includes a package.
     *
     * @hide
     */
    static Boolean ResourceHasPackage(
        /* [in] */ Int32 resid);

    /**
     * Return the full name for a given resource identifier.  This name is
     * a single string of the form "package:type/entry".
     *
     * @param resid The resource identifier whose name is to be retrieved.
     *
     * @return A string holding the name of the resource.
     *
     * @throws NotFoundException Throws NotFoundException if the given ID does not exist.
     *
     * @see #getResourcePackageName
     * @see #getResourceTypeName
     * @see #getResourceEntryName
     */
    CARAPI GetResourceName(
        /* [in] */ Int32 resid,
        /* [out] */ String* name);

    /**
     * Return the package name for a given resource identifier.
     *
     * @param resid The resource identifier whose package name is to be
     * retrieved.
     *
     * @return A string holding the package name of the resource.
     *
     * @throws NotFoundException Throws NotFoundException if the given ID does not exist.
     *
     * @see #getResourceName
     */
    CARAPI GetResourcePackageName(
        /* [in] */ Int32 resid,
        /* [out] */ String* name);

    /**
     * Return the type name for a given resource identifier.
     *
     * @param resid The resource identifier whose type name is to be
     * retrieved.
     *
     * @return A string holding the type name of the resource.
     *
     * @throws NotFoundException Throws NotFoundException if the given ID does not exist.
     *
     * @see #getResourceName
     */
    CARAPI GetResourceTypeName(
        /* [in] */ Int32 resid,
        /* [out] */ String* name);

    /**
     * Return the entry name for a given resource identifier.
     *
     * @param resid The resource identifier whose entry name is to be
     * retrieved.
     *
     * @return A string holding the entry name of the resource.
     *
     * @throws NotFoundException Throws NotFoundException if the given ID does not exist.
     *
     * @see #getResourceName
     */
    CARAPI GetResourceEntryName(
        /* [in] */ Int32 resid,
        /* [out] */ String* name);

    /**
     * Parse a series of {@link android.R.styleable#Extra &lt;extra&gt;} tags from
     * an XML file.  You call this when you are at the parent tag of the
     * extra tags, and it will return once all of the child tags have been parsed.
     * This will call {@link #parseBundleExtra} for each extra tag encountered.
     *
     * @param parser The parser from which to retrieve the extras.
     * @param outBundle A Bundle in which to place all parsed extras.
     * @throws XmlPullParserException
     * @throws IOException
     */
    CARAPI ParseBundleExtras(
        /* [in] */ IXmlResourceParser* parser,
        /* [in, out] */ IBundle* outBundle);

    /**
     * Parse a name/value pair out of an XML tag holding that data.  The
     * AttributeSet must be holding the data defined by
     * {@link android.R.styleable#Extra}.  The following value types are supported:
     * <ul>
     * <li> {@link TypedValue#TYPE_STRING}:
     * {@link Bundle#putCharSequence Bundle.putCharSequence()}
     * <li> {@link TypedValue#TYPE_INT_BOOLEAN}:
     * {@link Bundle#putCharSequence Bundle.putBoolean()}
     * <li> {@link TypedValue#TYPE_FIRST_INT}-{@link TypedValue#TYPE_LAST_INT}:
     * {@link Bundle#putCharSequence Bundle.putBoolean()}
     * <li> {@link TypedValue#TYPE_FLOAT}:
     * {@link Bundle#putCharSequence Bundle.putFloat()}
     * </ul>
     *
     * @param tagName The name of the tag these attributes come from; this is
     * only used for reporting error messages.
     * @param attrs The attributes from which to retrieve the name/value pair.
     * @param outBundle The Bundle in which to place the parsed value.
     * @throws XmlPullParserException If the attributes are not valid.
     */
    CARAPI ParseBundleExtra(
        /* [in] */ const String& tagName,
        /* [in] */ IAttributeSet* attrs,
        /* [in, out] */ IBundle* outBundle);

    /**
     * Retrieve underlying AssetManager storage for these resources.
     */
    CARAPI GetAssets(
        /* [out] */ IAssetManager** manager);

    /**
     * Call this to remove all cached loaded layout resources from the
     * Resources object.  Only intended for use with performance testing
     * tools.
     */
    CARAPI FlushLayoutCache();

    /**
     * Start preloading of resource data using this Resources object.  Only
     * for use by the zygote process for loading common system resources.
     * {@hide}
     */
    CARAPI StartPreloading();

    /**
     * Called by zygote when it is done preloading resources, to change back
     * to normal Resources operation.
     */
    CARAPI FinishPreloading();

    /**
     * @hide
     */
    CARAPI GetPreloadedDrawables(
        /* [out] */ IInt64SparseArray** drawables);

    CARAPI LoadDrawable(
        /* [in] */ ITypedValue* value,
        /* [in] */ Int32 id,
        /* [in] */ IResourcesTheme* theme,
        /* [out] */ IDrawable** drawable);

    CARAPI CacheDrawable(
        /* [in] */ ITypedValue* value,
        /* [in] */ IResourcesTheme* theme,
        /* [in] */ Boolean isColorDrawable,
        /* [in] */ DrawableMap&  caches,
        /* [in] */ Int64 key,
        /* [in] */ IDrawable* dr);

    CARAPI LoadDrawableForCookie(
        /* [in] */ ITypedValue* value,
        /* [in] */ Int32 id,
        /* [in] */ IResourcesTheme* theme,
        /* [out] */ IDrawable** drawable);

    CARAPI LoadColorStateList(
        /* [in] */ ITypedValue* value,
        /* [in] */ Int32 id,
        /* [out] */ IColorStateList** stateList);

    CARAPI LoadXmlResourceParser(
        /* [in] */ Int32 id,
        /* [in] */ const String& type,
        /* [out] */ IXmlResourceParser** resourceParser);

    CARAPI LoadXmlResourceParser(
        /* [in] */ const String& file,
        /* [in] */ Int32 id,
        /* [in] */ Int32 assetCookie,
        /* [in] */ const String& type,
        /* [out] */ IXmlResourceParser** resourceParser);

    /** @hide */
    CARAPI GetComposedIconInfo(
        /* [out] */ IComposedIconInfo** iconInfo);

private:
    CARAPI_(AutoPtr<INativePluralRules>) GetPluralRule();

    static CARAPI_(Int32) AttrForQuantityCode(
        /* [in] */ Int32 quantityCode);

    static CARAPI_(String) StringForQuantityCode(
        /* [in] */ Int32 quantityCode);

    CARAPI_(void) ClearDrawableCachesLocked(
        /* [in] */ DrawableMap& cache,
        /* [in] */ Int32 configChanges);

    CARAPI_(void) ClearDrawableCacheLocked(
        /* [in] */ IInt64SparseArray* cache,
        /* [in] */ Int32 configChanges);

    CARAPI_(Boolean) VerifyPreloadConfig(
        /* [in] */ Int32 changingConfigurations,
        /* [in] */ Int32 allowVarying,
        /* [in] */ Int32 resourceId,
        /* [in] */ const String& name);

    CARAPI_(AutoPtr<IDrawable>) GetCachedDrawable(
        /* [in] */ DrawableMap& drawableCache,
        /* [in] */ Int64 key,
        /* [in] */ IResourcesTheme* theme);

    CARAPI_(AutoPtr<IDrawableConstantState>) GetConstantStateLocked(
        /* [in] */ IInt64SparseArray* drawableCache,
        /* [in] */ Int64 key);

    CARAPI_(AutoPtr<IDrawable>) GetCachedDrawableLocked(
        /* [in] */ IInt64SparseArray* drawableCache,
        /* [in] */ Int64 key);

    CARAPI_(AutoPtr<IColorStateList>) GetCachedColorStateList(
        /* [in] */ Int64 key);

    CARAPI RecycleCachedStyledAttributes(
        /* [in] */ CTypedArray* attrs);

    /**
     * {@code Locale.toLanguageTag} will transform the obsolete (and deprecated)
     * language codes "in", "ji" and "iw" to "id", "yi" and "he" respectively.
     *
     * All released versions of android prior to "L" used the deprecated language
     * tags, so we will need to support them for backwards compatibility.
     *
     * Note that this conversion needs to take place *after* the call to
     * {@code toLanguageTag} because that will convert all the deprecated codes to
     * the new ones, even if they're set manually.
     */
    static String AdjustLanguageTag(
        /* [in] */ const String& languageTag);

public: /*package*/
    static const String TAG;

private:
    friend class CTypedArray;

    static const Boolean DEBUG_LOAD;
    static const Boolean DEBUG_CONFIG;
    static const Boolean TRACE_FOR_PRELOAD;
    static const Boolean TRACE_FOR_MISS_PRELOAD;

    static Int32 LAYOUT_DIR_CONFIG;
    static const Int32 ID_OTHER;

    static Object sSync;

    static AutoPtr< ArrayOf<IInt64SparseArray*> > sPreloadedDrawables;
    static AutoPtr<IInt64SparseArray> sPreloadedColorDrawables;
    static ColorStateMap sPreloadedColorStateLists;

    // Pool of TypedArrays targeted to this Resources object.
    AutoPtr<Pools::SynchronizedPool<ITypedArray> > mTypedArrayPool;

    static AutoPtr<IResources> sSystem;

    static Boolean sPreloaded;
    static Int32 sPreloadedDensity;

    // These are protected by the mTmpValue lock.
    Object mAccessLock;
    AutoPtr<CConfiguration> mTmpConfig;
    DrawableMap mDrawableCache;
    DrawableMap mColorDrawableCache;
    ColorStateMap mColorStateListCache;

    AutoPtr<CTypedValue> mTmpValue;
    Boolean mPreloading;

    AutoPtr<CTypedArray> mCachedStyledAttributes;

    Int32 mLastCachedXmlBlockIndex;
    AutoPtr< ArrayOf<Int32> > mCachedXmlBlockIds;
    Object mCachedXmlBlockIdsLock;
    AutoPtr< ArrayOf<XmlBlock*> > mCachedXmlBlocks;

    AutoPtr<CAssetManager> mAssets;
    AutoPtr<CDisplayMetrics> mMetrics;

    AutoPtr<CConfiguration> mConfiguration;
    AutoPtr<INativePluralRules> mPluralRule;

    AutoPtr<ICompatibilityInfo> mCompatibilityInfo;

    AutoPtr<IWeakReference> mToken;//WeakReference<IBinder> mToken;

    static const String WIDGET_SUFFIX;

    static const StaticInitializer sInitializer;
};

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_RES_CRESOURCES_H__
