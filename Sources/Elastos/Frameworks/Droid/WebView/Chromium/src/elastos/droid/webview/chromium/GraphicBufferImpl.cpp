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

#include "elastos/droid/webkit/webview/chromium/GraphicBufferImpl.h"
#include <utils/Errors.h>

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {

GraphicBufferImpl::GraphicBufferImpl(UInt32 w, UInt32 h)
      : mBuffer(new android::GraphicBuffer(w, h, android::PIXEL_FORMAT_RGBA_8888,
         android::GraphicBuffer::USAGE_HW_TEXTURE |
         android::GraphicBuffer::USAGE_SW_READ_OFTEN |
         android::GraphicBuffer::USAGE_SW_WRITE_OFTEN)) {
}

GraphicBufferImpl::~GraphicBufferImpl() {
}

// static
long GraphicBufferImpl::Create(Int32 w, Int32 h) {
    GraphicBufferImpl* buffer = new GraphicBufferImpl(
        static_cast<UInt32>(w), static_cast<UInt32>(h));
    if (buffer->InitCheck() != android::NO_ERROR) {
        delete buffer;
        return 0;
    }
    return reinterpret_cast<intptr_t>(buffer);
}

// static
void GraphicBufferImpl::Release(long buffer_id) {
    GraphicBufferImpl* buffer = reinterpret_cast<GraphicBufferImpl*>(buffer_id);
    delete buffer;
}

// static
Int32 GraphicBufferImpl::MapStatic(long buffer_id, AwMapMode mode, void** vaddr) {
    GraphicBufferImpl* buffer = reinterpret_cast<GraphicBufferImpl*>(buffer_id);
    return buffer->Map(mode, vaddr);
}

// static
Int32 GraphicBufferImpl::UnmapStatic(long buffer_id) {
    GraphicBufferImpl* buffer = reinterpret_cast<GraphicBufferImpl*>(buffer_id);
    return buffer->Unmap();
}

// static
void* GraphicBufferImpl::GetNativeBufferStatic(long buffer_id) {
    GraphicBufferImpl* buffer = reinterpret_cast<GraphicBufferImpl*>(buffer_id);
    return buffer->GetNativeBuffer();
}
// static
UInt32 GraphicBufferImpl::GetStrideStatic(long buffer_id) {
    GraphicBufferImpl* buffer = reinterpret_cast<GraphicBufferImpl*>(buffer_id);
    return buffer->GetStride();
}

android::status_t GraphicBufferImpl::Map(AwMapMode mode, void** vaddr) {
    Int32 usage = 0;
    switch (mode) {
        case MAP_READ_ONLY:
            usage = android::GraphicBuffer::USAGE_SW_READ_OFTEN;
            break;
        case MAP_WRITE_ONLY:
            usage = android::GraphicBuffer::USAGE_SW_WRITE_OFTEN;
            break;
        case MAP_READ_WRITE:
            usage = android::GraphicBuffer::USAGE_SW_READ_OFTEN |
                android::GraphicBuffer::USAGE_SW_WRITE_OFTEN;
            break;
        default:
            return android::INVALID_OPERATION;
    }
    return mBuffer->lock(usage, vaddr);
}

android::status_t GraphicBufferImpl::Unmap() {
    return mBuffer->unlock();
}
android::status_t GraphicBufferImpl::InitCheck() const {
    return mBuffer->initCheck();
}

void* GraphicBufferImpl::GetNativeBuffer() const {
    return mBuffer->getNativeBuffer();
}

UInt32 GraphicBufferImpl::GetStride() const {
    static const Int32 kBytesPerPixel = 4;
    return mBuffer->getStride() * kBytesPerPixel;
}

} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

