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

#include "CInt16BufferHelper.h"
#include "Int16Buffer.h"

namespace Elastos {
namespace IO {
CAR_SINGLETON_IMPL(CInt16BufferHelper)

CAR_INTERFACE_IMPL(CInt16BufferHelper, Singleton, IInt16BufferHelper)

ECode CInt16BufferHelper::Allocate(
    /* [in] */ Int32 capacity,
    /* [out] */ IInt16Buffer** int16Buffer)
{
    return Int16Buffer::Allocate(capacity, int16Buffer);
}

ECode CInt16BufferHelper::Wrap(
    /* [in] */ ArrayOf<Int16>* array,
    /* [out] */ IInt16Buffer** int16Buffer)
{
    return Int16Buffer::Wrap(array, int16Buffer);
}

ECode CInt16BufferHelper::Wrap(
    /* [in] */ ArrayOf<Int16>* array,
    /* [in] */ Int32 start,
    /* [in] */ Int32 int16Count,
    /* [out] */ IInt16Buffer** int16Buffer)
{
    return Int16Buffer::Wrap(array, start, int16Count, int16Buffer);
}

} // namespace IO
} // namespace Elastos
