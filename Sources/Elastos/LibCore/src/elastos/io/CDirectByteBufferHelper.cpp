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

#include "elastos/io/CDirectByteBufferHelper.h"
#include "elastos/io/DirectByteBuffer.h"

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(CDirectByteBufferHelper, Singleton, IDirectByteBufferHelper)

CAR_SINGLETON_IMPL(CDirectByteBufferHelper)

ECode CDirectByteBufferHelper::GetNewInstance(
    /* [in] */ Int64 address,
    /* [in] */ Int32 capacity,
    /* [out] */ IDirectByteBuffer** instance)
{
    AutoPtr<DirectByteBuffer> buf = new DirectByteBuffer();
    buf->constructor(address, capacity);
    *instance = buf;
    REFCOUNT_ADD(*instance)
    return NOERROR;
}

}
}