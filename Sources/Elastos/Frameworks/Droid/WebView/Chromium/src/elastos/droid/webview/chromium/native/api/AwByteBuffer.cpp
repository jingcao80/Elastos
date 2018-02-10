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

#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/webkit/webview/chromium/native/api/AwByteBuffer.h"
#include "elastos/droid/webkit/webview/chromium/native/api/ByteBuffer_dec.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::IO::IBuffer;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Api {

void* AwByteBuffer::GetDirectBufferAddress(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IBuffer> mObj = IBuffer::Probe(obj);// refer to art/runtime/jni_internal.cc:2524
    if (NULL == mObj)
    {
        Logger::E("AwByteBuffer", "AwByteBuffer::GetDirectBufferAddress, mObj is NULL");
        return NULL;
    }
    HANDLE effectiveDirectAddress;
    mObj->GetEffectiveDirectAddress(&effectiveDirectAddress);
    return (void*)effectiveDirectAddress;
}

Int64 AwByteBuffer::GetDirectBufferCapacity(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IBuffer> mObj = IBuffer::Probe(obj);// refer to art/runtime/jni_internal.cc:2524
    if (NULL == mObj)
    {
        Logger::E("AwByteBuffer", "AwByteBuffer::GetDirectBufferCapacity, mObj is NULL");
        return 0;
    }
    Int32 cap;
    mObj->GetCapacity(&cap);
    return cap;
}

} // namespace Api
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
