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

#ifndef __ELASTOS_DROID_TEXT_METHOD_SINGLELINETRANSFORMATIONMETHOD_H__
#define __ELASTOS_DROID_TEXT_METHOD_SINGLELINETRANSFORMATIONMETHOD_H__

#include "elastos/droid/text/method/ReplacementTransformationMethod.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

class ECO_PUBLIC SingleLineTransformationMethod
    : public ReplacementTransformationMethod
    , public ISingleLineTransformationMethod
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    static CARAPI GetInstance(
        /* [out] */ ISingleLineTransformationMethod** ret);

protected:
    /**
     * The characters to be replaced are \n and \r.
     */
    CARAPI_(AutoPtr< ArrayOf<Char32> >) GetOriginal();

    /**
     * The character \n is replaced with is space;
     * the character \r is replaced with is FEFF (zero width space).
     */
    CARAPI_(AutoPtr< ArrayOf<Char32> >) GetReplacement();

private:
    ECO_LOCAL static AutoPtr<ArrayOf<Char32> > ORIGINAL;
    ECO_LOCAL static AutoPtr<ArrayOf<Char32> > REPLACEMENT;
    ECO_LOCAL static AutoPtr<ISingleLineTransformationMethod> sInstance;
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_SINGLELINETRANSFORMATIONMETHOD_H__
