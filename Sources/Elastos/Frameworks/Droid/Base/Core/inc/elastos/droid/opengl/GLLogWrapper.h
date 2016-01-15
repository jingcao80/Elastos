#ifndef __ELASTOS_DROID_OPENGL_GLLOGWRAPPER_H__
#define __ELASTOS_DROID_OPENGL_GLLOGWRAPPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/StringBuilder.h>
#include "GLWrapperBase.h"

using Elastos::IO::IWriter;
using Elastos::IO::IBuffer;
using Elastos::IO::IByteBuffer;
using Elastos::IO::IFloatBuffer;
using Elastos::IO::IInt16Buffer;
using Elastos::IO::IInt32Buffer;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Opengl {

class GLLogWrapper
    : public GLWrapperBase
{
private:
    class PointerInfo
        : public ElRefBase
    {
    public:
        PointerInfo(
            /* [in] */ GLLogWrapper* host);

        PointerInfo(
            /* [in] */ Int32 size,
            /* [in] */ Int32 type,
            /* [in] */ Int32 stride,
            /* [in] */ IBuffer* pointer,
            /* [in] */ GLLogWrapper* host);

        CARAPI_(Int32) Sizeof(
            /* [in] */ Int32 type);

        CARAPI_(Int32) GetStride();

        CARAPI BindByteBuffer();

        CARAPI UnbindByteBuffer();

        /**
         * The number of coordinates per vertex. 1..4
         */
        Int32 mSize;
        /**
         * The type of each coordinate.
         */
        Int32 mType;
        /**
         * The byte offset between consecutive vertices. 0 means mSize *
         * sizeof(mType)
         */
        Int32 mStride;
        AutoPtr<IBuffer> mPointer;
        AutoPtr<IByteBuffer> mTempByteBuffer; // Only valid during glDrawXXX calls

        GLLogWrapper* mHost;
    };

public:
    GLLogWrapper(
        /* [in] */ IGL* gl,
        /* [in] */ IWriter* log,
        /* [in] */ Boolean logArgumentNames);

    CARAPI glActiveTexture(
        /* [in] */ Int32 texture);

    CARAPI glAlphaFunc(
        /* [in] */ Int32 func,
        /* [in] */ Float ref);

    CARAPI glAlphaFuncx(
        /* [in] */ Int32 func,
        /* [in] */ Int32 ref);

    CARAPI glBlendFunc(
        /* [in] */ Int32 sfactor,
        /* [in] */ Int32 dfactor);

    CARAPI glClear(
        /* [in] */ Int32 mask);

    CARAPI glClearColor(
        /* [in] */ Float red,
        /* [in] */ Float green,
        /* [in] */ Float blue,
        /* [in] */ Float alpha);

    CARAPI glClearColorx(
        /* [in] */ Int32 red,
        /* [in] */ Int32 green,
        /* [in] */ Int32 blue,
        /* [in] */ Int32 alpha);

    CARAPI glClearDepthf(
        /* [in] */ Float depth);

    CARAPI glClearDepthx(
        /* [in] */ Int32 depth);

    CARAPI glClearStencil(
        /* [in] */ Int32 s);

    CARAPI glClientActiveTexture(
        /* [in] */ Int32 texture);

    CARAPI glColor4f(
        /* [in] */ Float red,
        /* [in] */ Float green,
        /* [in] */ Float blue,
        /* [in] */ Float alpha);

    CARAPI glColor4x(
        /* [in] */ Int32 red,
        /* [in] */ Int32 green,
        /* [in] */ Int32 blue,
        /* [in] */ Int32 alpha);

    CARAPI glColorMask(
        /* [in] */ Boolean red,
        /* [in] */ Boolean green,
        /* [in] */ Boolean blue,
        /* [in] */ Boolean alpha);

    CARAPI glColorPointer(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ IBuffer* pointer);

    CARAPI glCompressedTexSubImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 xoffset,
        /* [in] */ Int32 yoffset,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 format,
        /* [in] */ Int32 imageSize,
        /* [in] */ IBuffer* data);

    CARAPI glCopyTexSubImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 xoffset,
        /* [in] */ Int32 yoffset,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI glCullFace(
        /* [in] */ Int32 mode);

    CARAPI glDeleteTextures(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* textures,
        /* [in] */ Int32 offset);

    CARAPI glDeleteTextures(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* textures);

    CARAPI glDepthFunc(
        /* [in] */ Int32 func);

    CARAPI glDepthMask(
        /* [in] */ Boolean flag);

    CARAPI glDepthRangef(
        /* [in] */ Float zNear,
        /* [in] */ Float zFar);

    CARAPI glDepthRangex(
        /* [in] */ Int32 zNear,
        /* [in] */ Int32 zFar);

    CARAPI glDisable(
        /* [in] */ Int32 cap);

    CARAPI glDisableClientState(
        /* [in] */ Int32 array);

    CARAPI glDrawArrays(
        /* [in] */ Int32 mode,
        /* [in] */ Int32 first,
        /* [in] */ Int32 count);

    CARAPI glDrawElements(
        /* [in] */ Int32 mode,
        /* [in] */ Int32 count,
        /* [in] */ Int32 type,
        /* [in] */ IBuffer* indices);

    CARAPI glFinish();

    CARAPI glFlush();

    CARAPI glFogf(
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    CARAPI glFogfv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI glFogfv(
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params);

    CARAPI glFogx(
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI glFogxv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glFogxv(
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI glFrontFace(
        /* [in] */ Int32 mode);

    CARAPI glFrustumf(
        /* [in] */ Float left,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Float top,
        /* [in] */ Float zNear,
        /* [in] */ Float zFar);

    CARAPI glFrustumx(
        /* [in] */ Int32 left,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom,
        /* [in] */ Int32 top,
        /* [in] */ Int32 zNear,
        /* [in] */ Int32 zFar);

    CARAPI glGenTextures(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* textures,
        /* [in] */ Int32 offset);

    CARAPI glGenTextures(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* textures);

    CARAPI glGetError(
        /* [out] */ Int32* error);

    CARAPI glGetString(
        /* [in] */ Int32 name,
        /* [out] */ String* str);

    CARAPI glHint(
        /* [in] */ Int32 target,
        /* [in] */ Int32 mode);

    CARAPI glLightModelf(
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    CARAPI glLightModelfv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI glLightModelfv(
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params);

    CARAPI glLightModelx(
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI glLightModelxv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glLightModelxv(
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI glLightf(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    CARAPI glLightfv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI glLightfv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params);

    CARAPI glLightx(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI glLightxv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glLightxv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI glLineWidth(
        /* [in] */ Float width);

    CARAPI glLineWidthx(
        /* [in] */ Int32 width);

    CARAPI glLoadIdentity();

    CARAPI glLoadMatrixf(
        /* [in] */ ArrayOf<Float>* m,
        /* [in] */ Int32 offset);

    CARAPI glLoadMatrixf(
        /* [in] */ IFloatBuffer* m);

    CARAPI glLoadMatrixx(
        /* [in] */ ArrayOf<Int32>* m,
        /* [in] */ Int32 offset);

    CARAPI glLoadMatrixx(
        /* [in] */ IInt32Buffer* m);

    CARAPI glLogicOp(
        /* [in] */ Int32 opcode);

    CARAPI glMaterialf(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    CARAPI glMaterialfv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI glMaterialfv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params);

    CARAPI glMaterialx(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI glMaterialxv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glMaterialxv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI glMatrixMode(
        /* [in] */ Int32 mode);

    CARAPI glMultMatrixf(
        /* [in] */ ArrayOf<Float>* m,
        /* [in] */ Int32 offset);

    CARAPI glMultMatrixf(
        /* [in] */ IFloatBuffer* m);

    CARAPI glMultMatrixx(
        /* [in] */ ArrayOf<Int32>* m,
        /* [in] */ Int32 offset);

    CARAPI glMultMatrixx(
        /* [in] */ IInt32Buffer* m);

    CARAPI glMultiTexCoord4f(
        /* [in] */ Int32 target,
        /* [in] */ Float s,
        /* [in] */ Float t,
        /* [in] */ Float r,
        /* [in] */ Float q);

    CARAPI glMultiTexCoord4x(
        /* [in] */ Int32 target,
        /* [in] */ Int32 s,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 q);

    CARAPI glNormal3f(
        /* [in] */ Float nx,
        /* [in] */ Float ny,
        /* [in] */ Float nz);

    CARAPI glNormal3x(
        /* [in] */ Int32 nx,
        /* [in] */ Int32 ny,
        /* [in] */ Int32 nz);

    CARAPI glNormalPointer(
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ IBuffer* pointer);

    CARAPI glOrthof(
        /* [in] */ Float left,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Float top,
        /* [in] */ Float zNear,
        /* [in] */ Float zFar);

    CARAPI glOrthox(
        /* [in] */ Int32 left,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom,
        /* [in] */ Int32 top,
        /* [in] */ Int32 zNear,
        /* [in] */ Int32 zFar);

    CARAPI glPixelStorei(
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI glPointSize(
        /* [in] */ Float size);

    CARAPI glPointSizex(
        /* [in] */ Int32 size);

    CARAPI glPolygonOffset(
        /* [in] */ Float factor,
        /* [in] */ Float units);

    CARAPI glPolygonOffsetx(
        /* [in] */ Int32 factor,
        /* [in] */ Int32 units);

    CARAPI glPopMatrix();

    CARAPI glPushMatrix();

    CARAPI glReadPixels(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 format,
        /* [in] */ Int32 type,
        /* [in] */ IBuffer* pixels);

    CARAPI glRotatef(
        /* [in] */ Float angle,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z);

    CARAPI glRotatex(
        /* [in] */ Int32 angle,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z);

    CARAPI glSampleCoverage(
        /* [in] */ Float value,
        /* [in] */ Boolean invert);

    CARAPI glSampleCoveragex(
        /* [in] */ Int32 value,
        /* [in] */ Boolean invert);

    CARAPI glScalef(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z);

    CARAPI glScalex(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z);

    CARAPI glScissor(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI glShadeModel(
        /* [in] */ Int32 mode);

    CARAPI glStencilFunc(
        /* [in] */ Int32 func,
        /* [in] */ Int32 ref,
        /* [in] */ Int32 mask);

    CARAPI glStencilMask(
        /* [in] */ Int32 mask);

    CARAPI glTexCoordPointer(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ IBuffer* pointer);

    CARAPI glTexImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 border,
        /* [in] */ Int32 format,
        /* [in] */ Int32 type,
        /* [in] */ IBuffer* pixels);

    CARAPI glTexParameterx(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI glTexSubImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 xoffset,
        /* [in] */ Int32 yoffset,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 format,
        /* [in] */ Int32 type,
        /* [in] */ IBuffer* pixels);

    CARAPI glTranslatef(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z);

    CARAPI glTranslatex(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z);

    CARAPI glVertexPointer(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ IBuffer* pointer);

    CARAPI glViewport(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI glEnable(
        /* [in] */ Int32 cap);

    CARAPI glEnableClientState(
        /* [in] */ Int32 array);


    CARAPI glTexParameterf(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    CARAPI glTexParameterfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI glTexParameterfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params);

    CARAPI glBindTexture(
        /* [in] */ Int32 target,
        /* [in] */ Int32 texture);

    CARAPI glCompressedTexImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 border,
        /* [in] */ Int32 imageSize,
        /* [in] */ IBuffer* data);

    CARAPI glCopyTexImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 border);

    CARAPI glGetIntegerv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetIntegerv(
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI glStencilOp(
        /* [in] */ Int32 fail,
        /* [in] */ Int32 zfail,
        /* [in] */ Int32 zpass);

    CARAPI glTexEnvf(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    CARAPI glTexEnvfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI glTexEnvfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params);

    CARAPI glTexEnvx(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI glTexEnvxv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glTexEnvxv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI glQueryMatrixxOES(
        /* [in] */ ArrayOf<Int32>* mantissa,
        /* [in] */ Int32 mantissaOffset,
        /* [in] */ ArrayOf<Int32>* exponent,
        /* [in] */ Int32 exponentOffset,
        /* [out] */ Int32* matrixxOES);

    CARAPI glQueryMatrixxOES(
        /* [in] */ IInt32Buffer* mantissa,
        /* [in] */ IInt32Buffer* exponent,
        /* [out] */ Int32* matrixxOES);

    CARAPI glGetPointerv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<IBuffer*>* params);

    CARAPI glBindBuffer(
        /* [in] */ Int32 target,
        /* [in] */ Int32 buffer);

    CARAPI glBufferData(
        /* [in] */ Int32 target,
        /* [in] */ Int32 size,
        /* [in] */ IBuffer* data,
        /* [in] */ Int32 usage);

    CARAPI glBufferSubData(
        /* [in] */ Int32 target,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 size,
        /* [in] */ IBuffer* data);

    CARAPI glClipPlanef(
        /* [in] */ Int32 plane,
        /* [in] */ ArrayOf<Float>* equation,
        /* [in] */ Int32 offset);

    CARAPI glClipPlanef(
        /* [in] */ Int32 plane,
        /* [in] */ IFloatBuffer* equation);

    CARAPI glClipPlanex(
        /* [in] */ Int32 plane,
        /* [in] */ ArrayOf<Int32>* equation,
        /* [in] */ Int32 offset);

    CARAPI glClipPlanex(
        /* [in] */ Int32 plane,
        /* [in] */ IInt32Buffer* equation);

    CARAPI glColor4ub(
        /* [in] */ Byte red,
        /* [in] */ Byte green,
        /* [in] */ Byte blue,
        /* [in] */ Byte alpha);

    CARAPI glColorPointer(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Int32 offset);

    CARAPI glDeleteBuffers(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* buffers,
        /* [in] */ Int32 offset);

    CARAPI glDeleteBuffers(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* buffers
    );

    CARAPI glDrawElements(
        /* [in] */ Int32 mode,
        /* [in] */ Int32 count,
        /* [in] */ Int32 type,
        /* [in] */ Int32 offset);

    CARAPI glGenBuffers(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* buffers,
        /* [in] */ Int32 offset);

    CARAPI glGenBuffers(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* buffers);

    CARAPI glGetBooleanv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Boolean>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetBooleanv(
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI glGetBufferParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetBufferParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI glGetClipPlanef(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* eqn,
        /* [in] */ Int32 offset);

    CARAPI glGetClipPlanef(
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* eqn);

    CARAPI glGetClipPlanex(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* eqn,
        /* [in] */ Int32 offset);

    CARAPI glGetClipPlanex(
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* eqn);

    CARAPI glGetFixedv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetFixedv(
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI glGetFloatv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetFloatv(
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params);

    CARAPI glGetLightfv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetLightfv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params);

    CARAPI glGetLightxv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetLightxv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI glGetMaterialfv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetMaterialfv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params);

    CARAPI glGetMaterialxv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetMaterialxv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI glGetTexEnviv(
        /* [in] */ Int32 env,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetTexEnviv(
        /* [in] */ Int32 env,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI glGetTexEnvxv(
        /* [in] */ Int32 env,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetTexEnvxv(
        /* [in] */ Int32 env,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI glGetTexParameterfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetTexParameterfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params);

    CARAPI glGetTexParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetTexParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI glGetTexParameterxv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetTexParameterxv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI glIsBuffer(
        /* [in] */ Int32 buffer,
        /* [out] */ Boolean* result);

    CARAPI glIsEnabled(
        /* [in] */ Int32 cap,
        /* [out] */ Boolean* result);

    CARAPI glIsTexture(
        /* [in] */ Int32 texture,
        /* [out] */ Boolean* result);

    CARAPI glNormalPointer(
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Int32 offset);

    CARAPI glPointParameterf(
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    CARAPI glPointParameterfv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI glPointParameterfv(
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params);

    CARAPI glPointParameterx(
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI glPointParameterxv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glPointParameterxv(
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI glPointSizePointerOES(
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ IBuffer* pointer);

    CARAPI glTexCoordPointer(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Int32 offset);

    CARAPI glTexEnvi(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI glTexEnviv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glTexEnviv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI glTexParameteri(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI glTexParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glTexParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI glTexParameterxv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glTexParameterxv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI glVertexPointer(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Int32 offset);

    CARAPI glCurrentPaletteMatrixOES(
        /* [in] */ Int32 matrixpaletteindex);

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
        /* [in] */ IFloatBuffer* coords);

    CARAPI glDrawTexiOES(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI glDrawTexivOES(
        /* [in] */ ArrayOf<Int32>* coords,
        /* [in] */ Int32 offset);

    CARAPI glDrawTexivOES(
        /* [in] */ IInt32Buffer* coords);

    CARAPI glDrawTexsOES(
        /* [in] */ Int16 x,
        /* [in] */ Int16 y,
        /* [in] */ Int16 z,
        /* [in] */ Int16 width,
        /* [in] */ Int16 height);

    CARAPI glDrawTexsvOES(
        /* [in] */ ArrayOf<Int16>* coords,
        /* [in] */ Int32 offset);

    CARAPI glDrawTexsvOES(
        /* [in] */ IInt16Buffer* coords);

    CARAPI glDrawTexxOES(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI glDrawTexxvOES(
        /* [in] */ ArrayOf<Int32>* coords,
        /* [in] */ Int32 offset);

    CARAPI glDrawTexxvOES(
        /* [in] */ IInt32Buffer* coords);

    CARAPI glLoadPaletteFromModelViewMatrixOES();

    CARAPI glMatrixIndexPointerOES(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ IBuffer* pointer);

    CARAPI glMatrixIndexPointerOES(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Int32 offset);

    CARAPI glWeightPointerOES(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ IBuffer* pointer);

    CARAPI glWeightPointerOES(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Int32 offset);

    CARAPI glBindFramebufferOES(
        /* [in] */ Int32 target,
        /* [in] */ Int32 framebuffer);

    CARAPI glBindRenderbufferOES(
        /* [in] */ Int32 target,
        /* [in] */ Int32 renderbuffer);

    CARAPI glBlendEquation(
        /* [in] */ Int32 mode);

    CARAPI glBlendEquationSeparate(
        /* [in] */ Int32 modeRGB,
        /* [in] */ Int32 modeAlpha);

    CARAPI glBlendFuncSeparate(
        /* [in] */ Int32 srcRGB,
        /* [in] */ Int32 dstRGB,
        /* [in] */ Int32 srcAlpha,
        /* [in] */ Int32 dstAlpha);

    CARAPI glCheckFramebufferStatusOES(
        /* [in] */ Int32 target,
        /* [out] */ Int32* status);

    CARAPI glDeleteFramebuffersOES(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* framebuffers,
        /* [in] */ Int32 offset);

    CARAPI glDeleteFramebuffersOES(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* framebuffers);

    CARAPI glDeleteRenderbuffersOES(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* renderbuffers,
        /* [in] */ Int32 offset);

    CARAPI glDeleteRenderbuffersOES(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* renderbuffers);

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

    CARAPI glGenerateMipmapOES(
        /* [in] */ Int32 target);

    CARAPI glGenFramebuffersOES(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* framebuffers,
        /* [in] */ Int32 offset);

    CARAPI glGenFramebuffersOES(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* framebuffers);

    CARAPI glGenRenderbuffersOES(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* renderbuffers,
        /* [in] */ Int32 offset);

    CARAPI glGenRenderbuffersOES(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* renderbuffers);

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
        /* [in] */ IInt32Buffer* params);

    CARAPI glGetRenderbufferParameterivOES(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetRenderbufferParameterivOES(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI glGetTexGenfv(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetTexGenfv(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params);

    CARAPI glGetTexGeniv(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetTexGeniv(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI glGetTexGenxv(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glGetTexGenxv(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI glIsFramebufferOES(
        /* [in] */ Int32 framebuffer,
        /* [out] */ Boolean* result);

    CARAPI glIsRenderbufferOES(
        /* [in] */ Int32 renderbuffer,
        /* [out] */ Boolean* result);

    CARAPI glRenderbufferStorageOES(
        /* [in] */ Int32 target,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI glTexGenf(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    CARAPI glTexGenfv(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI glTexGenfv(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params);

    CARAPI glTexGeni(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI glTexGeniv(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glTexGeniv(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI glTexGenx(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI glTexGenxv(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI glTexGenxv(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

private:
    CARAPI CheckError();

    CARAPI LogLine(
        /* [in] */ const String& message);

    CARAPI Log(
        /* [in] */ const String& message);

    CARAPI Begin(
        /* [in] */ const String& message);

    CARAPI Arg(
        /* [in] */ const String& name,
        /* [in] */ const String& value);

    CARAPI End();

    CARAPI Flush();

    CARAPI Arg(
        /* [in] */ const String& name,
        /* [in] */ Boolean value);

    CARAPI Arg(
        /* [in] */ const String& name,
        /* [in] */ Int32 value);

    CARAPI Arg(
        /* [in] */ const String& name,
        /* [in] */ Float value);

    CARAPI Returns(
        /* [in] */ const String& result);

    CARAPI Returns(
        /* [in] */ Int32 result);

    CARAPI Arg(
        /* [in] */ const String& name,
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* obj,
        /* [in] */ Int32 offset);

    CARAPI Arg(
        /* [in] */ const String& name,
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int16>* obj,
        /* [in] */ Int32 offset);

    CARAPI Arg(
        /* [in] */ const String& name,
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Float>* obj,
        /* [in] */ Int32 offset);

    CARAPI FormattedAppend(
        /* [in] */ StringBuilder& buf,
        /* [in] */ Int32 value,
        /* [in] */ Int32 format);

    CARAPI_(String) ToString(
        /* [in] */ Int32 n,
        /* [in] */ Int32 format,
        /* [in] */ ArrayOf<Int32>* arr,
        /* [in] */ Int32 offset);

    CARAPI_(String) ToString(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int16>* obj,
        /* [in] */ Int32 offset);

    CARAPI_(String) ToString(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Float>* obj,
        /* [in] */ Int32 offset);

    CARAPI_(String) ToString(
        /* [in] */ Int32 n,
        /* [in] */ IFloatBuffer* buf);

    CARAPI_(String) ToString(
        /* [in] */ Int32 n,
        /* [in] */ Int32 format,
        /* [in] */ IInt32Buffer* buf);

    CARAPI_(String) ToString(
        /* [in] */ Int32 n,
        /* [in] */ IInt16Buffer* buf);

    CARAPI Arg(
        /* [in] */ const String& name,
        /* [in] */ Int32 n,
        /* [in] */ IFloatBuffer* buf);

    CARAPI Arg(
        /* [in] */ const String& name,
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* buf);

    CARAPI Arg(
        /* [in] */ const String& name,
        /* [in] */ Int32 n,
        /* [in] */ IInt16Buffer* buf);

    CARAPI ArgPointer(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ IBuffer* pointer);

    static CARAPI_(String) GetHex(
        /* [in] */ Int32 value);

    static CARAPI_(String) GetErrorString(
        /* [in] */ Int32 error);

    CARAPI_(String) GetClearBufferMask(
        /* [in] */ Int32 mask);

    CARAPI_(String) GetFactor(
        /* [in] */ Int32 factor);

    CARAPI_(String) GetShadeModel(
        /* [in] */ Int32 model);

    CARAPI_(String) GetTextureTarget(
        /* [in] */ Int32 target);

    CARAPI_(String) GetTextureEnvTarget(
        /* [in] */ Int32 target);

    CARAPI_(String) GetTextureEnvPName(
        /* [in] */ Int32 pname);

    CARAPI_(Int32) GetTextureEnvParamCount(
        /* [in] */ Int32 pname);

    CARAPI_(String) GetTextureEnvParamName(
        /* [in] */ Float param);

    CARAPI_(String) GetMatrixMode(
        /* [in] */ Int32 matrixMode);

    CARAPI_(String) GetClientState(
        /* [in] */ Int32 clientState);

    CARAPI_(String) GetCap(
        /* [in] */ Int32 cap);

    CARAPI_(String) GetTexturePName(
        /* [in] */ Int32 pname);

    CARAPI_(String) GetTextureParamName(
        /* [in] */ Float param);

    CARAPI_(String) GetFogPName(
        /* [in] */ Int32 pname);

    CARAPI_(Int32) GetFogParamCount(
        /* [in] */ Int32 pname);

    CARAPI_(String) GetBeginMode(
        /* [in] */ Int32 mode);

    CARAPI_(String) GetIndexType(
        /* [in] */ Int32 type);

    CARAPI_(String) GetIntegerStateName(
        /* [in] */ Int32 pname);

    CARAPI_(Int32) GetIntegerStateSize(
        /* [in] */ Int32 pname);

    CARAPI_(Int32) GetIntegerStateFormat(
        /* [in] */ Int32 pname);

    CARAPI_(String) GetHintTarget(
        /* [in] */ Int32 target);

    CARAPI_(String) GetHintMode(
        /* [in] */ Int32 mode);

    CARAPI_(String) GetFaceName(
        /* [in] */ Int32 face);

    CARAPI_(String) GetMaterialPName(
        /* [in] */ Int32 pname);

    CARAPI_(Int32) GetMaterialParamCount(
        /* [in] */ Int32 pname);

    CARAPI_(String) GetLightName(
        /* [in] */ Int32 light);

    CARAPI_(String) GetLightPName(
        /* [in] */ Int32 pname);

    CARAPI_(Int32) GetLightParamCount(
        /* [in] */ Int32 pname);

    CARAPI_(String) GetLightModelPName(
        /* [in] */ Int32 pname);

    CARAPI_(Int32) GetLightModelParamCount(
        /* [in] */ Int32 pname);

    CARAPI_(String) GetPointerTypeName(
        /* [in] */ Int32 type);

    CARAPI ToByteBuffer(
        /* [in] */ Int32 byteCount,
        /* [in] */ IBuffer* input,
        /* [in] */ IByteBuffer** rBuf);

    CARAPI_(AutoPtr<ArrayOf<Char32> >) ToCharIndices(
        /* [in] */ Int32 count,
        /* [in] */ Int32 type,
        /* [in] */ IBuffer* indices);

    CARAPI DoArrayElement(
        /* [in] */ StringBuilder& builder,
        /* [in] */ Boolean enabled,
        /* [in] */ const String& name,
        /* [in] */ PointerInfo* pointer,
        /* [in] */ Int32 index);

    CARAPI DoElement(
        /* [in] */ StringBuilder& builder,
        /* [in] */ Int32 ordinal,
        /* [in] */ Int32 vertexIndex);

    template<typename T>
        CARAPI_(String) TranArrayToString(
            /* [in] */ ArrayOf<T>* array);

    CARAPI BindArrays();

    CARAPI UnbindArrays();

    CARAPI StartLogIndices();

    CARAPI EndLogIndices();

    template<typename T>
        CARAPI_(String) ParamsToString(
            /* [in] */ ArrayOf<T>* array);

    CARAPI_(String) ParamsToString(
        /* [in] */ IBuffer* buf);

private:
    static const Int32 FORMAT_INT = 0;
    static const Int32 FORMAT_FLOAT = 1;
    static const Int32 FORMAT_FIXED = 2;

    AutoPtr<IWriter> mLog;
    Boolean mLogArgumentNames;
    Int32 mArgCount;

    AutoPtr<PointerInfo> mColorPointer;
    AutoPtr<PointerInfo> mNormalPointer;
    AutoPtr<PointerInfo> mTexCoordPointer;
    AutoPtr<PointerInfo> mVertexPointer;

    Boolean mColorArrayEnabled;
    Boolean mNormalArrayEnabled;
    Boolean mTextureCoordArrayEnabled;
    Boolean mVertexArrayEnabled;

    StringBuilder mStringBuilder;
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos
#endif
