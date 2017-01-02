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

#include "CStandardCharsets.h"
#include "StandardCharsets.h"

namespace Elastos {
namespace IO {
namespace Charset {

CAR_INTERFACE_IMPL(CStandardCharsets, Singleton, IStandardCharsets)

CAR_SINGLETON_IMPL(CStandardCharsets)

ECode CStandardCharsets::GetISO_8859_1(
    /* [out] */ ICharset** outcharset)
{
    VALIDATE_NOT_NULL(outcharset)

    *outcharset = StandardCharsets::ISO_8859_1;
    REFCOUNT_ADD(*outcharset)
    return NOERROR;
}

ECode CStandardCharsets::GetUS_ASCII(
    /* [out] */ ICharset** outcharset)
{
    VALIDATE_NOT_NULL(outcharset)

    *outcharset = StandardCharsets::US_ASCII;
    REFCOUNT_ADD(*outcharset)
    return NOERROR;
}

ECode CStandardCharsets::GetUTF_8(
    /* [out] */ ICharset** outcharset)
{
    VALIDATE_NOT_NULL(outcharset)

    *outcharset = StandardCharsets::UTF_8;
    REFCOUNT_ADD(*outcharset)
    return NOERROR;
}

ECode CStandardCharsets::GetUTF_16(
    /* [out] */ ICharset** outcharset)
{
    VALIDATE_NOT_NULL(outcharset)

    *outcharset = StandardCharsets::UTF_16;
    REFCOUNT_ADD(*outcharset)
    return NOERROR;
}

ECode CStandardCharsets::GetUTF_16BE(
    /* [out] */ ICharset** outcharset)
{
    VALIDATE_NOT_NULL(outcharset)

    *outcharset = StandardCharsets::UTF_16BE;
    REFCOUNT_ADD(*outcharset)
    return NOERROR;
}

ECode CStandardCharsets::GetUTF_16LE(
    /* [out] */ ICharset** outcharset)
{
    VALIDATE_NOT_NULL(outcharset)

    *outcharset = StandardCharsets::UTF_16LE;
    REFCOUNT_ADD(*outcharset)
    return NOERROR;
}

} // namespace Charset
} // namespace IO
} // namespace Elastos
