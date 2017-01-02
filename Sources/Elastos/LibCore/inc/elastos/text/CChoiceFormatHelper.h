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

#ifndef __ELASTOS_TEXT_CCHOICEFORMATHELPER_H__
#define __ELASTOS_TEXT_CCHOICEFORMATHELPER_H__

#include "_Elastos_Text_CChoiceFormatHelper.h"
#include "elastos/core/Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace Text {

CarClass(CChoiceFormatHelper)
    , public Singleton
    , public IChoiceFormatHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI NextDouble(
        /* [in] */ Double value,
        /* [out] */ Double* nextValue);

    CARAPI NextDouble(
        /* [in] */ Double value,
        /* [in] */ Boolean increment,
        /* [out] */ Double* nextValue);

    CARAPI PreviousDouble(
        /* [in] */ Double value,
        /* [out] */ Double* previousValue);

};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CCHOICEFORMATHELPER_H__
