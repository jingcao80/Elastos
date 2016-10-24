
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_SURFACETEXTURERENDERER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_SURFACETEXTURERENDERER_H__

#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Opengl.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::View::ISurface;
using Elastos::Droid::Graphics::ISurfaceTexture;
using Elastos::Core::Object;
using Elastos::IO::IByteBuffer;
using Elastos::IO::IFloatBuffer;
using Elastos::Utility::IList;
using Elastos::Utility::ICollection;
using Elastos::Droid::Opengl::IEGLConfig;
using Elastos::Droid::Opengl::IEGLSurface;
using Elastos::Droid::Opengl::IEGLDisplay;
using Elastos::Droid::Opengl::IEGLContext;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

class SurfaceTextureRenderer
    : public Object
    , public ISurfaceTextureRenderer
{
private:
    class EGLSurfaceHolder
        : public Object
    {
    public:
        AutoPtr<ISurface> mSurface;
        AutoPtr<IEGLSurface> mEglSurface;
        Int32 mWidth;
        Int32 mHeight;
    };

public:
    CAR_INTERFACE_DECL()

    SurfaceTextureRenderer();

    virtual ~SurfaceTextureRenderer() {}

    CARAPI constructor(
        /* [in] */ Int32 facing);

    /**
     * Return the surface texture to draw to - this is the texture use to when producing output
     * surface buffers.
     *
     * @return a {@link SurfaceTexture}.
     */
    CARAPI GetSurfaceTexture(
        /* [out] */ ISurfaceTexture** outst);

    /**
     * Set a collection of output {@link Surface}s that can be drawn to.
     *
     * @param surfaces a {@link Collection} of surfaces.
     */
    CARAPI ConfigureSurfaces(
        /* [in] */ ICollection* surfaces);

    /**
     * Draw the current buffer in the {@link SurfaceTexture} returned from
     * {@link #getSurfaceTexture()} into the set of target {@link Surface}s
     * in the next request from the given {@link CaptureCollector}, or drop
     * the frame if none is available.
     *
     * <p>
     * Any {@link Surface}s targeted must be a subset of the {@link Surface}s
     * set in the last {@link #configureSurfaces(java.util.Collection)} call.
     * </p>
     *
     * @param targetCollector the surfaces to draw to.
     */
    CARAPI DrawIntoSurfaces(
        /* [in] */ ICaptureCollector* targetCollector);

    /**
     * Clean up the current GL context.
     */
    CARAPI CleanupEGLContext();

    /**
     * Drop all current GL operations on the floor.
     */
    CARAPI Flush();

private:
    CARAPI LoadShader(
        /* [in] */ Int32 shaderType,
        /* [in] */ const String& source,
        /* [out] */ Int32* result);

    CARAPI CreateProgram(
        /* [in] */ const String& vertexSource,
        /* [in] */ const String& fragmentSource,
        /* [out] */ Int32* result);

    CARAPI DrawFrame(
        /* [in] */ ISurfaceTexture* st,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    /**
     * Initializes GL state.  Call this after the EGL surface has been created and made current.
     */
    CARAPI InitializeGLState();

    CARAPI_(Int32) GetTextureId();

    CARAPI_(void) ClearState();

    CARAPI ConfigureEGLContext();

    CARAPI ConfigureEGLOutputSurfaces(
        /* [in] */ ICollection* surfaces);

    CARAPI ConfigureEGLPbufferSurfaces(
        /* [in] */ ICollection* surfaces);

    CARAPI_(void) ReleaseEGLContext();

    CARAPI MakeCurrent(
        /* [in] */ IEGLSurface* surface);

    CARAPI SwapBuffers(
        /* [in] */ IEGLSurface* surface,
        /* [out] */ Boolean* result);

    CARAPI CheckEglError(
        /* [in] */ const String& msg);

    CARAPI CheckGlError(
        /* [in] */ const String& msg);

    /**
     * Save a measurement dump to disk, in
     * {@code /sdcard/CameraLegacy/durations_<time>_<width1>x<height1>_...txt}
     */
    CARAPI_(void) DumpGlTiming();

    CARAPI_(void) SetupGlTiming();

    CARAPI_(void) BeginGlTiming();

    CARAPI_(void) AddGlTimestamp(
        /* [in] */ Int64 timestamp);

    CARAPI_(void) EndGlTiming();

private:
    static const String TAG;
    static const Boolean DEBUG;
    static const Int32 GL_MATRIX_SIZE;
    static const Int32 VERTEX_POS_SIZE;
    static const Int32 VERTEX_UV_SIZE;
    static const Int32 EGL_COLOR_BITLENGTH;
    static const Int32 GLES_VERSION;
    static const Int32 PBUFFER_PIXEL_BYTES;

    AutoPtr<IEGLDisplay> mEGLDisplay;
    AutoPtr<IEGLContext> mEGLContext;
    AutoPtr<IEGLConfig> mConfigs;

    AutoPtr<IList> mSurfaces;
    AutoPtr<IList> mConversionSurfaces;

    AutoPtr<IByteBuffer> mPBufferPixels;

    // Hold this to avoid GC
    /*volatile*/ AutoPtr<ISurfaceTexture> mSurfaceTexture;

    static const Int32 FLOAT_SIZE_BYTES;
    static const Int32 TRIANGLE_VERTICES_DATA_STRIDE_BYTES;
    static const Int32 TRIANGLE_VERTICES_DATA_POS_OFFSET;
    static const Int32 TRIANGLE_VERTICES_DATA_UV_OFFSET;

    // Sampling is mirrored across the vertical axis to undo horizontal flip from the front camera
    static const AutoPtr<ArrayOf<Float> > sFrontCameraTriangleVertices;

    // Sampling is 1:1 for a straight copy for the back camera
    static const AutoPtr<ArrayOf<Float> > sBackCameraTriangleVertices;

    AutoPtr<IFloatBuffer> mTriangleVertices;

    /**
     * As used in this file, this vertex shader maps a unit square to the view, and
     * tells the fragment shader to interpolate over it.  Each surface pixel position
     * is mapped to a 2D homogeneous texture coordinate of the form (s, t, 0, 1) with
     * s and t in the inclusive range [0, 1], and the matrix from
     * {@link SurfaceTexture#getTransformMatrix(float[])} is used to map this
     * coordinate to a texture location.
     */
    static const String VERTEX_SHADER;

    /**
     * This fragment shader simply draws the color in the 2D texture at
     * the location from the {@code VERTEX_SHADER}.
     */
    static const String FRAGMENT_SHADER;

    AutoPtr<ArrayOf<Float> > mMVPMatrix;
    AutoPtr<ArrayOf<Float> > mSTMatrix;

    Int32 mProgram;
    Int32 mTextureID;
    Int32 muMVPMatrixHandle;
    Int32 muSTMatrixHandle;
    Int32 maPositionHandle;
    Int32 maTextureHandle;

    AutoPtr<IPerfMeasurement> mPerfMeasurer;
    static const String LEGACY_PERF_PROPERTY;
};

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_SURFACETEXTURERENDERER_H__
