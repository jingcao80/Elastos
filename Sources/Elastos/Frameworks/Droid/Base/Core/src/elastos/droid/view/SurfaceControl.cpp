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

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/SurfaceControl.h"
#include "elastos/droid/view/CSurfaceSession.h"
#include "elastos/droid/view/CWindowContentFrameStats.h"
#include "elastos/droid/view/CWindowAnimationFrameStats.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CRegion.h"
#include "elastos/droid/graphics/GraphicsNative.h"
#include "elastos/droid/os/NativeBinder.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/AutoLock.h>
#include "elastos/droid/view/CPhysicalDisplayInfo.h"
#include <binder/Binder.h>
#include <binder/IBinder.h>
#include <gui/Surface.h>
#include <gui/SurfaceComposerClient.h>
#include <gui/SurfaceControl.h>

#include <ui/DisplayInfo.h>
#include <ui/FrameStats.h>
#include <ui/Rect.h>
#include <ui/Region.h>

#include <utils/Log.h>
#include <utils/Vector.h>
#include <utils/String8.h>

#include "SkBitmap.h"
#include "SkTemplates.h"

using android::sp;
using android::String8;
using Elastos::Droid::Os::DroidObjectForIBinder;
using Elastos::Droid::Os::IBinderForDroidObject;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CRegion;
using Elastos::Droid::Graphics::GraphicsNative;
using Elastos::Droid::Graphics::INinePatchInsetStruct;
using Elastos::Core::AutoLock;
using Elastos::Core::ICloseGuardHelper;
using Elastos::Core::CCloseGuardHelper;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace View {

Object SurfaceControl::sLock;
const String SurfaceControl::TAG("SurfaceControl");

static void assert_premultiplied(const SkBitmap& bitmap, bool isPremultiplied) {
    // kOpaque_SkAlphaType and kIgnore_SkAlphaType mean that isPremultiplied is
    // irrelevant. This just tests to ensure that the SkAlphaType is not
    // opposite of isPremultiplied.
    if (isPremultiplied) {
        SkASSERT(bitmap.alphaType() != kUnpremul_SkAlphaType);
    } else {
        SkASSERT(bitmap.alphaType() != kPremul_SkAlphaType);
    }
}

static AutoPtr<IBitmap> CreateBitmap(
    /* [in] */ SkBitmap* bitmap,
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 bitmapCreateFlags,
    /* [in] */ ArrayOf<Byte>* ninePatchChunk,
    /* [in] */ INinePatchInsetStruct* ninePatchInsets,
    /* [in] */ Int32 density)
{
    SkASSERT(bitmap);
    SkASSERT(bitmap->pixelRef());
    Boolean isMutable = bitmapCreateFlags & GraphicsNative::kBitmapCreateFlag_Mutable;
    Boolean isPremultiplied = bitmapCreateFlags & GraphicsNative::kBitmapCreateFlag_Premultiplied;

    // The caller needs to have already set the alpha type properly, so the
    // native SkBitmap stays in sync with the Java Bitmap.
    assert_premultiplied(*bitmap, isPremultiplied);

    AutoPtr<IBitmap> result;
    CBitmap::New(reinterpret_cast<Int64>(bitmap), buffer,
            bitmap->width(), bitmap->height(), density, isMutable, isPremultiplied,
            ninePatchChunk, ninePatchInsets, (IBitmap**)&result);

    return result;
}

// Implements SkMallocPixelRef::ReleaseProc, to delete the screenshot on unref.
void DeleteScreenshot(void* addr, void* context) {
    SkASSERT(addr == ((android::ScreenshotClient*) context)->getPixels());
    delete ((android::ScreenshotClient*) context);
}

CAR_INTERFACE_IMPL(SurfaceControl, Object, ISurfaceControl)

SurfaceControl::SurfaceControl()
    : mNativeObject(0)
    , mName(NULL)
{
    AutoPtr<ICloseGuardHelper> helper;
    CCloseGuardHelper::AcquireSingleton((ICloseGuardHelper**)&helper);
    helper->Get((ICloseGuard**)&mCloseGuard);
}

SurfaceControl::~SurfaceControl()
{
    // try {
        if (mCloseGuard != NULL) {
            mCloseGuard->WarnIfOpen();
        }
        if (mNativeObject != 0) {
            NativeRelease(mNativeObject);
        }
    // } finally {
        // super.finalize();
    // }
}

ECode SurfaceControl::ReleaseSurfaceControl()
{
    if (mNativeObject != 0) {
        NativeRelease(mNativeObject);
        mNativeObject = 0;
    }
    mCloseGuard->Close();
    return NOERROR;
}

ECode SurfaceControl::SetLayer(
    /* [in] */ Int32 zorder)
{
    FAIL_RETURN(CheckNotReleased())
    return NativeSetLayer(mNativeObject, zorder);
}

ECode SurfaceControl::Destroy()
{
    if (mNativeObject != 0) {
        NativeDestroy(mNativeObject);
        mNativeObject = 0;
    }
    mCloseGuard->Close();
    return NOERROR;
}

ECode SurfaceControl::SetPosition(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    FAIL_RETURN(CheckNotReleased())
    return NativeSetPosition(mNativeObject, x, y);
}

ECode SurfaceControl::SetSize(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h)
{
    FAIL_RETURN(CheckNotReleased())
    return NativeSetSize(mNativeObject, w, h);
}

ECode SurfaceControl::Hide()
{
    FAIL_RETURN(CheckNotReleased())
    return NativeSetFlags(mNativeObject, SURFACE_HIDDEN, SURFACE_HIDDEN);
}

ECode SurfaceControl::SetBlur(
    /* [in] */ Float blur)
{
    FAIL_RETURN(CheckNotReleased())
    return NativeSetBlur(mNativeObject, blur);
}

ECode SurfaceControl::SetBlurMaskSurface(
    /* [in] */ ISurfaceControl* _maskSurface)
{
    FAIL_RETURN(CheckNotReleased())
    SurfaceControl* maskSurface = (SurfaceControl*)_maskSurface;
    if (maskSurface != NULL) {
        FAIL_RETURN(maskSurface->CheckNotReleased())
    }
    return NativeSetBlurMaskSurface(mNativeObject, maskSurface == NULL ? 0 : maskSurface->mNativeObject);
}

ECode SurfaceControl::SetBlurMaskSampling(
    /* [in] */ Int32 blurMaskSampling)
{
    FAIL_RETURN(CheckNotReleased())
    return NativeSetBlurMaskSampling(mNativeObject, blurMaskSampling);
}

ECode SurfaceControl::SetBlurMaskAlphaThreshold(
    /* [in] */ Float alpha)
{
    FAIL_RETURN(CheckNotReleased())
    return NativeSetBlurMaskAlphaThreshold(mNativeObject, alpha);
}

ECode SurfaceControl::SetTransparent(
    /* [in] */ Boolean isTransparent)
{
    FAIL_RETURN(CheckNotReleased())
    if (isTransparent) {
        NativeSetFlags(mNativeObject, SURFACE_TRANSPARENT,
                SURFACE_TRANSPARENT);
    }
    else {
        NativeSetFlags(mNativeObject, 0, SURFACE_TRANSPARENT);
    }
    return NOERROR;
}

ECode SurfaceControl::Show()
{
    FAIL_RETURN(CheckNotReleased())
    return NativeSetFlags(mNativeObject, 0, SURFACE_HIDDEN);
}

ECode SurfaceControl::SetTransparentRegionHint(
    /* [in] */ IRegion* region)
{
    FAIL_RETURN(CheckNotReleased())
    return NativeSetTransparentRegionHint(mNativeObject, region);
}

ECode SurfaceControl::ClearContentFrameStats(
    /* [out] */ Boolean* result)
{
    FAIL_RETURN(CheckNotReleased())
    return NativeClearContentFrameStats(mNativeObject, result);
}

ECode SurfaceControl::GetContentFrameStats(
    /* [in] */ IWindowContentFrameStats* outStats,
    /* [out] */ Boolean* result)
{
    FAIL_RETURN(CheckNotReleased())
    return NativeGetContentFrameStats(mNativeObject, outStats, result);
}

ECode SurfaceControl::SetAlpha(
    /* [in] */ Float alpha)
{
    FAIL_RETURN(CheckNotReleased())
    NativeSetAlpha(mNativeObject, alpha);
    return NOERROR;
}

ECode SurfaceControl::SetMatrix(
    /* [in] */ Float dsdx,
    /* [in] */ Float dtdx,
    /* [in] */ Float dsdy,
    /* [in] */ Float dtdy)
{
    FAIL_RETURN(CheckNotReleased())
    NativeSetMatrix(mNativeObject, dsdx, dtdx, dsdy, dtdy);
    return NOERROR;
}

ECode SurfaceControl::SetWindowCrop(
    /* [in] */ IRect* crop)
{
    Int32 left, top, right, bottom;
    crop->GetLeft(&left);
    crop->GetTop(&top);
    crop->GetRight(&right);
    crop->GetBottom(&bottom);

    FAIL_RETURN(CheckNotReleased())
    if (crop != NULL) {
        NativeSetWindowCrop(mNativeObject, left, top, right, bottom);
    } else {
        NativeSetWindowCrop(mNativeObject, 0, 0, 0, 0);
    }
    return NOERROR;
}

ECode SurfaceControl::SetLayerStack(
    /* [in] */ Int32 layerStack)
{
    FAIL_RETURN(CheckNotReleased())
    NativeSetLayerStack(mNativeObject, layerStack);
    return NOERROR;
}

ECode SurfaceControl::SetOpaque(
    /* [in] */ Boolean isOpaque)
{
    FAIL_RETURN(CheckNotReleased())
    if (isOpaque) {
        NativeSetFlags(mNativeObject, SURFACE_OPAQUE, SURFACE_OPAQUE);
    } else {
        NativeSetFlags(mNativeObject, 0, SURFACE_OPAQUE);
    }
    return NOERROR;
}

ECode SurfaceControl::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("Surface(name=") + mName + ")";
    return NOERROR;
}

ECode SurfaceControl::SetDisplayPowerMode(
    /* [in] */ IBinder* displayToken,
    /* [in] */ Int32 mode)
{
    if (displayToken == NULL) {
        // throw new IllegalArgumentException("displayToken must not be null");
        Slogger::E(TAG, "displayToken must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    NativeSetDisplayPowerMode(displayToken, mode);
    return NOERROR;
}

ECode SurfaceControl::GetDisplayConfigs(
    /* [in] */ IBinder* displayToken,
    /* [out] */ ArrayOf<IPhysicalDisplayInfo*>** infos)
{
    VALIDATE_NOT_NULL(infos)
    *infos = NULL;
    if (displayToken == NULL) {
        // throw new IllegalArgumentException("displayToken must not be null");
        Slogger::E(TAG, "displayToken must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NativeGetDisplayConfigs(displayToken, infos);
}

ECode SurfaceControl::GetActiveConfig(
    /* [in] */ IBinder* displayToken,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    *res = 0;
    if (displayToken == NULL) {
        // throw new IllegalArgumentException("displayToken must not be null");
        Slogger::E(TAG, "displayToken must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NativeGetActiveConfig(displayToken, res);
}

ECode SurfaceControl::SetActiveConfig(
    /* [in] */ IBinder* displayToken,
    /* [in] */ Int32 id,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    if (displayToken == NULL) {
        // throw new IllegalArgumentException("displayToken must not be null");
        Slogger::E(TAG, "displayToken must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NativeSetActiveConfig(displayToken, id, res);
}

ECode SurfaceControl::SetDisplayProjection(
    /* [in] */ IBinder* displayToken,
    /* [in] */ Int32 orientation,
    /* [in] */ IRect* layerStackRect,
    /* [in] */ IRect* displayRect)
{
    if (displayToken == NULL) {
        // throw new IllegalArgumentException("displayToken must not be null");
        Slogger::E(TAG, "displayToken must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (layerStackRect == NULL) {
        // throw new IllegalArgumentException("layerStackRect must not be null");
        Slogger::E(TAG, "layerStackRect must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (displayRect == NULL) {
        // throw new IllegalArgumentException("displayRect must not be null");
        Slogger::E(TAG, "displayRect must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 lLeft, dLeft, lRight, dRight, lTop, dTop, lBottom, dBottom;

    layerStackRect->GetLeft(&lLeft);
    layerStackRect->GetTop(&lTop);
    layerStackRect->GetRight(&lRight);
    layerStackRect->GetBottom(&lBottom);

    displayRect->GetLeft(&dLeft);
    displayRect->GetTop(&dTop);
    displayRect->GetRight(&dRight);
    displayRect->GetBottom(&dBottom);

    NativeSetDisplayProjection(displayToken, orientation,
            lLeft, lTop, lRight, lBottom,
            dLeft, dTop, dRight, dBottom);
    return NOERROR;
}

ECode SurfaceControl::SetDisplayLayerStack(
    /* [in] */ IBinder* displayToken,
    /* [in] */ Int32 layerStack)
{
    if (displayToken == NULL) {
        // throw new IllegalArgumentException("displayToken must not be null");
        Slogger::E(TAG, "displayToken must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    NativeSetDisplayLayerStack(displayToken, layerStack);
    return NOERROR;
}

ECode SurfaceControl::SetDisplaySurface(
    /* [in] */ IBinder* displayToken,
    /* [in] */ ISurface* surface)
{
    if (displayToken == NULL) {
        // throw new IllegalArgumentException("displayToken must not be null");
        Slogger::E(TAG, "displayToken must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (surface != NULL) {
        AutoLock lock(sLock);
        Surface* imp = (Surface*)surface;
        NativeSetDisplaySurface(displayToken, imp->mNativeObject);
    } else {
        NativeSetDisplaySurface(displayToken, 0);
    }
    return NOERROR;
}

ECode SurfaceControl::SetDisplaySize(
    /* [in] */ IBinder* displayToken,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    if (displayToken == NULL) {
        // throw new IllegalArgumentException("displayToken must not be null");
        Slogger::E(TAG, "displayToken must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (width <= 0 || height <= 0) {
        // throw new IllegalArgumentException("width and height must be positive");
        Slogger::E(TAG, "width and height must be positive");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    NativeSetDisplaySize(displayToken, width, height);
    return NOERROR;
}

ECode SurfaceControl::CreateDisplay(
    /* [in] */ const String& name,
    /* [in] */ Boolean secure,
    /* [out] */ IBinder** token)
{
    VALIDATE_NOT_NULL(token)
    *token = NULL;
    if (name == NULL) {
        // throw new IllegalArgumentException("name must not be null");
        Slogger::E(TAG, "name must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IBinder> temp = NativeCreateDisplay(name, secure);
    *token = temp;
    REFCOUNT_ADD(*token)
    return NOERROR;
}

ECode SurfaceControl::DestroyDisplay(
    /* [in] */ IBinder* displayToken)
{
    if (displayToken == NULL) {
        // throw new IllegalArgumentException("displayToken must not be null");
        Slogger::E(TAG, "displayToken must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    NativeDestroyDisplay(displayToken);
    return NOERROR;
}

AutoPtr<IBinder> SurfaceControl::GetBuiltInDisplay(
    /* [in] */ Int32 builtInDisplayId)
{
    return NativeGetBuiltInDisplay(builtInDisplayId);
}
/**
 * Copy the current screen contents into the provided {@link Surface}
 *
 * @param display The display to take the screenshot of.
 * @param consumer The {@link Surface} to take the screenshot into.
 * @param width The desired width of the returned bitmap; the raw
 * screen will be scaled down to this size.
 * @param height The desired height of the returned bitmap; the raw
 * screen will be scaled down to this size.
 * @param minLayer The lowest (bottom-most Z order) surface layer to
 * include in the screenshot.
 * @param maxLayer The highest (top-most Z order) surface layer to
 * include in the screenshot.
 * @param useIdentityTransform Replace whatever transformation (rotation,
 * scaling, translation) the surface layers are currently using with the
 * identity transformation while taking the screenshot.
 */
 void SurfaceControl::Screenshot(
    /* [in] */ IBinder* display,
    /* [in] */ ISurface* consumer,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 minLayer,
    /* [in] */ Int32 maxLayer,
    /* [in] */ Boolean useIdentityTransform)
 {
    AutoPtr<IRect> r;
    CRect::New((IRect**)&r);
    Screenshot(display, consumer, r, width, height, minLayer, maxLayer,
            FALSE, useIdentityTransform);
 }

/**
 * Copy the current screen contents into the provided {@link Surface}
 *
 * @param display The display to take the screenshot of.
 * @param consumer The {@link Surface} to take the screenshot into.
 * @param width The desired width of the returned bitmap; the raw
 * screen will be scaled down to this size.
 * @param height The desired height of the returned bitmap; the raw
 * screen will be scaled down to this size.
 */
 void SurfaceControl::Screenshot(
    /* [in] */ IBinder* display,
    /* [in] */ ISurface* consumer,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
 {
    AutoPtr<IRect> r;
    CRect::New((IRect**)&r);
    Screenshot(display, consumer, r, width, height, 0, 0, TRUE, FALSE);
 }

/**
 * Copy the current screen contents into the provided {@link Surface}
 *
 * @param display The display to take the screenshot of.
 * @param consumer The {@link Surface} to take the screenshot into.
 */
 void SurfaceControl::Screenshot(
    /* [in] */ IBinder* display,
    /* [in] */ ISurface* consumer)
 {
    AutoPtr<IRect> r;
    CRect::New((IRect**)&r);
    Screenshot(display, consumer, r, 0, 0, 0, 0, TRUE, FALSE);
 }

/**
 * Copy the current screen contents into a bitmap and return it.
 *
 * CAVEAT: Versions of screenshot that return a {@link Bitmap} can
 * be extremely slow; avoid use unless absolutely necessary; prefer
 * the versions that use a {@link Surface} instead, such as
 * {@link SurfaceControl#screenshot(IBinder, Surface)}.
 *
 * @param sourceCrop The portion of the screen to capture into the Bitmap;
 * caller may pass in 'new Rect()' if no cropping is desired.
 * @param width The desired width of the returned bitmap; the raw
 * screen will be scaled down to this size.
 * @param height The desired height of the returned bitmap; the raw
 * screen will be scaled down to this size.
 * @param minLayer The lowest (bottom-most Z order) surface layer to
 * include in the screenshot.
 * @param maxLayer The highest (top-most Z order) surface layer to
 * include in the screenshot.
 * @param useIdentityTransform Replace whatever transformation (rotation,
 * scaling, translation) the surface layers are currently using with the
 * identity transformation while taking the screenshot.
 * @param rotation Apply a custom clockwise rotation to the screenshot, i.e.
 * Surface.ROTATION_0,90,180,270. Surfaceflinger will always take
 * screenshots in its native portrait orientation by default, so this is
 * useful for returning screenshots that are independent of device
 * orientation.
 * @return Returns a Bitmap containing the screen contents, or null
 * if an error occurs. Make sure to call Bitmap.recycle() as soon as
 * possible, once its content is not needed anymore.
 */
 AutoPtr<IBitmap> SurfaceControl::Screenshot(
    /* [in] */ IRect* sourceCrop,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 minLayer,
    /* [in] */ Int32 maxLayer,
    /* [in] */ Boolean useIdentityTransform,
    /* [in] */ Int32 rotation)
{
    // TODO: should take the display as a parameter
    AutoPtr<IBinder> displayToken = GetBuiltInDisplay(
            BUILT_IN_DISPLAY_ID_MAIN);
    return NativeScreenshot(displayToken, sourceCrop, width, height,
            minLayer, maxLayer, FALSE, useIdentityTransform, rotation);
}
/**
 * Like {@link SurfaceControl#screenshot(int, int, int, int, boolean)} but
 * includes all Surfaces in the screenshot.
 *
 * @param width The desired width of the returned bitmap; the raw
 * screen will be scaled down to this size.
 * @param height The desired height of the returned bitmap; the raw
 * screen will be scaled down to this size.
 * @return Returns a Bitmap containing the screen contents, or null
 * if an error occurs. Make sure to call Bitmap.recycle() as soon as
 * possible, once its content is not needed anymore.
 */
 AutoPtr<IBitmap> SurfaceControl::Screenshot(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    // TODO: should take the display as a parameter
    AutoPtr<IBinder>  displayToken = GetBuiltInDisplay(
            BUILT_IN_DISPLAY_ID_MAIN);
    AutoPtr<IRect> r;
    CRect::New((IRect**)&r);
    return NativeScreenshot(displayToken, r, width, height, 0, 0, TRUE,
            FALSE, ISurface::ROTATION_0);
}

ECode SurfaceControl::ClearAnimationFrameStats(
    /* [out] */ Boolean* resutl)
{
    return NativeClearAnimationFrameStats(resutl);
}

ECode SurfaceControl::GetAnimationFrameStats(
    /* [in] */ IWindowAnimationFrameStats* outStats,
    /* [out] */ Boolean* resutl)
{
    return NativeGetAnimationFrameStats(outStats, resutl);
}
/** start a transaction */

void SurfaceControl::OpenTransaction()
{
    NativeOpenTransaction();
}

/** end a transaction */
void SurfaceControl::CloseTransaction()
{
    NativeCloseTransaction();
}

/** flag the transaction as an animation */
void SurfaceControl::SetAnimationTransaction()
{
    NativeSetAnimationTransaction();
}

/**
 * Create a surface with a name.
 * <p>
 * The surface creation flags specify what kind of surface to create and
 * certain options such as whether the surface can be assumed to be opaque
 * and whether it should be initially hidden.  Surfaces should always be
 * created with the {@link #HIDDEN} flag set to ensure that they are not
 * made visible prematurely before all of the surface's properties have been
 * configured.
 * <p>
 * Good practice is to first create the surface with the {@link #HIDDEN} flag
 * specified, open a transaction, set the surface layer, layer stack, alpha,
 * and position, call {@link #show} if appropriate, and close the transaction.
 *
 * @param session The surface session, must not be null.
 * @param name The surface name, must not be null.
 * @param w The surface initial width.
 * @param h The surface initial height.
 * @param flags The surface creation flags.  Should always include {@link #HIDDEN}
 * in the creation flags.
 *
 * @throws throws OutOfResourcesException If the SurfaceControl cannot be created.
 */
ECode SurfaceControl::constructor(
    /* [in] */ ISurfaceSession* session,
    /* [in] */ const String& name,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 format,
    /* [in] */ Int32 flags)
{
    if (session == NULL) {
        // throw new IllegalArgumentException("session must not be null");
        Slogger::E(TAG, "session must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (name == NULL) {
        // throw new IllegalArgumentException("name must not be null");
        Slogger::E(TAG, "name must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if ((flags & ISurfaceControl::HIDDEN) == 0) {
        Slogger::W(TAG, "Surfaces should always be created with the HIDDEN flag set "
                "to ensure that they are not made visible prematurely before "
                "all of the surface's properties have been configured.  "
                "Set the other properties and make the surface visible within "
                "a transaction.  New surface name: %s", name.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mName = name;
    FAIL_RETURN(NativeCreate(session, name, w, h, format, flags, &mNativeObject))
    if (mNativeObject == 0) {
        // throw new OutOfResourcesException(
                // "Couldn't allocate SurfaceControl native object");
        Slogger::E(TAG, "Couldn't allocate SurfaceControl native object");
        return E_OUT_OF_RESOURCES_EXCEPTION;
    }

    mCloseGuard->Open(String("SurfaceControl::Release"));
    return NOERROR;
}

ECode SurfaceControl::CheckNotReleased()
{
    if (mNativeObject == 0) {
        Slogger::E(TAG, "mNativeObject is null. Have you called release() already?");
        return E_NULL_POINTER_EXCEPTION;
    }
    return NOERROR;
}

void SurfaceControl::Screenshot(
    /* [in] */ IBinder* display,
    /* [in] */ ISurface* consumer,
    /* [in] */ IRect* sourceCrop,
    /* [in] */ Int32 width,
    /* [in] */ Int32  height,
    /* [in] */ Int32  minLayer,
    /* [in] */ Int32  maxLayer,
    /* [in] */ Boolean allLayers,
    /* [in] */ Boolean useIdentityTransform)
{
    if (display == NULL) {
        // throw new IllegalArgumentException("displayToken must not be null");
        Slogger::E(TAG, "displayToken must not be null");
    }
    if (consumer == NULL) {
        // throw new IllegalArgumentException("consumer must not be null");
        Slogger::E(TAG, "consumer must not be null");
    }
    NativeScreenshot(display, consumer, sourceCrop, width, height,
            minLayer, maxLayer, allLayers, useIdentityTransform);
}

ECode SurfaceControl::NativeCreate(
    /* [in] */ ISurfaceSession* session,
    /* [in] */ const String& name,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 format,
    /* [in] */ Int32 flags,
    /* [out] */ Int64* ptr)
{
    VALIDATE_NOT_NULL(ptr)
    CSurfaceSession* sessionImpl = (CSurfaceSession*)session;

    sp<android::SurfaceComposerClient> client = sessionImpl->mNativeClient;
    sp<android::SurfaceControl> surface = client->createSurface(
            String8(name.string()), w, h, format, flags);
    if (surface == NULL) {
        // jniThrowException(env, OutOfResourcesException, NULL);
        *ptr = 0;
        return E_OUT_OF_RESOURCES_EXCEPTION;
    }
    surface->incStrong((void *)NativeCreate);
    *ptr = reinterpret_cast<Int64>(surface.get());
    return NOERROR;
}

void SurfaceControl::NativeRelease(
    /* [in] */ Int64 nativeObject)
{
    sp<android::SurfaceControl> ctrl(reinterpret_cast<android::SurfaceControl *>(nativeObject));
    ctrl->decStrong((void *)NativeCreate);
}

void SurfaceControl::NativeDestroy(
    /* [in] */ Int64 nativeObject)
{
    sp<android::SurfaceControl> ctrl(reinterpret_cast<android::SurfaceControl *>(nativeObject));
    ctrl->clear();
    ctrl->decStrong((void *)NativeCreate);
}

AutoPtr<IBitmap> SurfaceControl::NativeScreenshot(
    /* [in] */ IBinder* displayTokenObj,
    /* [in] */ IRect* sourceCropObj,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 minLayer,
    /* [in] */ Int32 maxLayer,
    /* [in] */ Boolean allLayers,
    /* [in] */ Boolean useIdentityTransform,
    /* [in] */ Int32 rotation)
{
    sp<android::IBinder> displayToken = IBinderForDroidObject(displayTokenObj);
    if (displayToken == NULL) {
        return NULL;
    }

    Int32 left, top, right, bottom;
    sourceCropObj->GetLeft(&left); sourceCropObj->GetTop(&top);
    sourceCropObj->GetRight(&right); sourceCropObj->GetBottom(&bottom);

    android::Rect sourceCrop(left, top, right, bottom);

    SkAutoTDelete<android::ScreenshotClient> screenshot(new android::ScreenshotClient());
    android::status_t res;
    if (allLayers) {
        minLayer = 0;
        maxLayer = -1UL;
    }

    res = screenshot->update(displayToken, sourceCrop, width, height,
        minLayer, maxLayer, useIdentityTransform, static_cast<uint32_t>(rotation));
    if (res != android::NO_ERROR) {
        return NULL;
    }

    SkImageInfo screenshotInfo;
    screenshotInfo.fWidth = screenshot->getWidth();
    screenshotInfo.fHeight = screenshot->getHeight();

    switch (screenshot->getFormat()) {
        case android::PIXEL_FORMAT_RGBX_8888: {
            screenshotInfo.fColorType = kRGBA_8888_SkColorType;
            screenshotInfo.fAlphaType = kIgnore_SkAlphaType;
            break;
        }
        case android::PIXEL_FORMAT_RGBA_8888: {
            screenshotInfo.fColorType = kRGBA_8888_SkColorType;
            screenshotInfo.fAlphaType = kPremul_SkAlphaType;
            break;
        }
        case android::PIXEL_FORMAT_RGB_565: {
            screenshotInfo.fColorType = kRGB_565_SkColorType;
            screenshotInfo.fAlphaType = kIgnore_SkAlphaType;
            break;
        }
        default: {
            return NULL;
        }
    }

    const ssize_t rowBytes =
            screenshot->getStride() * android::bytesPerPixel(screenshot->getFormat());

    SkBitmap* bitmap = new SkBitmap();
    bitmap->setInfo(screenshotInfo, (size_t)rowBytes);
    if (screenshotInfo.fWidth > 0 && screenshotInfo.fHeight > 0) {
        // takes ownership of ScreenshotClient
        SkMallocPixelRef* pixels = SkMallocPixelRef::NewWithProc(screenshotInfo,
                (size_t) rowBytes, NULL, (void*) screenshot->getPixels(), &DeleteScreenshot,
                (void*) (screenshot.get()));
        screenshot.detach();
        pixels->setImmutable();
        bitmap->setPixelRef(pixels)->unref();
        bitmap->lockPixels();
    }

    return CreateBitmap(bitmap, NULL,
            GraphicsNative::kBitmapCreateFlag_Premultiplied, NULL, NULL, -1);
}

void SurfaceControl::NativeScreenshot(
    /* [in] */ IBinder* displayTokenObj,
    /* [in] */ ISurface* surfaceObj,
    /* [in] */ IRect* sourceCropObj,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 minLayer,
    /* [in] */ Int32 maxLayer,
    /* [in] */ Boolean allLayers,
    /* [in] */ Boolean useIdentityTransform)
{
    sp<android::IBinder> displayToken = IBinderForDroidObject(displayTokenObj);
    if (displayToken != NULL) {
        Surface* surImpl = (Surface*)surfaceObj;
        AutoLock lock(surImpl);
        sp<android::Surface> consumer = reinterpret_cast<android::Surface*>(surImpl->mNativeObject);
        if (consumer != NULL) {
            Int32 left, top, right, bottom;
            sourceCropObj->GetLeft(&left); sourceCropObj->GetTop(&top);
            sourceCropObj->GetRight(&right); sourceCropObj->GetBottom(&bottom);

            android::Rect sourceCrop(left, top, right, bottom);

            if (allLayers) {
                minLayer = 0;
                maxLayer = -1;
            }
            android::ScreenshotClient::capture(displayToken,
                    consumer->getIGraphicBufferProducer(), sourceCrop,
                    width, height, uint32_t(minLayer), uint32_t(maxLayer),
                    useIdentityTransform);
        }
    }
}

void SurfaceControl::NativeOpenTransaction()
{
    android::SurfaceComposerClient::openGlobalTransaction();
}

void SurfaceControl::NativeCloseTransaction()
{
    android::SurfaceComposerClient::closeGlobalTransaction();
}

void SurfaceControl::NativeSetAnimationTransaction()
{
    android::SurfaceComposerClient::setAnimationTransaction();
}

ECode SurfaceControl::NativeSetLayer(
    /* [in] */ Int64 nativeObject,
    /* [in] */ Int32 zorder)
{
    android::SurfaceControl* const ctrl = reinterpret_cast<android::SurfaceControl *>(nativeObject);
    android::status_t err = ctrl->setLayer(zorder);
    if (err < 0 && err != android::NO_INIT) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode SurfaceControl::NativeSetPosition(
    /* [in] */ Int64 nativeObject,
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    android::SurfaceControl* const ctrl = reinterpret_cast<android::SurfaceControl *>(nativeObject);
    android::status_t err = ctrl->setPosition(x, y);
    if (err < 0 && err != android::NO_INIT) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode SurfaceControl::NativeSetSize(
    /* [in] */ Int64 nativeObject,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h)
{
    android::SurfaceControl* const ctrl = reinterpret_cast<android::SurfaceControl *>(nativeObject);
    android::status_t err = ctrl->setSize(w, h);
    if (err < 0 && err != android::NO_INIT) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode SurfaceControl::NativeSetTransparentRegionHint(
    /* [in] */ Int64 nativeObject,
    /* [in] */ IRegion* regionObj)
{
    android::SurfaceControl* const ctrl = reinterpret_cast<android::SurfaceControl *>(nativeObject);
    CRegion* regionImpl = (CRegion*)regionObj;

    SkRegion* region = reinterpret_cast<SkRegion*>(regionImpl->mNativeRegion);
    if (region != NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    const SkIRect& b(region->getBounds());
    android::Region reg(android::Rect(b.fLeft, b.fTop, b.fRight, b.fBottom));
    if (region->isComplex()) {
        SkRegion::Iterator it(*region);
        while (!it.done()) {
            const SkIRect& r(it.rect());
            reg.addRectUnchecked(r.fLeft, r.fTop, r.fRight, r.fBottom);
            it.next();
        }
    }

    android::status_t err = ctrl->setTransparentRegionHint(reg);
    if (err < 0 && err != android::NO_INIT) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode SurfaceControl::NativeSetAlpha(
    /* [in] */ Int64 nativeObject,
    /* [in] */ Float alpha)
{
    android::SurfaceControl* const ctrl = reinterpret_cast<android::SurfaceControl *>(nativeObject);
    android::status_t err = ctrl->setAlpha(alpha);
    if (err < 0 && err != android::NO_INIT) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode SurfaceControl::NativeSetMatrix(
    /* [in] */ Int64 nativeObject,
    /* [in] */ Float dsdx,
    /* [in] */ Float dtdx,
    /* [in] */ Float dsdy,
    /* [in] */ Float dtdy)
{
    android::SurfaceControl* const ctrl = reinterpret_cast<android::SurfaceControl *>(nativeObject);
    android::status_t err = ctrl->setMatrix(dsdx, dtdx, dsdy, dtdy);
    if (err < 0 && err != android::NO_INIT) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode SurfaceControl::NativeSetFlags(
    /* [in] */ Int64 nativeObject,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 mask)
{
    android::SurfaceControl* const ctrl = reinterpret_cast<android::SurfaceControl *>(nativeObject);
    android::status_t err = ctrl->setFlags(flags, mask);
    if (err < 0 && err != android::NO_INIT) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode SurfaceControl::NativeSetWindowCrop(
    /* [in] */ Int64 nativeObject,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    android::SurfaceControl* const ctrl = reinterpret_cast<android::SurfaceControl *>(nativeObject);
    android::Rect crop(l, t, r, b);
    android::status_t err = ctrl->setCrop(crop);
    if (err < 0 && err != android::NO_INIT) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode SurfaceControl::NativeSetLayerStack(
    /* [in] */ Int64 nativeObject,
    /* [in] */ Int32 layerStack)
{
    android::SurfaceControl* const ctrl = reinterpret_cast<android::SurfaceControl *>(nativeObject);
    android::status_t err = ctrl->setLayerStack(layerStack);
    if (err < 0 && err != android::NO_INIT) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode SurfaceControl::NativeSetBlur(
    /* [in] */ Int64 nativeObject,
    /* [in] */ Float blur)
{
    android::SurfaceControl* const ctrl = reinterpret_cast<android::SurfaceControl *>(nativeObject);
    android::status_t err = ctrl->setBlur(blur);
    if (err < 0 && err != android::NO_INIT) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode SurfaceControl::NativeSetBlurMaskSurface(
    /* [in] */ Int64 nativeObject,
    /* [in] */ Int64 maskLayerNativeObject)
{
    android::SurfaceControl* const ctrl = reinterpret_cast<android::SurfaceControl *>(nativeObject);
    android::SurfaceControl* const maskLayer = reinterpret_cast<android::SurfaceControl *>(maskLayerNativeObject);
    android::status_t err = ctrl->setBlurMaskSurface(maskLayer);
    if (err < 0 && err != android::NO_INIT) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode SurfaceControl::NativeSetBlurMaskSampling(
    /* [in] */ Int64 nativeObject,
    /* [in] */ Int32 blurMaskSampling)
{
    android::SurfaceControl* const ctrl = reinterpret_cast<android::SurfaceControl *>(nativeObject);
    android::status_t err = ctrl->setBlurMaskSampling(blurMaskSampling);
    if (err < 0 && err != android::NO_INIT) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode SurfaceControl::NativeSetBlurMaskAlphaThreshold(
    /* [in] */ Int64 nativeObject,
    /* [in] */ Float alpha)
{
    android::SurfaceControl* const ctrl = reinterpret_cast<android::SurfaceControl *>(nativeObject);
    android::status_t err = ctrl->setBlurMaskAlphaThreshold(alpha);
    if (err < 0 && err != android::NO_INIT) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode SurfaceControl::NativeClearContentFrameStats(
    /* [in] */ Int64 nativeObject,
    /* [out] */ Boolean* result)
{
    android::SurfaceControl* const ctrl = reinterpret_cast<android::SurfaceControl *>(nativeObject);
    android::status_t err = ctrl->clearLayerFrameStats();

    if (err < 0 && err != android::NO_INIT) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // The other end is not ready, just report we failed.
    if (err == android::NO_INIT) {
        *result = FALSE;
        return NOERROR;
    }

    *result = TRUE;
    return NOERROR;
}

ECode SurfaceControl::NativeGetContentFrameStats(
    /* [in] */ Int64 nativeObject,
    /* [in] */ IWindowContentFrameStats* outStats,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    android::FrameStats stats;

    android::SurfaceControl* const ctrl = reinterpret_cast<android::SurfaceControl *>(nativeObject);
    android::status_t err = ctrl->getLayerFrameStats(&stats);
    if (err < 0 && err != android::NO_INIT) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // The other end is not ready, fine just return empty stats.
    if (err == android::NO_INIT) {
        return NOERROR;
    }

    Int64 refreshPeriodNano = static_cast<Int64>(stats.refreshPeriodNano);
    size_t frameCount = stats.desiredPresentTimesNano.size();

    AutoPtr<ArrayOf<Int64> > postedTimesNanoDst = ArrayOf<Int64>::Alloc(frameCount);
    AutoPtr<ArrayOf<Int64> >  presentedTimesNanoDst = ArrayOf<Int64>::Alloc(frameCount);
    AutoPtr<ArrayOf<Int64> > readyTimesNanoDst = ArrayOf<Int64>::Alloc(frameCount);
    nsecs_t postedTimesNanoSrc[frameCount];
    nsecs_t presentedTimesNanoSrc[frameCount];
    nsecs_t readyTimesNanoSrc[frameCount];

    for (size_t i = 0; i < frameCount; i++) {
        nsecs_t postedTimeNano = stats.desiredPresentTimesNano[i];
        if (postedTimeNano == INT64_MAX) {
            postedTimeNano = IFrameStats::UNDEFINED_TIME_NANO;
        }
        postedTimesNanoSrc[i] = postedTimeNano;

        nsecs_t presentedTimeNano = stats.actualPresentTimesNano[i];
        if (presentedTimeNano == INT64_MAX) {
            presentedTimeNano = IFrameStats::UNDEFINED_TIME_NANO;
        }
        presentedTimesNanoSrc[i] = presentedTimeNano;

        nsecs_t readyTimeNano = stats.frameReadyTimesNano[i];
        if (readyTimeNano == INT64_MAX) {
            readyTimeNano = IFrameStats::UNDEFINED_TIME_NANO;
        }
        readyTimesNanoSrc[i] = readyTimeNano;
    }

    for (size_t i = 0; i < frameCount; i++) {
        (*postedTimesNanoDst)[i] = postedTimesNanoSrc[i];
        (*presentedTimesNanoDst)[i] = presentedTimesNanoSrc[i];
        (*readyTimesNanoDst)[i] = readyTimesNanoSrc[i];
    }

    ((CWindowContentFrameStats*)outStats)->Init(refreshPeriodNano,
             postedTimesNanoDst, presentedTimesNanoDst, readyTimesNanoDst);

    // if (env->ExceptionCheck()) {
    //     return JNI_FALSE;
    // }

    *result = TRUE;
    return NOERROR;
}

ECode SurfaceControl::NativeClearAnimationFrameStats(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    android::status_t err = android::SurfaceComposerClient::clearAnimationFrameStats();
    *result = FALSE;

    if (err < 0 && err != android::NO_INIT) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // The other end is not ready, just report we failed.
    if (err == android::NO_INIT) {
        return NOERROR;
    }

    *result = TRUE;
    return NOERROR;
}

ECode SurfaceControl::NativeGetAnimationFrameStats(
    /* [in] */ IWindowAnimationFrameStats* outStats,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    android::FrameStats stats;
    *result = FALSE;

    android::status_t err = android::SurfaceComposerClient::getAnimationFrameStats(&stats);
    if (err < 0 && err != android::NO_INIT) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // The other end is not ready, fine just return empty stats.
    if (err == android::NO_INIT) {
        return NOERROR;
    }

    Int64 refreshPeriodNano = static_cast<Int64>(stats.refreshPeriodNano);
    size_t frameCount = stats.desiredPresentTimesNano.size();

    AutoPtr<ArrayOf<Int64> > presentedTimesNanoDst = ArrayOf<Int64>::Alloc(frameCount);
    if (presentedTimesNanoDst == NULL) {
        return NOERROR;
    }

    for (size_t i = 0; i < frameCount; i++) {
        nsecs_t presentedTimeNano = stats.actualPresentTimesNano[i];
        if (presentedTimeNano == INT64_MAX) {
            presentedTimeNano = IFrameStats::UNDEFINED_TIME_NANO;
        }
        (*presentedTimesNanoDst)[i] = presentedTimeNano;
    }

    ((CWindowAnimationFrameStats*)outStats)->Init(refreshPeriodNano, presentedTimesNanoDst);

    // if (env->ExceptionCheck()) {
        // return JNI_FALSE;
    // }

    *result = TRUE;
    return NOERROR;
}


AutoPtr<IBinder> SurfaceControl::NativeGetBuiltInDisplay(
    /* [in] */ Int32 physicalDisplayId)
 {
    sp<android::IBinder> token(android::SurfaceComposerClient::getBuiltInDisplay(physicalDisplayId));
    return DroidObjectForIBinder(token);
}

AutoPtr<IBinder> SurfaceControl::NativeCreateDisplay(
    /* [in] */ const String& name,
    /* [in] */ Boolean secure)
{
    bool para = false;
    if (secure) {
        para = true;
    }
    sp<android::IBinder> token(android::SurfaceComposerClient::createDisplay(
            String8(name.string()), para));
    return DroidObjectForIBinder(token);
}

void SurfaceControl::NativeDestroyDisplay(
    /* [in] */ IBinder* displayToken)
{
    sp<android::IBinder> token(IBinderForDroidObject(displayToken));
    if (token == NULL) return;
    android::SurfaceComposerClient::destroyDisplay(token);
}

void SurfaceControl::NativeSetDisplaySurface(
    /* [in] */ IBinder* displayToken,
    /* [in] */ Int64 nativeSurfaceObject)
{
    sp<android::IBinder> token(IBinderForDroidObject(displayToken));
    if (token == NULL) return;
    sp<android::IGraphicBufferProducer> bufferProducer;
    sp<android::Surface> sur(reinterpret_cast<android::Surface *>(nativeSurfaceObject));
    if (sur != NULL) {
        bufferProducer = sur->getIGraphicBufferProducer();
    }
    android::SurfaceComposerClient::setDisplaySurface(token, bufferProducer);
}

void SurfaceControl::NativeSetDisplayLayerStack(
    /* [in] */ IBinder* displayToken,
    /* [in] */ Int32 layerStack)
{
    sp<android::IBinder> token(IBinderForDroidObject(displayToken));
    if (token == NULL) return;

    android::SurfaceComposerClient::setDisplayLayerStack(token, layerStack);
}

void SurfaceControl::NativeSetDisplayProjection(
    /* [in] */ IBinder* displayToken,
    /* [in] */ Int32 orientation,
    /* [in] */ Int32 layerStackRect_left,
    /* [in] */ Int32 layerStackRect_top,
    /* [in] */ Int32 layerStackRect_right,
    /* [in] */ Int32 layerStackRect_bottom,
    /* [in] */ Int32 displayRect_left,
    /* [in] */ Int32 displayRect_top,
    /* [in] */ Int32 displayRect_right,
    /* [in] */ Int32 displayRect_bottom)
{
    sp<android::IBinder> token(IBinderForDroidObject(displayToken));
    if (token == NULL) return;
    android::Rect layerStackRect(layerStackRect_left, layerStackRect_top, layerStackRect_right, layerStackRect_bottom);
    android::Rect displayRect(displayRect_left, displayRect_top, displayRect_right, displayRect_bottom);
    android::SurfaceComposerClient::setDisplayProjection(token, orientation, layerStackRect, displayRect);
}

void SurfaceControl::NativeSetDisplaySize(
    /* [in] */ IBinder* displayToken,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    sp<android::IBinder> token(IBinderForDroidObject(displayToken));
    if (token == NULL) return;
    android::SurfaceComposerClient::setDisplaySize(token, width, height);
}

ECode SurfaceControl::NativeGetDisplayConfigs(
    /* [in] */ IBinder* displayToken,
    /* [out] */ ArrayOf<IPhysicalDisplayInfo*>** infos)
{
    VALIDATE_NOT_NULL(infos)
    *infos = NULL;

    sp<android::IBinder> token(IBinderForDroidObject(displayToken));
    if (token == NULL) {
        Slogger::I(TAG, "NativeGetDisplayConfigs IBinderForDroidObject %s is null.", TO_CSTR(displayToken));
        return NOERROR;
    }

    android::Vector<android::DisplayInfo> configs;
    if (android::SurfaceComposerClient::getDisplayConfigs(token, &configs) != android::NO_ERROR ||
            configs.size() == 0) {
        Slogger::I(TAG, "NativeGetDisplayConfigs getDisplayConfigs: no configs!");
        return NOERROR;
    }

    AutoPtr<ArrayOf<IPhysicalDisplayInfo*> > configArray =
        ArrayOf<IPhysicalDisplayInfo*>::Alloc(configs.size());
    for(size_t c = 0; c < configs.size(); c++) {
        const android::DisplayInfo& nInfo = configs[c];

        AutoPtr<IPhysicalDisplayInfo> info;
        CPhysicalDisplayInfo::New((IPhysicalDisplayInfo**)&info);
        configArray->Set(c, info);
        info->SetWidth(nInfo.w);
        info->SetHeight(nInfo.h);
        info->SetRefreshRate(nInfo.fps);
        info->SetDensity(nInfo.density);
        info->SetXDpi(nInfo.xdpi);
        info->SetYDpi(nInfo.ydpi);
        info->SetSecure(nInfo.secure);
        info->SetAppVsyncOffsetNanos(nInfo.appVsyncOffset);
        info->SetPresentationDeadlineNanos(nInfo.presentationDeadline);
    }

    *infos = configArray;
    REFCOUNT_ADD(*infos)

    return NOERROR;
}

ECode SurfaceControl::NativeGetActiveConfig(
    /* [in] */ IBinder* displayToken,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    sp<android::IBinder> token(IBinderForDroidObject(displayToken));
    if (token == NULL) {
        *res = -1;
        return NOERROR;
    }
    *res = static_cast<Int32>(android::SurfaceComposerClient::getActiveConfig(token));
    return NOERROR;
}

ECode SurfaceControl::NativeSetActiveConfig(
    /* [in] */ IBinder* displayToken,
    /* [in] */ Int32 id,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    sp<android::IBinder> token(IBinderForDroidObject(displayToken));
    if (token == NULL) return NOERROR;
    android::status_t err = android::SurfaceComposerClient::setActiveConfig(token, static_cast<int>(id));
    err == android::NO_ERROR ? *res = TRUE : *res = FALSE;
    return NOERROR;
}

void SurfaceControl::NativeSetDisplayPowerMode(
    /* [in] */ IBinder* displayToken,
    /* [in] */ Int32 mode)
{
    sp<android::IBinder> token(IBinderForDroidObject(displayToken));
    if (token == NULL) return;

    ALOGD_IF_SLOW(100, "Excessive delay in setPowerMode()");
    android::SurfaceComposerClient::setDisplayPowerMode(token, mode);
}
} // namespace View
} // namespace Droid
} // namespace Elastos
