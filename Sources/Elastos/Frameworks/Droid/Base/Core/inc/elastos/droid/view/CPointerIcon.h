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

#ifndef __ELASTOS_DROID_VIEW_CPOINTERICON_H__
#define __ELASTOS_DROID_VIEW_CPOINTERICON_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Graphics.h"
#include "_Elastos_Droid_View_CPointerIcon.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::View::IPointerIcon;

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CPointerIcon)
    , public Object
    , public IPointerIcon
    , public IParcelable
{
public:
    /**
     * Gets a special pointer icon that has no bitmap.
     *
     * @return The NULL pointer icon.
     *
     * @see #STYLE_NULL
     */
    static CARAPI GetNullIcon(
        /* [out] */ IPointerIcon** pointerIcon);

    /**
     * Gets the default pointer icon.
     *
     * @param context The context.
     * @return The default pointer icon.
     *
     * @throws IllegalArgumentException if context is NULL.
     */
    static CARAPI GetDefaultIcon(
        /* [in] */ IContext* context,
        /* [out] */ IPointerIcon** pointerIcon);

    /**
     * Gets a system pointer icon for the given style.
     * If style is not recognized, returns the default pointer icon.
     *
     * @param context The context.
     * @param style The pointer icon style.
     * @return The pointer icon.
     *
     * @throws IllegalArgumentException if context is NULL.
     */
    static CARAPI GetSystemIcon(
        /* [in] */ IContext* context,
        /* [in] */ Int32 style,
        /* [out] */ IPointerIcon** pointerIcon);

    /**
     * Creates a custom pointer from the given bitmap and hotspot information.
     *
     * @param bitmap The bitmap for the icon.
     * @param hotspotX The X offset of the pointer icon hotspot in the bitmap.
     *        Must be within the [0, bitmap.getWidth()) range.
     * @param hotspotY The Y offset of the pointer icon hotspot in the bitmap.
     *        Must be within the [0, bitmap.getHeight()) range.
     * @return A pointer icon for this bitmap.
     *
     * @throws IllegalArgumentException if bitmap is NULL, or if the x/y hotspot
     *         parameters are invalid.
     */
    static CARAPI CreateCustomIcon(
        /* [in] */ IBitmap* bitmap,
        /* [in] */ Float hotSpotX,
        /* [in] */ Float hotSpotY,
        /* [out] */ IPointerIcon** pointerIcon);

    /**
     * Loads a custom pointer icon from an XML resource.
     * <p>
     * The XML resource should have the following form:
     * <code>
     * &lt;?xml version="1.0" encoding="utf-8"?&gt;
     * &lt;pointer-icon xmlns:android="http://schemas.android.com/apk/res/android"
     *   android:bitmap="@drawable/my_pointer_bitmap"
     *   android:hotSpotX="24"
     *   android:hotSpotY="24" /&gt;
     * </code>
     * </p>
     *
     * @param resources The resources object.
     * @param resourceId The resource id.
     * @return The pointer icon.
     *
     * @throws IllegalArgumentException if resources is NULL.
     * @throws Resources.NotFoundException if the resource was not found or the drawable
     * linked in the resource was not found.
     */
    static CARAPI LoadCustomIcon(
        /* [in] */ IResources* resources,
        /* [in] */ Int32 resourceId,
        /* [out] */ IPointerIcon** pointerIcon);

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CPointerIcon();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 style);

    /**
     * Loads the bitmap and hotspot information for a pointer icon, if it is not already loaded.
     * Returns a pointer icon (not necessarily the same instance) with the information filled in.
     *
     * @param context The context.
     * @return The loaded pointer icon.
     *
     * @throws IllegalArgumentException if context is NULL.
     * @see #isLoaded()
     * @hide
     */
    CARAPI Load(
        /* [in] */ IContext* context,
        /* [out] */ IPointerIcon** result);

    /**
     * Returns true if the pointer icon style is {@link #STYLE_NULL}.
     *
     * @return True if the pointer icon style is {@link #STYLE_NULL}.
     */
    CARAPI IsNullIcon(
        /* [out] */ Boolean* isNullIcon);

    /**
     * Returns true if the pointer icon has been loaded and its bitmap and hotspot
     * information are available.
     *
     * @return True if the pointer icon is loaded.
     * @see #load(Context)
     */
    CARAPI IsLoaded(
        /* [out] */ Boolean* isLoaded);

    /**
     * Gets the style of the pointer icon.
     *
     * @return The pointer icon style.
     */
    CARAPI GetStyle(
        /* [out] */ Int32* style);

    /**
     * Gets the bitmap of the pointer icon.
     *
     * @return The pointer icon bitmap, or NULL if the style is {@link #STYLE_NULL}.
     *
     * @throws IllegalStateException if the bitmap is not loaded.
     * @see #isLoaded()
     * @see #load(Context)
     */
    CARAPI GetBitmap(
        /* [out] */ IBitmap** bitmap);

    /**
     * Gets the X offset of the pointer icon hotspot.
     *
     * @return The hotspot X offset.
     *
     * @throws IllegalStateException if the bitmap is not loaded.
     * @see #isLoaded()
     * @see #load(Context)
     */
    CARAPI GetHotSpotX(
        /* [out] */ Float* hotSpotX);

    /**
     * Gets the Y offset of the pointer icon hotspot.
     *
     * @return The hotspot Y offset.
     *
     * @throws IllegalStateException if the bitmap is not loaded.
     * @see #isLoaded()
     * @see #load(Context)
     */
    CARAPI GetHotSpotY(
        /* [out] */ Float* hotSpotY);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* parcel);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

private:
    static CARAPI ValidateHotSpot(
        /* [in] */ IBitmap* bitmap,
        /* [in] */ Float hotSpotX,
        /* [in] */ Float hotSpotY);

    static Int32 GetSystemIconStyleIndex(
        /* [in] */ Int32 style);

private:
    CARAPI ThrowIfIconIsNotLoaded();

    CARAPI LoadResource(
        /* [in] */ IContext* context,
        /* [in] */ IResources* resources,
        /* [in] */ Int32 resourceId);

private:
    // OEM private styles should be defined starting at this range to avoid
    // conflicts with any system styles that may be defined in the future.
    static const Int32 STYLE_OEM_FIRST;

    // The default pointer icon.
    static const Int32 STYLE_DEFAULT;

    static AutoPtr<IPointerIcon> gNullIcon;

private:
    Int32 mStyle;
    Int32 mSystemIconResourceId;
    AutoPtr<IBitmap> mBitmap;
    Float mHotSpotX;
    Float mHotSpotY;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_CPOINTERICON_H__

