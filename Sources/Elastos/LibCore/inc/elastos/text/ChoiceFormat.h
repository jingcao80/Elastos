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

#ifndef __ELASTOS_TEXT_CHOICEFORMAT_H__
#define __ELASTOS_TEXT_CHOICEFORMAT_H__

#include "NumberFormat.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Core::IStringBuffer;
using Elastos::Text::IFieldPosition;
using Elastos::Text::IParsePosition;

namespace Elastos {
namespace Text {

class ChoiceFormat
    : public NumberFormat
    , public IChoiceFormat
{
public:
    CAR_INTERFACE_DECL()

    virtual ~ChoiceFormat();

    CARAPI constructor(
        /* [in] */ ArrayOf<Double>* limits,
        /* [in] */ ArrayOf<String>* formats);

    CARAPI constructor(
        /* [in] */ const String& tem);

    virtual CARAPI ApplyPattern(
        /* [in] */ const String& tem);

    CARAPI Clone(
        /* [out] */ IInterface ** object);

    CARAPI CloneImpl(
        /* [in] */ IChoiceFormat * object);

    CARAPI Equals(
        /* [in] */ IInterface * object,
        /* [out] */ Boolean * value);

    CARAPI Format(
        /* [in] */ Double value,
        /* [in] */ IStringBuffer * inbuffer,
        /* [in] */ IFieldPosition * field);

    CARAPI Format(
        /* [in] */ Int64 value,
        /* [in] */ IStringBuffer * inbuffer,
        /* [in] */ IFieldPosition * field);

    CARAPI GetHashCode(
        /* [out] */ Int32 * value);

    CARAPI GetFormats(
        /* [out, callee] */ ArrayOf<String>** arrayOfFormattedString);

    virtual CARAPI GetLimits(
        /* [out, callee] */ ArrayOf<Double>** arrayOfDoubles);

    static CARAPI_(Double) NextDouble(
        /* [in] */ Double value);

    static CARAPI_(Double) NextDouble(
        /* [in] */ Double value,
        /* [in] */ Boolean increment);

    //@Override
    CARAPI Parse(
        /* [in] */ const String& string,
        /* [in] */ IParsePosition* position,
        /* [out] */ INumber** value);

    static CARAPI_(Double) PreviousDouble(
        /* [in] */ Double value);

    virtual CARAPI SetChoices(
        /* [in] */ ArrayOf<Double>* limits,
        /* [in] */ ArrayOf<String>* formats);

    virtual CARAPI ToPattern(
        /* [out] */ String* pattern);

private:
    CARAPI_(Int32) SkipWhitespace(
        /* [in] */ const String& string,
        /* [in] */ Int32 index);

public:
    AutoPtr<ArrayOf<Double> > mChoiceLimits;

    AutoPtr<ArrayOf<String> > mChoiceFormats;
};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CHOICEFORMAT_H__
