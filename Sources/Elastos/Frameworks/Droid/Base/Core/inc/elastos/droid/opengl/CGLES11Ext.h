
#ifndef __ELASTOS_DROID_OPENGL_CGLES11EXT_H__
#define __ELASTOS_DROID_OPENGL_CGLES11EXT_H__

#include "_Elastos_Droid_Opengl_CGLES11Ext.h"

using Elastos::IO::IBuffer;

namespace Elastos {
namespace Droid {
namespace Opengl {

CarClass(CGLES11Ext)
{
public:
    CARAPI glBlendEquationSeparateOES(
        /* [in] */ Int32 modeRGB,
        /* [in] */ Int32 modeAlpha);

    CARAPI glBlendFuncSeparateOES(
        /* [in] */ Int32 srcRGB,
        /* [in] */ Int32 dstRGB,
        /* [in] */ Int32 srcAlpha,
        /* [in] */ Int32 dstAlpha);

    CARAPI glBlendEquationOES(
        /* [in] */ Int32 mode);

    CARAPI glDrawTexsOES(
        /* [in] */ Int16 x,
        /* [in] */ Int16 y,
        /* [in] */ Int16 z,
        /* [in] */ Int16 width,
        /* [in] */ Int16 height);

    CARAPI glDrawTexiOES(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI glDrawTexxOES(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI glDrawTexsvOES(
        /* [in] */ ArrayOf<Int16>* coords,
        /* [in] */ Int32 offset);

    CARAPI glDrawTexsvOES(
        /* [in] */ Elastos::IO::IInt16Buffer* coords);

    CARAPI glDrawTexivOES(
        /* [in] */ ArrayOf<Int32>* coords,
        /* [in] */ Int32 offset);

    CARAPI glDrawTexivOES(
        /* [in] */ Elastos::IO::IInt32Buffer* coords);

    CARAPI glDrawTexxvOES(
        /* [in] */ ArrayOf<Int32>* coords,
        /* [in] */ Int32 offset);

    CARAPI glDrawTexxvOES(
        /* [in] */ Elastos::IO::IInt32Buffer* coords);

    CARAPI glDrawTexfOES(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z,
        /* [in] */ Float width,
        /* [in] */ Float height);

    CARAPI glDrawTexfvOES(
        /* [in] */ ArrayOf<Float>* coords,
        /* [in] */ Int32 offset);

    CARAPI glDrawTexfvOES(
        /* [in] */ Elastos::IO::IFloatBuffer* coords);

    CARAPI glEGLImageTargetTexture2DOES(
        /* [in] */ Int32 target,
        /* [in] */ Elastos::IO::IBuffer* image);

    CARAPI glEGLImageTargetRenderbufferStorageOES(
        /* [in] */ Int32 target,
        /* [in] */ Elastos::IO::IBuffer* image);

    CARAPI glAlphaFuncxOES(
        /* [in] */ Int32 func,
        /* [in] */ Int32 ref);

    CARAPI glClearColorxOES(
        /* [in] */ Int32 red,
        /* [in] */ Int32 green,
        /* [in] */ Int32 blue,
        /* [in] */ Int32 alpha);

    CARAPI glClearDepthxOES(
        /* [in] */ Int32 depth);

    CARAPI glClipPlanexOES(
        /* [in] */ Int32 plane,
        /* [in] */ ArrayOf<Int32>* equation,
        /* [in] */ Int32 offset);

    CARAPI glClipPlanexOES(
        /* [in] */ Int32 plane,
        /* [in] */ Elastos::IO::IInt32Buffer* equation);

    CARAPI glColor4xOES(
        /* [in] */ Int32 red,
        /* [in] */ Int32 green,
        /* [in] */ Int32 blue,
        /* [in] */ Int32 alpha);

    CARAPI glDepthRangexOES(
        /* [in] */ Int32 zNear,
        /* [in] */ Int32 zFar);

    CARAPI glFogxOES(
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI glFogxvOES(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glFogxvOES(
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI glFrustumxOES(
        /* [in] */ Int32 left,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom,
        /* [in] */ Int32 top,
        /* [in] */ Int32 zNear,
        /* [in] */ Int32 zFar);

    CARAPI glGetClipPlanexOES(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* eqn,
        /* [in] */ Int32 offset);

    CARAPI glGetClipPlanexOES(
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* eqn);

    CARAPI glGetFixedvOES(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetFixedvOES(
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI glGetLightxvOES(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetLightxvOES(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI glGetMaterialxvOES(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetMaterialxvOES(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI glGetTexEnvxvOES(
        /* [in] */ Int32 env,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetTexEnvxvOES(
        /* [in] */ Int32 env,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI glGetTexParameterxvOES(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetTexParameterxvOES(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI glLightModelxOES(
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI glLightModelxvOES(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glLightModelxvOES(
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI glLightxOES(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI glLightxvOES(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glLightxvOES(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI glLineWidthxOES(
        /* [in] */ Int32 width);

    CARAPI glLoadMatrixxOES(
        /* [in] */ ArrayOf<Int32>* m,
        /* [in] */ Int32 offset);

    CARAPI glLoadMatrixxOES(
        /* [in] */ Elastos::IO::IInt32Buffer* m);

    CARAPI glMaterialxOES(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI glMaterialxvOES(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glMaterialxvOES(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI glMultMatrixxOES(
        /* [in] */ ArrayOf<Int32>* m,
        /* [in] */ Int32 offset);

    CARAPI glMultMatrixxOES(
        /* [in] */ Elastos::IO::IInt32Buffer* m);

    CARAPI glMultiTexCoord4xOES(
        /* [in] */ Int32 target,
        /* [in] */ Int32 s,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 q);

    CARAPI glNormal3xOES(
        /* [in] */ Int32 nx,
        /* [in] */ Int32 ny,
        /* [in] */ Int32 nz);

    CARAPI glOrthoxOES(
        /* [in] */ Int32 left,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom,
        /* [in] */ Int32 top,
        /* [in] */ Int32 zNear,
        /* [in] */ Int32 zFar);

    CARAPI glPointParameterxOES(
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI glPointParameterxvOES(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glPointParameterxvOES(
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI glPointSizexOES(
        /* [in] */ Int32 size);

    CARAPI glPolygonOffsetxOES(
        /* [in] */ Int32 factor,
        /* [in] */ Int32 units);

    CARAPI glRotatexOES(
        /* [in] */ Int32 angle,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z);

    CARAPI glSampleCoveragexOES(
        /* [in] */ Int32 value,
        /* [in] */ Boolean invert);

    CARAPI glScalexOES(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z);

    CARAPI glTexEnvxOES(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI glTexEnvxvOES(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glTexEnvxvOES(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI glTexParameterxOES(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI glTexParameterxvOES(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glTexParameterxvOES(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI glTranslatexOES(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z);

    CARAPI glIsRenderbufferOES(
        /* [in] */ Int32 renderbuffer,
        /* [out] */ Boolean* result);

    CARAPI glBindRenderbufferOES(
        /* [in] */ Int32 target,
        /* [in] */ Int32 renderbuffer);

    CARAPI glDeleteRenderbuffersOES(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* renderbuffers,
        /* [in] */ Int32 offset);

    CARAPI glDeleteRenderbuffersOES(
        /* [in] */ Int32 n,
        /* [in] */ Elastos::IO::IInt32Buffer* renderbuffers);

    CARAPI glGenRenderbuffersOES(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* renderbuffers,
        /* [in] */ Int32 offset);

    CARAPI glGenRenderbuffersOES(
        /* [in] */ Int32 n,
        /* [in] */ Elastos::IO::IInt32Buffer* renderbuffers);

    CARAPI glRenderbufferStorageOES(
        /* [in] */ Int32 target,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI glGetRenderbufferParameterivOES(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetRenderbufferParameterivOES(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI glIsFramebufferOES(
        /* [in] */ Int32 framebuffer,
        /* [out] */ Boolean* result);

    CARAPI glBindFramebufferOES(
        /* [in] */ Int32 target,
        /* [in] */ Int32 framebuffer);

    CARAPI glDeleteFramebuffersOES(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* framebuffers,
        /* [in] */ Int32 offset);

    CARAPI glDeleteFramebuffersOES(
        /* [in] */ Int32 n,
        /* [in] */ Elastos::IO::IInt32Buffer* framebuffers);

    CARAPI glGenFramebuffersOES(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* framebuffers,
        /* [in] */ Int32 offset);

    CARAPI glGenFramebuffersOES(
        /* [in] */ Int32 n,
        /* [in] */ Elastos::IO::IInt32Buffer* framebuffers);

    CARAPI glCheckFramebufferStatusOES(
        /* [in] */ Int32 target,
        /* [out] */ Int32* status);

    CARAPI glFramebufferRenderbufferOES(
        /* [in] */ Int32 target,
        /* [in] */ Int32 attachment,
        /* [in] */ Int32 renderbuffertarget,
        /* [in] */ Int32 renderbuffer);

    CARAPI glFramebufferTexture2DOES(
        /* [in] */ Int32 target,
        /* [in] */ Int32 attachment,
        /* [in] */ Int32 textarget,
        /* [in] */ Int32 texture,
        /* [in] */ Int32 level);

    CARAPI glGetFramebufferAttachmentParameterivOES(
        /* [in] */ Int32 target,
        /* [in] */ Int32 attachment,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetFramebufferAttachmentParameterivOES(
        /* [in] */ Int32 target,
        /* [in] */ Int32 attachment,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI glGenerateMipmapOES(
        /* [in] */ Int32 target);

    CARAPI glCurrentPaletteMatrixOES(
        /* [in] */ Int32 matrixpaletteindex);

    CARAPI glLoadPaletteFromModelViewMatrixOES();

    CARAPI glMatrixIndexPointerOES(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Elastos::IO::IBuffer* pointer);

    CARAPI glWeightPointerOES(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Elastos::IO::IBuffer* pointer);

    CARAPI glDepthRangefOES(
        /* [in] */ Float zNear,
        /* [in] */ Float zFar);

    CARAPI glFrustumfOES(
        /* [in] */ Float left,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Float top,
        /* [in] */ Float zNear,
        /* [in] */ Float zFar);

    CARAPI glOrthofOES(
        /* [in] */ Float left,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Float top,
        /* [in] */ Float zNear,
        /* [in] */ Float zFar);

    CARAPI glClipPlanefOES(
        /* [in] */ Int32 plane,
        /* [in] */ ArrayOf<Float>* equation,
        /* [in] */ Int32 offset);

    CARAPI glClipPlanefOES(
        /* [in] */ Int32 plane,
        /* [in] */ Elastos::IO::IFloatBuffer* equation);

    CARAPI glGetClipPlanefOES(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* eqn,
        /* [in] */ Int32 offset);

    CARAPI glGetClipPlanefOES(
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IFloatBuffer* eqn);

    CARAPI glClearDepthfOES(
        /* [in] */ Float depth);

    CARAPI glTexGenfOES(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    CARAPI glTexGenfvOES(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI glTexGenfvOES(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IFloatBuffer* params);

    CARAPI glTexGeniOES(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI glTexGenivOES(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glTexGenivOES(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI glTexGenxOES(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI glTexGenxvOES(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glTexGenxvOES(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI glGetTexGenfvOES(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetTexGenfvOES(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IFloatBuffer* params);

    CARAPI glGetTexGenivOES(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetTexGenivOES(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

    CARAPI glGetTexGenxvOES(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetTexGenxvOES(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params);

private:
    CARAPI glMatrixIndexPointerOESBounds(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Elastos::IO::IBuffer* pointer,
        /* [in] */ Int32 remaining);

    CARAPI glWeightPointerOESBounds(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Elastos::IO::IBuffer* pointer,
        /* [in] */ Int32 remaining);

    CARAPI GetPointer(
        /* [in] */ IBuffer* buffer,
        /* [in, out] */ Handle32* array,
        /* [in, out] */ Int32* remaining,
        /* [in, out] */ Int32* offset,
        /* [out] */ Handle32* rst);

    CARAPI GetDirectBufferPointer(
        /* [in] */ IBuffer* buffer,
        /* [out] */ Handle32* result);

private:
    static const Int32 GL_BYTE = IGLES10::_GL_BYTE;
    static const Int32 GL_FIXED = IGLES10::_GL_FIXED;
    static const Int32 GL_FLOAT = IGLES10::_GL_FLOAT;
    static const Int32 GL_SHORT = IGLES10::_GL_SHORT;
    static AutoPtr<IBuffer> sMatrixIndexPointerOES;
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OPENGL_CGLES11EXT_H__
