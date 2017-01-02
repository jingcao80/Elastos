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

#include "elastos/droid/text/method/HideReturnsTransformationMethod.h"
#include "elastos/droid/text/method/CHideReturnsTransformationMethod.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

AutoPtr<IHideReturnsTransformationMethod> HideReturnsTransformationMethod::sInstance;

static AutoPtr<ArrayOf<Char32> > HInitOriginal()
{
    Char32 array[] = { '\r' };
    AutoPtr<ArrayOf<Char32> > ch = ArrayOf<Char32>::Alloc(ArraySize(array));
    (*ch)[0] = '\r';
    return ch;
}

static AutoPtr<ArrayOf<Char32> > HInitReplacement()
{
    Char32 array[] = { 0xFEFF };
    AutoPtr<ArrayOf<Char32> > ch = ArrayOf<Char32>::Alloc(ArraySize(array));
    (*ch)[0] = 0xFEFF;
    return ch;
}

CAR_INTERFACE_IMPL(HideReturnsTransformationMethod, ReplacementTransformationMethod, \
    IHideReturnsTransformationMethod)

HideReturnsTransformationMethod::HideReturnsTransformationMethod()
{}

HideReturnsTransformationMethod::~HideReturnsTransformationMethod()
{}

ECode HideReturnsTransformationMethod::constructor()
{
    return NOERROR;
}

AutoPtr<ArrayOf<Char32> > HideReturnsTransformationMethod::ORIGINAL = HInitOriginal();

AutoPtr<ArrayOf<Char32> > HideReturnsTransformationMethod::REPLACEMENT = HInitReplacement();

AutoPtr< ArrayOf<Char32> > HideReturnsTransformationMethod::GetOriginal()
{
    return ORIGINAL;
}

AutoPtr< ArrayOf<Char32> > HideReturnsTransformationMethod::GetReplacement()
{
    return REPLACEMENT;
}

ECode HideReturnsTransformationMethod::GetInstance(
    /* [out] */ IHideReturnsTransformationMethod** ret)
{
    VALIDATE_NOT_NULL(ret)
    if (sInstance == NULL) {
        CHideReturnsTransformationMethod::New((IHideReturnsTransformationMethod**)&sInstance);
    }

    *ret = sInstance;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos
