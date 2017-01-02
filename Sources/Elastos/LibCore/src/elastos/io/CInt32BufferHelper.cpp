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

#include "CInt32BufferHelper.h"
#include "Int32Buffer.h"

namespace Elastos {
namespace IO {
CAR_SINGLETON_IMPL(CInt32BufferHelper)

CAR_INTERFACE_IMPL(CInt32BufferHelper, Singleton, IInt32BufferHelper)

ECode CInt32BufferHelper::Allocate(
    /* [in] */ Int32 capacity,
    /* [out] */ IInt32Buffer** int32Buffer)
{
    return Int32Buffer::Allocate(capacity, int32Buffer);
}

ECode CInt32BufferHelper::Wrap(
    /* [in] */ ArrayOf<Int32>* array,
    /* [out] */ IInt32Buffer** int32Buffer)
{
    return Int32Buffer::Wrap(array, int32Buffer);
}

ECode CInt32BufferHelper::Wrap(
    /* [in] */ ArrayOf<Int32>* array,
    /* [in] */ Int32 start,
    /* [in] */ Int32 int32Count,
    /* [out] */ IInt32Buffer** int32Buffer)
{
    return Int32Buffer::Wrap(array, start, int32Count, int32Buffer);
}

} // namespace IO
} // namespace Elastos

