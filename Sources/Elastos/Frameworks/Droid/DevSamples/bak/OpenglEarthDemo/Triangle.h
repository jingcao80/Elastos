#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#include "Elastos.Droid.Core.h"
#include "Elastos.Core.h"

using Elastos::IO::IFloatBuffer;
using Elastos::Droid::Opengl::IGLSurfaceView;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace OpenglEarthDemo {

class Triangle : public ElRefBase
{
public:
    Triangle(
        /* [in] */ IGLSurfaceView* v);

    CARAPI InitVertexData();

    CARAPI InitShader(
        /* [in] */ IGLSurfaceView* v);

    CARAPI DrawSelf();

    static CARAPI_(AutoPtr<ArrayOf<Float> >) GetFinalMatrix(
        /* [in] */ ArrayOf<Float>* spec);

    static AutoPtr<ArrayOf<Float> > mProjMatrix;//4x4���� ͶӰ��
    static AutoPtr<ArrayOf<Float> > mVMatrix;//�����λ�ó���9�������
    static AutoPtr<ArrayOf<Float> > mMVPMatrix;//��������õ��ܱ任����

    Int32 mProgram;//�Զ�����Ⱦ���߳���id
    Int32 muMVPMatrixHandle;//�ܱ任��������id
    Int32 maPositionHandle; //����λ����������id
    Int32 maColorHandle; //������ɫ��������id
    String mVertexShader;//������ɫ��
    String mFragmentShader;//ƬԪ��ɫ��
    static AutoPtr<ArrayOf<Float> > mMMatrix;//����������ƶ���ת������ת��ƽ��

    AutoPtr<IFloatBuffer>   mVertexBuffer;//���������ݻ���
    AutoPtr<IFloatBuffer>   mColorBuffer;//������ɫ��ݻ���
    Int32 vCount;
    Float xAngle;//��x����ת�ĽǶ�
};

} // namespace OpenglEarthDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif