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

#include "CollationKey.h"

using Elastos::Core::EIID_IComparable;

namespace Elastos {
namespace Text {

CAR_INTERFACE_IMPL_2(CollationKey, Object, ICollationKey, IComparable)

CollationKey::CollationKey()
{}

CollationKey:: ~CollationKey()
{}

ECode CollationKey::constructor(
    /* [in] */ const String& source)
{
    mSource = String(source);
    return NOERROR;
}

ECode CollationKey::GetSourceString(
    /* [out] */ String* sourceString)
{
    VALIDATE_NOT_NULL(sourceString);
    *sourceString = mSource;
    return NOERROR;
}

} // namespace Text
} // namespace Elastos
