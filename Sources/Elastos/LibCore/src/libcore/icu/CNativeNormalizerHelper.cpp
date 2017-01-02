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

#include "CNativeNormalizerHelper.h"
#include "NativeNormalizer.h"

namespace Libcore {
namespace ICU {

CAR_SINGLETON_IMPL(CNativeNormalizerHelper)

CAR_INTERFACE_IMPL(CNativeNormalizerHelper, Singleton, INativeNormalizerHelper)

ECode CNativeNormalizerHelper::IsNormalized(
    /* [in] */ ICharSequence * src,
    /* [in] */ Form form,
    /* [out] */ Boolean * value)
{
    return NativeNormalizer::IsNormalized(src,form,value);
}

ECode CNativeNormalizerHelper::Normalize(
    /* [in] */ ICharSequence * src,
    /* [in] */ Form form,
    /* [out] */ String * str)
{
    return NativeNormalizer::Normalize(src,form,str);
}

} // namespace ICU
} // namespace Libcore
