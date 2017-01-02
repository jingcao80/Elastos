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

#ifndef __ELASTOS_DROID_VIEW_CGRAPHICS_BUFFER_HELPER_H__
#define __ELASTOS_DROID_VIEW_CGRAPHICS_BUFFER_HELPER_H__

#include "_Elastos_Droid_View_CGraphicBufferHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CGraphicBufferHelper)
    , public Singleton
    , public IGraphicBufferHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL();

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
    CARAPI Create(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 format,
        /* [in] */ Int32 usage,
        /* [out] */ IGraphicBuffer** buf);
};

} // namespace View
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_VIEW_CGRAPHICS_BUFFER_HELPER_H__
