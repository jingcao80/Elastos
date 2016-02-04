#include "Moon.h"
#include "MatrixState.h"
#include <elastos/utility/etl/List.h>
#include <elastos/core/Math.h>
#include "ShaderUtil.h"

using Elastos::Utility::Etl::List;
using Elastos::IO::CByteBufferHelper;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::ByteOrder;
using Elastos::IO::IByteOrderHelper;
using Elastos::IO::CByteOrderHelper;
using Elastos::IO::IByteBuffer;
using Elastos::Droid::Opengl::IGLES20;
using Elastos::Droid::Opengl::CGLES20;
using Elastos::Droid::Opengl::CMatrix;
using Elastos::Droid::Opengl::IMatrix;
using Elastos::Droid::Content::Res::IResources;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace OpenglEarthDemo {

Moon::Moon(
    /* [in] */ IGLSurfaceView* mv,
    /* [in] */ Float r)
{
    InitVertexData(r);
    InitShader(mv);
}

ECode Moon::InitVertexData(
    /* [in] */ Float r)
{
    //顶点坐标数据的初始化================begin============================
    Float UNIT_SIZE = 0.5f;
    List<Float> alVertix;//存放顶点坐标的ArrayList
    Float angleSpan = 10.f;//将球进行单位切分的角度
    for(Float vAngle = 90; vAngle > -90; vAngle = vAngle - angleSpan)//垂直方向angleSpan度一份
    {
        for(Float hAngle = 360; hAngle > 0; hAngle = hAngle - angleSpan)//水平方向angleSpan度一份
        {//纵向横向各到一个角度后计算对应的此点在球面上的坐标
            Double xozLength = r * UNIT_SIZE * Elastos::Core::Math::Cos(Elastos::Core::Math::ToRadians(vAngle));
            Float x1 = (Float)(xozLength * Elastos::Core::Math::Cos(Elastos::Core::Math::ToRadians(hAngle)));
            Float z1 = (Float)(xozLength * Elastos::Core::Math::Sin(Elastos::Core::Math::ToRadians(hAngle)));
            Float y1 = (Float)(r * UNIT_SIZE * Elastos::Core::Math::Sin(Elastos::Core::Math::ToRadians(vAngle)));

            xozLength = r * UNIT_SIZE * Elastos::Core::Math::Cos(Elastos::Core::Math::ToRadians(vAngle-angleSpan));
            Float x2 = (Float)(xozLength * Elastos::Core::Math::Cos(Elastos::Core::Math::ToRadians(hAngle)));
            Float z2 = (Float)(xozLength * Elastos::Core::Math::Sin(Elastos::Core::Math::ToRadians(hAngle)));
            Float y2 = (Float)(r * UNIT_SIZE  *  Elastos::Core::Math::Sin(Elastos::Core::Math::ToRadians(vAngle-angleSpan)));

            xozLength = r * UNIT_SIZE * Elastos::Core::Math::Cos(Elastos::Core::Math::ToRadians(vAngle-angleSpan));
            Float x3 = (Float)(xozLength * Elastos::Core::Math::Cos(Elastos::Core::Math::ToRadians(hAngle-angleSpan)));
            Float z3 = (Float)(xozLength * Elastos::Core::Math::Sin(Elastos::Core::Math::ToRadians(hAngle-angleSpan)));
            Float y3 = (Float)(r * UNIT_SIZE * Elastos::Core::Math::Sin(Elastos::Core::Math::ToRadians(vAngle-angleSpan)));

            xozLength = r * UNIT_SIZE * Elastos::Core::Math::Cos(Elastos::Core::Math::ToRadians(vAngle));
            Float x4 = (Float)(xozLength * Elastos::Core::Math::Cos(Elastos::Core::Math::ToRadians(hAngle-angleSpan)));
            Float z4 = (Float)(xozLength * Elastos::Core::Math::Sin(Elastos::Core::Math::ToRadians(hAngle-angleSpan)));
            Float y4 = (Float)(r * UNIT_SIZE * Elastos::Core::Math::Sin(Elastos::Core::Math::ToRadians(vAngle)));

            //构建第一三角形
            alVertix.PushBack(x1); alVertix.PushBack(y1); alVertix.PushBack(z1);
            alVertix.PushBack(x2); alVertix.PushBack(y2); alVertix.PushBack(z2);
            alVertix.PushBack(x4); alVertix.PushBack(y4); alVertix.PushBack(z4);
            //构建第二三角形
            alVertix.PushBack(x4); alVertix.PushBack(y4); alVertix.PushBack(z4);
            alVertix.PushBack(x2); alVertix.PushBack(y2); alVertix.PushBack(z2);
            alVertix.PushBack(x3); alVertix.PushBack(y3); alVertix.PushBack(z3);
        }
    }
    mVCount = alVertix.GetSize() / 3;//顶点的数量为坐标值数量的1/3，因为一个顶点有3个坐标

    //将alVertix中的坐标值转存到一个Float数组中
    AutoPtr<ArrayOf<Float> > vertices = ArrayOf<Float>::Alloc(mVCount * 3);
    List<Float>::Iterator it = alVertix.Begin();
    for(Int32 i = 0; it != alVertix.End(); it++, i++)
    {
        (*vertices)[i] = *it;
    }

    AutoPtr<IByteBufferHelper> helper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&helper);
    AutoPtr<IByteBuffer> vbb;
    helper->AllocateDirect(vertices->GetLength() * 4, (IByteBuffer**)&vbb);
    AutoPtr<IByteOrderHelper> orderHelper;
    CByteOrderHelper::AcquireSingleton((IByteOrderHelper**)&orderHelper);
    ByteOrder nativeOrder;
    orderHelper->GetNativeOrder(&nativeOrder);
    vbb->SetOrder(nativeOrder);
    vbb->AsFloatBuffer((IFloatBuffer**)&mVertexBuffer);

    mVertexBuffer->PutFloats(*vertices);
    mVertexBuffer->SetPosition(0);

    //特别提示：由于不同平台字节顺序不同数据单元不是字节的一定要经过ByteBuffer
    //转换，关键是要通过ByteOrder设置nativeOrder()，否则有可能会出问题

    //将alTexCoor中的纹理坐标值转存到一个Float数组中
    AutoPtr<ArrayOf<Float> > texCoor = GenerateTexCoor//获取切分整图的纹理数组
    (
         (Int32)(360/angleSpan), //纹理图切分的列数
         (Int32)(180/angleSpan)  //纹理图切分的行数
    );
    AutoPtr<IByteBuffer> llbb;
    helper->AllocateDirect(texCoor->GetLength() * 4, (IByteBuffer**)&llbb);
    llbb->SetOrder(nativeOrder);
    llbb->AsFloatBuffer((IFloatBuffer**)&mTexCoorBuffer);
    mTexCoorBuffer->PutFloats(*texCoor);
    mTexCoorBuffer->SetPosition(0);
    //顶点坐标数据的初始化================end============================
    return NOERROR;
}

ECode Moon::InitShader(
    /* [in] */ IGLSurfaceView* mv)
{
    AutoPtr<IResources> res;
    mv->GetResources((IResources**)&res);
    //加载顶点着色器的脚本内容
    mVertexShader = ShaderUtil::LoadFromAssetsFile(String("vertex_moon.sh"), res);
    //加载片元着色器的脚本内容
    mFragmentShader = ShaderUtil::LoadFromAssetsFile(String("frag_moon.sh"), res);
    //基于顶点着色器与片元着色器创建程序
    mProgram = ShaderUtil::CreateProgram(mVertexShader, mFragmentShader);
    //获取程序中顶点位置属性引用id
    AutoPtr<IGLES20> gl;
    CGLES20::AcquireSingleton((IGLES20**)&gl);
    gl->GlGetAttribLocation(mProgram, String("aPosition"), &maPositionHandle);
    //获取程序中顶点经纬度属性引用id
    gl->GlGetAttribLocation(mProgram, String("aTexCoor"), &maTexCoorHandle);
    //获取程序中顶点法向量属性引用id
    gl->GlGetAttribLocation(mProgram, String("aNormal"), &maNormalHandle);
    //获取程序中总变换矩阵引用id
    gl->GlGetUniformLocation(mProgram, String("uMVPMatrix"), &muMVPMatrixHandle);
    //获取程序中摄像机位置引用id
    gl->GlGetUniformLocation(mProgram, String("uCamera"), &maCameraHandle);
    //获取程序中光源位置引用id
    gl->GlGetUniformLocation(mProgram, String("uLightLocationSun"), &maSunLightLocationHandle);
    //获取位置、旋转变换矩阵引用id
    gl->GlGetUniformLocation(mProgram, String("uMMatrix"), &muMMatrixHandle);
    return NOERROR;
}

ECode Moon::DrawSelf(
    /* [in] */ Int32 texId)
{
    AutoPtr<IGLES20> gl;
    CGLES20::AcquireSingleton((IGLES20**)&gl);
    //制定使用某套shader程序
    gl->GlUseProgram(mProgram);
    //将最终变换矩阵传入shader程序
    gl->GlUniformMatrix4fv(muMVPMatrixHandle, 1, FALSE, MatrixState::GetFinalMatrix(), 0);
    //将位置、旋转变换矩阵传入shader程序
    gl->GlUniformMatrix4fv(muMMatrixHandle, 1, FALSE, MatrixState::GetMMatrix(), 0);
    //将摄像机位置传入shader程序
    gl->GlUniform3fv(maCameraHandle, 1, MatrixState::sCameraFB);
    //将光源位置传入shader程序
    gl->GlUniform3fv(maSunLightLocationHandle, 1, MatrixState::sLightPositionFBSun);

    //为画笔指定顶点位置数据
    gl->GlVertexAttribPointerEx
    (
        maPositionHandle,
        3,
        IGLES20::_GL_FLOAT,
        FALSE,
        3*4,
        mVertexBuffer
    );
    //为画笔指定顶点经纬度数据
    gl->GlVertexAttribPointerEx
    (
        maTexCoorHandle,
        2,
        IGLES20::_GL_FLOAT,
        FALSE,
        2*4,
        mTexCoorBuffer
    );
    //为画笔指定顶点法向量数据
    gl->GlVertexAttribPointerEx
    (
        maNormalHandle,
        4,
        IGLES20::_GL_FLOAT,
        FALSE,
        3*4,
        mVertexBuffer
    );
    //允许顶点位置数据数组
    gl->GlEnableVertexAttribArray(maPositionHandle);
    gl->GlEnableVertexAttribArray(maTexCoorHandle);
    gl->GlEnableVertexAttribArray(maNormalHandle);
    //绑定纹理
    gl->GlActiveTexture(IGLES20::_GL_TEXTURE0);
    gl->GlBindTexture(IGLES20::_GL_TEXTURE_2D, texId);
    //绘制三角形
    gl->GlDrawArrays(IGLES20::_GL_TRIANGLES, 0, mVCount);
    return NOERROR;
}

AutoPtr<ArrayOf<Float> > Moon::GenerateTexCoor(
    /* [in] */ Int32 bw,
    /* [in] */ Int32 bh)
{
    AutoPtr<ArrayOf<Float> > result = ArrayOf<Float>::Alloc(bw * bh * 6 * 2);
    Float sizew = 1.0f / bw;//列数
    Float sizeh = 1.0f / bh;//行数
    Int32 c=0;
    for(Int32 i= 0; i < bh; i++)
    {
        for(Int32 j = 0; j < bw; j++)
        {
            //每行列一个矩形，由两个三角形构成，共六个点，12个纹理坐标
            Float s = j * sizew;
            Float t = i * sizeh;

            (*result)[c++] = s;
            (*result)[c++] = t;

            (*result)[c++] = s;
            (*result)[c++] = t+sizeh;

            (*result)[c++] = s+sizew;
            (*result)[c++] = t;

            (*result)[c++] = s+sizew;
            (*result)[c++] = t;

            (*result)[c++] = s;
            (*result)[c++] = t+sizeh;

            (*result)[c++] = s+sizew;
            (*result)[c++] = t+sizeh;
        }
    }
    return result;
}

} // namespace OpenglEarthDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos