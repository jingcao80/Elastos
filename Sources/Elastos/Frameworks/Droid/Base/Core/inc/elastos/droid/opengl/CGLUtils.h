
#ifndef __ELASTOS_DROID_OPENGL_CGLUTILS_H__
#define __ELASTOS_DROID_OPENGL_CGLUTILS_H__

#include "_Elastos_Droid_Opengl_CGLUtils.h"

using Elastos::Droid::Graphics::IBitmap;

namespace Elastos {
namespace Droid {
namespace Opengl {

CarClass(CGLUtils)
{
public:
    CARAPI GetInternalFormat(
        /* [in] */ IBitmap* bitmap,
        /* [out] */ Int32* rst);

    CARAPI GetType(
        /* [in] */ IBitmap* bitmap,
        /* [out] */ Int32* type);

    CARAPI TexImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 internalformat,
        /* [in] */ IBitmap* bitmap,
        /* [in] */ Int32 border);

    CARAPI TexImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 internalformat,
        /* [in] */ IBitmap* bitmap,
        /* [in] */ Int32 type,
        /* [in] */ Int32 border);

    CARAPI TexImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ IBitmap* bitmap,
        /* [in] */ Int32 border);

    CARAPI TexSubImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 xoffset,
        /* [in] */ Int32 yoffset,
        /* [in] */ IBitmap* bitmap);

    CARAPI TexSubImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 xoffset,
        /* [in] */ Int32 yoffset,
        /* [in] */ IBitmap* bitmap,
        /* [in] */ Int32 format,
        /* [in] */ Int32 type);

    CARAPI GetEGLErrorString(
        /* [in] */ Int32 error,
        /* [out] */ String* str);

    CARAPI EnableTracing();

private:
    static CARAPI_(Int32) Native_getInternalFormat(
        /* [in] */ IBitmap* bitmap);

    static CARAPI_(Int32) Native_getType(
        /* [in] */ IBitmap* bitmap);

    static CARAPI_(Int32) Native_texImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 internalformat,
        /* [in] */ IBitmap* bitmap,
        /* [in] */ Int32 type,
        /* [in] */ Int32 border);

    static CARAPI_(Int32) Native_texSubImage2D(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 xoffset,
        /* [in] */ Int32 yoffset,
        /* [in] */ IBitmap* bitmap,
        /* [in] */ Int32 format,
        /* [in] */ Int32 type);

    static CARAPI_(void) Native_enableTracing();
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OPENGL_CGLUTILS_H__
