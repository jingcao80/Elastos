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

#include "elastos/core/Object.h"
#include "elastos/io/CNIOAccess.h"
#include "elastos/io/NIOAccess.h"

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(CNIOAccess, Singleton, INIOAccess)

CAR_SINGLETON_IMPL(CNIOAccess)

ECode CNIOAccess::GetBasePointer(
    /* [in] */ IBuffer* b,
    /* [out] */ Int64* pointer)
{
    return NIOAccess::GetBasePointer(b, pointer);
}

ECode CNIOAccess::GetBaseArray(
        /* [in] */ IBuffer* b,
        /* [out] */ IInterface** result)
{
    return NIOAccess::GetBaseArray(b, result);
}

ECode CNIOAccess::GetRemainingBytes(
    /* [in] */ IBuffer* b,
    /* [out] */ Int32* remaining)
{
    return NIOAccess::GetRemainingBytes(b, remaining);
}

ECode CNIOAccess::GetBaseArrayOffset(
        /* [in] */ IBuffer* b,
        /* [out] */ Int32* result)
{
    return NIOAccess::GetBaseArrayOffset(b, result);
}

} // namespace IO
} // namespace Elastos