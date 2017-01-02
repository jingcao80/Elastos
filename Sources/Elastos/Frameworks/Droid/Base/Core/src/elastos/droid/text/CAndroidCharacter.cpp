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

#include "elastos/droid/text/CAndroidCharacter.h"

namespace Elastos {
namespace Droid {
namespace Text {

CAR_INTERFACE_IMPL(CAndroidCharacter, Object, IAndroidCharacter)

CAR_SINGLETON_IMPL(CAndroidCharacter)

ECode CAndroidCharacter::GetDirectionalities(
    /* [in] */ ArrayOf<Char32>* src,
    /* [in] */ ArrayOf<Byte>* dest,
    /* [in] */ Int32 count)
{
    return E_NOT_IMPLEMENTED;
}

ECode CAndroidCharacter::GetEastAsianWidth(
    /* [in] */ Char32 input,
    /* [out] */ Int32* result)
{
    return NOERROR;
}

ECode CAndroidCharacter::GetEastAsianWidths(
    /* [in] */ ArrayOf<Char32>* src,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [in] */ ArrayOf<Byte>* dest)
{
    return NOERROR;
}

ECode CAndroidCharacter::Mirror(
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [out] */ Boolean* result)
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CAndroidCharacter::GetMirror(
    /* [in] */ Char32 ch,
    /* [out] */ Char32* result)
{
    // TODO: Add your code here
    return NOERROR;
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos