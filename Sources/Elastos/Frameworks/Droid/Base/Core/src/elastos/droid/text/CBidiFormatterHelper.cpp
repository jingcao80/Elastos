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

#include "elastos/droid/text/CBidiFormatterHelper.h"
#include "elastos/droid/text/CBidiFormatterBuilder.h"

namespace Elastos {
namespace Droid {
namespace Text {

CAR_INTERFACE_IMPL(CBidiFormatterHelper, Singleton, IBidiFormatterHelper)

CAR_SINGLETON_IMPL(CBidiFormatterHelper)

ECode CBidiFormatterHelper::GetInstance(
    /* [out] */ IBidiFormatter** instance)
{
    VALIDATE_NOT_NULL(instance)

    AutoPtr<IBidiFormatterBuilder> builder;
    CBidiFormatterBuilder::New((IBidiFormatterBuilder**)&builder);
    return builder->Build(instance);
}

ECode CBidiFormatterHelper::GetInstance(
    /* [in] */ Boolean rtlContext,
    /* [out] */ IBidiFormatter** instance)
{
    VALIDATE_NOT_NULL(instance)

    AutoPtr<IBidiFormatterBuilder> builder;
    CBidiFormatterBuilder::New(rtlContext, (IBidiFormatterBuilder**)&builder);
    return builder->Build(instance);
}

ECode CBidiFormatterHelper::GetInstance(
    /* [in] */ ILocale* locale,
    /* [out] */ IBidiFormatter** instance)
{
    VALIDATE_NOT_NULL(instance)

    AutoPtr<IBidiFormatterBuilder> builder;
    CBidiFormatterBuilder::New(locale, (IBidiFormatterBuilder**)&builder);
    return builder->Build(instance);
}

}//namespace Text
}//namespace Droid
}//namespace Elastos