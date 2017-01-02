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

#include "Normalizer.h"
#include "NativeNormalizer.h"

using Libcore::ICU::NativeNormalizer;

namespace Elastos {
namespace Text {

Boolean Normalizer::IsNormalized(
    /* [in] */ ICharSequence* src,
    /* [in] */ NormalizerForm form)
{
    assert(src);

    Boolean result;
    NativeNormalizer::IsNormalized(src, form, &result);
    return result;
}

String Normalizer::Normalize(
    /* [in] */ ICharSequence* src,
    /* [in] */ NormalizerForm form)
{
    assert(src);

    String result;
    NativeNormalizer::Normalize(src, form, &result);
    return result;
}

} // namespace Text
} // namespace Elastos
