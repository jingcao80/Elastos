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

#ifndef __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYEVENT_H__
#define __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYEVENT_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "_Elastos_Droid_View_Accessibility_CAccessibilityEvent.h"
#include "elastos/droid/utility/Pools.h"
#include "elastos/droid/view/accessibility/AccessibilityRecord.h"
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Utility::Pools;
using Elastos::Droid::View::IView;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Etl::List;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CarClass(CAccessibilityEvent)
    , public AccessibilityRecord
    , public IAccessibilityEvent
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CAccessibilityEvent();

    ~CAccessibilityEvent();

    CARAPI constructor();

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
     * Return an instance back to be reused.
     * <p>
     * <strong>Note:</strong> You must not touch the object after calling this function.
     *
     * @throws IllegalStateException If the record is already recycled.
     */
    CARAPI Recycle();

    /**
     * Initialize an event from another one.
     *
     * @param event The event to initialize from.
     */
    CARAPI_(void) Init(
        /* [in] */ IAccessibilityEvent* event);

    /**
     * Gets the number of records contained in the event.
     *
     * @return The number of records.
     */
    CARAPI GetRecordCount(
        /* [out] */ Int32* count);

    /**
     * Appends an {@link AccessibilityRecord} to the end of event records.
     *
     * @param record The record to append.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI AppendRecord(
        /* [in] */ IAccessibilityRecord* record);

    /**
     * Gets the record at a given index.
     *
     * @param index The index.
     * @return The record at the specified index.
     */
    CARAPI GetRecord(
        /* [in] */ Int32 index,
        /* [out] */ IAccessibilityRecord** record);

    /**
     * Gets the event type.
     *
     * @return The event type.
     */
    CARAPI GetEventType(
        /* [out] */ Int32* type);

    /**
     * Sets the event type.
     *
     * @param eventType The event type.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetEventType(
        /* [in] */ Int32 eventType);

    /**
     * Gets the bit mask of change types signaled by an
     * {@link #TYPE_WINDOW_CONTENT_CHANGED} event. A single event may represent
     * multiple change types.
     *
     * @return The bit mask of change types. One or more of:
     *         <ul>
     *         <li>{@link AccessibilityEvent#CONTENT_CHANGE_TYPE_CONTENT_DESCRIPTION}
     *         <li>{@link AccessibilityEvent#CONTENT_CHANGE_TYPE_SUBTREE}
     *         <li>{@link AccessibilityEvent#CONTENT_CHANGE_TYPE_TEXT}
     *         <li>{@link AccessibilityEvent#CONTENT_CHANGE_TYPE_UNDEFINED}
     *         </ul>
     */
    CARAPI GetContentChangeTypes(
        /* [out] */ Int32* type);

    /**
     * Sets the bit mask of node tree changes signaled by an
     * {@link #TYPE_WINDOW_CONTENT_CHANGED} event.
     *
     * @param changeTypes The bit mask of change types.
     * @throws IllegalStateException If called from an AccessibilityService.
     * @see #getContentChangeTypes()
     */
    CARAPI SetContentChangeTypes(
        /* [in] */ Int32 changeTypes);

    /**
     * Gets the time in which this event was sent.
     *
     * @return The event time.
     */
    CARAPI GetEventTime(
        /* [out] */ Int64* eventTime);

    /**
     * Sets the time in which this event was sent.
     *
     * @param eventTime The event time.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetEventTime(
        /* [in] */ Int64 eventTime);

    /**
     * Gets the package name of the source.
     *
     * @return The package name.
     */
    CARAPI GetPackageName(
        /* [out] */ ICharSequence** packageName);

    /**
     * Sets the package name of the source.
     *
     * @param packageName The package name.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetPackageName(
        /* [in] */ ICharSequence* packageName);

    /**
     * Sets the movement granularity that was traversed.
     *
     * @param granularity The granularity.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetMovementGranularity(
        /* [in] */ Int32 granularity);

    /**
     * Gets the movement granularity that was traversed.
     *
     * @return The granularity.
     */
    CARAPI GetMovementGranularity(
        /* [out] */ Int32* granularity);

    /**
     * Sets the performed action that triggered this event.
     * <p>
     * Valid actions are defined in {@link AccessibilityNodeInfo}:
     * <ul>
     * <li>{@link AccessibilityNodeInfo#ACTION_ACCESSIBILITY_FOCUS}
     * <li>{@link AccessibilityNodeInfo#ACTION_CLEAR_ACCESSIBILITY_FOCUS}
     * <li>{@link AccessibilityNodeInfo#ACTION_CLEAR_FOCUS}
     * <li>{@link AccessibilityNodeInfo#ACTION_CLEAR_SELECTION}
     * <li>{@link AccessibilityNodeInfo#ACTION_CLICK}
     * <li>etc.
     * </ul>
     *
     * @param action The action.
     * @throws IllegalStateException If called from an AccessibilityService.
     * @see AccessibilityNodeInfo#performAction(int)
     */
    CARAPI SetAction(
        /* [in] */ Int32 action);

    /**
     * Gets the performed action that triggered this event.
     *
     * @return The action.
     */
    CARAPI GetAction(
        /* [out] */ Int32* action);

    /**
     * Returns a cached instance if such is available or a new one is
     * instantiated with its type property set.
     *
     * @param eventType The event type.
     * @return An instance.
     */
    static CARAPI Obtain(
        /* [in] */ Int32 eventType,
        /* [out] */ IAccessibilityEvent** event);

    /**
     * Returns a cached instance if such is available or a new one is
     * created. The returned instance is initialized from the given
     * <code>event</code>.
     *
     * @param event The other event.
     * @return An instance.
     */
    static CARAPI Obtain(
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ IAccessibilityEvent** resultEvent);

    /**
     * Returns a cached instance if such is available or a new one is
     * instantiated.
     *
     * @return An instance.
     */
    static CARAPI Obtain(
        /* [out] */ IAccessibilityEvent** event);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Returns the string representation of an event type. For example,
     * {@link #TYPE_VIEW_CLICKED} is represented by the string TYPE_VIEW_CLICKED.
     *
     * @param eventType The event type
     * @return The string representation.
     */
    static CARAPI_(String) EventTypeToString(
        /* [in] */ Int32 eventType);

protected:
    /**
     * Clears the state of this instance.
     *
     * @hide
     */
    // @Override
    CARAPI_(void) Clear();

private:
    /**
     * Reads an {@link AccessibilityRecord} from a parcel.
     *
     * @param record The record to initialize.
     * @param parcel The parcel to read from.
     */
    CARAPI_(void) ReadAccessibilityRecordFromParcel(
        /* [in] */ IAccessibilityRecord* record,
        /* [in] */ IParcel* parcel);

    /**
     * Writes an {@link AccessibilityRecord} to a parcel.
     *
     * @param record The record to write.
     * @param parcel The parcel to which to write.
     */
    CARAPI_(void) WriteAccessibilityRecordToParcel(
        /* [in] */ IAccessibilityRecord* record,
        /* [in] */ IParcel* parcel);

private:
    static const String TAG;
    static const Boolean DEBUG;
    static const Int32 MAX_POOL_SIZE;
    static AutoPtr< Pools::SynchronizedPool<IAccessibilityEvent> > sPool;

    Int32 mEventType;
    AutoPtr<ICharSequence> mPackageName;
    Int64 mEventTime;
    Int32 mMovementGranularity;
    Int32 mAction;
    Int32 mContentChangeTypes;

    AutoPtr<IArrayList> mRecords;
};

} // Accessibility
} // View
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYEVENT_H__
