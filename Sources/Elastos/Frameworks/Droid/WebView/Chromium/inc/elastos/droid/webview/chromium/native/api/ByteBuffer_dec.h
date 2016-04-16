//./Elastos/LibCore/inc/Elastos/IO/Buffer.h
//put this file at the end of the include list
//so the type definition used in this file will be found
#ifndef ELASTOS_BYTEBUFFER_CALLBACK_DEC_HH
#define ELASTOS_BYTEBUFFER_CALLBACK_DEC_HH

#ifdef __cplusplus
extern "C"
{
#endif
    extern void Elastos_ByteBuffer_InitCallback(Handle32 cb);
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

    Elastos_ByteBuffer_InitCallback((Handle32)&sElaByteBufferCallback);
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
