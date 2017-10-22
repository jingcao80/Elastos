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

#ifndef __ELASTOS_DROID_OPENGL_GLLOGWRAPPER_H__
#define __ELASTOS_DROID_OPENGL_GLLOGWRAPPER_H__

#include "Elastos.Droid.Opengl.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/opengl/GLWrapperBase.h"

#include <elastos/core/StringBuilder.h>

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
        : public Object
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

    CARAPI GlActiveTexture(
        /* [in] */ Int32 texture);

    CARAPI GlAlphaFunc(
        /* [in] */ Int32 func,
        /* [in] */ Float ref);

    CARAPI GlAlphaFuncx(
        /* [in] */ Int32 func,
        /* [in] */ Int32 ref);

    CARAPI GlBlendFunc(
        /* [in] */ Int32 sfactor,
        /* [in] */ Int32 dfactor);

    CARAPI GlClear(
        /* [in] */ Int32 mask);

    CARAPI GlClearColor(
        /* [in] */ Float red,
        /* [in] */ Float green,
        /* [in] */ Float blue,
        /* [in] */ Float alpha);

    CARAPI GlClearColorx(
        /* [in] */ Int32 red,
        /* [in] */ Int32 green,
        /* [in] */ Int32 blue,
        /* [in] */ Int32 alpha);

    CARAPI GlClearDepthf(
        /* [in] */ Float depth);

    CARAPI GlClearDepthx(
        /* [in] */ Int32 depth);

    CARAPI GlClearStencil(
        /* [in] */ Int32 s);

    CARAPI GlClientActiveTexture(
        /* [in] */ Int32 texture);

    CARAPI GlColor4f(
        /* [in] */ Float red,
        /* [in] */ Float green,
        /* [in] */ Float blue,
        /* [in] */ Float alpha);

    CARAPI GlColor4x(
        /* [in] */ Int32 red,
        /* [in] */ Int32 green,
        /* [in] */ Int32 blue,
        /* [in] */ Int32 alpha);

    CARAPI GlColorMask(
        /* [in] */ Boolean red,
        /* [in] */ Boolean green,
        /* [in] */ Boolean blue,
        /* [in] */ Boolean alpha);

    CARAPI GlColorPointer(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ IBuffer* pointer);

    CARAPI GlCompressedTexSubImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 xoffset,
        /* [in] */ Int32 yoffset,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 format,
        /* [in] */ Int32 imageSize,
        /* [in] */ IBuffer* data);

    CARAPI GlCopyTexSubImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 xoffset,
        /* [in] */ Int32 yoffset,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI GlCullFace(
        /* [in] */ Int32 mode);

    CARAPI GlDeleteTextures(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* textures,
        /* [in] */ Int32 offset);

    CARAPI GlDeleteTextures(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* textures);

    CARAPI GlDepthFunc(
        /* [in] */ Int32 func);

    CARAPI GlDepthMask(
        /* [in] */ Boolean flag);

    CARAPI GlDepthRangef(
        /* [in] */ Float zNear,
        /* [in] */ Float zFar);

    CARAPI GlDepthRangex(
        /* [in] */ Int32 zNear,
        /* [in] */ Int32 zFar);

    CARAPI GlDisable(
        /* [in] */ Int32 cap);

    CARAPI GlDisableClientState(
        /* [in] */ Int32 array);

    CARAPI GlDrawArrays(
        /* [in] */ Int32 mode,
        /* [in] */ Int32 first,
        /* [in] */ Int32 count);

    CARAPI GlDrawElements(
        /* [in] */ Int32 mode,
        /* [in] */ Int32 count,
        /* [in] */ Int32 type,
        /* [in] */ IBuffer* indices);

    CARAPI GlFinish();

    CARAPI GlFlush();

    CARAPI GlFogf(
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    CARAPI GlFogfv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI GlFogfv(
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params);

    CARAPI GlFogx(
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI GlFogxv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlFogxv(
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI GlFrontFace(
        /* [in] */ Int32 mode);

    CARAPI GlFrustumf(
        /* [in] */ Float left,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Float top,
        /* [in] */ Float zNear,
        /* [in] */ Float zFar);

    CARAPI GlFrustumx(
        /* [in] */ Int32 left,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom,
        /* [in] */ Int32 top,
        /* [in] */ Int32 zNear,
        /* [in] */ Int32 zFar);

    CARAPI GlGenTextures(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* textures,
        /* [in] */ Int32 offset);

    CARAPI GlGenTextures(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* textures);

    CARAPI GlGetError(
        /* [out] */ Int32* error);

    CARAPI GlGetString(
        /* [in] */ Int32 name,
        /* [out] */ String* str);

    CARAPI GlHint(
        /* [in] */ Int32 target,
        /* [in] */ Int32 mode);

    CARAPI GlLightModelf(
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    CARAPI GlLightModelfv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI GlLightModelfv(
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params);

    CARAPI GlLightModelx(
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI GlLightModelxv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlLightModelxv(
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI GlLightf(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    CARAPI GlLightfv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI GlLightfv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params);

    CARAPI GlLightx(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI GlLightxv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlLightxv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI GlLineWidth(
        /* [in] */ Float width);

    CARAPI GlLineWidthx(
        /* [in] */ Int32 width);

    CARAPI GlLoadIdentity();

    CARAPI GlLoadMatrixf(
        /* [in] */ ArrayOf<Float>* m,
        /* [in] */ Int32 offset);

    CARAPI GlLoadMatrixf(
        /* [in] */ IFloatBuffer* m);

    CARAPI GlLoadMatrixx(
        /* [in] */ ArrayOf<Int32>* m,
        /* [in] */ Int32 offset);

    CARAPI GlLoadMatrixx(
        /* [in] */ IInt32Buffer* m);

    CARAPI GlLogicOp(
        /* [in] */ Int32 opcode);

    CARAPI GlMaterialf(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    CARAPI GlMaterialfv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI GlMaterialfv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params);

    CARAPI GlMaterialx(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI GlMaterialxv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlMaterialxv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI GlMatrixMode(
        /* [in] */ Int32 mode);

    CARAPI GlMultMatrixf(
        /* [in] */ ArrayOf<Float>* m,
        /* [in] */ Int32 offset);

    CARAPI GlMultMatrixf(
        /* [in] */ IFloatBuffer* m);

    CARAPI GlMultMatrixx(
        /* [in] */ ArrayOf<Int32>* m,
        /* [in] */ Int32 offset);

    CARAPI GlMultMatrixx(
        /* [in] */ IInt32Buffer* m);

    CARAPI GlMultiTexCoord4f(
        /* [in] */ Int32 target,
        /* [in] */ Float s,
        /* [in] */ Float t,
        /* [in] */ Float r,
        /* [in] */ Float q);

    CARAPI GlMultiTexCoord4x(
        /* [in] */ Int32 target,
        /* [in] */ Int32 s,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 q);

    CARAPI GlNormal3f(
        /* [in] */ Float nx,
        /* [in] */ Float ny,
        /* [in] */ Float nz);

    CARAPI GlNormal3x(
        /* [in] */ Int32 nx,
        /* [in] */ Int32 ny,
        /* [in] */ Int32 nz);

    CARAPI GlNormalPointer(
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ IBuffer* pointer);

    CARAPI GlOrthof(
        /* [in] */ Float left,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Float top,
        /* [in] */ Float zNear,
        /* [in] */ Float zFar);

    CARAPI GlOrthox(
        /* [in] */ Int32 left,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom,
        /* [in] */ Int32 top,
        /* [in] */ Int32 zNear,
        /* [in] */ Int32 zFar);

    CARAPI GlPixelStorei(
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI GlPointSize(
        /* [in] */ Float size);

    CARAPI GlPointSizex(
        /* [in] */ Int32 size);

    CARAPI GlPolygonOffset(
        /* [in] */ Float factor,
        /* [in] */ Float units);

    CARAPI GlPolygonOffsetx(
        /* [in] */ Int32 factor,
        /* [in] */ Int32 units);

    CARAPI GlPopMatrix();

    CARAPI GlPushMatrix();

    CARAPI GlReadPixels(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 format,
        /* [in] */ Int32 type,
        /* [in] */ IBuffer* pixels);

    CARAPI GlRotatef(
        /* [in] */ Float angle,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z);

    CARAPI GlRotatex(
        /* [in] */ Int32 angle,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z);

    CARAPI GlSampleCoverage(
        /* [in] */ Float value,
        /* [in] */ Boolean invert);

    CARAPI GlSampleCoveragex(
        /* [in] */ Int32 value,
        /* [in] */ Boolean invert);

    CARAPI GlScalef(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z);

    CARAPI GlScalex(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z);

    CARAPI GlScissor(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI GlShadeModel(
        /* [in] */ Int32 mode);

    CARAPI GlStencilFunc(
        /* [in] */ Int32 func,
        /* [in] */ Int32 ref,
        /* [in] */ Int32 mask);

    CARAPI GlStencilMask(
        /* [in] */ Int32 mask);

    CARAPI GlTexCoordPointer(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ IBuffer* pointer);

    CARAPI GlTexImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 border,
        /* [in] */ Int32 format,
        /* [in] */ Int32 type,
        /* [in] */ IBuffer* pixels);

    CARAPI GlTexParameterx(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI GlTexSubImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 xoffset,
        /* [in] */ Int32 yoffset,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 format,
        /* [in] */ Int32 type,
        /* [in] */ IBuffer* pixels);

    CARAPI GlTranslatef(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z);

    CARAPI GlTranslatex(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z);

    CARAPI GlVertexPointer(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ IBuffer* pointer);

    CARAPI GlViewport(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI GlEnable(
        /* [in] */ Int32 cap);

    CARAPI GlEnableClientState(
        /* [in] */ Int32 array);


    CARAPI GlTexParameterf(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    CARAPI GlTexParameterfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI GlTexParameterfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params);

    CARAPI GlBindTexture(
        /* [in] */ Int32 target,
        /* [in] */ Int32 texture);

    CARAPI GlCompressedTexImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 border,
        /* [in] */ Int32 imageSize,
        /* [in] */ IBuffer* data);

    CARAPI GlCopyTexImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 border);

    CARAPI GlGetIntegerv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlGetIntegerv(
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI GlStencilOp(
        /* [in] */ Int32 fail,
        /* [in] */ Int32 zfail,
        /* [in] */ Int32 zpass);

    CARAPI GlTexEnvf(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    CARAPI GlTexEnvfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI GlTexEnvfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params);

    CARAPI GlTexEnvx(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI GlTexEnvxv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlTexEnvxv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI GlQueryMatrixxOES(
        /* [in] */ ArrayOf<Int32>* mantissa,
        /* [in] */ Int32 mantissaOffset,
        /* [in] */ ArrayOf<Int32>* exponent,
        /* [in] */ Int32 exponentOffset,
        /* [out] */ Int32* matrixxOES);

    CARAPI GlQueryMatrixxOES(
        /* [in] */ IInt32Buffer* mantissa,
        /* [in] */ IInt32Buffer* exponent,
        /* [out] */ Int32* matrixxOES);

    CARAPI GlGetPointerv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<IBuffer*>* params);

    CARAPI GlBindBuffer(
        /* [in] */ Int32 target,
        /* [in] */ Int32 buffer);

    CARAPI GlBufferData(
        /* [in] */ Int32 target,
        /* [in] */ Int32 size,
        /* [in] */ IBuffer* data,
        /* [in] */ Int32 usage);

    CARAPI GlBufferSubData(
        /* [in] */ Int32 target,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 size,
        /* [in] */ IBuffer* data);

    CARAPI GlClipPlanef(
        /* [in] */ Int32 plane,
        /* [in] */ ArrayOf<Float>* equation,
        /* [in] */ Int32 offset);

    CARAPI GlClipPlanef(
        /* [in] */ Int32 plane,
        /* [in] */ IFloatBuffer* equation);

    CARAPI GlClipPlanex(
        /* [in] */ Int32 plane,
        /* [in] */ ArrayOf<Int32>* equation,
        /* [in] */ Int32 offset);

    CARAPI GlClipPlanex(
        /* [in] */ Int32 plane,
        /* [in] */ IInt32Buffer* equation);

    CARAPI GlColor4ub(
        /* [in] */ Byte red,
        /* [in] */ Byte green,
        /* [in] */ Byte blue,
        /* [in] */ Byte alpha);

    CARAPI GlColorPointer(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Int32 offset);

    CARAPI GlDeleteBuffers(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* buffers,
        /* [in] */ Int32 offset);

    CARAPI GlDeleteBuffers(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* buffers
    );

    CARAPI GlDrawElements(
        /* [in] */ Int32 mode,
        /* [in] */ Int32 count,
        /* [in] */ Int32 type,
        /* [in] */ Int32 offset);

    CARAPI GlGenBuffers(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* buffers,
        /* [in] */ Int32 offset);

    CARAPI GlGenBuffers(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* buffers);

    CARAPI GlGetBooleanv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Boolean>* params,
        /* [in] */ Int32 offset);

    CARAPI GlGetBooleanv(
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI GlGetBufferParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlGetBufferParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI GlGetClipPlanef(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* eqn,
        /* [in] */ Int32 offset);

    CARAPI GlGetClipPlanef(
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* eqn);

    CARAPI GlGetClipPlanex(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* eqn,
        /* [in] */ Int32 offset);

    CARAPI GlGetClipPlanex(
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* eqn);

    CARAPI GlGetFixedv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlGetFixedv(
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI GlGetFloatv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI GlGetFloatv(
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params);

    CARAPI GlGetLightfv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI GlGetLightfv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params);

    CARAPI GlGetLightxv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlGetLightxv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI GlGetMaterialfv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI GlGetMaterialfv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params);

    CARAPI GlGetMaterialxv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlGetMaterialxv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI GlGetTexEnviv(
        /* [in] */ Int32 env,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlGetTexEnviv(
        /* [in] */ Int32 env,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI GlGetTexEnvxv(
        /* [in] */ Int32 env,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlGetTexEnvxv(
        /* [in] */ Int32 env,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI GlGetTexParameterfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI GlGetTexParameterfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params);

    CARAPI GlGetTexParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlGetTexParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI GlGetTexParameterxv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlGetTexParameterxv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI GlIsBuffer(
        /* [in] */ Int32 buffer,
        /* [out] */ Boolean* result);

    CARAPI GlIsEnabled(
        /* [in] */ Int32 cap,
        /* [out] */ Boolean* result);

    CARAPI GlIsTexture(
        /* [in] */ Int32 texture,
        /* [out] */ Boolean* result);

    CARAPI GlNormalPointer(
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Int32 offset);

    CARAPI GlPointParameterf(
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    CARAPI GlPointParameterfv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI GlPointParameterfv(
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params);

    CARAPI GlPointParameterx(
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI GlPointParameterxv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlPointParameterxv(
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI GlPointSizePointerOES(
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ IBuffer* pointer);

    CARAPI GlTexCoordPointer(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Int32 offset);

    CARAPI GlTexEnvi(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI GlTexEnviv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlTexEnviv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI GlTexParameteri(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI GlTexParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlTexParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI GlTexParameterxv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlTexParameterxv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI GlVertexPointer(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Int32 offset);

    CARAPI GlCurrentPaletteMatrixOES(
        /* [in] */ Int32 matrixpaletteindex);

    CARAPI GlDrawTexfOES(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z,
        /* [in] */ Float width,
        /* [in] */ Float height);

    CARAPI GlDrawTexfvOES(
        /* [in] */ ArrayOf<Float>* coords,
        /* [in] */ Int32 offset);

    CARAPI GlDrawTexfvOES(
        /* [in] */ IFloatBuffer* coords);

    CARAPI GlDrawTexiOES(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI GlDrawTexivOES(
        /* [in] */ ArrayOf<Int32>* coords,
        /* [in] */ Int32 offset);

    CARAPI GlDrawTexivOES(
        /* [in] */ IInt32Buffer* coords);

    CARAPI GlDrawTexsOES(
        /* [in] */ Int16 x,
        /* [in] */ Int16 y,
        /* [in] */ Int16 z,
        /* [in] */ Int16 width,
        /* [in] */ Int16 height);

    CARAPI GlDrawTexsvOES(
        /* [in] */ ArrayOf<Int16>* coords,
        /* [in] */ Int32 offset);

    CARAPI GlDrawTexsvOES(
        /* [in] */ IInt16Buffer* coords);

    CARAPI GlDrawTexxOES(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI GlDrawTexxvOES(
        /* [in] */ ArrayOf<Int32>* coords,
        /* [in] */ Int32 offset);

    CARAPI GlDrawTexxvOES(
        /* [in] */ IInt32Buffer* coords);

    CARAPI GlLoadPaletteFromModelViewMatrixOES();

    CARAPI GlMatrixIndexPointerOES(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ IBuffer* pointer);

    CARAPI GlMatrixIndexPointerOES(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Int32 offset);

    CARAPI GlWeightPointerOES(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ IBuffer* pointer);

    CARAPI GlWeightPointerOES(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Int32 offset);

    CARAPI GlBindFramebufferOES(
        /* [in] */ Int32 target,
        /* [in] */ Int32 framebuffer);

    CARAPI GlBindRenderbufferOES(
        /* [in] */ Int32 target,
        /* [in] */ Int32 renderbuffer);

    CARAPI GlBlendEquation(
        /* [in] */ Int32 mode);

    CARAPI GlBlendEquationSeparate(
        /* [in] */ Int32 modeRGB,
        /* [in] */ Int32 modeAlpha);

    CARAPI GlBlendFuncSeparate(
        /* [in] */ Int32 srcRGB,
        /* [in] */ Int32 dstRGB,
        /* [in] */ Int32 srcAlpha,
        /* [in] */ Int32 dstAlpha);

    CARAPI GlCheckFramebufferStatusOES(
        /* [in] */ Int32 target,
        /* [out] */ Int32* status);

    CARAPI GlDeleteFramebuffersOES(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* framebuffers,
        /* [in] */ Int32 offset);

    CARAPI GlDeleteFramebuffersOES(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* framebuffers);

    CARAPI GlDeleteRenderbuffersOES(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* renderbuffers,
        /* [in] */ Int32 offset);

    CARAPI GlDeleteRenderbuffersOES(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* renderbuffers);

    CARAPI GlFramebufferRenderbufferOES(
        /* [in] */ Int32 target,
        /* [in] */ Int32 attachment,
        /* [in] */ Int32 renderbuffertarget,
        /* [in] */ Int32 renderbuffer);

    CARAPI GlFramebufferTexture2DOES(
        /* [in] */ Int32 target,
        /* [in] */ Int32 attachment,
        /* [in] */ Int32 textarget,
        /* [in] */ Int32 texture,
        /* [in] */ Int32 level);

    CARAPI GlGenerateMipmapOES(
        /* [in] */ Int32 target);

    CARAPI GlGenFramebuffersOES(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* framebuffers,
        /* [in] */ Int32 offset);

    CARAPI GlGenFramebuffersOES(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* framebuffers);

    CARAPI GlGenRenderbuffersOES(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* renderbuffers,
        /* [in] */ Int32 offset);

    CARAPI GlGenRenderbuffersOES(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* renderbuffers);

    CARAPI GlGetFramebufferAttachmentParameterivOES(
        /* [in] */ Int32 target,
        /* [in] */ Int32 attachment,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlGetFramebufferAttachmentParameterivOES(
        /* [in] */ Int32 target,
        /* [in] */ Int32 attachment,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI GlGetRenderbufferParameterivOES(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlGetRenderbufferParameterivOES(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI GlGetTexGenfv(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI GlGetTexGenfv(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params);

    CARAPI GlGetTexGeniv(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlGetTexGeniv(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI GlGetTexGenxv(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlGetTexGenxv(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI GlIsFramebufferOES(
        /* [in] */ Int32 framebuffer,
        /* [out] */ Boolean* result);

    CARAPI GlIsRenderbufferOES(
        /* [in] */ Int32 renderbuffer,
        /* [out] */ Boolean* result);

    CARAPI GlRenderbufferStorageOES(
        /* [in] */ Int32 target,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI GlTexGenf(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    CARAPI GlTexGenfv(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    CARAPI GlTexGenfv(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params);

    CARAPI GlTexGeni(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI GlTexGeniv(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlTexGeniv(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    CARAPI GlTexGenx(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI GlTexGenxv(
        /* [in] */ Int32 coord,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    CARAPI GlTexGenxv(
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

    using Object::ToString;

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

    template<typename T>
        CARAPI_(String) ParamsToString(
            /* [in] */ T* buf);

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
