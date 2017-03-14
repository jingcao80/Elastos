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

#ifndef __ELASTOS_DROID_VIEW_SURFACECONTROL_H__
#define __ELASTOS_DROID_VIEW_SURFACECONTROL_H__

#include "elastos/droid/view/Surface.h"

using Elastos::Droid::Graphics::IRegion;
using Elastos::Droid::Graphics::IBitmap;

namespace Elastos {
namespace Droid {
namespace View {

class SurfaceControl
    : public Object
    , public ISurfaceControl
{
public:
    CAR_INTERFACE_DECL()

    SurfaceControl();

    ~SurfaceControl();

    CARAPI ReleaseSurfaceControl();

    CARAPI SetLayer(
        /* [in] */ Int32 zorder);

    CARAPI Destroy();

    CARAPI SetPosition(
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI SetSize(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h);

    CARAPI SetBlur(
        /* [in] */ Float blur);

    CARAPI SetBlurMaskSurface(
        /* [in] */ ISurfaceControl* maskSurface);

    CARAPI SetBlurMaskSampling(
        /* [in] */ Int32 blurMaskSampling);

    CARAPI SetBlurMaskAlphaThreshold(
        /* [in] */ Float alpha);

    CARAPI SetTransparent(
        /* [in] */ Boolean isTransparent);

    CARAPI Hide();

    CARAPI Show();

    CARAPI SetTransparentRegionHint(
        /* [in] */ IRegion* region);

    CARAPI ClearContentFrameStats(
        /* [out] */ Boolean* result);

    CARAPI GetContentFrameStats(
        /* [in] */ IWindowContentFrameStats* outStats,
        /* [out] */ Boolean* result);

    CARAPI SetAlpha(
        /* [in] */ Float alpha);

    CARAPI SetMatrix(
        /* [in] */ Float dsdx,
        /* [in] */ Float dtdx,
        /* [in] */ Float dsdy,
        /* [in] */ Float dtdy);

    CARAPI SetWindowCrop(
        /* [in] */ IRect* crop);

    CARAPI SetLayerStack(
        /* [in] */ Int32 layerStack);

    CARAPI SetOpaque(
        /* [in] */ Boolean isOpaque);

    CARAPI ToString(
        /* [out] */ String* str);

    static CARAPI SetDisplayPowerMode(
        /* [in] */ IBinder* displayToken,
        /* [in] */ Int32 mode);

    static CARAPI GetDisplayConfigs(
        /* [in] */ IBinder* displayToken,
        /* [out] */ ArrayOf<IPhysicalDisplayInfo*>** infos);

    static CARAPI GetActiveConfig(
        /* [in] */ IBinder* displayToken,
        /* [out] */ Int32* res);

    static CARAPI SetActiveConfig(
        /* [in] */ IBinder* displayToken,
        /* [in] */ Int32 id,
        /* [out] */ Boolean* res);

    static CARAPI SetDisplayProjection(
        /* [in] */ IBinder* displayToken,
        /* [in] */ Int32 orientation,
        /* [in] */ IRect* layerStackRect,
        /* [in] */ IRect* displayRect);

    static CARAPI SetDisplayLayerStack(
        /* [in] */ IBinder* displayToken,
        /* [in] */ Int32 layerStack);

    static CARAPI SetDisplaySurface(
        /* [in] */ IBinder* displayToken,
        /* [in] */ ISurface* surface);

    static CARAPI SetDisplaySize(
        /* [in] */ IBinder* displayToken,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    static CARAPI CreateDisplay(
        /* [in] */ const String& name,
        /* [in] */ Boolean secure,
        /* [out] */ IBinder** token);

    static CARAPI DestroyDisplay(
        /* [in] */ IBinder* displayToken);

    static CARAPI_(AutoPtr<IBinder>) GetBuiltInDisplay(
        /* [in] */ Int32 builtInDisplayId);
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
    static CARAPI_(void) Screenshot(
        /* [in] */ IBinder* display,
        /* [in] */ ISurface* consumer,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 minLayer,
        /* [in] */ Int32 maxLayer,
        /* [in] */ Boolean useIdentityTransform);

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
    static CARAPI_(void) Screenshot(
        /* [in] */ IBinder* display,
        /* [in] */ ISurface* consumer,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    /**
     * Copy the current screen contents into the provided {@link Surface}
     *
     * @param display The display to take the screenshot of.
     * @param consumer The {@link Surface} to take the screenshot into.
     */
    static CARAPI_(void) Screenshot(
        /* [in] */ IBinder* display,
        /* [in] */ ISurface* consumer);

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
    static CARAPI_(AutoPtr<IBitmap>) Screenshot(
        /* [in] */ IRect* sourceCrop,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 minLayer,
        /* [in] */ Int32 maxLayer,
        /* [in] */ Boolean useIdentityTransform,
        /* [in] */ Int32 rotation);
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
    static CARAPI_(AutoPtr<IBitmap>) Screenshot(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    static CARAPI ClearAnimationFrameStats(
        /* [out] */ Boolean* resutl);

    static CARAPI GetAnimationFrameStats(
        /* [in] */ IWindowAnimationFrameStats* outStats,
        /* [out] */ Boolean* resutl);
    /** start a transaction */

    static CARAPI_(void) OpenTransaction();

    /** end a transaction */
    static CARAPI_(void) CloseTransaction();

    /** flag the transaction as an animation */
    static CARAPI_(void) SetAnimationTransaction();

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
    CARAPI constructor(
        /* [in] */ ISurfaceSession* session,
        /* [in] */ const String& name,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 format,
        /* [in] */ Int32 flags);

private:
    CARAPI CheckNotReleased();

    static CARAPI_(void) Screenshot(
        /* [in] */ IBinder* display,
        /* [in] */ ISurface* consumer,
        /* [in] */ IRect* sourceCrop,
        /* [in] */ Int32 width,
        /* [in] */ Int32  height,
        /* [in] */ Int32  minLayer,
        /* [in] */ Int32  maxLayer,
        /* [in] */ Boolean allLayers,
        /* [in] */ Boolean useIdentityTransform);

    static CARAPI NativeCreate(
        /* [in] */ ISurfaceSession* session,
        /* [in] */ const String& name,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 format,
        /* [in] */ Int32 flags,
        /* [out] */ Int64* ptr);

    static CARAPI_(void) NativeRelease(
        /* [in] */ Int64 nativeObject);

    static CARAPI_(void) NativeDestroy(
        /* [in] */ Int64 nativeObject);

    static CARAPI_(AutoPtr<IBitmap>) NativeScreenshot(
        /* [in] */ IBinder* displayToken,
        /* [in] */ IRect* sourceCrop,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 minLayer,
        /* [in] */ Int32 maxLayer,
        /* [in] */ Boolean allLayers,
        /* [in] */ Boolean useIdentityTransform,
        /* [in] */ Int32 rotation);

    static CARAPI_(void) NativeScreenshot(
        /* [in] */ IBinder* displayToken,
        /* [in] */ ISurface* consumer,
        /* [in] */ IRect* sourceCrop,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 minLayer,
        /* [in] */ Int32 maxLayer,
        /* [in] */ Boolean allLayers,
        /* [in] */ Boolean useIdentityTransform);

    static CARAPI_(void) NativeOpenTransaction();

    static CARAPI_(void) NativeCloseTransaction();

    static CARAPI_(void) NativeSetAnimationTransaction();

    static CARAPI NativeSetLayer(
        /* [in] */ Int64 nativeObject,
        /* [in] */ Int32 zorder);

    static CARAPI NativeSetPosition(
        /* [in] */ Int64 nativeObject,
        /* [in] */ Float x,
        /* [in] */ Float y);

    static CARAPI NativeSetSize(
        /* [in] */ Int64 nativeObject,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h);

    static CARAPI NativeSetTransparentRegionHint(
        /* [in] */ Int64 nativeObject,
        /* [in] */ IRegion* region);

    static CARAPI NativeSetAlpha(
        /* [in] */ Int64 nativeObject,
        /* [in] */ Float alpha);

    static CARAPI NativeSetMatrix(
        /* [in] */ Int64 nativeObject,
        /* [in] */ Float dsdx,
        /* [in] */ Float dtdx,
        /* [in] */ Float dsdy,
        /* [in] */ Float dtdy);

    static CARAPI NativeSetFlags(
        /* [in] */ Int64 nativeObject,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 mask);

    static CARAPI NativeSetWindowCrop(
        /* [in] */ Int64 nativeObject,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    static CARAPI NativeSetLayerStack(
        /* [in] */ Int64 nativeObject,
        /* [in] */ Int32 layerStack);

    static CARAPI NativeSetBlur(
        /* [in] */ Int64 nativeObject,
        /* [in] */ Float blur);

    static CARAPI NativeSetBlurMaskSurface(
        /* [in] */ Int64 nativeObject,
        /* [in] */ Int64 maskLayerNativeObject);

    static CARAPI NativeSetBlurMaskSampling(
        /* [in] */ Int64 nativeObject,
        /* [in] */ Int32 blurMaskSampling);

    static CARAPI NativeSetBlurMaskAlphaThreshold(
        /* [in] */ Int64 nativeObject,
        /* [in] */ Float alpha);

    static CARAPI NativeClearContentFrameStats(
        /* [in] */ Int64 nativeObject,
        /* [out] */ Boolean* resutl);

    static CARAPI NativeGetContentFrameStats(
        /* [in] */ Int64 nativeObject,
        /* [in] */ IWindowContentFrameStats* outStats,
        /* [out] */ Boolean* resutl);

    static CARAPI NativeClearAnimationFrameStats(
        /* [out] */ Boolean* resutl);

    static CARAPI NativeGetAnimationFrameStats(
        /* [in] */ IWindowAnimationFrameStats* outStats,
        /* [out] */ Boolean* resutl);

    static CARAPI_(AutoPtr<IBinder>) NativeGetBuiltInDisplay(
        /* [in] */ Int32 physicalDisplayId);

    static CARAPI_(AutoPtr<IBinder>) NativeCreateDisplay(
        /* [in] */ const String& name,
        /* [in] */ Boolean secure);

    static CARAPI_(void) NativeDestroyDisplay(
        /* [in] */ IBinder* displayToken);

    static CARAPI_(void) NativeSetDisplaySurface(
        /* [in] */ IBinder* displayToken,
        /* [in] */ Int64 nativeSurfaceObject);

    static CARAPI_(void) NativeSetDisplayLayerStack(
        /* [in] */ IBinder* displayToken,
        /* [in] */ Int32 layerStack);

    static CARAPI_(void) NativeSetDisplayProjection(
        /* [in] */ IBinder* displayToken,
        /* [in] */ Int32 orientation,
        /* [in] */ Int32 layerStackRect_left,
        /* [in] */ Int32 layerStackRect_top,
        /* [in] */ Int32 layerStackRect_right,
        /* [in] */ Int32 layerStackRect_bottom,
        /* [in] */ Int32 displayRect_left,
        /* [in] */ Int32 displayRect_top,
        /* [in] */ Int32 displayRect_right,
        /* [in] */ Int32 displayRect_bottom);

    static CARAPI_(void) NativeSetDisplaySize(
        /* [in] */ IBinder* displayToken,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    static CARAPI NativeGetDisplayConfigs(
        /* [in] */ IBinder* displayToken,
        /* [out] */ ArrayOf<IPhysicalDisplayInfo*>** infos);

    static CARAPI NativeGetActiveConfig(
        /* [in] */ IBinder* displayToken,
        /* [out] */ Int32* res);

    static CARAPI NativeSetActiveConfig(
        /* [in] */ IBinder* displayToken,
        /* [in] */ Int32 id,
        /* [out] */ Boolean* res);

    static CARAPI_(void) NativeSetDisplayPowerMode(
        /* [in] */ IBinder* displayToken,
        /* [in] */ Int32 mode);

protected:
    Int64 mNativeObject; // package visibility only for Surface.java access

private:
    static const String TAG;
    AutoPtr<ICloseGuard> mCloseGuard;
    String mName;

    /**
     * Surface flag: Hide the surface.
     * Equivalent to calling hide().
     * Updates the value set during Surface creation (see {@link #HIDDEN}).
     */
    static const Int32 SURFACE_HIDDEN = 0x01;

    /**
     * Surface flag: composite without blending when possible.
     * Updates the value set during Surface creation (see {@link #OPAQUE}).
     */
    static const Int32 SURFACE_OPAQUE = 0x02;

    /**
     * Surface flag: Fully transparent, drop in composition if possible
     * @hide
     */
    static const Int32 SURFACE_TRANSPARENT = 0x80;

    static Object sLock;

    friend class Surface;
};

} // view
} // droid
} // elastos

#endif