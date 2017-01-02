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

#include "elastos/droid/text/method/SingleLineTransformationMethod.h"
#include "elastos/droid/text/method/CSingleLineTransformationMethod.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

static AutoPtr<ArrayOf<Char32> > InitOriginal()
{
    Char32 array[] = {'\n', '\r'};
    AutoPtr<ArrayOf<Char32> > ch = ArrayOf<Char32>::Alloc(ArraySize(array));
    (*ch)[0] = '\n';
    (*ch)[1] = '\r';
    return ch;
}

static AutoPtr<ArrayOf<Char32> > InitReplacement()
{
    Char32 array[] = {' ', 0xFEFF};
    AutoPtr<ArrayOf<Char32> > ch = ArrayOf<Char32>::Alloc(ArraySize(array));
    (*ch)[0] = ' ';
    (*ch)[1] = 0xFEFF;
    return ch;
}

AutoPtr<ArrayOf<Char32> > SingleLineTransformationMethod::ORIGINAL = InitOriginal();
AutoPtr<ArrayOf<Char32> > SingleLineTransformationMethod::REPLACEMENT = InitReplacement();

AutoPtr<ISingleLineTransformationMethod> SingleLineTransformationMethod::sInstance;

CAR_INTERFACE_IMPL(SingleLineTransformationMethod, ReplacementTransformationMethod, \
    ISingleLineTransformationMethod)

ECode SingleLineTransformationMethod::constructor()
{
    return NOERROR;
}

ECode SingleLineTransformationMethod::GetInstance(
    /* [out] */ ISingleLineTransformationMethod** ret)
{
    VALIDATE_NOT_NULL(ret)
    if (sInstance == NULL) {
        CSingleLineTransformationMethod::New((ISingleLineTransformationMethod**)&sInstance);
    }

    *ret = sInstance;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

AutoPtr<ArrayOf<Char32> > SingleLineTransformationMethod::GetOriginal()
{
    return ORIGINAL;
}

AutoPtr< ArrayOf<Char32> > SingleLineTransformationMethod::GetReplacement()
{
    return REPLACEMENT;
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

