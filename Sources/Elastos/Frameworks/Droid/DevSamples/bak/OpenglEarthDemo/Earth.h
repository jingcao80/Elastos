#ifndef __EARTH_H__
#define __EARTH_H__

#include "Elastos.Droid.Core.h"

using Elastos::IO::IFloatBuffer;
using Elastos::Droid::Opengl::IGLSurfaceView;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace OpenglEarthDemo {

class Earth : public ElRefBase
{
public:
    Earth(
        /* [in] */ IGLSurfaceView* mv,
        /* [in] */ Float r);

    CARAPI InitVertexData(
        /* [in] */ Float r);

    CARAPI InitShader(
        /* [in] */ IGLSurfaceView* mv);

    CARAPI DrawSelf(
        /* [in] */ Int32 texId,
        /* [in] */ Int32 texIdNight);

    CARAPI_(AutoPtr<ArrayOf<Float> >) GenerateTexCoor(
        /* [in] */ Int32 bw,
        /* [in] */ Int32 bh);

    Int32 mProgram;//自定义渲染管线程序id
    Int32 muMVPMatrixHandle;//总变换矩阵引用id
    Int32 muMMatrixHandle;//位置、旋转变换矩阵
    Int32 maCameraHandle; //摄像机位置属性引用id
    Int32 maPositionHandle; //顶点位置属性引用id
    Int32 maNormalHandle; //顶点法向量属性引用id
    Int32 maTexCoorHandle; //顶点纹理坐标属性引用id
    Int32 maSunLightLocationHandle;//光源位置属性引用id
    Int32 mDayTexHandle;//白天纹理属性引用id
    Int32 mNightTexHandle;//黑夜纹理属性引用id
    String mVertexShader;//顶点着色器
    String mFragmentShader;//片元着色器
    AutoPtr<IFloatBuffer> mVertexBuffer;//顶点坐标数据缓冲
    AutoPtr<IFloatBuffer> mTexCoorBuffer;//顶点纹理坐标数据缓冲
    Int32 mVCount;
};

} // namespace OpenglEarthDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif