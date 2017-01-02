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

#include "CCoderResultHelper.h"
#include "CCoderResult.h"

namespace Elastos {
namespace IO {
namespace Charset {

CAR_INTERFACE_IMPL(CCoderResultHelper, Singleton, ICoderResultHelper)

CAR_SINGLETON_IMPL(CCoderResultHelper)

ECode CCoderResultHelper::GetUNDERFLOW(
    /* [out] */ ICoderResult** result)
{
    return CCoderResult::GetUNDERFLOW(result);
}

ECode CCoderResultHelper::GetOVERFLOW(
    /* [out] */ ICoderResult** result)
{
    return CCoderResult::GetOVERFLOW(result);
}

ECode CCoderResultHelper::MalformedForLength(
    /* [in] */ Int32 length,
    /* [out] */ ICoderResult** result)
{
    return CCoderResult::MalformedForLength(length, result);
}

ECode CCoderResultHelper::UnmappableForLength(
    /* [in] */ Int32 length,
    /* [out] */ ICoderResult** result)
{
    return CCoderResult::UnmappableForLength(length, result);
}

} // namespace Charset
} // namespace IO
} // namespace Elastos
