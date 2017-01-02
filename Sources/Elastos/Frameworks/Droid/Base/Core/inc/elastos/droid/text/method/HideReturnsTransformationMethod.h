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

#ifndef __ELASTOS_DROID_TEXT_METHOD_HIDERETURNSTRANSFORMATIONMETHOD_H__
#define __ELASTOS_DROID_TEXT_METHOD_HIDERETURNSTRANSFORMATIONMETHOD_H__

#include "elastos/droid/text/method/ReplacementTransformationMethod.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

/**
 * This transformation method causes any carriage return characters (\r)
 * to be hidden by displaying them as zero-width non-breaking space
 * characters (\uFEFF).
 */
//public class
class HideReturnsTransformationMethod
    : public ReplacementTransformationMethod
    , public IHideReturnsTransformationMethod
{
public:
    HideReturnsTransformationMethod();

    ~HideReturnsTransformationMethod();

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    static CARAPI GetInstance(
        /* [out] */ IHideReturnsTransformationMethod** ret);

protected:
    /**
     * The character to be replaced is \r.
     */
    CARAPI_(AutoPtr< ArrayOf<Char32> >) GetOriginal();

    /**
     * The character that \r is replaced with is \uFEFF.
     */
    CARAPI_(AutoPtr< ArrayOf<Char32> >) GetReplacement();

private:
    static AutoPtr<ArrayOf<Char32> > ORIGINAL;
    static AutoPtr<ArrayOf<Char32> > REPLACEMENT;
    static AutoPtr<IHideReturnsTransformationMethod> sInstance;
};


} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_HIDERETURNSTRANSFORMATIONMETHOD_H__
