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

#ifndef __ELASTOS_TEXT_CNORMALIZERHELPER_H__
#define __ELASTOS_TEXT_CNORMALIZERHELPER_H__

#include "_Elastos_Text_CNormalizerHelper.h"
#include "Singleton.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::Singleton;
using Elastos::Utility::ILocale;

namespace Elastos {
namespace Text {

CarClass(CNormalizerHelper)
    , public Singleton
    , public INormalizer
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI IsNormalized(
        /* [in] */ ICharSequence* src,
        /* [in] */ NormalizerForm form,
        /* [out] */ Boolean* isNormalized);

    CARAPI Normalize(
        /* [in] */ ICharSequence* src,
        /* [in] */ NormalizerForm form,
        /* [out] */ String* normalizeString);

};

} // namespace Text
} // namespace Elastos

#endif // __ELASTOS_TEXT_CNORMALIZERHELPER_H__
