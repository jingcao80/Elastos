
#ifndef __ELASTOS_DROID_GRAPHICS_SHADER_H__
#define __ELASTOS_DROID_GRAPHICS_SHADER_H__

#include "Elastos.Droid.Graphics.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Graphics {

extern "C" const InterfaceID EIID_Shader;

/**
 * Shader is the based class for objects that return horizontal spans of colors
 * during drawing. A subclass of Shader is installed in a Paint calling
 * paint.setShader(shader). After that any object (other than a bitmap) that is
 * drawn with that paint will get its color(s) from the shader.
 */
class Shader
    : public Object
    , public IShader
{
public:
    CAR_INTERFACE_DECL()

    virtual ~Shader();

    /**
     * Return true if the shader has a non-identity local matrix.
     * @param localM If not null, it is set to the shader's local matrix.
     * @return true if the shader has a non-identity local matrix
     */
    virtual CARAPI GetLocalMatrix(
        /* [in, out] */ IMatrix* localM,
        /* [out] */ Boolean* has);

    /**
     * Set the shader's local matrix. Passing null will reset the shader's
     * matrix to identity
     * @param localM The shader's new local matrix, or null to specify identity
     */
    virtual CARAPI SetLocalMatrix(
        /* [in] */ IMatrix* localM);

    /**
     * @hide
     */
    virtual CARAPI Copy(
        /* [out] */ IShader** shader);

protected:
    /**
     * Initialization step that should be called by subclasses in their
     * constructors. Calling again may result in memory leaks.
     * @hide
     */
    virtual CARAPI_(void) Init(
        /* [in] */ Int64 ni);

    /**
     * @hide
     */
    virtual CARAPI_(void) CopyLocalMatrix(
        /* [in] */ IShader* dest);

    /* package */ CARAPI_(Int64) GetNativeInstance();

private:
    static CARAPI_(void) NativeDestructor(
        /* [in] */ Int64 shader);

    static CARAPI_(void) NativeSetLocalMatrix(
        /* [in] */ Int64 shader,
        /* [in] */ Int64 matrix);

public:
    Int64 mNativeInstance;

private:
    AutoPtr<IMatrix> mLocalMatrix;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_SHADER_H__
