#ifndef __ELASTOS_DROID_OPENGL_CGLES31_H__
#define __ELASTOS_DROID_OPENGL_CGLES31_H__

#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.Droid.Opengl.h"
#include "elastos/droid/opengl/GLES30.h"

using Elastos::IO::IBuffer;
using Elastos::IO::IInt32Buffer;
using Elastos::IO::IInt64Buffer;
using Elastos::IO::IFloatBuffer;

namespace Elastos {
namespace Droid {
namespace Opengl {

class GLES31
    : public GLES30
    , public IGLES31
{
public:
    CAR_INTERFACE_DECL()

    // C function void glDispatchCompute ( GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z )

    CARAPI GlDispatchCompute(
        /* [in] */ Int32 num_groups_x,
        /* [in] */ Int32 num_groups_y,
        /* [in] */ Int32 num_groups_z);

    // C function void glDispatchComputeIndirect ( GLintptr indirect );

    CARAPI GlDispatchComputeIndirect(
        /* [in] */ Int64 indirect);

    // C function void glDrawArraysIndirect ( GLenum mode, const void *indirect );

    CARAPI GlDrawArraysIndirect(
        /* [in] */ Int32 mode,
        /* [in] */ Int64 indirect);

    // C function glDrawElementsIndirect ( GLenum mode, GLenum type, const void *indirect );

    CARAPI GlDrawElementsIndirect(
        /* [in] */ Int32 mode,
        /* [in] */ Int32 type,
        /* [in] */ Int64 indirect);

    // C function void glFramebufferParameteri ( GLenum target, GLenum pname, GLint param )

    CARAPI GlFramebufferParameteri(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ Int32 param);

    // C function void glGetFramebufferParameteriv ( GLenum target, GLenum pname, GLint *params )

    CARAPI GlGetFramebufferParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    // C function void glGetFramebufferParameteriv ( GLenum target, GLenum pname, GLint *params )

    CARAPI GlGetFramebufferParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    // C function void glGetProgramInterfaceiv ( GLuint program, GLenum programInterface, GLenum pname, GLint *params )

    CARAPI GlGetProgramInterfaceiv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 programInterface,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    // C function void glGetProgramInterfaceiv ( GLuint program, GLenum programInterface, GLenum pname, GLint *params )

    CARAPI GlGetProgramInterfaceiv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 programInterface,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    // C function GLuint glGetProgramResourceIndex ( GLuint program, GLenum programInterface, const GLchar *name )

    CARAPI GlGetProgramResourceIndex(
        /* [in] */ Int32 program,
        /* [in] */ Int32 programInterface,
        /* [in] */ const String& name,
        /* [out] */ Int32* index);

    // C function void glGetProgramResourceName ( GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei *length, GLchar *name )

    CARAPI GlGetProgramResourceName(
        /* [in] */ Int32 program,
        /* [in] */ Int32 programInterface,
        /* [in] */ Int32 index,
        /* [out] */ String* result);

    // C function void glGetProgramResourceiv ( GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum *props, GLsizei bufSize, GLsizei *length, GLint *params )

    CARAPI GlGetProgramResourceiv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 programInterface,
        /* [in] */ Int32 index,
        /* [in] */ Int32 propCount,
        /* [in] */ ArrayOf<Int32>* props,
        /* [in] */ Int32 propsOffset,
        /* [in] */ Int32 bufSize,
        /* [in] */ ArrayOf<Int32>* length,
        /* [in] */ Int32 lengthOffset,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 paramsOffset);

    // C function void glGetProgramResourceiv ( GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum *props, GLsizei bufSize, GLsizei *length, GLint *params )

    CARAPI GlGetProgramResourceiv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 programInterface,
        /* [in] */ Int32 index,
        /* [in] */ Int32 propCount,
        /* [in] */ IInt32Buffer* props,
        /* [in] */ Int32 bufSize,
        /* [in] */ IInt32Buffer* length,
        /* [in] */ IInt32Buffer* params);

    // C function GLint glGetProgramResourceLocation ( GLuint program, GLenum programInterface, const GLchar *name )

    CARAPI GlGetProgramResourceLocation(
        /* [in] */ Int32 program,
        /* [in] */ Int32 programInterface,
        /* [in] */ const String& name,
        /* [out] */ Int32* result);

    // C function void glUseProgramStages ( GLuint pipeline, GLbitfield stages, GLuint program )

    CARAPI GlUseProgramStages(
        /* [in] */ Int32 pipeline,
        /* [in] */ Int32 stages,
        /* [in] */ Int32 program);

    // C function void glActiveShaderProgram ( GLuint pipeline, GLuint program )

    CARAPI GlActiveShaderProgram(
        /* [in] */ Int32 pipeline,
        /* [in] */ Int32 program);

    // C function GLuint glCreateShaderProgramv ( GLenum type, GLsizei count, const GLchar *const *strings )

    CARAPI GlCreateShaderProgramv(
        /* [in] */ Int32 type,
        /* [in] */ ArrayOf<String>* strings,
        /* [out] */ Int32* result);

    // C function void glBindProgramPipeline ( GLuint pipeline )

    CARAPI GlBindProgramPipeline(
        /* [in] */ Int32 pipeline);

    // C function void glDeleteProgramPipelines ( GLsizei n, const GLuint *pipelines )

    CARAPI GlDeleteProgramPipelines(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* pipelines,
        /* [in] */ Int32 offset);

    // C function void glDeleteProgramPipelines ( GLsizei n, const GLuint *pipelines )

    CARAPI GlDeleteProgramPipelines(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* pipelines);

    // C function void glGenProgramPipelines ( GLsizei n, GLuint *pipelines )

    CARAPI GlGenProgramPipelines(
        /* [in] */ Int32 n,
        /* [in] */ ArrayOf<Int32>* pipelines,
        /* [in] */ Int32 offset);

    // C function void glGenProgramPipelines ( GLsizei n, GLuint *pipelines )

    CARAPI GlGenProgramPipelines(
        /* [in] */ Int32 n,
        /* [in] */ IInt32Buffer* pipelines);

    // C function GLboolean glIsProgramPipeline ( GLuint pipeline )

    CARAPI GlIsProgramPipeline(
        /* [in] */ Int32 pipeline,
        /* [out] */ Boolean* result);

    // C function void glGetProgramPipelineiv ( GLuint pipeline, GLenum pname, GLint *params )

    CARAPI GlGetProgramPipelineiv(
        /* [in] */ Int32 pipeline,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    // C function void glGetProgramPipelineiv ( GLuint pipeline, GLenum pname, GLint *params )

    CARAPI GlGetProgramPipelineiv(
        /* [in] */ Int32 pipeline,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    // C function void glProgramUniform1i ( GLuint program, GLint location, GLint v0 )

    CARAPI GlProgramUniform1i(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 v0);

    // C function void glProgramUniform2i ( GLuint program, GLint location, GLint v0, GLint v1 )

    CARAPI GlProgramUniform2i(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 v0,
        /* [in] */ Int32 v1);

    // C function void glProgramUniform3i ( GLuint program, GLint location, GLint v0, GLint v1, GLint v2 )

    CARAPI GlProgramUniform3i(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 v0,
        /* [in] */ Int32 v1,
        /* [in] */ Int32 v2);

    // C function void glProgramUniform4i ( GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3 )

    CARAPI GlProgramUniform4i(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 v0,
        /* [in] */ Int32 v1,
        /* [in] */ Int32 v2,
        /* [in] */ Int32 v3);

    // C function void glProgramUniform1ui ( GLuint program, GLint location, GLuint v0 )

    CARAPI GlProgramUniform1ui(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 v0);

    // C function void glProgramUniform2ui ( GLuint program, GLint location, GLuint v0, GLuint v1 )

    CARAPI GlProgramUniform2ui(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 v0,
        /* [in] */ Int32 v1);

    // C function void glProgramUniform3ui ( GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2 )

    CARAPI GlProgramUniform3ui(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 v0,
        /* [in] */ Int32 v1,
        /* [in] */ Int32 v2);

    // C function void glProgramUniform4ui ( GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3 )

    CARAPI GlProgramUniform4ui(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 v0,
        /* [in] */ Int32 v1,
        /* [in] */ Int32 v2,
        /* [in] */ Int32 v3);

    // C function void glProgramUniform1f ( GLuint program, GLint location, GLfloat v0 )

    CARAPI GlProgramUniform1f(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Float v0);

    // C function void glProgramUniform2f ( GLuint program, GLint location, GLfloat v0, GLfloat v1 )

    CARAPI GlProgramUniform2f(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Float v0,
        /* [in] */ Float v1);

    // C function void glProgramUniform3f ( GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2 )

    CARAPI GlProgramUniform3f(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Float v0,
        /* [in] */ Float v1,
        /* [in] */ Float v2);

    // C function void glProgramUniform4f ( GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3 )

    CARAPI GlProgramUniform4f(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Float v0,
        /* [in] */ Float v1,
        /* [in] */ Float v2,
        /* [in] */ Float v3);

    // C function void glProgramUniform1iv ( GLuint program, GLint location, GLsizei count, const GLint *value )

    CARAPI GlProgramUniform1iv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Int32>* value,
        /* [in] */ Int32 offset);

    // C function void glProgramUniform1iv ( GLuint program, GLint location, GLsizei count, const GLint *value )

    CARAPI GlProgramUniform1iv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ IInt32Buffer* value);

    // C function void glProgramUniform2iv ( GLuint program, GLint location, GLsizei count, const GLint *value )

    CARAPI GlProgramUniform2iv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Int32>* value,
        /* [in] */ Int32 offset);

    // C function void glProgramUniform2iv ( GLuint program, GLint location, GLsizei count, const GLint *value )

    CARAPI GlProgramUniform2iv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ IInt32Buffer* value);

    // C function void glProgramUniform3iv ( GLuint program, GLint location, GLsizei count, const GLint *value )

    CARAPI GlProgramUniform3iv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Int32>* value,
        /* [in] */ Int32 offset);

    // C function void glProgramUniform3iv ( GLuint program, GLint location, GLsizei count, const GLint *value )

    CARAPI GlProgramUniform3iv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ IInt32Buffer* value);

    // C function void glProgramUniform4iv ( GLuint program, GLint location, GLsizei count, const GLint *value )

    CARAPI GlProgramUniform4iv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Int32>* value,
        /* [in] */ Int32 offset);

    // C function void glProgramUniform4iv ( GLuint program, GLint location, GLsizei count, const GLint *value )

    CARAPI GlProgramUniform4iv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ IInt32Buffer* value);

    // C function void glProgramUniform1uiv ( GLuint program, GLint location, GLsizei count, const GLuint *value )

    CARAPI GlProgramUniform1uiv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Int32>* value,
        /* [in] */ Int32 offset);

    // C function void glProgramUniform1uiv ( GLuint program, GLint location, GLsizei count, const GLuint *value )

    CARAPI GlProgramUniform1uiv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ IInt32Buffer* value);

    // C function void glProgramUniform2uiv ( GLuint program, GLint location, GLsizei count, const GLuint *value )

    CARAPI GlProgramUniform2uiv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Int32>* value,
        /* [in] */ Int32 offset);

    // C function void glProgramUniform2uiv ( GLuint program, GLint location, GLsizei count, const GLuint *value )

    CARAPI GlProgramUniform2uiv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ IInt32Buffer* value);

    // C function void glProgramUniform3uiv ( GLuint program, GLint location, GLsizei count, const GLuint *value )

    CARAPI GlProgramUniform3uiv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Int32>* value,
        /* [in] */ Int32 offset);

    // C function void glProgramUniform3uiv ( GLuint program, GLint location, GLsizei count, const GLuint *value )

    CARAPI GlProgramUniform3uiv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ IInt32Buffer* value);

    // C function void glProgramUniform4uiv ( GLuint program, GLint location, GLsizei count, const GLuint *value )

    CARAPI GlProgramUniform4uiv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Int32>* value,
        /* [in] */ Int32 offset);

    // C function void glProgramUniform4uiv ( GLuint program, GLint location, GLsizei count, const GLuint *value )

    CARAPI GlProgramUniform4uiv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ IInt32Buffer* value);

    // C function void glProgramUniform1fv ( GLuint program, GLint location, GLsizei count, const GLfloat *value )

    CARAPI GlProgramUniform1fv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Float>* value,
        /* [in] */ Int32 offset);

    // C function void glProgramUniform1fv ( GLuint program, GLint location, GLsizei count, const GLfloat *value )

    CARAPI GlProgramUniform1fv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ IFloatBuffer* value);

    // C function void glProgramUniform2fv ( GLuint program, GLint location, GLsizei count, const GLfloat *value )

    CARAPI GlProgramUniform2fv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Float>* value,
        /* [in] */ Int32 offset);

    // C function void glProgramUniform2fv ( GLuint program, GLint location, GLsizei count, const GLfloat *value )

    CARAPI GlProgramUniform2fv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ IFloatBuffer* value);

    // C function void glProgramUniform3fv ( GLuint program, GLint location, GLsizei count, const GLfloat *value )

    CARAPI GlProgramUniform3fv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Float>* value,
        /* [in] */ Int32 offset);

    // C function void glProgramUniform3fv ( GLuint program, GLint location, GLsizei count, const GLfloat *value )

    CARAPI GlProgramUniform3fv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ IFloatBuffer* value);

    // C function void glProgramUniform4fv ( GLuint program, GLint location, GLsizei count, const GLfloat *value )

    CARAPI GlProgramUniform4fv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ ArrayOf<Float>* value,
        /* [in] */ Int32 offset);

    // C function void glProgramUniform4fv ( GLuint program, GLint location, GLsizei count, const GLfloat *value )

    CARAPI GlProgramUniform4fv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ IFloatBuffer* value);

    // C function void glProgramUniformMatrix2fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlProgramUniformMatrix2fv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ ArrayOf<Float>* value,
        /* [in] */ Int32 offset);

    // C function void glProgramUniformMatrix2fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlProgramUniformMatrix2fv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ IFloatBuffer* value);

    // C function void glProgramUniformMatrix3fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlProgramUniformMatrix3fv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ ArrayOf<Float>* value,
        /* [in] */ Int32 offset);

    // C function void glProgramUniformMatrix3fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlProgramUniformMatrix3fv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ IFloatBuffer* value);

    // C function void glProgramUniformMatrix4fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlProgramUniformMatrix4fv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ ArrayOf<Float>* value,
        /* [in] */ Int32 offset);

    // C function void glProgramUniformMatrix4fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlProgramUniformMatrix4fv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ IFloatBuffer* value);

    // C function void glProgramUniformMatrix2x3fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlProgramUniformMatrix2x3fv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ ArrayOf<Float>* value,
        /* [in] */ Int32 offset);

    // C function void glProgramUniformMatrix2x3fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlProgramUniformMatrix2x3fv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ IFloatBuffer* value);

    // C function void glProgramUniformMatrix3x2fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlProgramUniformMatrix3x2fv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ ArrayOf<Float>* value,
        /* [in] */ Int32 offset);

    // C function void glProgramUniformMatrix3x2fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlProgramUniformMatrix3x2fv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ IFloatBuffer* value);

    // C function void glProgramUniformMatrix2x4fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlProgramUniformMatrix2x4fv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ ArrayOf<Float>* value,
        /* [in] */ Int32 offset);

    // C function void glProgramUniformMatrix2x4fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlProgramUniformMatrix2x4fv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ IFloatBuffer* value);

    // C function void glProgramUniformMatrix4x2fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlProgramUniformMatrix4x2fv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ ArrayOf<Float>* value,
        /* [in] */ Int32 offset);

    // C function void glProgramUniformMatrix4x2fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlProgramUniformMatrix4x2fv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ IFloatBuffer* value);

    // C function void glProgramUniformMatrix3x4fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlProgramUniformMatrix3x4fv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ ArrayOf<Float>* value,
        /* [in] */ Int32 offset);

    // C function void glProgramUniformMatrix3x4fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlProgramUniformMatrix3x4fv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ IFloatBuffer* value);

    // C function void glProgramUniformMatrix4x3fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlProgramUniformMatrix4x3fv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ ArrayOf<Float>* value,
        /* [in] */ Int32 offset);

    // C function void glProgramUniformMatrix4x3fv ( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )

    CARAPI GlProgramUniformMatrix4x3fv(
        /* [in] */ Int32 program,
        /* [in] */ Int32 location,
        /* [in] */ Int32 count,
        /* [in] */ Boolean transpose,
        /* [in] */ IFloatBuffer* value);

    // C function void glValidateProgramPipeline ( GLuint pipeline )

    CARAPI GlValidateProgramPipeline(
        /* [in] */ Int32 pipeline);

    // C function void glGetProgramPipelineInfoLog( GLuint program, GLsizei maxLength, GLsizei * length, GLchar * infoLog);

    CARAPI GlGetProgramPipelineInfoLog(
        /* [in] */ Int32 program,
        /* [out] */ String* result);

    // C function void glBindImageTexture ( GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format )

    CARAPI GlBindImageTexture(
        /* [in] */ Int32 unit,
        /* [in] */ Int32 texture,
        /* [in] */ Int32 level,
        /* [in] */ Boolean layered,
        /* [in] */ Int32 layer,
        /* [in] */ Int32 access,
        /* [in] */ Int32 format);

    // C function void glGetBooleani_v ( GLenum target, GLuint index, GLboolean *data )

    CARAPI GlGetBooleani_v(
        /* [in] */ Int32 target,
        /* [in] */ Int32 index,
        /* [in] */ ArrayOf<Boolean>* data,
        /* [in] */ Int32 offset);

    // C function void glGetBooleani_v ( GLenum target, GLuint index, GLboolean *data )

    CARAPI GlGetBooleani_v(
        /* [in] */ Int32 target,
        /* [in] */ Int32 index,
        /* [in] */ IInt32Buffer* data);

    // C function void glMemoryBarrier ( GLbitfield barriers )

    CARAPI GlMemoryBarrier(
        /* [in] */ Int32 barriers);

    // C function void glMemoryBarrierByRegion ( GLbitfield barriers )

    CARAPI GlMemoryBarrierByRegion(
        /* [in] */ Int32 barriers);

    // C function void glTexStorage2DMultisample ( GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations )

    CARAPI GlTexStorage2DMultisample(
        /* [in] */ Int32 target,
        /* [in] */ Int32 samples,
        /* [in] */ Int32 internalformat,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Boolean fixedsamplelocations);

    // C function void glGetMultisamplefv ( GLenum pname, GLuint index, GLfloat *val )

    CARAPI GlGetMultisamplefv(
        /* [in] */ Int32 pname,
        /* [in] */ Int32 index,
        /* [in] */ ArrayOf<Float>* val,
        /* [in] */ Int32 offset);

    // C function void glGetMultisamplefv ( GLenum pname, GLuint index, GLfloat *val )

    CARAPI GlGetMultisamplefv(
        /* [in] */ Int32 pname,
        /* [in] */ Int32 index,
        /* [in] */ IFloatBuffer* val);

    // C function void glSampleMaski ( GLuint maskNumber, GLbitfield mask )

    CARAPI GlSampleMaski(
        /* [in] */ Int32 maskNumber,
        /* [in] */ Int32 mask);

    // C function void glGetTexLevelParameteriv ( GLenum target, GLint level, GLenum pname, GLint *params )

    CARAPI GlGetTexLevelParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Int32>* params,
        /* [in] */ Int32 offset);

    // C function void glGetTexLevelParameteriv ( GLenum target, GLint level, GLenum pname, GLint *params )

    CARAPI GlGetTexLevelParameteriv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 pname,
        /* [in] */ IInt32Buffer* params);

    // C function void glGetTexLevelParameterfv ( GLenum target, GLint level, GLenum pname, GLfloat *params )

    CARAPI GlGetTexLevelParameterfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 pname,
        /* [in] */ ArrayOf<Float>* params,
        /* [in] */ Int32 offset);

    // C function void glGetTexLevelParameterfv ( GLenum target, GLint level, GLenum pname, GLfloat *params )

    CARAPI GlGetTexLevelParameterfv(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 pname,
        /* [in] */ IFloatBuffer* params);

    // C function void glBindVertexBuffer ( GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride )

    CARAPI GlBindVertexBuffer(
        /* [in] */ Int32 bindingindex,
        /* [in] */ Int32 buffer,
        /* [in] */ Int64 offset,
        /* [in] */ Int32 stride);

    // C function void glVertexAttribFormat ( GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset )

    CARAPI GlVertexAttribFormat(
        /* [in] */ Int32 attribindex,
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Boolean normalized,
        /* [in] */ Int32 relativeoffset);

    // C function void glVertexAttribIFormat ( GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset )

    CARAPI GlVertexAttribIFormat(
        /* [in] */ Int32 attribindex,
        /* [in] */ Int32 size,
        /* [in] */ Int32 type,
        /* [in] */ Int32 relativeoffset);

    // C function void glVertexAttribBinding ( GLuint attribindex, GLuint bindingindex )

    CARAPI GlVertexAttribBinding(
        /* [in] */ Int32 attribindex,
        /* [in] */ Int32 bindingindex);

    // C function void glVertexBindingDivisor ( GLuint bindingindex, GLuint divisor )

    CARAPI GlVertexBindingDivisor(
        /* [in] */ Int32 bindingindex,
        /* [in] */ Int32 divisor);

};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OPENGL_CGLES30_H__