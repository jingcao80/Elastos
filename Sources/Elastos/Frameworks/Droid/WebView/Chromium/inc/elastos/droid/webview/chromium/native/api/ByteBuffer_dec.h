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

//./Elastos/LibCore/inc/Elastos/IO/Buffer.h
//put this file at the end of the include list
//so the type definition used in this file will be found
#ifndef ELASTOS_BYTEBUFFER_CALLBACK_DEC_HH
#define ELASTOS_BYTEBUFFER_CALLBACK_DEC_HH

#ifdef __cplusplus
extern "C"
{
#endif
    extern void Elastos_ByteBuffer_InitCallback(HANDLE cb);
#ifdef __cplusplus
}
#endif

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Api {

struct ElaByteBufferCallback
{
    void* (*elastos_ByteBuffer_GetDirectBufferAddress)(IInterface* obj);
    Int64 (*elastos_ByteBuffer_GetDirectBufferCapacity)(IInterface* obj);
};

void* AwByteBuffer::ElaByteBufferCallback_Init()
{
    static ElaByteBufferCallback sElaByteBufferCallback;

    sElaByteBufferCallback.elastos_ByteBuffer_GetDirectBufferAddress = &AwByteBuffer::GetDirectBufferAddress;
    sElaByteBufferCallback.elastos_ByteBuffer_GetDirectBufferCapacity = &AwByteBuffer::GetDirectBufferCapacity;

    Elastos_ByteBuffer_InitCallback((HANDLE)&sElaByteBufferCallback);
    return &sElaByteBufferCallback;
}

static void* sPElaByteBufferCallback = AwByteBuffer::ElaByteBufferCallback_Init();

} // namespace Api
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif //
