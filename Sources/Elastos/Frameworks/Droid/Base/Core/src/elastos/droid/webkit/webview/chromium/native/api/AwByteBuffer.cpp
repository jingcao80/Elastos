
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
    Handle64 effectiveDirectAddress;
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
