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

#include "CNormalizerHelper.h"
#include "Normalizer.h"

namespace Elastos {
namespace Text {

CAR_INTERFACE_IMPL(CNormalizerHelper, Singleton, INormalizer)

CAR_SINGLETON_IMPL(CNormalizerHelper)

ECode CNormalizerHelper::IsNormalized(
    /* [in] */ ICharSequence* src,
    /* [in] */ NormalizerForm form,
    /* [out] */ Boolean* isNormalized)
{
    VALIDATE_NOT_NULL(isNormalized)
    *isNormalized = FALSE;
    VALIDATE_NOT_NULL(src)
    *isNormalized = Normalizer::IsNormalized(src, form);
    return NOERROR;
}

ECode CNormalizerHelper::Normalize(
    /* [in] */ ICharSequence* src,
    /* [in] */ NormalizerForm form,
    /* [out] */ String* normalizeString)
{
    VALIDATE_NOT_NULL(normalizeString)
    *normalizeString = String(NULL);
    VALIDATE_NOT_NULL(src)

    *normalizeString = Normalizer::Normalize(src, form);
    return NOERROR;
}

} // namespace Text
} // namespace Elastos

