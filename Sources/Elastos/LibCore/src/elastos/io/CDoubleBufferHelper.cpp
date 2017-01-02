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

#include "CDoubleBufferHelper.h"
#include "DoubleBuffer.h"

namespace Elastos {
namespace IO {

CAR_SINGLETON_IMPL(CDoubleBufferHelper)

CAR_INTERFACE_IMPL(CDoubleBufferHelper, Singleton, IDoubleBufferHelper)

ECode CDoubleBufferHelper::Allocate(
    /* [in] */ Int32 capacity,
    /* [out] */ IDoubleBuffer** doubleBuf)
{
    return DoubleBuffer::Allocate(capacity, doubleBuf);
}

ECode CDoubleBufferHelper::Wrap(
    /* [in] */ ArrayOf<Double>* array,
    /* [out] */ IDoubleBuffer** doubleBuf)
{
    return DoubleBuffer::Wrap(array, doubleBuf);
}

ECode CDoubleBufferHelper::Wrap(
    /* [in] */ ArrayOf<Double>* array,
    /* [in] */ Int32 start,
    /* [in] */ Int32 doubleCount,
    /* [out] */ IDoubleBuffer** doubleBuf)
{
    return DoubleBuffer::Wrap(array, start, doubleCount, doubleBuf);
}

} // namespace IO
} // namespace Elastos
