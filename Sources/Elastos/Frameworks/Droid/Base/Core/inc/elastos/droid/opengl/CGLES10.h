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

#ifndef __ELASTOS_DROID_OPENGL_CGLES10_H__
#define __ELASTOS_DROID_OPENGL_CGLES10_H__

#include "Elastos.Droid.Opengl.h"
#include "Elastos.CoreLibrary.IO.h"
#include "_Elastos_Droid_Opengl_CGLES10.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;
using Elastos::IO::IBuffer;

namespace Elastos {
namespace Droid {
namespace Opengl {

CarClass(CGLES10)
    , public Singleton
    , public IGLES10
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GlActiveTexture(
        /* [in] */ Int32 texture);

    CARAPI GlAlphaFunc(
        /* [in] */ Int32 func,
        /* [in] */ Float ref);

    CARAPI GlAlphaFuncx(
        /* [in] */ Int32 func,
        /* [in] */ Int32 ref);

    CARAPI GlBindTexture(
        /* [in] */ Int32 target,
        /* [in] */ Int32 texture);

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
        /* [in] */ Elastos::IO::IBuffer* pointer);

    CARAPI GlCompressedTexImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 border,
        /* [in] */ Int32 imageSize,
        /* [in] */ Elastos::IO::IBuffer* data_buf);

    CARAPI GlCompressedTexSubImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 xoffset,
        /* [in] */ Int32 yoffset,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 format,
        /* [in] */ Int32 imageSize,
        /* [in] */ Elastos::IO::IBuffer* data_buf);

    CARAPI GlCopyTexImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 border);

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
        /* [in] */ ArrayOf<Int32>* textures_buf,
        /* [in] */ Int32 offset);

    CARAPI GlDeleteTextures(
        /* [in] */ Int32 n,
        /* [in] */ Elastos::IO::IInt32Buffer* textures_buf);

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
        /* [in] */ Elastos::IO::IBuffer* indices_buf);

    CARAPI GlEnable(
        /* [in] */ Int32 cap);

    CARAPI GlEnableClientState(
        /* [in] */ Int32 array);

    CARAPI GlFinish();

    CARAPI GlFlush();

    CARAPI GlFogf(
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    CARAPI GlFogfv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params_ref,
        /* [in] */ Int32 offset);

    CARAPI GlFogfv(
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IFloatBuffer* params_buf);

    CARAPI GlFogx(
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI GlFogxv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params_ref,
        /* [in] */ Int32 offset);

    CARAPI GlFogxv(
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params_buf);

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
        /* [in] */ ArrayOf<Int32>* textures_ref,
        /* [in] */ Int32 offset);

    CARAPI GlGenTextures(
        /* [in] */ Int32 n,
        /* [in] */ Elastos::IO::IInt32Buffer* textures_buf);

    CARAPI GlGetError(
        /* [out] */ Int32* error);

    CARAPI GlGetIntegerv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params_ref,
        /* [in] */ Int32 offset);

    CARAPI GlGetIntegerv(
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params_buf);

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
        /* [in] */ ArrayOf<Float>* params_ref,
        /* [in] */ Int32 offset);

    CARAPI GlLightModelfv(
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IFloatBuffer* params_buf);

    CARAPI GlLightModelx(
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI GlLightModelxv(
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params_ref,
        /* [in] */ Int32 offset);

    CARAPI GlLightModelxv(
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params_buf);

    CARAPI GlLightf(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    CARAPI GlLightfv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params_ref,
        /* [in] */ Int32 offset);

    CARAPI GlLightfv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IFloatBuffer* params_buf);

    CARAPI GlLightx(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI GlLightxv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params_ref,
        /* [in] */ Int32 offset);

    CARAPI GlLightxv(
        /* [in] */ Int32 light,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params_buf);

    CARAPI GlLineWidth(
        /* [in] */ Float width);

    CARAPI GlLineWidthx(
        /* [in] */ Int32 width);

    CARAPI GlLoadIdentity();

    CARAPI GlLoadMatrixf(
        /* [in] */ ArrayOf<Float>* m,
        /* [in] */ Int32 offset);

    CARAPI GlLoadMatrixf(
        /* [in] */ Elastos::IO::IFloatBuffer* m);

    CARAPI GlLoadMatrixx(
        /* [in] */ ArrayOf<Int32>* m,
        /* [in] */ Int32 offset);

    CARAPI GlLoadMatrixx(
        /* [in] */ Elastos::IO::IInt32Buffer* m);

    CARAPI GlLogicOp(
        /* [in] */ Int32 opcode);

    CARAPI GlMaterialf(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    CARAPI GlMaterialfv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params_ref,
        /* [in] */ Int32 offset);

    CARAPI GlMaterialfv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IFloatBuffer* params_buf);

    CARAPI GlMaterialx(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI GlMaterialxv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params_ref,
        /* [in] */ Int32 offset);

    CARAPI GlMaterialxv(
        /* [in] */ Int32 face,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params_buf);

    CARAPI GlMatrixMode(
        /* [in] */ Int32 mode);

    CARAPI GlMultMatrixf(
        /* [in] */ ArrayOf<Float>* m,
        /* [in] */ Int32 offset);

    CARAPI GlMultMatrixf(
        /* [in] */ Elastos::IO::IFloatBuffer* m);

    CARAPI GlMultMatrixx(
        /* [in] */ ArrayOf<Int32>* m,
        /* [in] */ Int32 offset);

    CARAPI GlMultMatrixx(
        /* [in] */ Elastos::IO::IInt32Buffer* m);

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
        /* [in] */ Elastos::IO::IBuffer* pointer);

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
        /* [in] */ Elastos::IO::IBuffer* pixels);

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

    CARAPI GlStencilOp(
        /* [in] */ Int32 fail,
        /* [in] */ Int32 zfail,
        /* [in] */ Int32 zpass);

    CARAPI GlTexCoordPointer(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Elastos::IO::IBuffer* pointer);

    CARAPI GlTexEnvf(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

    CARAPI GlTexEnvfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params_ref,
        /* [in] */ Int32 offset);

    CARAPI GlTexEnvfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IFloatBuffer* params_buf);

    CARAPI GlTexEnvx(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    CARAPI GlTexEnvxv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params_ref,
        /* [in] */ Int32 offset);

    CARAPI GlTexEnvxv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Elastos::IO::IInt32Buffer* params_buf);

    CARAPI GlTexImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 border,
        /* [in] */ Int32 format,
        /* [in] */ Int32 type,
        /* [in] */ Elastos::IO::IBuffer* pixels);

    CARAPI GlTexParameterf(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Float param);

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
        /* [in] */ Elastos::IO::IBuffer* pixels);

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
        /* [in] */ Elastos::IO::IBuffer* pointer);

    CARAPI GlViewport(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

private:
    CARAPI GlColorPointerBounds(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Elastos::IO::IBuffer* pointer,
        /* [in] */ Int32 remaining);

    CARAPI GetDirectBufferPointer(
        /* [in] */ IBuffer* buffer,
        /* [out] */ Handle64* result);

    CARAPI GlTexCoordPointerBounds(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Elastos::IO::IBuffer* pointer_buf,
        /* [in] */ Int32 remaining);

    CARAPI GlNormalPointerBounds(
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Elastos::IO::IBuffer* pointer_buf,
        /* [in] */ Int32 remaining);

    CARAPI GlVertexPointerBounds(
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 stride,
        /* [in] */ Elastos::IO::IBuffer* pointer_buf,
        /* [in] */ Int32 remaining);

    CARAPI_(Int32) GetNumCompressedTextureFormats();

private:
    AutoPtr<IBuffer> m_ColorPointer;
    AutoPtr<IBuffer> m_NormalPointer;
    AutoPtr<IBuffer> m_TexCoordPointer;
    AutoPtr<IBuffer> m_VertexPointer;
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OPENGL_CGLES10_H__
