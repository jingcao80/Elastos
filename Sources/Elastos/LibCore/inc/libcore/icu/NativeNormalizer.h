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

#ifndef __LIBCORE_ICU_NATIVENORMALIZER_H__
#define __LIBCORE_ICU_NATIVENORMALIZER_H__

#include "Elastos.CoreLibrary.h"

using Elastos::Core::ICharSequence;
using Elastos::Text::NormalizerForm;

namespace Libcore {
namespace ICU {

class NativeNormalizer
{
public:
    static CARAPI IsNormalized(
        /* [in] */ ICharSequence* src,
        /* [in] */ NormalizerForm form,
        /* [out] */ Boolean* isNormalized);

    static CARAPI Normalize(
        /* [in] */ ICharSequence* src,
        /* [in] */ NormalizerForm form,
        /* [out] */ String* result);

private:
    NativeNormalizer() {}

    static CARAPI_(Int32) ToUNormalizationMode(
        /* [in] */ NormalizerForm form);

    static CARAPI NormalizeImpl(
        /* [in] */ const String& src,
        /* [in] */ NormalizerForm form,
        /* [out] */ String* result);

    static CARAPI IsNormalizedImpl(
        /* [in] */ const String& src,
        /* [in] */ NormalizerForm form,
        /* [out] */ Boolean* isNormalized);
};

} // namespace ICU
} // namespace Libcore

#endif //__LIBCORE_ICU_NATIVENORMALIZER_H__
