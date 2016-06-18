
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