
#include "elastos/droid/animation/BidirectionalTypeConverter.h"

namespace Elastos {
namespace Droid {
namespace Animation {

InvertedConverter::InvertedConverter(
    /* [in] */ BidirectionalTypeConverter/*<To, From>*/* converter)
{
    InterfaceID targetType, sourceType;
    converter->GetTargetType(&targetType);
    converter->GetSourceType(&sourceType);
    constructor(targetType, sourceType);
    mConverter = converter;
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

BidirectionalTypeConverter::BidirectionalTypeConverter(
    /* [in] */ const InterfaceID& fromClass,
    /* [in] */ const InterfaceID& toClass)
{
    TypeConverter::constructor(fromClass, toClass);
}

ECode BidirectionalTypeConverter::Invert(
    /* [out] */ IBidirectionalTypeConverter** result)
{
    VALIDATE_NOT_NULL(result);
    if (mInvertedConverter == NULL) {
        mInvertedConverter = new InvertedConverter(this);
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
