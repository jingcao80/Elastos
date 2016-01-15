
#ifndef __ELASTOS_DROID_VIEW_CPOINTERICONHELPER_H__
#define __ELASTOS_DROID_VIEW_CPOINTERICONHELPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_View_CPointerIconHelper.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CPointerIconHelper)
    , public Object
{
public:
    /**
     * Gets a special pointer icon that has no bitmap.
     *
     * @return The NULL pointer icon.
     *
     * @see #STYLE_NULL
     */
    CARAPI GetNullIcon(
        /* [out] */ IPointerIcon** pointerIcon);

    /**
     * Gets the default pointer icon.
     *
     * @param context The context.
     * @return The default pointer icon.
     *
     * @throws IllegalArgumentException if context is NULL.
     */
    CARAPI GetDefaultIcon(
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
    CARAPI GetSystemIcon(
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
    CARAPI CreateCustomIcon(
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
    CARAPI LoadCustomIcon(
        /* [in] */ IResources* resources,
        /* [in] */ Int32 resourceId,
        /* [out] */ IPointerIcon** pointerIcon);
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_CPOINTERICONHELPER_H__

