#ifndef __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYWINDOWINFO_H__
#define __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYWINDOWINFO_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "_Elastos_Droid_View_Accessibility_CAccessibilityWindowInfo.h"
#include "elastos/droid/utility/CInt64Array.h"
#include "elastos/droid/utility/Pools.h"

using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Utility::CInt64Array;
using Elastos::Droid::Utility::IInt64Array;
using Elastos::Droid::Utility::Pools;

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CarClass(CAccessibilityWindowInfo)
    , public Object
    , public IAccessibilityWindowInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CAccessibilityWindowInfo();

    ~CAccessibilityWindowInfo();

    CARAPI constructor();

    /**
     * Gets the type of the window.
     *
     * @return The type.
     *
     * @see #TYPE_APPLICATION
     * @see #TYPE_INPUT_METHOD
     * @see #TYPE_SYSTEM
     */
    CARAPI GetType(
        /* [out] */ Int32* type);

    /**
     * Sets the type of the window.
     *
     * @param The type
     *
     * @hide
     */
    CARAPI SetType(
        /* [in] */ Int32 type);

    /**
     * Gets the layer which determines the Z-order of the window. Windows
     * with greater layer appear on top of windows with lesser layer.
     *
     * @return The window layer.
     */
    CARAPI GetLayer(
        /* [out] */ Int32* layer);

    /**
     * Sets the layer which determines the Z-order of the window. Windows
     * with greater layer appear on top of windows with lesser layer.
     *
     * @param The window layer.
     *
     * @hide
     */
    CARAPI SetLayer(
        /* [in] */ Int32 layer);

    /**
     * Gets the root node in the window's hierarchy.
     *
     * @return The root node.
     */
    CARAPI GetRoot(
        /* [out] */ IAccessibilityNodeInfo** info);

    /**
     * Gets the parent window if such.
     *
     * @return The parent window.
     */
    CARAPI GetParent(
        /* [out] */ IAccessibilityWindowInfo** info);

    /**
     * Sets the parent window id.
     *
     * @param parentId The parent id.
     *
     * @hide
     */
    CARAPI SetParentId(
        /* [in] */ Int32 parentId);

    /**
     * Gets the unique window id.
     *
     * @return windowId The window id.
     */
    CARAPI GetId(
        /* [out] */ Int32* id);

    /**
     * Sets the unique window id.
     *
     * @param windowId The window id.
     *
     * @hide
     */
    CARAPI SetId(
        /* [in] */ Int32 id);

    /**
     * Sets the unique id of the IAccessibilityServiceConnection over which
     * this instance can send requests to the system.
     *
     * @param connectionId The connection id.
     *
     * @hide
     */
    CARAPI SetConnectionId(
        /* [in] */ Int32 connectionId);

    /**
     * Gets the bounds of this window in the screen.
     *
     * @param outBounds The out window bounds.
     */
    CARAPI GetBoundsInScreen(
        /* [in, out] */ IRect* outBounds);

    /**
     * Sets the bounds of this window in the screen.
     *
     * @param bounds The out window bounds.
     *
     * @hide
     */
    CARAPI SetBoundsInScreen(
        /* [in] */ IRect* bounds);

    /**
     * Gets if this window is active. An active window is the one
     * the user is currently touching or the window has input focus
     * and the user is not touching any window.
     *
     * @return Whether this is the active window.
     */
    CARAPI IsActive(
        /* [out] */ Boolean* active);

    /**
     * Sets if this window is active, which is this is the window
     * the user is currently touching or the window has input focus
     * and the user is not touching any window.
     *
     * @param Whether this is the active window.
     *
     * @hide
     */
    CARAPI SetActive(
        /* [in] */ Boolean active);

    /**
     * Gets if this window has input focus.
     *
     * @return Whether has input focus.
     */
    CARAPI IsFocused(
        /* [out] */ Boolean* focused);

    /**
     * Sets if this window has input focus.
     *
     * @param Whether has input focus.
     *
     * @hide
     */
    CARAPI SetFocused(
        /* [in] */ Boolean focused);

    /**
     * Gets if this window has accessibility focus.
     *
     * @return Whether has accessibility focus.
     */
    CARAPI IsAccessibilityFocused(
        /* [out] */ Boolean* focused);

    /**
     * Sets if this window has accessibility focus.
     *
     * @param Whether has accessibility focus.
     *
     * @hide
     */
    CARAPI SetAccessibilityFocused(
        /* [in] */ Boolean focused);

    /**
     * Gets the number of child windows.
     *
     * @return The child count.
     */
    CARAPI GetChildCount(
        /* [out] */ Int32* count);

    /**
     * Gets the child window at a given index.
     *
     * @param index The index.
     * @return The child.
     */
    CARAPI GetChild(
        /* [in] */ Int32 index,
        /* [out] */ IAccessibilityWindowInfo** info);

    /**
     * Adds a child window.
     *
     * @param childId The child window id.
     *
     * @hide
     */
    CARAPI AddChild(
        /* [in] */ Int32 childId);

    /**
     * Returns a cached instance if such is available or a new one is
     * created.
     *
     * @return An instance.
     */
    static CARAPI Obtain(
        /* [out] */ IAccessibilityWindowInfo** info);

    /**
     * Returns a cached instance if such is available or a new one is
     * created. The returned instance is initialized from the given
     * <code>info</code>.
     *
     * @param info The other info.
     * @return An instance.
     */
    static CARAPI Obtain(
        /* [in] */ IAccessibilityWindowInfo* info,
        /* [out] */ IAccessibilityWindowInfo** cloneInfo);

    /**
     * Return an instance back to be reused.
     * <p>
     * <strong>Note:</strong> You must not touch the object after calling this function.
     * </p>
     *
     * @throws IllegalStateException If the info is already recycled.
     */
    CARAPI Recycle();

    CARAPI WriteToParcel(
        /* [in] */ IParcel* parcel);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* s);

    /**
     * Checks whether this window changed. The argument should be
     * another state of the same window, which is have the same id
     * and type as they never change.
     *
     * @param other The new state.
     * @return Whether something changed.
     *
     * @hide
     */
    CARAPI Changed(
        /* [in] */ IAccessibilityWindowInfo* other,
        /* [out] */ Boolean* result);

private:
    CARAPI_(void) Clear();

    /**
     * Gets the value of a boolean property.
     *
     * @param property The property.
     * @return The value.
     */
    CARAPI_(Boolean) GetBooleanProperty(
        /* [in] */ Int32 property);

    /**
     * Sets a boolean property.
     *
     * @param property The property.
     * @param value The value.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetBooleanProperty(
        /* [in] */ Int32 property,
        /* [in] */ Boolean value);

    static CARAPI_(String) TypeToString(
        /* [in] */ Int32 type);

private:
    static const Boolean DEBUG;
    static const Int32 UNDEFINED;

    static const Int32 BOOLEAN_PROPERTY_ACTIVE;
    static const Int32 BOOLEAN_PROPERTY_FOCUSED;
    static const Int32 BOOLEAN_PROPERTY_ACCESSIBLITY_FOCUSED;

    Int32 mType;
    Int32 mLayer;
    Int32 mBooleanProperties;
    Int32 mId;
    Int32 mParentId;
    AutoPtr<IRect> mBoundsInScreen;
    AutoPtr<IInt64Array> mChildIds;

    Int32 mConnectionId;

    static const Int32 MAX_POOL_SIZE;
    static AutoPtr< Pools::SynchronizedPool<IAccessibilityWindowInfo> > sPool;
};

} // Accessibility
} // View
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYWINDOWINFO_H__
