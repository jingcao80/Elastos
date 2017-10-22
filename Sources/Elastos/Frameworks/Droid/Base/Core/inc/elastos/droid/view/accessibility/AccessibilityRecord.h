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

#ifndef __ELASTOS_DROID_VIEW_ACCESSIBILITY_ACCESSIBILITYRECORD_H__
#define __ELASTOS_DROID_VIEW_ACCESSIBILITY_ACCESSIBILITYRECORD_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>

using Elastos::Droid::View::IView;
using Elastos::Core::ICharSequence;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

class CAccessibilityRecord;

class AccessibilityRecord
    : public Object
    , public IAccessibilityRecord
{
public:
    CAR_INTERFACE_DECL()

    AccessibilityRecord();

    ~AccessibilityRecord();

    CARAPI constructor();

    /**
     * Sets the event source.
     *
     * @param source The source.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetSource(
        /* [in] */ IView* source);

    /**
     * Sets the source to be a virtual descendant of the given <code>root</code>.
     * If <code>virtualDescendantId</code> equals to {@link View#NO_ID} the root
     * is set as the source.
     * <p>
     * A virtual descendant is an imaginary View that is reported as a part of the view
     * hierarchy for accessibility purposes. This enables custom views that draw complex
     * content to report them selves as a tree of virtual views, thus conveying their
     * logical structure.
     * </p>
     *
     * @param root The root of the virtual subtree.
     * @param virtualDescendantId The id of the virtual descendant.
     */
    CARAPI SetSource(
        /* [in] */ IView* root,
        /* [in] */ Int32 virtualDescendantId);

    /**
     * Gets the {@link AccessibilityNodeInfo} of the event source.
     * <p>
     *   <strong>Note:</strong> It is a client responsibility to recycle the received info
     *   by calling {@link AccessibilityNodeInfo#recycle() AccessibilityNodeInfo#recycle()}
     *   to avoid creating of multiple instances.
     * </p>
     * @return The info of the source.
     */
    CARAPI GetSource(
        /* [out] */ IAccessibilityNodeInfo** info);

    /**
     * Sets the window id.
     *
     * @param windowId The window id.
     *
     * @hide
     */
    CARAPI SetWindowId(
        /* [in] */ Int32 windowId);

    /**
     * Gets the id of the window from which the event comes from.
     *
     * @return The window id.
     */
    CARAPI GetWindowId(
        /* [out] */ Int32* windowId);

    /**
     * Gets if the source is checked.
     *
     * @return True if the view is checked, false otherwise.
     */
    CARAPI IsChecked(
        /* [out] */ Boolean* checked);

    /**
     * Sets if the source is checked.
     *
     * @param isChecked True if the view is checked, false otherwise.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetChecked(
        /* [in] */ Boolean isChecked);

    /**
     * Gets if the source is enabled.
     *
     * @return True if the view is enabled, false otherwise.
     */
    CARAPI IsEnabled(
        /* [out] */ Boolean* enabled);

    /**
     * Sets if the source is enabled.
     *
     * @param isEnabled True if the view is enabled, false otherwise.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

    /**
     * Gets if the source is a password field.
     *
     * @return True if the view is a password field, false otherwise.
     */
    CARAPI IsPassword(
        /* [out] */ Boolean* password);

    /**
     * Sets if the source is a password field.
     *
     * @param isPassword True if the view is a password field, false otherwise.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetPassword(
        /* [in] */ Boolean password);

    /**
     * Gets if the source is taking the entire screen.
     *
     * @return True if the source is full screen, false otherwise.
     */
    CARAPI IsFullScreen(
        /* [out] */ Boolean* isFull);

    /**
     * Sets if the source is taking the entire screen.
     *
     * @param isFullScreen True if the source is full screen, false otherwise.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetFullScreen(
        /* [in] */ Boolean full);

    /**
     * Gets if the source is scrollable.
     *
     * @return True if the source is scrollable, false otherwise.
     */
    CARAPI IsScrollable(
        /* [out] */ Boolean* scrollable);

    /**
     * Sets if the source is scrollable.
     *
     * @param scrollable True if the source is scrollable, false otherwise.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetScrollable(
        /* [in] */ Boolean scrollable);

    /**
     * Gets if the source is important for accessibility.
     *
     * <strong>Note:</strong> Used only internally to determine whether
     * to deliver the event to a given accessibility service since some
     * services may want to regard all views for accessibility while others
     * may want to regard only the important views for accessibility.
     *
     * @return True if the source is important for accessibility,
     *        false otherwise.
     *
     * @hide
     */
    CARAPI IsImportantForAccessibility(
        /* [out] */ Boolean* important);

    /**
     * Gets the number of items that can be visited.
     *
     * @return The number of items.
     */
    CARAPI GetItemCount(
        /* [out] */ Int32* count);

    /**
     * Sets the number of items that can be visited.
     *
     * @param itemCount The number of items.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetItemCount(
        /* [in] */ Int32 count);

    /**
     * Gets the index of the source in the list of items the can be visited.
     *
     * @return The current item index.
     */
    CARAPI GetCurrentItemIndex(
        /* [out] */ Int32* index);

    /**
     * Sets the index of the source in the list of items that can be visited.
     *
     * @param currentItemIndex The current item index.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetCurrentItemIndex(
        /* [in] */ Int32 index);

    /**
     * Gets the index of the first character of the changed sequence,
     * or the beginning of a text selection or the index of the first
     * visible item when scrolling.
     *
     * @return The index of the first character or selection
     *        start or the first visible item.
     */
    CARAPI GetFromIndex(
        /* [out] */ Int32* index);

    /**
     * Sets the index of the first character of the changed sequence
     * or the beginning of a text selection or the index of the first
     * visible item when scrolling.
     *
     * @param fromIndex The index of the first character or selection
     *        start or the first visible item.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetFromIndex(
        /* [in] */ Int32 index);

    /**
     * Gets the index of text selection end or the index of the last
     * visible item when scrolling.
     *
     * @return The index of selection end or last item index.
     */
    CARAPI GetToIndex(
        /* [out] */ Int32* index);

    /**
     * Sets the index of text selection end or the index of the last
     * visible item when scrolling.
     *
     * @param toIndex The index of selection end or last item index.
     */
    CARAPI SetToIndex(
        /* [in] */ Int32 index);

    /**
     * Gets the scroll offset of the source left edge in pixels.
     *
     * @return The scroll.
     */
    CARAPI GetScrollX(
        /* [out] */ Int32* x);

    /**
     * Sets the scroll offset of the source left edge in pixels.
     *
     * @param scrollX The scroll.
     */
    CARAPI SetScrollX(
        /* [in] */ Int32 x);

    /**
     * Gets the scroll offset of the source top edge in pixels.
     *
     * @return The scroll.
     */
    CARAPI GetScrollY(
        /* [out] */ Int32* y);

    /**
     * Sets the scroll offset of the source top edge in pixels.
     *
     * @param scrollY The scroll.
     */
    CARAPI SetScrollY(
        /* [in] */ Int32 y);

    /**
     * Gets the max scroll offset of the source left edge in pixels.
     *
     * @return The max scroll.
     */
    CARAPI GetMaxScrollX(
        /* [out] */ Int32* x);

    /**
     * Sets the max scroll offset of the source left edge in pixels.
     *
     * @param maxScrollX The max scroll.
     */
    CARAPI SetMaxScrollX(
        /* [in] */ Int32 x);

    /**
     * Gets the max scroll offset of the source top edge in pixels.
     *
     * @return The max scroll.
     */
    CARAPI GetMaxScrollY(
        /* [out] */ Int32* y);

    /**
     * Sets the max scroll offset of the source top edge in pixels.
     *
     * @param maxScrollY The max scroll.
     */
    CARAPI SetMaxScrollY(
        /* [in] */ Int32 y);

    /**
     * Gets the number of added characters.
     *
     * @return The number of added characters.
     */
    CARAPI GetAddedCount(
        /* [out] */ Int32* count);

    /**
     * Sets the number of added characters.
     *
     * @param addedCount The number of added characters.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetAddedCount(
        /* [in] */ Int32 count);

    /**
     * Gets the number of removed characters.
     *
     * @return The number of removed characters.
     */
    CARAPI GetRemovedCount(
        /* [out] */ Int32* count);

    /**
     * Sets the number of removed characters.
     *
     * @param removedCount The number of removed characters.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetRemovedCount(
        /* [in] */ Int32 count);

    /**
     * Gets the class name of the source.
     *
     * @return The class name.
     */
    CARAPI GetClassName(
        /* [out] */ ICharSequence** name);

    /**
     * Sets the class name of the source.
     *
     * @param className The lass name.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetClassName(
        /* [in] */ ICharSequence* name);

    /**
     * Gets the text of the event. The index in the list represents the priority
     * of the text. Specifically, the lower the index the higher the priority.
     *
     * @return The text.
     */
    CARAPI GetText(
        /* [out] */ IList** list);

    /**
     * Sets the text before a change.
     *
     * @return The text before the change.
     */
    CARAPI GetBeforeText(
        /* [out] */ ICharSequence** text);

    /**
     * Sets the text before a change.
     *
     * @param beforeText The text before the change.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetBeforeText(
        /* [in] */ ICharSequence* text);

    /**
     * Gets the description of the source.
     *
     * @return The description.
     */
    CARAPI GetContentDescription(
        /* [out] */ ICharSequence** contentDescription);

    /**
     * Sets the description of the source.
     *
     * @param contentDescription The description.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetContentDescription(
        /* [in] */ ICharSequence* contentDescription);

    /**
     * Gets the {@link Parcelable} data.
     *
     * @return The parcelable data.
     */
    CARAPI GetParcelableData(
        /* [out] */ IParcelable** parcelableData);

    /**
     * Sets the {@link Parcelable} data of the event.
     *
     * @param parcelableData The parcelable data.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetParcelableData(
        /* [in] */ IParcelable* parcelableData);

    /**
     * Gets the id of the source node.
     *
     * @return The id.
     *
     * @hide
     */
    CARAPI GetSourceNodeId(
        /* [out] */ Int64* nodeId);

    /**
     * Sets the unique id of the IIAccessibilityServiceConnection over which
     * this instance can send requests to the system.
     *
     * @param connectionId The connection id.
     *
     * @hide
     */
    CARAPI SetConnectionId(
        /* [in] */ Int32 connectionId);

    /**
     * Sets if this instance is sealed.
     *
     * @param sealed Whether is sealed.
     *
     * @hide
     */
    CARAPI SetSealed(
        /* [in] */ Boolean sealed);

    /**
     * Gets if this instance is sealed.
     *
     * @return Whether is sealed.
     */
    CARAPI_(Boolean) IsSealed();

    /**
     * Enforces that this instance is sealed.
     *
     * @throws IllegalStateException If this instance is not sealed.
     */
    CARAPI EnforceSealed();

    /**
     * Enforces that this instance is not sealed.
     *
     * @throws IllegalStateException If this instance is sealed.
     */
    CARAPI EnforceNotSealed();

    /**
     * Returns a cached instance if such is available or a new one is
     * instantiated. The instance is initialized with data from the
     * given record.
     *
     * @return An instance.
     */
    static CARAPI Obtain(
        /* [in] */ IAccessibilityRecord* record,
        /* [out] */ IAccessibilityRecord** newInstance);

    /**
     * Returns a cached instance if such is available or a new one is
     * instantiated.
     *
     * @return An instance.
     */
    static CARAPI Obtain(
        /* [out] */ IAccessibilityRecord** newInstance);

    /**
     * Return an instance back to be reused.
     * <p>
     * <strong>Note:</strong> You must not touch the object after calling this function.
     *
     * @throws IllegalStateException If the record is already recycled.
     */
    CARAPI Recycle();

    /**
     * Initialize this record from another one.
     *
     * @param record The to initialize from.
     */
    CARAPI_(void) Init(
        /* [in] */ IAccessibilityRecord* record);

    /**
     * Clears the state of this instance.
     */
    CARAPI_(void) Clear();

    using Object::ToString;

    CARAPI_(String) ToString();

private:
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
     */
    CARAPI_(void) SetBooleanProperty(
        /* [in] */ Int32 property,
        /* [in] */ Boolean value);

public:
    AutoPtr<AccessibilityRecord> mNext;
    Boolean mIsInPool;

    Boolean mSealed;
    Int32 mBooleanProperties;
    Int32 mCurrentItemIndex;
    Int32 mItemCount;
    Int32 mFromIndex;
    Int32 mToIndex;
    Int32 mScrollX;
    Int32 mScrollY;
    Int32 mMaxScrollX;
    Int32 mMaxScrollY;

    Int32 mAddedCount;
    Int32 mRemovedCount;
    Int64 mSourceNodeId;
    Int32 mSourceWindowId;

    AutoPtr<ICharSequence> mClassName;
    AutoPtr<ICharSequence> mContentDescription;
    AutoPtr<ICharSequence> mBeforeText;
    AutoPtr<IParcelable> mParcelableData;

    AutoPtr<IList> mText;

    Int32 mConnectionId;

private:
    static const String TAG;
    static const Int32 UNDEFINED;

    static const Int32 PROPERTY_CHECKED;
    static const Int32 PROPERTY_ENABLED;
    static const Int32 PROPERTY_PASSWORD;
    static const Int32 PROPERTY_FULL_SCREEN;
    static const Int32 PROPERTY_SCROLLABLE;
    static const Int32 PROPERTY_IMPORTANT_FOR_ACCESSIBILITY;

    static const Int32 GET_SOURCE_PREFETCH_FLAGS;

    // Housekeeping
    static const Int32 MAX_POOL_SIZE;
    static Object sPoolLock;
    static AutoPtr<AccessibilityRecord> sPool;
    static Int32 sPoolSize;

    friend class CAccessibilityRecord;
};

} // Accessibility
} // View
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_VIEW_ACCESSIBILITY_ACCESSIBILITYRECORD_H__
