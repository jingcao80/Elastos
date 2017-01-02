//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
