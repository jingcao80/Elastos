
#include "elastos/droid/animation/BidirectionalTypeConverter.h"

namespace Elastos {
namespace Droid {
namespace Animation {

InvertedConverter::InvertedConverter()
{}

ECode InvertedConverter::constructor(
    /* [in] */ BidirectionalTypeConverter* converter)
{
    InterfaceID targetType, sourceType;
    converter->GetTargetType(&targetType);
    converter->GetSourceType(&sourceType);
    BidirectionalTypeConverter::constructor(targetType, sourceType);
    mConverter = converter;
    return NOERROR;
}

ECode InvertedConverter::ConvertBack(
    /* [in] */ IInterface* value,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    return mConverter->Convert(value, result);
}

ECode InvertedConverter::Convert(
    /* [in] */ IInterface* value,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    return mConverter->ConvertBack(value, result);
}


CAR_INTERFACE_IMPL(BidirectionalTypeConverter, TypeConverter, IBidirectionalTypeConverter);

BidirectionalTypeConverter::BidirectionalTypeConverter()
{}

ECode BidirectionalTypeConverter::Invert(
    /* [out] */ IBidirectionalTypeConverter** result)
{
    VALIDATE_NOT_NULL(result);
    if (mInvertedConverter == NULL) {
        AutoPtr<InvertedConverter> ic = new InvertedConverter();
        ic->constructor(this);
        mInvertedConverter = ic;
    }
    *result = mInvertedConverter;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode BidirectionalTypeConverter::constructor(
    /* [in] */ const InterfaceID& fromClass,
    /* [in] */ const InterfaceID& toClass)
{
    return TypeConverter::constructor(fromClass, toClass);
}

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos
