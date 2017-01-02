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

#ifndef __ELASTOS_DROID_OPENGL_CGLES31EXT_H__
#define __ELASTOS_DROID_OPENGL_CGLES31EXT_H__

#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.Droid.Opengl.h"
#include <elastos/core/Singleton.h>

using Elastos::IO::IBuffer;
using Elastos::IO::IInt32Buffer;
using Elastos::IO::IInt64Buffer;
using Elastos::IO::IFloatBuffer;
using Elastos::IO::IByteBuffer;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Opengl {

class GLES31Ext
    : public Singleton
    , public IGLES31Ext
{
public:
    CAR_INTERFACE_DECL()

    CARAPI GlBlendBarrierKHR(
    );

    // C function void glDebugMessageControlKHR ( GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled )

    CARAPI GlDebugMessageControlKHR(
        /* [in] */ Int32 source,
        /* [in] */ Int32 type,
        /* [in] */ Int32 severity,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Int32>* ids,
        /* [in] */ Int32 offset,
        /* [in] */ Boolean enabled
    );

    // C function void glDebugMessageControlKHR ( GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled )

    CARAPI GlDebugMessageControlKHR(
        /* [in] */ Int32 source,
        /* [in] */ Int32 type,
        /* [in] */ Int32 severity,
        /* [in] */ Int32 count,
        /* [in] */ IInt32Buffer* ids,
        /* [in] */ Boolean enabled
    );

    // C function void glDebugMessageInsertKHR ( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *buf )

    CARAPI GlDebugMessageInsertKHR(
        /* [in] */ Int32 source,
        /* [in] */ Int32 type,
        /* [in] */ Int32 id,
        /* [in] */ Int32 severity,
        /* [in] */ const String& buf
    );

    // C function void glDebugMessageCallbackKHR ( GLDEBUGPROCKHR callback, const void *userParam )

    CARAPI GlDebugMessageCallbackKHR(
        /* [in] */ IDebugProcKHR* cb);

    // C function GLuint glGetDebugMessageLogKHR ( GLuint count, GLsizei bufSize, GLenum *sources, GLenum *types, GLuint *ids, GLenum *severities, GLsizei *lengths, GLchar *messageLog )

    CARAPI GlGetDebugMessageLogKHR(
        /* [in] */ Int32 count,
        /* [in] */ Int32 bufSize,
        /* [in] */ ArrayOf<Int32>* sources,
        /* [in] */ Int32 sourcesOffset,
        /* [in] */ ArrayOf<Int32>* types,
        /* [in] */ Int32 typesOffset,
        /* [in] */ ArrayOf<Int32>* ids,
        /* [in] */ Int32 idsOffset,
        /* [in] */ ArrayOf<Int32>* severities,
        /* [in] */ Int32 severitiesOffset,
        /* [in] */ ArrayOf<Int32>* lengths,
        /* [in] */ Int32 lengthsOffset,
        /* [in] */ ArrayOf<Byte>* messageLog,
        /* [in] */ Int32 messageLogOffset,
        /* [out] */ Int32* result);

    // C function GLuint glGetDebugMessageLogKHR ( GLuint count, GLsizei bufSize, GLenum *sources, GLenum *types, GLuint *ids, GLenum *severities, GLsizei *lengths, GLchar *messageLog )

    CARAPI GlGetDebugMessageLogKHR(
        /* [in] */ Int32 count,
        /* [in] */ IInt32Buffer* sources,
        /* [in] */ IInt32Buffer* types,
        /* [in] */ IInt32Buffer* ids,
        /* [in] */ IInt32Buffer* severities,
        /* [in] */ IInt32Buffer* lengths,
        /* [in] */ IByteBuffer* messageLog,
        /* [out] */ Int32* result);

    // C function GLuint glGetDebugMessageLogKHR ( GLuint count, GLsizei bufSize, GLenum *sources, GLenum *types, GLuint *ids, GLenum *severities, GLsizei *lengths, GLchar *messageLog )

    CARAPI GlGetDebugMessageLogKHR(
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Int32>* sources,
        /* [in] */ Int32 sourcesOffset,
        /* [in] */ ArrayOf<Int32>* types,
        /* [in] */ Int32 typesOffset,
        /* [in] */ ArrayOf<Int32>* ids,
        /* [in] */ Int32 idsOffset,
        /* [in] */ ArrayOf<Int32>* severities,
        /* [in] */ Int32 severitiesOffset,
        /* [out, callee] */ ArrayOf<String>** result);

    // C function GLuint glGetDebugMessageLogKHR ( GLuint count, GLsizei bufSize, GLenum *sources, GLenum *types, GLuint *ids, GLenum *severities, GLsizei *lengths, GLchar *messageLog )

    CARAPI GlGetDebugMessageLogKHR(
        /* [in] */ Int32 count,
        /* [in] */ IInt32Buffer* sources,
        /* [in] */ IInt32Buffer* types,
        /* [in] */ IInt32Buffer* ids,
        /* [in] */ IInt32Buffer* severities,
        /* [out, callee] */ ArrayOf<String>** result);

    // C function void glPushDebugGroupKHR ( GLenum source, GLuint id, GLsizei length, const GLchar *message )

    CARAPI GlPushDebugGroupKHR(
        /* [in] */ Int32 source,
        /* [in] */ Int32 id,
        /* [in] */ Int32 length,
        /* [in] */ const String& message
    );

    // C function void glPopDebugGroupKHR ( void )

    CARAPI GlPopDebugGroupKHR(
    );

    // C function void glObjectLabelKHR ( GLenum identifier, GLuint name, GLsizei length, const GLchar *label )

    CARAPI GlObjectLabelKHR(
        /* [in] */ Int32 identifier,
        /* [in] */ Int32 name,
        /* [in] */ Int32 length,
        /* [in] */ const String& label
    );

    // C function void glGetObjectLabelKHR ( GLenum identifier, GLuint name, GLsizei bufSize, GLsizei *length, GLchar *label )

    CARAPI GlGetObjectLabelKHR(
        /* [in] */ Int32 identifier,
        /* [in] */ Int32 name,
        /* [out] */ String* result);

    // C function void glObjectPtrLabelKHR ( const void *ptr, GLsizei length, const GLchar *label )

    CARAPI GlObjectPtrLabelKHR(
        /* [in] */ Int64 ptr,
        /* [in] */ const String& label);

    // C function void glGetObjectPtrLabelKHR ( const void *ptr, GLsizei bufSize, GLsizei *length, GLchar *label )

    CARAPI GlGetObjectPtrLabelKHR(
        /* [in] */ Int64 ptr,
        /* [out] */ String* label);

    // C function void glGetPointervKHR ( GLenum pname, void **params )

    CARAPI GlGetDebugMessageCallbackKHR(
        /* [out] */ IDebugProcKHR** khr);

    // C function void glMinSampleShadingOES ( GLfloat value )

    CARAPI GlMinSampleShadingOES(
        /* [in] */ Float value
    );

    // C function void glTexStorage3DMultisampleOES ( GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations )

    CARAPI GlTexStorage3DMultisampleOES(
        /* [in] */ Int32 target,
        /* [in] */ Int32 samples,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 depth,
        /* [in] */ Boolean fixedsamplelocations
    );

    // C function void glCopyImageSubDataEXT ( GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth )

    CARAPI GlCopyImageSubDataEXT(
        /* [in] */ Int32 srcName,
        /* [in] */ Int32 srcTarget,
        /* [in] */ Int32 srcLevel,
        /* [in] */ Int32 srcX,
        /* [in] */ Int32 srcY,
        /* [in] */ Int32 srcZ,
        /* [in] */ Int32 dstName,
        /* [in] */ Int32 dstTarget,
        /* [in] */ Int32 dstLevel,
        /* [in] */ Int32 dstX,
        /* [in] */ Int32 dstY,
        /* [in] */ Int32 dstZ,
        /* [in] */ Int32 srcWidth,
        /* [in] */ Int32 srcHeight,
        /* [in] */ Int32 srcDepth
    );

    // C function void glEnableiEXT ( GLenum target, GLuint index )

    CARAPI GlEnableiEXT(
        /* [in] */ Int32 target,
        /* [in] */ Int32 index
    );

    // C function void glDisableiEXT ( GLenum target, GLuint index )

    CARAPI GlDisableiEXT(
        /* [in] */ Int32 target,
        /* [in] */ Int32 index
    );

    // C function void glBlendEquationiEXT ( GLuint buf, GLenum mode )

    CARAPI GlBlendEquationiEXT(
        /* [in] */ Int32 buf,
        /* [in] */ Int32 mode
    );

    // C function void glBlendEquationSeparateiEXT ( GLuint buf, GLenum modeRGB, GLenum modeAlpha )

    CARAPI GlBlendEquationSeparateiEXT(
        /* [in] */ Int32 buf,
        /* [in] */ Int32 modeRGB,
        /* [in] */ Int32 modeAlpha
    );

    // C function void glBlendFunciEXT ( GLuint buf, GLenum src, GLenum dst )

    CARAPI GlBlendFunciEXT(
        /* [in] */ Int32 buf,
        /* [in] */ Int32 src,
        /* [in] */ Int32 dst
    );

    // C function void glBlendFuncSeparateiEXT ( GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha )

    CARAPI GlBlendFuncSeparateiEXT(
        /* [in] */ Int32 buf,
        /* [in] */ Int32 srcRGB,
        /* [in] */ Int32 dstRGB,
        /* [in] */ Int32 srcAlpha,
        /* [in] */ Int32 dstAlpha
    );

    // C function void glColorMaskiEXT ( GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a )

    CARAPI GlColorMaskiEXT(
        /* [in] */ Int32 index,
        /* [in] */ Boolean r,
        /* [in] */ Boolean g,
        /* [in] */ Boolean b,
        /* [in] */ Boolean a
    );

    // C function GLboolean glIsEnablediEXT ( GLenum target, GLuint index )

    CARAPI GlIsEnablediEXT(
        /* [in] */ Int32 target,
        /* [in] */ Int32 index,
        /* [out] */ Boolean* result
    );

    // C function void glFramebufferTextureEXT ( GLenum target, GLenum attachment, GLuint texture, GLint level )

    CARAPI GlFramebufferTextureEXT(
        /* [in] */ Int32 target,
        /* [in] */ Int32 attachment,
        /* [in] */ Int32 texture,
        /* [in] */ Int32 level
    );

    // C function void glPrimitiveBoundingBoxEXT ( GLfloat minX, GLfloat minY, GLfloat minZ, GLfloat minW, GLfloat maxX, GLfloat maxY, GLfloat maxZ, GLfloat maxW )

    CARAPI GlPrimitiveBoundingBoxEXT(
        /* [in] */ Float minX,
        /* [in] */ Float minY,
        /* [in] */ Float minZ,
        /* [in] */ Float minW,
        /* [in] */ Float maxX,
        /* [in] */ Float maxY,
        /* [in] */ Float maxZ,
        /* [in] */ Float maxW
    );

    // C function void glPatchParameteriEXT ( GLenum pname, GLint value )

    CARAPI GlPatchParameteriEXT(
        /* [in] */ Int32 pname,
        /* [in] */ Int32 value
    );

    // C function void glTexParameterIivEXT ( GLenum target, GLenum pname, const GLint *params )

    CARAPI GlTexParameterIivEXT(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset
    );

    // C function void glTexParameterIivEXT ( GLenum target, GLenum pname, const GLint *params )

    CARAPI GlTexParameterIivEXT(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params
    );

    // C function void glTexParameterIuivEXT ( GLenum target, GLenum pname, const GLuint *params )

    CARAPI GlTexParameterIuivEXT(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset
    );

    // C function void glTexParameterIuivEXT ( GLenum target, GLenum pname, const GLuint *params )

    CARAPI GlTexParameterIuivEXT(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params
    );

    // C function void glGetTexParameterIivEXT ( GLenum target, GLenum pname, GLint *params )

    CARAPI GlGetTexParameterIivEXT(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset
    );

    // C function void glGetTexParameterIivEXT ( GLenum target, GLenum pname, GLint *params )

    CARAPI GlGetTexParameterIivEXT(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params
    );

    // C function void glGetTexParameterIuivEXT ( GLenum target, GLenum pname, GLuint *params )

    CARAPI GlGetTexParameterIuivEXT(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset
    );

    // C function void glGetTexParameterIuivEXT ( GLenum target, GLenum pname, GLuint *params )

    CARAPI GlGetTexParameterIuivEXT(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params
    );

    // C function void glSamplerParameterIivEXT ( GLuint sampler, GLenum pname, const GLint *param )

    CARAPI GlSamplerParameterIivEXT(
        /* [in] */ Int32 sampler,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* param,
        /* [in] */ Int32 offset
    );

    // C function void glSamplerParameterIivEXT ( GLuint sampler, GLenum pname, const GLint *param )

    CARAPI GlSamplerParameterIivEXT(
        /* [in] */ Int32 sampler,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* param
    );

    // C function void glSamplerParameterIuivEXT ( GLuint sampler, GLenum pname, const GLuint *param )

    CARAPI GlSamplerParameterIuivEXT(
        /* [in] */ Int32 sampler,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* param,
        /* [in] */ Int32 offset
    );

    // C function void glSamplerParameterIuivEXT ( GLuint sampler, GLenum pname, const GLuint *param )

    CARAPI GlSamplerParameterIuivEXT(
        /* [in] */ Int32 sampler,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* param
    );

    // C function void glGetSamplerParameterIivEXT ( GLuint sampler, GLenum pname, GLint *params )

    CARAPI GlGetSamplerParameterIivEXT(
        /* [in] */ Int32 sampler,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset
    );

    // C function void glGetSamplerParameterIivEXT ( GLuint sampler, GLenum pname, GLint *params )

    CARAPI GlGetSamplerParameterIivEXT(
        /* [in] */ Int32 sampler,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params
    );

    // C function void glGetSamplerParameterIuivEXT ( GLuint sampler, GLenum pname, GLuint *params )

    CARAPI GlGetSamplerParameterIuivEXT(
        /* [in] */ Int32 sampler,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset
    );

    // C function void glGetSamplerParameterIuivEXT ( GLuint sampler, GLenum pname, GLuint *params )

    CARAPI GlGetSamplerParameterIuivEXT(
        /* [in] */ Int32 sampler,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params
    );

    // C function void glTexBufferEXT ( GLenum target, GLenum internalformat, GLuint buffer )

    CARAPI GlTexBufferEXT(
        /* [in] */ Int32 target,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 buffer
    );

    // C function void glTexBufferRangeEXT ( GLenum target, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size )

    CARAPI GlTexBufferRangeEXT(
        /* [in] */ Int32 target,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 size
    );
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OPENGL_CGLES31EXT_H__