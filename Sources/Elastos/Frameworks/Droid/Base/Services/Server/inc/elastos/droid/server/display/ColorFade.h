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

#ifndef __ELASTOS_DROID_SERVER_DISPLAY_COLOR_FADE_H__
#define __ELASTOS_DROID_SERVER_DISPLAY_COLOR_FADE_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <Elastos.Droid.Hardware.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Hardware::Display::IDisplayManagerInternal;
using Elastos::Droid::Hardware::Display::IDisplayTransactionListener;
using Elastos::Droid::Opengl::IEGLConfig;
using Elastos::Droid::Opengl::IEGLContext;
using Elastos::Droid::Opengl::IEGLDisplay;
using Elastos::Droid::Opengl::IEGLSurface;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::ISurfaceControl;
using Elastos::Droid::View::ISurfaceSession;

using Elastos::IO::IPrintWriter;
using Elastos::IO::IFloatBuffer;


namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

/**
 * <p>
 * Animates a screen transition from on to off or off to on by applying
 * some GL transformations to a screenshot.
 * </p><p>
 * This component must only be created or accessed by the {@link Looper} thread
 * that belongs to the {@link DisplayPowerController}.
 * </p>
 */
class ColorFade
    : public Object
    , public IColorFade
{
private:
    /**
     * Keeps a surface aligned with the natural orientation of the device.
     * Updates the position and transformation of the matrix whenever the display
     * is rotated.  This is a little tricky because the display transaction
     * callback can be invoked on any thread, not necessarily the thread that
     * owns the color fade.
     */
    class NaturalSurfaceLayout
        : public Object
        , public IDisplayTransactionListener
    {
    public:
        CAR_INTERFACE_DECL()

        NaturalSurfaceLayout(
            /* [in ]*/ IDisplayManagerInternal* displayManagerInternal,
            /* [in ]*/ Int32 displayId,
            /* [in ]*/ ISurfaceControl* surfaceControl);

        virtual ~NaturalSurfaceLayout();

        void Dispose();

        // @Override
        CARAPI OnDisplayTransaction();

    private:
        AutoPtr<IDisplayManagerInternal> mDisplayManagerInternal;
        Int32 mDisplayId;
        AutoPtr<ISurfaceControl> mSurfaceControl;
    };

public:
    CAR_INTERFACE_DECL();

    ColorFade(
        /* [in] */ Int32 displayId);

    /**
     * Warms up the color fade in preparation for turning on or off.
     * This method prepares a GL context, and captures a screen shot.
     *
     * @param mode The desired mode for the upcoming animation.
     * @return True if the color fade is ready, false if it is uncontrollable.
     */
    Boolean Prepare(
        /* [in] */ IContext* context,
        /* [in] */ Int32 mode);

    /**
     * Dismisses the color fade animation surface and cleans up.
     *
     * To prevent stray photons from leaking out after the color fade has been
     * turned off, it is a good idea to defer dismissing the animation until the
     * color fade has been turned back on fully.
     */
    void Dismiss();

    /**
     * Draws an animation frame showing the color fade activated at the
     * specified level.
     *
     * @param level The color fade level.
     * @return True if successful.
     */
    Boolean Draw(
        /* [in] */ Float level);

    void Dump(
        /* [in] */ IPrintWriter* pw);

private:
    CARAPI ReadFile(
        /* [in] */ IContext* context,
        /* [in] */ Int32 resourceId,
        /* [out] */ String* result);

    Int32 LoadShader(
        /* [in] */ IContext* context,
        /* [in] */ Int32 resourceId,
        /* [in] */ Int32 type);

    Boolean InitGLShaders(
        /* [in] */ IContext* context);

    void DestroyGLShaders();

    Boolean InitGLBuffers();

    void DestroyGLBuffers();

    static void SetQuad(
        /* [in] */ IFloatBuffer* vtx,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float w,
        /* [in] */ Float h);

    void DrawFaded(
        /* [in] */ Float opacity,
        /* [in] */ Float gamma,
        /* [in] */ Float saturation,
        /* [in] */ Float scale);

    void Ortho(
        /* [in] */ Float left,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Float top,
        /* [in] */ Float znear,
        /* [in] */ Float zfar);

    Boolean CaptureScreenshotTextureAndSetViewport();

    void DestroyScreenshotTexture();

    Boolean CreateEglContext();

    Boolean CreateSurface();

    Boolean CreateEglSurface();

    void DestroyEglSurface();

    void DestroySurface();

    Boolean ShowSurface(
        /* [in] */ Float alpha);

    Boolean AttachEglContext();

    void DetachEglContext();

    static AutoPtr<IFloatBuffer> CreateNativeFloatBuffer(
        /* [in] */ Int32 size);

    static void LogEglError(
        /* [in] */ const String& func);

    static Boolean CheckGlErrors(
        /* [in] */ const String& func);

    static Boolean CheckGlErrors(
        /* [in] */ const String& func,
        /* [in] */ Boolean log);

public:

    /**
     * Animates an color fade warming up.
     */
    static const Int32 MODE_WARM_UP;

    /**
     * Animates an color fade shutting off.
     */
    static const Int32 MODE_COOL_DOWN;

    /**
     * Animates a simple dim layer to fade the contents of the screen in or out progressively.
     */
    static const Int32 MODE_FADE;

private:

    static const String TAG;

    static const Boolean DEBUG;

    // The layer for the electron beam surface.
    // This is currently hardcoded to be one layer above the boot animation.
    static const Int32 COLOR_FADE_LAYER;

    // The number of frames to draw when preparing the animation so that it will
    // be ready to run smoothly.  We use 3 frames because we are triple-buffered.
    // See code for details.
    static const Int32 DEJANK_FRAMES;

    Int32 mDisplayId;

    // Set to true when the animation context has been fully prepared.
    Boolean mPrepared;
    Int32 mMode;

    AutoPtr<IDisplayManagerInternal> mDisplayManagerInternal;
    Int32 mDisplayLayerStack; // layer stack associated with primary display
    Int32 mDisplayWidth;      // real width, not rotated
    Int32 mDisplayHeight;     // real height, not rotated
    AutoPtr<ISurfaceSession> mSurfaceSession;
    AutoPtr<ISurfaceControl> mSurfaceControl;
    AutoPtr<ISurface> mSurface;
    AutoPtr<NaturalSurfaceLayout> mSurfaceLayout;
    AutoPtr<IEGLDisplay> mEglDisplay;
    AutoPtr<IEGLConfig> mEglConfig;
    AutoPtr<IEGLContext> mEglContext;
    AutoPtr<IEGLSurface> mEglSurface;
    Boolean mSurfaceVisible;
    Float mSurfaceAlpha;

    // Texture names.  We only use one texture, which contains the screenshot.
    AutoPtr<ArrayOf<Int32> > mTexNames;
    Boolean mTexNamesGenerated;
    AutoPtr<ArrayOf<Float> > mTexMatrix;
    AutoPtr<ArrayOf<Float> > mProjMatrix;
    AutoPtr<ArrayOf<Int32> > mGLBuffers;
    Int32 mTexCoordLoc, mVertexLoc, mTexUnitLoc, mProjMatrixLoc, mTexMatrixLoc;
    Int32 mOpacityLoc, mScaleLoc, mGammaLoc, mSaturationLoc;
    Int32 mProgram;

    // Vertex and corresponding texture coordinates.
    // We have 4 2D vertices, so 8 elements.  The vertices form a quad.
    AutoPtr<IFloatBuffer> mVertexBuffer;
    AutoPtr<IFloatBuffer> mTexCoordBuffer;
};

} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_DISPLAY_COLOR_FADE_H__
