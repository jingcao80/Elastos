
#include <Elastos.Droid.Opengl.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Graphics.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Libcore.h>
#include "elastos/droid/server/display/ColorFade.h"
#include "elastos/droid/server/LocalServices.h"
#include <elastos/droid/utility/FloatMath.h>
#include <elastos/droid/R.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>

using Elastos::Droid::R;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::ISurfaceTexture;
using Elastos::Droid::Graphics::CSurfaceTexture;
using Elastos::Droid::Hardware::Display::EIID_IDisplayManagerInternal;
using Elastos::Droid::Hardware::Display::EIID_IDisplayTransactionListener;
using Elastos::Droid::Opengl::IEGL14;
using Elastos::Droid::Opengl::IGLES20;
using Elastos::Droid::Opengl::IGLES11Ext;
using Elastos::Droid::Utility::FloatMath;
using Elastos::Droid::View::IDisplayInfo;
using Elastos::Droid::View::CSurface;
using Elastos::Droid::View::CSurfaceControl;
using Elastos::Droid::View::ISurfaceControlHelper;
using Elastos::Droid::View::CSurfaceControlHelper;
using Elastos::Droid::View::CSurfaceSession;

using Elastos::IO::IInputStream;
using Elastos::IO::IReader;
using Elastos::IO::IInputStreamReader;
using Elastos::IO::CInputStreamReader;
using Elastos::IO::IBuffer;
using Elastos::IO::IByteBuffer;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::CByteBufferHelper;
using Elastos::IO::ByteOrder;
using Elastos::IO::IByteOrderHelper;
using Elastos::IO::CByteOrderHelper;
using Elastos::Utility::Logging::Slogger;
using Libcore::IO::IStreams;
using Libcore::IO::CStreams;


namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

//=====================================================================
// ColorFade::NaturalSurfaceLayout
//=====================================================================

CAR_INTERFACE_IMPL(ColorFade::NaturalSurfaceLayout, Object, IDisplayTransactionListener)

ColorFade::NaturalSurfaceLayout::NaturalSurfaceLayout(
    /* [in ]*/ IDisplayManagerInternal* displayManagerInternal,
    /* [in ]*/ Int32 displayId,
    /* [in ]*/ ISurfaceControl* surfaceControl)
    : mDisplayId(0)
{
    mDisplayManagerInternal = displayManagerInternal;
    mDisplayId = displayId;
    mSurfaceControl = surfaceControl;
    mDisplayManagerInternal->RegisterDisplayTransactionListener(this);
}

ColorFade::NaturalSurfaceLayout::~NaturalSurfaceLayout()
{
    Dispose();
}

void ColorFade::NaturalSurfaceLayout::Dispose()
{
    synchronized(this) {
        mSurfaceControl = NULL;
    }
    mDisplayManagerInternal->UnregisterDisplayTransactionListener(this);
}

ECode ColorFade::NaturalSurfaceLayout::OnDisplayTransaction()
{
    synchronized(this) {
        if (mSurfaceControl == NULL) {
            return NOERROR;
        }

        AutoPtr<IDisplayInfo> displayInfo;
        mDisplayManagerInternal->GetDisplayInfo(mDisplayId, (IDisplayInfo**)&displayInfo);
        Int32 rotation, lw, lh;
        displayInfo->GetRotation(&rotation);
        displayInfo->GetLogicalWidth(&lw);
        displayInfo->GetLogicalHeight(&lh);
        switch (rotation) {
            case ISurface::ROTATION_0:
                mSurfaceControl->SetPosition(0, 0);
                mSurfaceControl->SetMatrix(1, 0, 0, 1);
                break;
            case ISurface::ROTATION_90:
                mSurfaceControl->SetPosition(0, lh);
                mSurfaceControl->SetMatrix(0, -1, 1, 0);
                break;
            case ISurface::ROTATION_180:
                mSurfaceControl->SetPosition(lw, lh);
                mSurfaceControl->SetMatrix(-1, 0, 0, -1);
                break;
            case ISurface::ROTATION_270:
                mSurfaceControl->SetPosition(lw, 0);
                mSurfaceControl->SetMatrix(0, 1, -1, 0);
                break;
        }
    }
    return NOERROR;
}

//=====================================================================
// ColorFade
//=====================================================================

const Int32 ColorFade::MODE_WARM_UP = 0;
const Int32 ColorFade::MODE_COOL_DOWN = 1;
const Int32 ColorFade::MODE_FADE = 2;
const String ColorFade::TAG("ColorFade");
const Boolean ColorFade::DEBUG = FALSE;
const Int32 ColorFade::COLOR_FADE_LAYER = 0x40000001;
const Int32 ColorFade::DEJANK_FRAMES = 3;

ColorFade::ColorFade(
    /* [in] */ Int32 displayId)
    : mPrepared(FALSE)
    , mMode(0)
    , mDisplayLayerStack(0)
    , mDisplayWidth(0)
    , mDisplayHeight(0)
    , mSurfaceVisible(FALSE)
    , mSurfaceAlpha(0)
    , mTexNamesGenerated(FALSE)
    , mTexCoordLoc(0)
    , mVertexLoc(0)
    , mTexUnitLoc(0)
    , mProjMatrixLoc(0)
    , mTexMatrixLoc(0)
    , mOpacityLoc(0)
    , mScaleLoc(0)
    , mGammaLoc(0)
    , mSaturationLoc(0)
    , mProgram(0)
{
    mDisplayId = displayId;
    AutoPtr<IInterface> obj = LocalServices::GetService(EIID_IDisplayManagerInternal);
    mDisplayManagerInternal = IDisplayManagerInternal::Probe(obj);

    mTexNames = ArrayOf<Int32>::Alloc(1);
    mTexMatrix = ArrayOf<Float>::Alloc(16);
    mProjMatrix = ArrayOf<Float>::Alloc(16);
    mGLBuffers = ArrayOf<Int32>::Alloc(2);

    mVertexBuffer = CreateNativeFloatBuffer(8);
    mTexCoordBuffer = CreateNativeFloatBuffer(8);
}

Boolean ColorFade::Prepare(
    /* [in] */ IContext* context,
    /* [in] */ Int32 mode)
{
    if (DEBUG) {
        Slogger::D(TAG, "prepare: mode=%d", mode);
    }

    mMode = mode;

    // Get the display size and layer stack.
    // This is not expected to change while the color fade surface is showing.
    AutoPtr<IDisplayInfo> displayInfo;
    mDisplayManagerInternal->GetDisplayInfo(mDisplayId, (IDisplayInfo**)&displayInfo);
    displayInfo->GetLayerStack(&mDisplayLayerStack);
    displayInfo->GetNaturalWidth(&mDisplayWidth);
    displayInfo->GetNaturalHeight(&mDisplayHeight);

    // Prepare the surface for drawing.
    if (!(CreateSurface() && CreateEglContext() && CreateEglSurface() &&
          CaptureScreenshotTextureAndSetViewport())) {
        Dismiss();
        return FALSE;
    }

    // Init GL
    if (!AttachEglContext()) {
        return FALSE;
    }
    // try {
        if(!InitGLShaders(context) || !InitGLBuffers() || CheckGlErrors(String("prepare"))) {
            DetachEglContext();
            Dismiss();
            DetachEglContext();
            return FALSE;
        }
    // } finally {
        DetachEglContext();
    // }

    // Done.
    mPrepared = TRUE;

    // Dejanking optimization.
    // Some GL drivers can introduce a lot of lag in the first few frames as they
    // initialize their state and allocate graphics buffers for rendering.
    // Work around this problem by rendering the first frame of the animation a few
    // times.  The rest of the animation should run smoothly thereafter.
    // The frames we draw here aren't visible because we are essentially just
    // painting the screenshot as-is.
    if (mode == MODE_COOL_DOWN) {
        for (Int32 i = 0; i < DEJANK_FRAMES; i++) {
            Draw(1.0f);
        }
    }
    return TRUE;
}

ECode ColorFade::ReadFile(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resourceId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String(NULL);

    // try{
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IInputStream> stream;
    res->OpenRawResource(resourceId, (IInputStream**)&stream);
    AutoPtr<IStreams> streams;
    CStreams::AcquireSingleton((IStreams**)&streams);
    AutoPtr<IReader> reader;
    CInputStreamReader::New(stream, (IReader**)&reader);
    String str;
    ECode ec = streams->ReadFully(reader, &str);
    if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::E(TAG, "Unrecognized shader %d", resourceId);
        return E_RUNTIME_EXCEPTION;
    }

    *result = str;
    return NOERROR;
}

Int32 ColorFade::LoadShader(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resourceId,
    /* [in] */ Int32 type)
{
    String source;
    ReadFile(context, resourceId, &source);

    AutoPtr<IGLES20> gles20;
    // CGLES20::AcquireSingleton((IGLES20**)&gles20);
    Int32 shader;
    gles20->GlCreateShader(type, &shader);

    gles20->GlShaderSource(shader, source);
    gles20->GlCompileShader(shader);

    AutoPtr<ArrayOf<Int32> > compiled = ArrayOf<Int32>::Alloc(1);
    gles20->GlGetShaderiv(shader, IGLES20::_GL_COMPILE_STATUS, compiled, 0);
    if ((*compiled)[0] == 0) {
        String str;
        Slogger::E(TAG, "Could not compile shader %d, %d :", shader, type);
        gles20->GlGetShaderSource(shader, &str);
        Slogger::E(TAG, str.string());
        gles20->GlGetShaderInfoLog(shader, &str);
        Slogger::E(TAG, str.string());
        gles20->GlDeleteShader(shader);
        shader = 0;
    }

    return shader;
}

Boolean ColorFade::InitGLShaders(
    /* [in] */ IContext* context)
{
    AutoPtr<IGLES20> gles20;
    // CGLES20::AcquireSingleton((IGLES20**)&gles20);
    Int32 vshader = LoadShader(context, R::raw::color_fade_vert,
            IGLES20::_GL_VERTEX_SHADER);
    Int32 fshader = LoadShader(context, R::raw::color_fade_frag,
            IGLES20::_GL_FRAGMENT_SHADER);
    gles20->GlReleaseShaderCompiler();
    if (vshader == 0 || fshader == 0) return FALSE;

    gles20->GlCreateProgram(&mProgram);

    gles20->GlAttachShader(mProgram, vshader);
    gles20->GlAttachShader(mProgram, fshader);
    gles20->GlDeleteShader(vshader);
    gles20->GlDeleteShader(fshader);

    gles20->GlLinkProgram(mProgram);

    gles20->GlGetAttribLocation(mProgram, String("position"), &mVertexLoc);
    gles20->GlGetAttribLocation(mProgram, String("uv"), &mTexCoordLoc);

    gles20->GlGetUniformLocation(mProgram, String("proj_matrix"), &mProjMatrixLoc);
    gles20->GlGetUniformLocation(mProgram, String("tex_matrix"), &mTexMatrixLoc);

    gles20->GlGetUniformLocation(mProgram, String("opacity"), &mOpacityLoc);
    gles20->GlGetUniformLocation(mProgram, String("gamma"), &mGammaLoc);
    gles20->GlGetUniformLocation(mProgram, String("saturation"), &mSaturationLoc);
    gles20->GlGetUniformLocation(mProgram, String("scale"), &mScaleLoc);
    gles20->GlGetUniformLocation(mProgram, String("texUnit"), &mTexUnitLoc);

    gles20->GlUseProgram(mProgram);
    gles20->GlUniform1i(mTexUnitLoc, 0);
    gles20->GlUseProgram(0);

    return TRUE;
}

void ColorFade::DestroyGLShaders()
{
    AutoPtr<IGLES20> gles20;
    // CGLES20::AcquireSingleton((IGLES20**)&gles20);
    gles20->GlDeleteProgram(mProgram);
    CheckGlErrors(String("glDeleteProgram"));
}

Boolean ColorFade::InitGLBuffers()
{
    //Fill vertices
    SetQuad(mVertexBuffer, 0, 0, mDisplayWidth, mDisplayHeight);

    AutoPtr<IGLES20> gles20;
    // CGLES20::AcquireSingleton((IGLES20**)&gles20);

    // Setup GL Textures
    gles20->GlBindTexture(IGLES11Ext::_GL_TEXTURE_EXTERNAL_OES, (*mTexNames)[0]);
    gles20->GlTexParameteri(IGLES11Ext::_GL_TEXTURE_EXTERNAL_OES,
        IGLES20::_GL_TEXTURE_MAG_FILTER, IGLES20::_GL_NEAREST);
    gles20->GlTexParameteri(IGLES11Ext::_GL_TEXTURE_EXTERNAL_OES,
        IGLES20::_GL_TEXTURE_MIN_FILTER, IGLES20::_GL_NEAREST);
    gles20->GlTexParameteri(IGLES11Ext::_GL_TEXTURE_EXTERNAL_OES,
        IGLES20::_GL_TEXTURE_WRAP_S, IGLES20::_GL_CLAMP_TO_EDGE);
    gles20->GlTexParameteri(IGLES11Ext::_GL_TEXTURE_EXTERNAL_OES,
        IGLES20::_GL_TEXTURE_WRAP_T, IGLES20::_GL_CLAMP_TO_EDGE);
    gles20->GlBindTexture(IGLES11Ext::_GL_TEXTURE_EXTERNAL_OES, 0);

    // Setup GL Buffers
    gles20->GlGenBuffers(2, mGLBuffers, 0);

    // fill vertex buffer
    gles20->GlBindBuffer(IGLES20::_GL_ARRAY_BUFFER, (*mGLBuffers)[0]);
    Int32 capacity;
    IBuffer* buffer = IBuffer::Probe(mVertexBuffer);
    buffer->GetCapacity(&capacity);
    gles20->GlBufferData(IGLES20::_GL_ARRAY_BUFFER, capacity * 4,
        buffer, IGLES20::_GL_STATIC_DRAW);

    // fill tex buffer
    gles20->GlBindBuffer(IGLES20::_GL_ARRAY_BUFFER, (*mGLBuffers)[1]);
    buffer = IBuffer::Probe(mTexCoordBuffer);
    gles20->GlBufferData(IGLES20::_GL_ARRAY_BUFFER, capacity * 4,
        buffer, IGLES20::_GL_STATIC_DRAW);

    gles20->GlBindBuffer(IGLES20::_GL_ARRAY_BUFFER, 0);

    return TRUE;
}

void ColorFade::DestroyGLBuffers()
{
    AutoPtr<IGLES20> gles20;
    // CGLES20::AcquireSingleton((IGLES20**)&gles20);
    gles20->GlDeleteBuffers(2, mGLBuffers, 0);
    CheckGlErrors(String("glDeleteBuffers"));
}

void ColorFade::SetQuad(
    /* [in] */ IFloatBuffer* vtx,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float w,
    /* [in] */ Float h)
{
    if (DEBUG) {
        Slogger::D(TAG, "SetQuad: x=%f, y=%f, w=%f, h=%f", x, y, w, h);
    }
    vtx->Put(0, x);
    vtx->Put(1, y);
    vtx->Put(2, x);
    vtx->Put(3, y + h);
    vtx->Put(4, x + w);
    vtx->Put(5, y + h);
    vtx->Put(6, x + w);
    vtx->Put(7, y);
}

void ColorFade::Dismiss()
{
    if (DEBUG) {
        Slogger::D(TAG, "dismiss");
    }

    if (mPrepared) {
        AttachEglContext();
        // try {
            DestroyScreenshotTexture();
            DestroyGLShaders();
            DestroyGLBuffers();
            DestroyEglSurface();
        // } finally {
            DetachEglContext();
        // }
        DestroySurface();

        AutoPtr<IGLES20> gles20;
        // CGLES20::AcquireSingleton((IGLES20**)&gles20);
        gles20->GlFlush();
        mPrepared = FALSE;
    }
}

Boolean ColorFade::Draw(
    /* [in] */ Float level)
{
    if (DEBUG) {
        Slogger::D(TAG, "drawFrame: level=%f", level);
    }

    if (!mPrepared) {
        return FALSE;
    }

    if (mMode == MODE_FADE) {
        return ShowSurface(1.0f - level);
    }

    if (!AttachEglContext()) {
        return FALSE;
    }
    // try {
        AutoPtr<IGLES20> gles20;
        // CGLES20::AcquireSingleton((IGLES20**)&gles20);
        // Clear frame to solid black.
        gles20->GlClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        gles20->GlClear(IGLES20::_GL_COLOR_BUFFER_BIT);

        // Draw the frame.
        Float one_minus_level = 1 - level;
        Float cos = FloatMath::Cos((Float)Elastos::Core::Math::PI * one_minus_level);
        Float sign = cos < 0 ? -1 : 1;
        Float opacity = -FloatMath::Pow(one_minus_level, 2) + 1;
        Float saturation = FloatMath::Pow(level, 4);
        Float scale = (-FloatMath::Pow(one_minus_level, 2) + 1) * 0.1f + 0.9f;
        Float gamma = (0.5f * sign * FloatMath::Pow(cos, 2) + 0.5f) * 0.9f + 0.1f;
        DrawFaded(opacity, 1.0f / gamma, saturation, scale);
        if (CheckGlErrors(String("drawFrame"))) {
            return FALSE;
        }

        AutoPtr<IEGL14> egl14;
        // CEGL14::AcquireSingleton((IEGL14**)&egl14);
        Boolean bval;
        egl14->EglSwapBuffers(mEglDisplay, mEglSurface, &bval);
    // } finally {
        DetachEglContext();
    // }
    return ShowSurface(1.0f);
}

void ColorFade::DrawFaded(
    /* [in] */ Float opacity,
    /* [in] */ Float gamma,
    /* [in] */ Float saturation,
    /* [in] */ Float scale)
{
    if (DEBUG) {
        Slogger::D(TAG, "DrawFaded: opacity=%f, gamma=%f, saturation=%f, scale=%f",
            opacity, gamma, saturation, scale);
    }

    AutoPtr<IGLES20> gles20;
    // CGLES20::AcquireSingleton((IGLES20**)&gles20);

    // Use shaders
    gles20->GlUseProgram(mProgram);

    // Set Uniforms
    gles20->GlUniformMatrix4fv(mProjMatrixLoc, 1, FALSE, mProjMatrix, 0);
    gles20->GlUniformMatrix4fv(mTexMatrixLoc, 1, FALSE, mTexMatrix, 0);
    gles20->GlUniform1f(mOpacityLoc, opacity);
    gles20->GlUniform1f(mGammaLoc, gamma);
    gles20->GlUniform1f(mSaturationLoc, saturation);
    gles20->GlUniform1f(mScaleLoc, scale);

    // Use textures
    gles20->GlActiveTexture(IGLES20::_GL_TEXTURE0);
    gles20->GlBindTexture(IGLES11Ext::_GL_TEXTURE_EXTERNAL_OES, (*mTexNames)[0]);

    // draw the plane
    gles20->GlBindBuffer(IGLES20::_GL_ARRAY_BUFFER, (*mGLBuffers)[0]);
    gles20->GlEnableVertexAttribArray(mVertexLoc);
    gles20->GlVertexAttribPointer(mVertexLoc, 2, IGLES20::_GL_FLOAT, FALSE, 0, 0);

    gles20->GlBindBuffer(IGLES20::_GL_ARRAY_BUFFER, (*mGLBuffers)[1]);
    gles20->GlEnableVertexAttribArray(mTexCoordLoc);
    gles20->GlVertexAttribPointer(mTexCoordLoc, 2, IGLES20::_GL_FLOAT, FALSE, 0, 0);

    gles20->GlDrawArrays(IGLES20::_GL_TRIANGLE_FAN, 0, 4);

    // clean up
    gles20->GlBindTexture(IGLES11Ext::_GL_TEXTURE_EXTERNAL_OES, 0);
    gles20->GlBindBuffer(IGLES20::_GL_ARRAY_BUFFER, 0);
}

void ColorFade::Ortho(
    /* [in] */ Float left,
    /* [in] */ Float right,
    /* [in] */ Float bottom,
    /* [in] */ Float top,
    /* [in] */ Float znear,
    /* [in] */ Float zfar)
{
    (*mProjMatrix)[0] = 2.0f / (right - left);
    (*mProjMatrix)[1] = 0;
    (*mProjMatrix)[2] = 0;
    (*mProjMatrix)[3] = 0;
    (*mProjMatrix)[4] = 0;
    (*mProjMatrix)[5] = 2.0f / (top - bottom);
    (*mProjMatrix)[6] = 0;
    (*mProjMatrix)[7] = 0;
    (*mProjMatrix)[8] = 0;
    (*mProjMatrix)[9] = 0;
    (*mProjMatrix)[10] = -2.0f / (zfar - znear);
    (*mProjMatrix)[11] = 0;
    (*mProjMatrix)[12] = -(right + left) / (right - left);
    (*mProjMatrix)[13] = -(top + bottom) / (top - bottom);
    (*mProjMatrix)[14] = -(zfar + znear) / (zfar - znear);
    (*mProjMatrix)[15] = 1.0f;
}

Boolean ColorFade::CaptureScreenshotTextureAndSetViewport()
{
    if (!AttachEglContext()) {
        return FALSE;
    }
    // try {
        AutoPtr<IGLES20> gles20;
        // CGLES20::AcquireSingleton((IGLES20**)&gles20);
        if (!mTexNamesGenerated) {
            gles20->GlGenTextures(1, mTexNames, 0);
            if (CheckGlErrors(String("glGenTextures"))) {
                DetachEglContext();
                return FALSE;
            }
            mTexNamesGenerated = TRUE;
        }

        AutoPtr<ISurfaceTexture> st;
        CSurfaceTexture::New((*mTexNames)[0], (ISurfaceTexture**)&st);
        AutoPtr<ISurface> s;
        CSurface::New(st, (ISurface**)&s);
        // try {
        AutoPtr<ISurfaceControlHelper> surfaceControl;
        CSurfaceControlHelper::AcquireSingleton((ISurfaceControlHelper**)&surfaceControl);
        AutoPtr<IBinder> display;
        surfaceControl->GetBuiltInDisplay(ISurfaceControl::BUILT_IN_DISPLAY_ID_MAIN,
            (IBinder**)&display);
        surfaceControl->Screenshot(display, s);
        // } finally {
        s->ReleaseSurface();
        // }

        st->UpdateTexImage();
        st->GetTransformMatrix(mTexMatrix);

        // Set up texture coordinates for a quad.
        // We might need to change this if the texture ends up being
        // a different size from the display for some reason.
        mTexCoordBuffer->Put(0, 0.0f); mTexCoordBuffer->Put(1, 0.0f);
        mTexCoordBuffer->Put(2, 0.0f); mTexCoordBuffer->Put(3, 1.0f);
        mTexCoordBuffer->Put(4, 1.0f); mTexCoordBuffer->Put(5, 1.0f);
        mTexCoordBuffer->Put(6, 1.0f); mTexCoordBuffer->Put(7, 0.0f);

        // Set up our viewport.
        gles20->GlViewport(0, 0, mDisplayWidth, mDisplayHeight);
        Ortho(0, mDisplayWidth, 0, mDisplayHeight, -1, 1);
    // } finally {
        DetachEglContext();
    // }
    return TRUE;
}

void ColorFade::DestroyScreenshotTexture()
{
    if (mTexNamesGenerated) {
        mTexNamesGenerated = FALSE;
        AutoPtr<IGLES20> gles20;
        // CGLES20::AcquireSingleton((IGLES20**)&gles20);
        gles20->GlDeleteTextures(1, mTexNames, 0);
        CheckGlErrors(String("glDeleteTextures"));
    }
}

Boolean ColorFade::CreateEglContext()
{
    AutoPtr<IEGL14> egl14;
    // CEGL14::AcquireSingleton((IEGL14**)&egl14);
    Boolean bval;

    if (mEglDisplay == NULL) {
        egl14->EglGetDisplay(IEGL14::_EGL_DEFAULT_DISPLAY, (IEGLDisplay**)&mEglDisplay);
        if (mEglDisplay == NULL /* CEGL14::EGL_NO_DISPLAY */) {
            LogEglError(String("eglGetDisplay"));
            return FALSE;
        }

        AutoPtr<ArrayOf<Int32> > version = ArrayOf<Int32>::Alloc(2);
        egl14->EglInitialize(mEglDisplay, version, 0, version, 1, &bval);
        if (!bval) {
            mEglDisplay = NULL;
            LogEglError(String("eglInitialize"));
            return FALSE;
        }
    }

    if (mEglConfig == NULL) {
        AutoPtr<ArrayOf<Int32> > eglConfigAttribList = ArrayOf<Int32>::Alloc(11);
        eglConfigAttribList->Set(0, IEGL14::EGL_RENDERABLE_TYPE);
        eglConfigAttribList->Set(1, IEGL14::EGL_OPENGL_ES2_BIT);
        eglConfigAttribList->Set(2, IEGL14::EGL_RED_SIZE);
        eglConfigAttribList->Set(3, 8);
        eglConfigAttribList->Set(4, IEGL14::EGL_GREEN_SIZE);
        eglConfigAttribList->Set(5, 8);
        eglConfigAttribList->Set(6, IEGL14::EGL_BLUE_SIZE);
        eglConfigAttribList->Set(7, 8);
        eglConfigAttribList->Set(8, IEGL14::EGL_ALPHA_SIZE);
        eglConfigAttribList->Set(9, 8);
        eglConfigAttribList->Set(10, IEGL14::EGL_NONE);

        AutoPtr<ArrayOf<Int32> > numEglConfigs = ArrayOf<Int32>::Alloc(1);
        AutoPtr<ArrayOf<IEGLConfig*> > eglConfigs = ArrayOf<IEGLConfig*>::Alloc(1);

        egl14->EglChooseConfig(mEglDisplay, eglConfigAttribList, 0,
                eglConfigs, 0, eglConfigs->GetLength(), numEglConfigs, 0, &bval);
        if (!bval) {
            LogEglError(String("eglChooseConfig"));
            return FALSE;
        }
        mEglConfig = (*eglConfigs)[0];
    }

    if (mEglContext == NULL) {
        AutoPtr<ArrayOf<Int32> > eglContextAttribList = ArrayOf<Int32>::Alloc(3);
        eglContextAttribList->Set(0, IEGL14::EGL_CONTEXT_CLIENT_VERSION);
        eglContextAttribList->Set(1, 2);
        eglContextAttribList->Set(2, IEGL14::EGL_NONE);
        egl14->EglCreateContext(mEglDisplay, mEglConfig,
            NULL/*CEGL14::EGL_NO_CONTEXT*/, eglContextAttribList, 0, (IEGLContext**)&mEglContext);
        if (mEglContext == NULL) {
            LogEglError(String("eglCreateContext"));
            return FALSE;
        }
    }
    return TRUE;
}

Boolean ColorFade::CreateSurface()
{
    if (mSurfaceSession == NULL) {
        CSurfaceSession::New((ISurfaceSession**)&mSurfaceSession);
    }

    AutoPtr<ISurfaceControlHelper> surfaceControl;
    CSurfaceControlHelper::AcquireSingleton((ISurfaceControlHelper**)&surfaceControl);
    surfaceControl->OpenTransaction();
    // try {
        if (mSurfaceControl == NULL) {
            Int32 flags;
            if (mMode == MODE_FADE) {
                flags = ISurfaceControl::FX_SURFACE_DIM | ISurfaceControl::HIDDEN;
            }
            else {
                flags = ISurfaceControl::OPAQUE | ISurfaceControl::HIDDEN;
            }

            ECode ec = CSurfaceControl::New(mSurfaceSession,
                String("ColorFade"), mDisplayWidth, mDisplayHeight,
                IPixelFormat::OPAQUE, flags, (ISurfaceControl**)&mSurfaceControl);

            if (ec == (ECode)E_OUT_OF_RESOURCES_EXCEPTION) {
                Slogger::E(TAG, "Unable to create surface.");
                surfaceControl->CloseTransaction();
                return FALSE;
            }
        }

        mSurfaceControl->SetLayerStack(mDisplayLayerStack);
        mSurfaceControl->SetSize(mDisplayWidth, mDisplayHeight);
        mSurface = NULL;
        CSurface::New((ISurface**)&mSurface);
        mSurface->CopyFrom(mSurfaceControl);

        mSurfaceLayout = new NaturalSurfaceLayout(mDisplayManagerInternal,
                mDisplayId, mSurfaceControl);
        mSurfaceLayout->OnDisplayTransaction();
    // } finally {
        surfaceControl->CloseTransaction();
    // }
    return TRUE;
}

Boolean ColorFade::CreateEglSurface()
{
    if (mEglSurface == NULL) {
        AutoPtr<ArrayOf<Int32> > eglSurfaceAttribList = ArrayOf<Int32>::Alloc(1);
        eglSurfaceAttribList->Set(0, IEGL14::EGL_NONE);

        // turn our SurfaceControl into a Surface
        AutoPtr<IEGL14> egl14;
        // CEGL14::AcquireSingleton((IEGL14**)&egl14);
        egl14->EglCreateWindowSurface(mEglDisplay, mEglConfig, mSurface,
            eglSurfaceAttribList, 0, (IEGLSurface**)&mEglSurface);
        if (mEglSurface == NULL) {
            LogEglError(String("eglCreateWindowSurface"));
            return FALSE;
        }
    }
    return TRUE;
}

void ColorFade::DestroyEglSurface()
{
    if (mEglSurface != NULL) {
        AutoPtr<IEGL14> egl14;
        // CEGL14::AcquireSingleton((IEGL14**)&egl14);
        Boolean bval;
        egl14->EglDestroySurface(mEglDisplay, mEglSurface, &bval);
        if (!bval) {
            LogEglError(String("eglDestroySurface"));
        }
        mEglSurface = NULL;
    }
}

void ColorFade::DestroySurface()
{
    if (mSurfaceControl != NULL) {
        mSurfaceLayout->Dispose();
        mSurfaceLayout = NULL;

        AutoPtr<ISurfaceControlHelper> surfaceControl;
        CSurfaceControlHelper::AcquireSingleton((ISurfaceControlHelper**)&surfaceControl);
        surfaceControl->OpenTransaction();
        // try {
            mSurfaceControl->Destroy();
            mSurface->ReleaseSurface();
        // } finally {
            surfaceControl->CloseTransaction();
        // }
        mSurfaceControl = NULL;
        mSurfaceVisible = FALSE;
        mSurfaceAlpha = 0.0f;
    }
}

Boolean ColorFade::ShowSurface(
    /* [in] */ Float alpha)
{
    AutoPtr<ISurfaceControlHelper> surfaceControl;
    CSurfaceControlHelper::AcquireSingleton((ISurfaceControlHelper**)&surfaceControl);
    if (!mSurfaceVisible || mSurfaceAlpha != alpha) {
        surfaceControl->OpenTransaction();
        // try {
            mSurfaceControl->SetLayer(COLOR_FADE_LAYER);
            mSurfaceControl->SetAlpha(alpha);
            mSurfaceControl->Show();
        // } finally {
            surfaceControl->CloseTransaction();
        // }
        mSurfaceVisible = TRUE;
        mSurfaceAlpha = alpha;
    }
    return TRUE;
}

Boolean ColorFade::AttachEglContext()
{
    if (mEglSurface == NULL) {
        return FALSE;
    }
    AutoPtr<IEGL14> egl14;
    // CEGL14::AcquireSingleton((IEGL14**)&egl14);
    Boolean bval;
    egl14->EglMakeCurrent(mEglDisplay, mEglSurface, mEglSurface, mEglContext, &bval);
    if (!bval) {
        LogEglError(String("eglMakeCurrent"));
        return FALSE;
    }
    return TRUE;
}

void ColorFade::DetachEglContext()
{
    if (mEglDisplay != NULL) {
        Boolean bval;
        AutoPtr<IEGL14> egl14;
        // CEGL14::AcquireSingleton((IEGL14**)&egl14);
        egl14->EglMakeCurrent(mEglDisplay,
            NULL/*CEGL14::EGL_NO_SURFACE*/,
            NULL/*CEGL14::EGL_NO_SURFACE*/,
            NULL/*CEGL14::EGL_NO_CONTEXT*/,
            &bval);
    }
}

AutoPtr<IFloatBuffer> ColorFade::CreateNativeFloatBuffer(
    /* [in] */ Int32 size)
{
    AutoPtr<IByteBufferHelper> helper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&helper);
    AutoPtr<IByteBuffer> bb;
    helper->AllocateDirect(size * 4, (IByteBuffer**)&bb);
    AutoPtr<IByteOrderHelper> byteOrderHelper;
    CByteOrderHelper::AcquireSingleton((IByteOrderHelper**)&byteOrderHelper);
    ByteOrder order;
    byteOrderHelper->GetNativeOrder(&order);
    bb->GetOrder(&order);
    AutoPtr<IFloatBuffer>  buffer;
    bb->AsFloatBuffer((IFloatBuffer**)&buffer);
    return buffer;
}

void ColorFade::LogEglError(
    /* [in] */ const String& func)
{
    AutoPtr<IEGL14> egl14;
    // CEGL14::AcquireSingleton((IEGL14**)&egl14);
    Int32 error;
    egl14->EglGetError(&error);
    Slogger::E(TAG, "%s failed: error %d", func.string(), error);
}

Boolean ColorFade::CheckGlErrors(
    /* [in] */ const String& func)
{
    return CheckGlErrors(func, TRUE);
}

Boolean ColorFade::CheckGlErrors(
    /* [in] */ const String& func,
    /* [in] */ Boolean log)
{
    Boolean hadError = FALSE;
    AutoPtr<IGLES20> gles20;
    // CGLES20::AcquireSingleton((IGLES20**)&gles20);

    Int32 error;
    gles20->GlGetError(&error);
    while (error != IGLES20::_GL_NO_ERROR) {
        if (log) {
            Slogger::E(TAG, "%s failed: error %d", func.string(), error);
        }
        hadError = TRUE;
        gles20->GlGetError(&error);
    }
    return hadError;
}

void ColorFade::Dump(
    /* [in] */ IPrintWriter* pw)
{
    // pw.println();
    // pw.println("Color Fade State:");
    // pw.println("  mPrepared=" + mPrepared);
    // pw.println("  mMode=" + mMode);
    // pw.println("  mDisplayLayerStack=" + mDisplayLayerStack);
    // pw.println("  mDisplayWidth=" + mDisplayWidth);
    // pw.println("  mDisplayHeight=" + mDisplayHeight);
    // pw.println("  mSurfaceVisible=" + mSurfaceVisible);
    // pw.println("  mSurfaceAlpha=" + mSurfaceAlpha);
}


} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos

