
#ifndef __ELASTOS_DROID_CONTENT_RES_CTYPEDARRAY_H__
#define __ELASTOS_DROID_CONTENT_RES_CTYPEDARRAY_H__

#include "_Elastos_Droid_Content_Res_CTypedArray.h"
#include "elastos/droid/content/res/XmlBlock.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/utility/CTypedValue.h"

using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Utility::IDisplayMetrics;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

/**
 * Container for an array of values that were retrieved with
 * {@link Resources.Theme#obtainStyledAttributes(AttributeSet, int[], int, int)}
 * or {@link Resources#obtainAttributes}.  Be
 * sure to call {@link #recycle} when done with them.
 *
 * The indices used to retrieve values from this structure correspond to
 * the positions of the attributes given to obtainStyledAttributes.
 */
CarClass(CTypedArray)
    , public Object
    , public ITypedArray
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CTypedArray();

    virtual ~CTypedArray();

    /**
     * Return the number of values in this array.
     */
    CARAPI GetLength(
        /* [out] */ Int32* len);

    /**
     * Return the number of indices in the array that actually have data.
     */
    CARAPI GetIndexCount(
        /* [out] */ Int32* count);

    /**
     * Return an index in the array that has data.
     *
     * @param at The index you would like to returned, ranging from 0 to
     * {@link #getIndexCount()}.
     *
     * @return The index at the given offset, which can be used with
     * {@link #getValue} and related APIs.
     */
    CARAPI GetIndex(
        /* [in] */ Int32 at,
        /* [out] */ Int32* index);

    /**
     * Return the Resources object this array was loaded from.
     */
    CARAPI GetResources(
        /* [out] */ IResources** res);

    /**
     * Retrieve the styled string value for the attribute at <var>index</var>.
     *
     * @param index Index of attribute to retrieve.
     *
     * @return CharSequence holding string data.  May be styled.  Returns
     *         null if the attribute is not defined.
     */
    CARAPI GetText(
        /* [in] */ Int32 index,
        /* [out] */ ICharSequence** csq);

    /**
     * Retrieve the string value for the attribute at <var>index</var>.
     *
     * @param index Index of attribute to retrieve.
     *
     * @return String holding string data.  Any styling information is
     * removed.  Returns null if the attribute is not defined.
     */
    CARAPI GetString(
        /* [in] */ Int32 index,
        /* [out] */ String* str);

    /**
     * Retrieve the string value for the attribute at <var>index</var>, but
     * only if that string comes from an immediate value in an XML file.  That
     * is, this does not allow references to string resources, string
     * attributes, or conversions from other types.  As such, this method
     * will only return strings for TypedArray objects that come from
     * attributes in an XML file.
     *
     * @param index Index of attribute to retrieve.
     *
     * @return String holding string data.  Any styling information is
     * removed.  Returns null if the attribute is not defined or is not
     * an immediate string value.
     */
    CARAPI GetNonResourceString(
        /* [in] */ Int32 index,
        /* [out] */ String* str);

    /**
     * @hide
     * Retrieve the string value for the attribute at <var>index</var> that is
     * not allowed to change with the given configurations.
     *
     * @param index Index of attribute to retrieve.
     * @param allowedChangingConfigs Bit mask of configurations from
     * ActivityInfo that are allowed to change.
     *
     * @return String holding string data.  Any styling information is
     * removed.  Returns null if the attribute is not defined.
     */
    CARAPI GetNonConfigurationString(
        /* [in] */ Int32 index,
        /* [in] */ Int32 allowedChangingConfigs,
        /* [out] */ String* str);

    /**
     * Retrieve the boolean value for the attribute at <var>index</var>.
     *
     * @param index Index of attribute to retrieve.
     * @param defValue Value to return if the attribute is not defined.
     *
     * @return Attribute boolean value, or defValue if not defined.
     */
    CARAPI GetBoolean(
        /* [in] */ Int32 index,
        /* [in] */ Boolean defValue,
        /* [out] */ Boolean* value);

    /**
     * Retrieve the integer value for the attribute at <var>index</var>.
     *
     * @param index Index of attribute to retrieve.
     * @param defValue Value to return if the attribute is not defined.
     *
     * @return Attribute int value, or defValue if not defined.
     */
    CARAPI GetInt32(
        /* [in] */ Int32 index,
        /* [in] */ Int32 defValue,
        /* [out] */ Int32* value);

    /**
     * Retrieve the float value for the attribute at <var>index</var>.
     *
     * @param index Index of attribute to retrieve.
     *
     * @return Attribute float value, or defValue if not defined..
     */
    CARAPI GetFloat(
        /* [in] */ Int32 index,
        /* [in] */ Float defValue,
        /* [out] */ Float* value);

    /**
     * Retrieve the color value for the attribute at <var>index</var>.  If
     * the attribute references a color resource holding a complex
     * {@link android.content.res.ColorStateList}, then the default color from
     * the set is returned.
     *
     * @param index Index of attribute to retrieve.
     * @param defValue Value to return if the attribute is not defined or
     *                 not a resource.
     *
     * @return Attribute color value, or defValue if not defined.
     */
    CARAPI GetColor(
        /* [in] */ Int32 index,
        /* [in] */ Int32 defValue,
        /* [out] */ Int32* value);

    /**
     * Retrieve the ColorStateList for the attribute at <var>index</var>.
     * The value may be either a single solid color or a reference to
     * a color or complex {@link android.content.res.ColorStateList} description.
     *
     * @param index Index of attribute to retrieve.
     *
     * @return ColorStateList for the attribute, or null if not defined.
     */
    CARAPI GetColorStateList(
        /* [in] */ Int32 index,
        /* [out] */ IColorStateList** list);

    /**
     * Retrieve the integer value for the attribute at <var>index</var>.
     *
     * @param index Index of attribute to retrieve.
     * @param defValue Value to return if the attribute is not defined or
     *                 not a resource.
     *
     * @return Attribute integer value, or defValue if not defined.
     */
    CARAPI GetInteger(
        /* [in] */ Int32 index,
        /* [in] */ Int32 defValue,
        /* [out] */ Int32* value);

    /**
     * Retrieve a dimensional unit attribute at <var>index</var>.  Unit
     * conversions are based on the current {@link DisplayMetrics}
     * associated with the resources this {@link TypedArray} object
     * came from.
     *
     * @param index Index of attribute to retrieve.
     * @param defValue Value to return if the attribute is not defined or
     *                 not a resource.
     *
     * @return Attribute dimension value multiplied by the appropriate
     * metric, or defValue if not defined.
     *
     * @see #getDimensionPixelOffset
     * @see #getDimensionPixelSize
     */
    CARAPI GetDimension(
        /* [in] */ Int32 index,
        /* [in] */ Float defValue,
        /* [out] */ Float* value);

    /**
     * Retrieve a dimensional unit attribute at <var>index</var> for use
     * as an offset in raw pixels.  This is the same as
     * {@link #getDimension}, except the returned value is converted to
     * integer pixels for you.  An offset conversion involves simply
     * truncating the base value to an integer.
     *
     * @param index Index of attribute to retrieve.
     * @param defValue Value to return if the attribute is not defined or
     *                 not a resource.
     *
     * @return Attribute dimension value multiplied by the appropriate
     * metric and truncated to integer pixels, or defValue if not defined.
     *
     * @see #getDimension
     * @see #getDimensionPixelSize
     */
    CARAPI GetDimensionPixelOffset(
        /* [in] */ Int32 index,
        /* [in] */ Int32 defValue,
        /* [out] */ Int32* value);

    /**
     * Retrieve a dimensional unit attribute at <var>index</var> for use
     * as a size in raw pixels.  This is the same as
     * {@link #getDimension}, except the returned value is converted to
     * integer pixels for use as a size.  A size conversion involves
     * rounding the base value, and ensuring that a non-zero base value
     * is at least one pixel in size.
     *
     * @param index Index of attribute to retrieve.
     * @param defValue Value to return if the attribute is not defined or
     *                 not a resource.
     *
     * @return Attribute dimension value multiplied by the appropriate
     * metric and truncated to integer pixels, or defValue if not defined.
     *
     * @see #getDimension
     * @see #getDimensionPixelOffset
     */
    CARAPI GetDimensionPixelSize(
        /* [in] */ Int32 index,
        /* [in] */ Int32 defValue,
        /* [out] */ Int32* value);

    /**
     * Special version of {@link #getDimensionPixelSize} for retrieving
     * {@link android.view.ViewGroup}'s layout_width and layout_height
     * attributes.  This is only here for performance reasons; applications
     * should use {@link #getDimensionPixelSize}.
     *
     * @param index Index of the attribute to retrieve.
     * @param name Textual name of attribute for error reporting.
     *
     * @return Attribute dimension value multiplied by the appropriate
     * metric and truncated to integer pixels.
     */
    CARAPI GetLayoutDimension(
        /* [in] */ Int32 index,
        /* [in] */ const String& name,
        /* [out] */ Int32* dimension);

    /**
     * Special version of {@link #getDimensionPixelSize} for retrieving
     * {@link android.view.ViewGroup}'s layout_width and layout_height
     * attributes.  This is only here for performance reasons; applications
     * should use {@link #getDimensionPixelSize}.
     *
     * @param index Index of the attribute to retrieve.
     * @param defValue The default value to return if this attribute is not
     * default or contains the wrong type of data.
     *
     * @return Attribute dimension value multiplied by the appropriate
     * metric and truncated to integer pixels.
     */
    CARAPI GetLayoutDimension(
        /* [in] */ Int32 index,
        /* [in] */ Int32 defValue,
        /* [out] */ Int32* dimension);

    /**
     * Retrieve a fractional unit attribute at <var>index</var>.
     *
     * @param index Index of attribute to retrieve.
     * @param base The base value of this fraction.  In other words, a
     *             standard fraction is multiplied by this value.
     * @param pbase The parent base value of this fraction.  In other
     *             words, a parent fraction (nn%p) is multiplied by this
     *             value.
     * @param defValue Value to return if the attribute is not defined or
     *                 not a resource.
     *
     * @return Attribute fractional value multiplied by the appropriate
     * base value, or defValue if not defined.
     */
    CARAPI GetFraction(
        /* [in] */ Int32 index,
        /* [in] */ Int32 base,
        /* [in] */ Int32 pbase,
        /* [in] */ Float defValue,
        /* [out] */ Float* value);

    /**
     * Retrieve the resource identifier for the attribute at
     * <var>index</var>.  Note that attribute resource as resolved when
     * the overall {@link TypedArray} object is retrieved.  As a
     * result, this function will return the resource identifier of the
     * final resource value that was found, <em>not</em> necessarily the
     * original resource that was specified by the attribute.
     *
     * @param index Index of attribute to retrieve.
     * @param defValue Value to return if the attribute is not defined or
     *                 not a resource.
     *
     * @return Attribute resource identifier, or defValue if not defined.
     */
    CARAPI GetResourceId(
        /* [in] */ Int32 index,
        /* [in] */ Int32 defValue,
        /* [out] */ Int32* value);

    /**
     * Retrieve the theme attribute resource identifier for the attribute at
     * <var>index</var>.
     *
     * @param index Index of attribute to retrieve.
     * @param defValue Value to return if the attribute is not defined or not a
     *            resource.
     * @return Theme attribute resource identifier, or defValue if not defined.
     * @hide
     */
    CARAPI GetThemeAttributeId(
        /* [in] */ Int32 index,
        /* [in] */ Int32 defValue,
        /* [out] */ Int32* value);

    /**
     * Retrieve the Drawable for the attribute at <var>index</var>.  This
     * gets the resource ID of the selected attribute, and uses
     * {@link Resources#getDrawable Resources.getDrawable} of the owning
     * Resources object to retrieve its Drawable.
     *
     * @param index Index of attribute to retrieve.
     *
     * @return Drawable for the attribute, or null if not defined.
     */
    CARAPI GetDrawable(
        /* [in] */ Int32 index,
        /* [out] */ IDrawable** drawable);

    /**
     * Retrieve the CharSequence[] for the attribute at <var>index</var>.
     * This gets the resource ID of the selected attribute, and uses
     * {@link Resources#getTextArray Resources.getTextArray} of the owning
     * Resources object to retrieve its String[].
     *
     * @param index Index of attribute to retrieve.
     *
     * @return CharSequence[] for the attribute, or null if not defined.
     */
    CARAPI GetTextArray(
        /* [in] */ Int32 index,
        /* [out, callee] */ ArrayOf<ICharSequence*>** array);

    /**
     * Retrieve the raw TypedValue for the attribute at <var>index</var>.
     *
     * @param index Index of attribute to retrieve.
     * @param outValue TypedValue object in which to place the attribute's
     *                 data.
     *
     * @return Returns true if the value was retrieved, else false.
     */
    CARAPI GetValue(
        /* [in] */ Int32 index,
        /* [in, out] */ ITypedValue* outValue,
        /* [out] */ Boolean* value);

    /**
     * Returns the type of attribute at the specified index.
     *
     * @param index Index of attribute whose type to retrieve.
     * @return Attribute type.
     */
    CARAPI GetType(
        /* [in] */ Int32 index,
        /* [out] */ Int32* type);

    /**
     * Determines whether there is an attribute at <var>index</var>.
     *
     * @param index Index of attribute to retrieve.
     *
     * @return True if the attribute has a value, false otherwise.
     */
    CARAPI HasValue(
        /* [in] */ Int32 index,
        /* [out] */ Boolean* hasValue);

    /**
     * Retrieve the raw TypedValue for the attribute at <var>index</var>
     * and return a temporary object holding its data.  This object is only
     * valid until the next call on to {@link TypedArray}.
     *
     * @param index Index of attribute to retrieve.
     *
     * @return Returns a TypedValue object if the attribute is defined,
     *         containing its data; otherwise returns null.  (You will not
     *         receive a TypedValue whose type is TYPE_NULL.)
     */
    CARAPI PeekValue(
        /* [in] */ Int32 index,
        /* [out] */ ITypedValue** value);

    /**
     * Returns a message about the parser state suitable for printing error messages.
     */
    CARAPI GetPositionDescription(
        /* [out] */ String* des);

    /**
     * Give back a previously retrieved array, for later re-use.
     */
    CARAPI Recycle();

    /**
     * Extracts theme attributes from a typed array for later resolution using
     * {@link android.content.res.Resources.Theme#resolveAttributes(int[], int[])}.
     * Removes the entries from the typed array so that subsequent calls to typed
     * getters will return the default value without crashing.
     *
     * @return an array of length {@link #getIndexCount()} populated with theme
     *         attributes, or null if there are no theme attributes in the typed
     *         array
     * @hide
     */
    CARAPI ExtractThemeAttrs(
        /* [out, callee] */ ArrayOf<Int32>** attrs);

    /**
     * Return a mask of the configuration parameters for which the values in
     * this typed array may change.
     *
     * @return Returns a mask of the changing configuration parameters, as
     *         defined by {@link android.content.pm.ActivityInfo}.
     * @see android.content.pm.ActivityInfo
     */
    CARAPI GetChangingConfigurations(
        /* [out] */ Int32* cfgs);

    CARAPI constructor(
        /* [in] */ IResources* resources,
        /* [in] */ ArrayOf<Int32>* data,
        /* [in] */ ArrayOf<Int32>* indices,
        /* [in] */ Int32 len);

    static AutoPtr<ITypedArray> Obtain(
        /* [in] */ IResources* res,
        /* [in] */ Int32 len);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CARAPI_(Boolean) GetValueAt(
        /* [in] */ Int32 index,
        /* [in, out] */ CTypedValue* outValue);

    CARAPI_(AutoPtr<ICharSequence>) LoadStringValueAt(
        /* [in] */ Int32 index);

public:
    /*package*/ AutoPtr<XmlBlock::Parser> mXml;
    /*package*/ AutoPtr<IResourcesTheme> mTheme;
    /*package*/ AutoPtr< ArrayOf<Int32> > mData;
    /*package*/ AutoPtr< ArrayOf<Int32> > mIndices;
    /*package*/ Int32 mLength;
    /*package*/ AutoPtr<CTypedValue>    mValue;

private:
    CResources*     mResources; // mResources->mCachedStyledAttributes == this sometime
    AutoPtr<IDisplayMetrics> mMetrics;
    AutoPtr<IAssetManager> mAssets;

    Boolean mRecycled;
};

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_RES_CTYPEDARRAY_H__
