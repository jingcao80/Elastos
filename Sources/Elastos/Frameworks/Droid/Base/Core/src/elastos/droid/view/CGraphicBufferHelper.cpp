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

#include "elastos/droid/view/CGraphicBufferHelper.h"
#include "elastos/droid/view/GraphicBuffer.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(CGraphicBufferHelper, Singleton, IGraphicBufferHelper)

CAR_SINGLETON_IMPL(CGraphicBufferHelper);

/**
 * Creates new <code>GraphicBuffer</code> instance. This method will return null
 * if the buffer cannot be created.
 *
 * @param width The width in pixels of the buffer
 * @param height The height in pixels of the buffer
 * @param format The format of each pixel as specified in {@link PixelFormat}
 * @param usage Hint indicating how the buffer will be used
 *
 * @return A <code>GraphicBuffer</code> instance or null
 */
ECode CGraphicBufferHelper::Create(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 format,
    /* [in] */ Int32 usage,
    /* [out] */ IGraphicBuffer** buf)
{
    return GraphicBuffer::Create(width, height, format, usage, buf);
}

} // namespace View
} // namepsace Droid
} // namespace Elastos