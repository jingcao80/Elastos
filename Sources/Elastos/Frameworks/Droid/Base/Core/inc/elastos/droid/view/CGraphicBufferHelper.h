
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
