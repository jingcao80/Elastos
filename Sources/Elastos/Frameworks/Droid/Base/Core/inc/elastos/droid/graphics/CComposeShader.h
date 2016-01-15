
#ifndef __ELASTOS_DROID_GRAPHICS_CCOMPOSESHADER_H__
#define __ELASTOS_DROID_GRAPHICS_CCOMPOSESHADER_H__

#include "_Elastos_Droid_Graphics_CComposeShader.h"
#include "elastos/droid/graphics/Shader.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CComposeShader)
    , public Shader
    , public IComposeShader
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CComposeShader();

    /**
     * Create a new compose shader, given shaders A, B, and a combining mode.
     * When the mode is applied, it will be given the result from shader A as its
     * "dst", and the result from shader B as its "src".
     *
     * @param shaderA  The colors from this shader are seen as the "dst" by the mode
     * @param shaderB  The colors from this shader are seen as the "src" by the mode
     * @param mode     The mode that combines the colors from the two shaders. If mode
     *                 is null, then SRC_OVER is assumed.
     */
    CARAPI constructor(
        /* [in] */ IShader* shaderA,
        /* [in] */ IShader* shaderB,
        /* [in] */ IXfermode* mode);

    /**
     * Create a new compose shader, given shaders A, B, and a combining PorterDuff mode.
     * When the mode is applied, it will be given the result from shader A as its
     * "dst", and the result from shader B as its "src".
     *
     * @param shaderA  The colors from this shader are seen as the "dst" by the mode
     * @param shaderB  The colors from this shader are seen as the "src" by the mode
     * @param mode     The PorterDuff mode that combines the colors from the two shaders.
     */
    CARAPI constructor(
        /* [in] */ IShader* shaderA,
        /* [in] */ IShader* shaderB,
        /* [in] */ PorterDuffMode mode);

protected:
    /**
     * @hide
     */
    // @Override
    CARAPI Copy(
        /* [out] */ IShader** shader);

private:
    static CARAPI_(Int64) NativeCreate1(
        /* [in] */ Int64 nativeShaderA,
        /* [in] */ Int64 nativeShaderB,
        /* [in] */ Int64 nativeMode);

    static CARAPI_(Int64) NativeCreate2(
        /* [in] */ Int64 nativeShaderA,
        /* [in] */ Int64 nativeShaderB,
        /* [in] */ Int32 porterDuffMode);

private:
    static const Int32 TYPE_XFERMODE;
    static const Int32 TYPE_PORTERDUFFMODE;

    /**
     * Type of the ComposeShader: can be either TYPE_XFERMODE or TYPE_PORTERDUFFMODE
     */
    Int32 mType;

    AutoPtr<IXfermode> mXferMode;
    PorterDuffMode mPorterDuffMode;

    /**
     * Hold onto the shaders to avoid GC.
     */
    AutoPtr<IShader> mShaderA;

    AutoPtr<IShader> mShaderB;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CCOMPOSESHADER_H__
