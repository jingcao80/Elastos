
#ifndef __ELASTOS_DROID_UTILITY_CTYPEDVALUE_H__
#define __ELASTOS_DROID_UTILITY_CTYPEDVALUE_H__

#include "_Elastos_Droid_Utility_CTypedValue.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Utility {

CarClass(CTypedValue)
    , public Object
    , public ITypedValue
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CTypedValue();

    virtual ~CTypedValue();

    CARAPI GetFloat(
        /* [out] */ Float* f);

    CARAPI GetFraction(
        /* [in] */ Float base,
        /* [in] */ Float pbase,
        /* [out] */ Float* fraction);

    CARAPI GetDimension(
        /* [in] */ IDisplayMetrics* metric,
        /* [out] */ Float* dimension);

    CARAPI CoerceToString(
        /* [out] */ ICharSequence** csq);

    CARAPI SetTo(
        /* [in] */ ITypedValue* other);

    CARAPI GetDescription(
        /* [out] */ String* str);

    CARAPI GetType(
        /* [out] */ Int32* type);

    CARAPI SetType(
        /* [in] */ Int32 type);

    CARAPI GetString(
        /* [out] */ ICharSequence** string);

    CARAPI SetString(
        /* [in] */ ICharSequence* string);

    CARAPI GetData(
        /* [out] */ Int32* data);

    CARAPI SetData(
        /* [in] */ Int32 data);

    CARAPI GetAssetCookie(
        /* [out] */ Int32* cookie);

    CARAPI SetAssetCookie(
        /* [in] */ Int32 cookie);

    CARAPI GetResourceId(
        /* [out] */ Int32* resId);

    CARAPI SetResourceId(
        /* [in] */ Int32 resId);

    CARAPI SetChangingConfigurations(
        /* [in] */ Int32 configurations);

    CARAPI GetDensity(
        /* [out] */ Int32* density);

    CARAPI SetDensity(
        /* [in] */ Int32 density);

public: /*package*/
    static CARAPI_(Float) ComplexToFloat(
        /* [in] */ Int32 complex);

    static CARAPI_(Float) ComplexToDimension(
        /* [in] */ Int32 data,
        /* [in] */ IDisplayMetrics* metrics);

    static CARAPI_(Int32) ComplexToDimensionPixelOffset(
        /* [in] */ Int32 data,
        /* [in] */ IDisplayMetrics* metrics);

    static CARAPI_(Int32) ComplexToDimensionPixelSize(
        /* [in] */ Int32 data,
        /* [in] */ IDisplayMetrics* metrics);

    static CARAPI_(Float) ComplexToDimensionNoisy(
        /* [in] */ Int32 data,
        /* [in] */ IDisplayMetrics* metrics);

    static CARAPI_(Float) ApplyDimension(
        /* [in] */ Int32 unit,
        /* [in] */ Float value,
        /* [in] */ IDisplayMetrics* metrics);

    static CARAPI_(Float) ComplexToFraction(
        /* [in] */ Int32 data,
        /* [in] */ Float base,
        /* [in] */ Float pbase);

    /**
     * Perform type conversion as per {@link #coerceToString()} on an
     * explicitly supplied type and data.
     *
     * @param type The data type identifier.
     * @param data The data value.
     *
     * @return String The coerced string value.  If the value is
     *         null or the type is not known, null is returned.
     */
    static CARAPI_(String) CoerceToString(
        /* [in] */ Int32 type,
        /* [in] */ Int32 data);

    CARAPI ToString(
        /* [out] */ String* str);
public:
    /** The type held by this value, as defined by the constants here.
     *  This tells you how to interpret the other fields in the object. */
    Int32 mType;

    /** If the value holds a string, this is it. */
    AutoPtr<ICharSequence> mString;

    /** Basic data in the value, interpreted according to {@link #type} */
    Int32 mData;

    /** Additional information about where the value came from; only
     *  set for strings. */
    Int32 mAssetCookie;

    /** If Value came from a resource, this holds the corresponding resource id. */
    Int32 mResourceId;

    /** If Value came from a resource, these are the configurations for which
     *  its contents can change. */
    Int32 mChangingConfigurations;

    /**
     * If the Value came from a resource, this holds the corresponding pixel density.
     * */
    Int32 mDensity;

private:
    static const Float MANTISSA_MULT;

    static const Float RADIX_MULTS[];

    static const String DIMENSION_UNIT_STRS[];

    static const String FRACTION_UNIT_STRS[];
};

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_UTILITY_CTYPEDVALUE_H__
