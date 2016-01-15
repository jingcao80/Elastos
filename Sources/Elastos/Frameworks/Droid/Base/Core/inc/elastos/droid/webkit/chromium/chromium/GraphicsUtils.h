
#ifndef __ELASTOS_DROID_CHROMIUM_CHROMIUM_GRAPHICSUTILS_H__
#define __ELASTOS_DROID_CHROMIUM_CHROMIUM_GRAPHICSUTILS_H__

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Chromium {
namespace Chromium {

class GraphicsUtils
{
public:
    static CARAPI_(Int64) GetDrawSWFunctionTable();

    static CARAPI_(Int64) GetDrawGLFunctionTable();

private:
    static CARAPI_(Int64) NativeGetDrawSWFunctionTable();

    static CARAPI_(Int64) NativeGetDrawGLFunctionTable();
};

} // namespace Chromium
} // namespace Chromium
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_CHROMIUM_CHROMIUM_GRAPHICSUTILS_H__
