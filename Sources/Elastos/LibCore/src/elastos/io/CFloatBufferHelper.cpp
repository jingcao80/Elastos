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

#include "CFloatBufferHelper.h"
#include "FloatBuffer.h"

namespace Elastos {
namespace IO {
CAR_SINGLETON_IMPL(CFloatBufferHelper)

CAR_INTERFACE_IMPL(CFloatBufferHelper, Singleton, IFloatBufferHelper)

ECode CFloatBufferHelper::Allocate(
    /* [in] */ Int32 capacity,
    /* [out] */ IFloatBuffer** floatBuffer)
{
    return FloatBuffer::Allocate(capacity, floatBuffer);
}

ECode CFloatBufferHelper::Wrap(
    /* [in] */ ArrayOf<Float>* array,
    /* [out] */ IFloatBuffer** floatBuffer)
{
    return FloatBuffer::Wrap(array, floatBuffer);
}

ECode CFloatBufferHelper::Wrap(
    /* [in] */ ArrayOf<Float>* array,
    /* [in] */ Int32 start,
    /* [in] */ Int32 floatCount,
    /* [out] */ IFloatBuffer** floatBuffer)
{
    return FloatBuffer::Wrap(array, start, floatCount, floatBuffer);
}

} // namespace IO
} // namespace Elastos

