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

#include "CInt64BufferHelper.h"
#include "Int64Buffer.h"

namespace Elastos {
namespace IO {
CAR_SINGLETON_IMPL(CInt64BufferHelper)

CAR_INTERFACE_IMPL(CInt64BufferHelper, Singleton, IInt64BufferHelper)

ECode CInt64BufferHelper::Allocate(
    /* [in] */ Int32 capacity,
    /* [out] */ IInt64Buffer** int64Buffer)
{
    return Int64Buffer::Allocate(capacity, int64Buffer);
}

ECode CInt64BufferHelper::Wrap(
    /* [in] */ ArrayOf<Int64>* array,
    /* [out] */ IInt64Buffer** int64Buffer)
{
    return Int64Buffer::Wrap(array, int64Buffer);
}

ECode CInt64BufferHelper::Wrap(
    /* [in] */ ArrayOf<Int64>* array,
    /* [in] */ Int32 start,
    /* [in] */ Int32 int64Count,
    /* [out] */ IInt64Buffer** int64Buffer)
{
    return Int64Buffer::Wrap(array, start, int64Count, int64Buffer);
}

} // namespace IO
} // namespace Elastos

