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

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/accessibility/CAccessibilityEvent.h"
#include "elastos/droid/view/accessibility/CAccessibilityRecord.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

const String CAccessibilityEvent::TAG("AccessibilityEvent");
const Boolean CAccessibilityEvent::DEBUG = FALSE;
const Int32 CAccessibilityEvent::MAX_POOL_SIZE = 10;
AutoPtr< Pools::SynchronizedPool<IAccessibilityEvent> > CAccessibilityEvent::sPool =
        new Pools::SynchronizedPool<IAccessibilityEvent>(MAX_POOL_SIZE);

CAR_INTERFACE_IMPL_2(CAccessibilityEvent, AccessibilityRecord, IAccessibilityEvent, IParcelable)

CAR_OBJECT_IMPL(CAccessibilityEvent)

CAccessibilityEvent::CAccessibilityEvent()
    : mEventType(0)
    , mEventTime(0)
    , mMovementGranularity(0)
    , mAction(0)
    , mContentChangeTypes(0)
{}

CAccessibilityEvent::~CAccessibilityEvent()
{}

ECode CAccessibilityEvent::constructor()
{
    return NOERROR;
}

ECode CAccessibilityEvent::SetSealed(
    /* [in] */ Boolean sealed)
{
    AccessibilityRecord::SetSealed(sealed);
    AutoPtr<IArrayList> records = mRecords;
    if (records != NULL) {
        Int32 recordCount;
        records->GetSize(&recordCount);
        for (Int32 i = 0; i < recordCount; ++i) {
            AutoPtr<IInterface> obj;
            records->Get(i, (IInterface**)&obj);
            AutoPtr<IAccessibilityRecord> record = IAccessibilityRecord::Probe(obj);
            record->SetSealed(sealed);
        }
    }
    return NOERROR;
}

ECode CAccessibilityEvent::Recycle()
{
    Clear();
    sPool->ReleaseItem(this);
    return NOERROR;
}

void CAccessibilityEvent::Init(
    /* [in] */ IAccessibilityEvent* event)
{
    AutoPtr<IAccessibilityRecord> record = IAccessibilityRecord::Probe(event);
    AccessibilityRecord::Init(record);

    AutoPtr<CAccessibilityEvent> cls = (CAccessibilityEvent*)event;
    mEventType = cls->mEventType;
    mMovementGranularity = cls->mMovementGranularity;
    mAction = cls->mAction;
    mContentChangeTypes = cls->mContentChangeTypes;
    mEventTime = cls->mEventTime;
    mPackageName = cls->mPackageName;
}

ECode CAccessibilityEvent::GetRecordCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = 0;

    if (mRecords != NULL) {
        mRecords->GetSize(count);
    }
    return NOERROR;
}

ECode CAccessibilityEvent::AppendRecord(
    /* [in] */ IAccessibilityRecord* record)
{
    FAIL_RETURN(EnforceNotSealed());
    if (mRecords == NULL) {
        CArrayList::New((IArrayList**)&mRecords);
    }
    mRecords->Add(record);
    return NOERROR;
}

ECode CAccessibilityEvent::GetRecord(
    /* [in] */ Int32 index,
    /* [out] */ IAccessibilityRecord** record)
{
    VALIDATE_NOT_NULL(record);
    *record = NULL;
    if (mRecords == NULL) {
        Slogger::E(TAG, "Invalid index: %d , size is 0", index);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    AutoPtr<IInterface> obj;
    mRecords->Get(index, (IInterface**)&obj);
    *record = IAccessibilityRecord::Probe(obj);
    REFCOUNT_ADD(*record);
    return NOERROR;
}

ECode CAccessibilityEvent::GetEventType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mEventType;
    return NOERROR;
}

ECode CAccessibilityEvent::SetEventType(
    /* [in] */ Int32 eventType)
{
    FAIL_RETURN(EnforceNotSealed());
    mEventType = eventType;
    return NOERROR;
}

ECode CAccessibilityEvent::GetContentChangeTypes(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mContentChangeTypes;
    return NOERROR;
}

ECode CAccessibilityEvent::SetContentChangeTypes(
    /* [in] */ Int32 changeTypes)
{
    FAIL_RETURN(EnforceNotSealed());
    mContentChangeTypes = changeTypes;
    return NOERROR;
}

ECode CAccessibilityEvent::GetEventTime(
    /* [out] */ Int64* eventTime)
{
    VALIDATE_NOT_NULL(eventTime);
    *eventTime = mEventTime;
    return NOERROR;
}

ECode CAccessibilityEvent::SetEventTime(
    /* [in] */ Int64 eventTime)
{
    FAIL_RETURN(EnforceNotSealed());
    mEventTime = eventTime;
    return NOERROR;
}

ECode CAccessibilityEvent::GetPackageName(
    /* [out] */ ICharSequence** packageName)
{
    VALIDATE_NOT_NULL(packageName);
    *packageName = mPackageName;
    REFCOUNT_ADD(*packageName);
    return NOERROR;
}

ECode CAccessibilityEvent::SetPackageName(
    /* [in] */ ICharSequence* packageName)
{
    FAIL_RETURN(EnforceNotSealed());
    mPackageName = packageName;
    return NOERROR;
}

ECode CAccessibilityEvent::SetMovementGranularity(
    /* [in] */ Int32 granularity)
{
    FAIL_RETURN(EnforceNotSealed());
    mMovementGranularity = granularity;
    return NOERROR;
}

ECode CAccessibilityEvent::GetMovementGranularity(
    /* [out] */ Int32* granularity)
{
    VALIDATE_NOT_NULL(granularity);
    *granularity = mMovementGranularity;
    return NOERROR;
}

ECode CAccessibilityEvent::SetAction(
    /* [in] */ Int32 action)
{
    FAIL_RETURN(EnforceNotSealed());
    mAction = action;
    return NOERROR;
}

ECode CAccessibilityEvent::GetAction(
    /* [out] */ Int32* action)
{
    VALIDATE_NOT_NULL(action);
    *action = mAction;
    return NOERROR;
}

ECode CAccessibilityEvent::Obtain(
    /* [in] */ Int32 eventType,
    /* [out] */ IAccessibilityEvent** _event)
{
    VALIDATE_NOT_NULL(_event);
    AutoPtr<IAccessibilityEvent> event;
    Obtain((IAccessibilityEvent**)&event);
    event->SetEventType(eventType);
    *_event = event;
    REFCOUNT_ADD(*_event);
    return NOERROR;
}

ECode CAccessibilityEvent::Obtain(
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ IAccessibilityEvent** resultEvent)
{
    VALIDATE_NOT_NULL(resultEvent);
    AutoPtr<IAccessibilityEvent> eventClone;
    Obtain((IAccessibilityEvent**)&eventClone);
    AutoPtr<CAccessibilityEvent> eventCloneObj = (CAccessibilityEvent*)eventClone.Get();
    eventCloneObj->Init(event);

    AutoPtr<CAccessibilityEvent> eventObj = (CAccessibilityEvent*)event;
    if (eventObj->mRecords != NULL) {
        Int32 recordCount;
        eventObj->mRecords->GetSize(&recordCount);
        CArrayList::New(recordCount, (IArrayList**)&eventCloneObj->mRecords);
        for (Int32 i = 0; i < recordCount; i++) {
            AutoPtr<IInterface> obj;
            eventObj->mRecords->Get(i, (IInterface**)&obj);
            AutoPtr<IAccessibilityRecord> record = IAccessibilityRecord::Probe(obj);
            AutoPtr<IAccessibilityRecord> recordClone;
            AccessibilityRecord::Obtain(record, (IAccessibilityRecord**)&recordClone);
            eventCloneObj->mRecords->Add(recordClone);
        }
    }

    *resultEvent = (IAccessibilityEvent*)eventCloneObj.Get();
    REFCOUNT_ADD(*resultEvent);
    return NOERROR;
}

ECode CAccessibilityEvent::Obtain(
    /* [out] */ IAccessibilityEvent** event)
{
    VALIDATE_NOT_NULL(event);
    AutoPtr<IAccessibilityEvent> _event = sPool->AcquireItem();
    if (_event == NULL) {
        AutoPtr<IAccessibilityEvent> otherEvent;
        CAccessibilityEvent::New((IAccessibilityEvent**)&otherEvent);
        *event = otherEvent;
        REFCOUNT_ADD(*event);
        return NOERROR;
    }

    *event = _event;
    REFCOUNT_ADD(*event);
    return NOERROR;
}

void CAccessibilityEvent::Clear()
{
    AccessibilityRecord::Clear();
    mEventType = 0;
    mMovementGranularity = 0;
    mAction = 0;
    mContentChangeTypes = 0;
    mPackageName = NULL;
    mEventTime = 0;
    if (mRecords != NULL) {
        Boolean res;
        while(!(mRecords->IsEmpty(&res), res)) {
            AutoPtr<IInterface> obj;
            mRecords->Remove(0, (IInterface**)&obj);
            AutoPtr<IAccessibilityRecord> record = IAccessibilityRecord::Probe(obj);
            record->Recycle();
        }
    }
}

void CAccessibilityEvent::ReadAccessibilityRecordFromParcel(
    /* [in] */ IAccessibilityRecord* record,
    /* [in] */ IParcel* parcel)
{
    AccessibilityRecord* cls = reinterpret_cast<AccessibilityRecord*>(record->Probe(EIID_AccessibilityRecord));
    parcel->ReadInt32(&cls->mBooleanProperties);
    parcel->ReadInt32(&cls->mCurrentItemIndex);
    parcel->ReadInt32(&cls->mItemCount);
    parcel->ReadInt32(&cls->mFromIndex);
    parcel->ReadInt32(&cls->mToIndex);
    parcel->ReadInt32(&cls->mScrollX);
    parcel->ReadInt32(&cls->mScrollY);
    parcel->ReadInt32(&cls->mMaxScrollX);
    parcel->ReadInt32(&cls->mMaxScrollY);
    parcel->ReadInt32(&cls->mAddedCount);
    parcel->ReadInt32(&cls->mRemovedCount);
    assert(0);
    // cls->mClassName = TextUtils.CHAR_SEQUENCE_CREATOR.createFromParcel(parcel);
    // cls->mContentDescription = TextUtils.CHAR_SEQUENCE_CREATOR.createFromParcel(parcel);
    // cls->mBeforeText = TextUtils.CHAR_SEQUENCE_CREATOR.createFromParcel(parcel);
    // cls->mParcelableData = parcel.readParcelable(null);
    Int32 count;
    parcel->ReadInt32(&count);
    for (Int32 i = 0; i < count; ++i) {
        AutoPtr<ICharSequence> r;
        parcel->ReadInterfacePtr((Handle32*)(ICharSequence**)&r);
        cls->mText->Add(r);
    }
    parcel->ReadInt32(&cls->mSourceWindowId);
    parcel->ReadInt64(&cls->mSourceNodeId);
    parcel->ReadBoolean(&cls->mSealed);
}

ECode CAccessibilityEvent::WriteToParcel(
    /* [in] */ IParcel* parcel)
{
    parcel->WriteBoolean(IsSealed() ? 1 : 0);
    parcel->WriteInt32(mEventType);
    parcel->WriteInt32(mMovementGranularity);
    parcel->WriteInt32(mAction);
    parcel->WriteInt32(mContentChangeTypes);
    assert(0);
    // TextUtils::WriteToParcel(mPackageName, parcel);
    parcel->WriteInt64(mEventTime);
    parcel->WriteInt32(mConnectionId);
    WriteAccessibilityRecordToParcel(this, parcel);

    // Write the records.
    Int32 recordCount;
    GetRecordCount(&recordCount);
    parcel->WriteInt32(recordCount);
    for (Int32 i = 0; i < recordCount; ++i) {
        AutoPtr<IInterface> obj;
        mRecords->Get(i, (IInterface**)&obj);
        AutoPtr<IAccessibilityRecord> record = IAccessibilityRecord::Probe(obj);
        WriteAccessibilityRecordToParcel(record, parcel);
    }

    return NOERROR;
}

void CAccessibilityEvent::WriteAccessibilityRecordToParcel(
    /* [in] */ IAccessibilityRecord* record,
    /* [in] */ IParcel* parcel)
{
    AccessibilityRecord* cls = reinterpret_cast<AccessibilityRecord*>(record->Probe(EIID_AccessibilityRecord));
    parcel->WriteInt32(cls->mBooleanProperties);
    parcel->WriteInt32(cls->mCurrentItemIndex);
    parcel->WriteInt32(cls->mItemCount);
    parcel->WriteInt32(cls->mFromIndex);
    parcel->WriteInt32(cls->mToIndex);
    parcel->WriteInt32(cls->mScrollX);
    parcel->WriteInt32(cls->mScrollY);
    parcel->WriteInt32(cls->mMaxScrollX);
    parcel->WriteInt32(cls->mMaxScrollY);
    parcel->WriteInt32(cls->mAddedCount);
    parcel->WriteInt32(cls->mRemovedCount);
    // TextUtils.writeToParcel(record.mClassName, parcel, flags);
    // TextUtils.writeToParcel(record.mContentDescription, parcel, flags);
    // TextUtils.writeToParcel(record.mBeforeText, parcel, flags);
    // parcel->WriteParcelable(record.mParcelableData, flags);
    Int32 size;
    cls->mText->GetSize(&size);
    parcel->WriteInt32(size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        cls->mText->Get(i, (IInterface**)&obj);
        AutoPtr<ICharSequence> cs = ICharSequence::Probe(obj);
        parcel->WriteInterfacePtr(cs);
    }
    parcel->WriteInterfacePtr(cls->mText);
    parcel->WriteInt32(cls->mSourceWindowId);
    parcel->WriteInt64(cls->mSourceNodeId);
    parcel->WriteBoolean(cls->mSealed);
}

ECode CAccessibilityEvent::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    parcel->ReadBoolean(&mSealed);
    parcel->ReadInt32(&mEventType);
    parcel->ReadInt32(&mMovementGranularity);
    parcel->ReadInt32(&mAction);
    parcel->ReadInt32(&mContentChangeTypes);
    assert(0);
    // mPackageName = TextUtils.CHAR_SEQUENCE_CREATOR.createFromParcel(parcel);
    parcel->ReadInt64(&mEventTime);
    parcel->ReadInt32(&mConnectionId);
    ReadAccessibilityRecordFromParcel(this, parcel);

    // Read the records.
    Int32 recordCount;
    parcel->ReadInt32(&recordCount);
    if (recordCount > 0) {
        CArrayList::New(recordCount, (IArrayList**)&mRecords);
        for (Int32 i = 0; i < recordCount; i++) {
            AutoPtr<IAccessibilityRecord> record;
            CAccessibilityRecord::Obtain((IAccessibilityRecord**)&record);
            ReadAccessibilityRecordFromParcel(record, parcel);
            ((CAccessibilityRecord*)record.Get())->mConnectionId = mConnectionId;
            mRecords->Add(record);
        }
    }
    return NOERROR;
}

ECode CAccessibilityEvent::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder builder;
    builder.Append("EventType: ");
    builder.Append(EventTypeToString(mEventType));
    builder.Append("; EventTime: ");
    builder.Append(mEventTime);
    builder.Append("; PackageName: ");
    builder.Append(mPackageName);
    builder.Append("; MovementGranularity: ");
    builder.Append(mMovementGranularity);
    builder.Append("; Action: ");
    builder.Append(mAction);
    builder.Append(AccessibilityRecord::ToString());
    Int32 count;
    if (DEBUG) {
        builder.Append("\n");
        builder.Append("; ContentChangeTypes: ");
        builder.Append(mContentChangeTypes);
        builder.Append("; sourceWindowId: ");
        builder.Append(mSourceWindowId);
        builder.Append("; mSourceNodeId: ");
        builder.Append(mSourceNodeId);
        for (Int32 i = 0; i < (GetRecordCount(&count), count); i++) {
            AutoPtr<IAccessibilityRecord> record;
            GetRecord(i, (IAccessibilityRecord**)&record);
            builder.Append("  Record ");
            builder.Append(i);
            builder.Append(":");
            AutoPtr<AccessibilityRecord> _record = (AccessibilityRecord*)record.Get();
            builder.Append(" [ ClassName: ");
            builder.Append(_record->mClassName);
            builder.Append("; Text: ");
            builder.Append((IInterface*)_record->mText);
            builder.Append("; ContentDescription: ");
            builder.Append(_record->mContentDescription);
            builder.Append("; ItemCount: ");
            builder.Append(_record->mItemCount);
            builder.Append("; CurrentItemIndex: ");
            builder.Append(_record->mCurrentItemIndex);
            builder.Append("; IsEnabled: ");
            Boolean res;
            builder.Append((record->IsEnabled(&res), res));
            builder.Append("; IsPassword: ");
            builder.Append((record->IsPassword(&res), res));
            builder.Append("; IsChecked: ");
            builder.Append((record->IsChecked(&res), res));
            builder.Append("; IsFullScreen: ");
            builder.Append((record->IsFullScreen(&res), res));
            builder.Append("; Scrollable: ");
            builder.Append((record->IsScrollable(&res), res));
            builder.Append("; BeforeText: ");
            builder.Append(_record->mBeforeText);
            builder.Append("; FromIndex: ");
            builder.Append(_record->mFromIndex);
            builder.Append("; ToIndex: ");
            builder.Append(_record->mToIndex);
            builder.Append("; ScrollX: ");
            builder.Append(_record->mScrollX);
            builder.Append("; ScrollY: ");
            builder.Append(_record->mScrollY);
            builder.Append("; AddedCount: ");
            builder.Append(_record->mAddedCount);
            builder.Append("; RemovedCount: ");
            builder.Append(_record->mRemovedCount);
            builder.Append("; ParcelableData: ");
            builder.Append((IInterface*)_record->mParcelableData);
            builder.Append(" ]");
            builder.Append("\n");
        }
    }
    else {
        builder.Append("; recordCount: ");
        builder.Append((GetRecordCount(&count), count));
    }
    *str = builder.ToString();
    return NOERROR;
}

String CAccessibilityEvent::EventTypeToString(
    /* [in] */ Int32 eventType)
{
    if (eventType == TYPES_ALL_MASK) {
        return String("TYPES_ALL_MASK");
    }
    StringBuilder builder;
    Int32 eventTypeCount = 0;
    while (eventType != 0) {
        const Int32 eventTypeFlag = 1 << Elastos::Core::Math::NumberOfTrailingZeros(eventType);
        eventType &= ~eventTypeFlag;
        switch (eventTypeFlag) {
            case TYPE_VIEW_CLICKED:
                if (eventTypeCount > 0) {
                    builder.Append(", ");
                }
                builder.Append("TYPE_VIEW_CLICKED");
                eventTypeCount++;
                break;
            case TYPE_VIEW_LONG_CLICKED:
                if (eventTypeCount > 0) {
                    builder.Append(", ");
                }
                builder.Append("TYPE_VIEW_LONG_CLICKED");
                eventTypeCount++;
                break;
            case TYPE_VIEW_SELECTED:
                if (eventTypeCount > 0) {
                    builder.Append(", ");
                }
                builder.Append("TYPE_VIEW_SELECTED");
                eventTypeCount++;
                break;
            case TYPE_VIEW_FOCUSED:
                if (eventTypeCount > 0) {
                    builder.Append(", ");
                }
                builder.Append("TYPE_VIEW_FOCUSED");
                eventTypeCount++;
                break;
            case TYPE_VIEW_TEXT_CHANGED:
                if (eventTypeCount > 0) {
                    builder.Append(", ");
                }
                builder.Append("TYPE_VIEW_TEXT_CHANGED");
                eventTypeCount++;
                break;
            case TYPE_WINDOW_STATE_CHANGED:
                if (eventTypeCount > 0) {
                    builder.Append(", ");
                }
                builder.Append("TYPE_WINDOW_STATE_CHANGED");
                eventTypeCount++;
                break;
            case TYPE_VIEW_HOVER_ENTER:
                if (eventTypeCount > 0) {
                    builder.Append(", ");
                }
                builder.Append("TYPE_VIEW_HOVER_ENTER");
                eventTypeCount++;
                break;
            case TYPE_VIEW_HOVER_EXIT:
                if (eventTypeCount > 0) {
                    builder.Append(", ");
                }
                builder.Append("TYPE_VIEW_HOVER_EXIT");
                eventTypeCount++;
                break;
            case TYPE_NOTIFICATION_STATE_CHANGED:
                if (eventTypeCount > 0) {
                    builder.Append(", ");
                }
                builder.Append("TYPE_NOTIFICATION_STATE_CHANGED");
                eventTypeCount++;
                break;
            case TYPE_TOUCH_EXPLORATION_GESTURE_START:
                if (eventTypeCount > 0) {
                    builder.Append(", ");
                }
                builder.Append("TYPE_TOUCH_EXPLORATION_GESTURE_START");
                eventTypeCount++;
                break;
            case TYPE_TOUCH_EXPLORATION_GESTURE_END:
                if (eventTypeCount > 0) {
                    builder.Append(", ");
                }
                builder.Append("TYPE_TOUCH_EXPLORATION_GESTURE_END");
                eventTypeCount++;
                break;
            case TYPE_WINDOW_CONTENT_CHANGED:
                if (eventTypeCount > 0) {
                    builder.Append(", ");
                }
                builder.Append("TYPE_WINDOW_CONTENT_CHANGED");
                eventTypeCount++;
                break;
            case TYPE_VIEW_TEXT_SELECTION_CHANGED:
                if (eventTypeCount > 0) {
                    builder.Append(", ");
                }
                builder.Append("TYPE_VIEW_TEXT_SELECTION_CHANGED");
                eventTypeCount++;
                break;
            case TYPE_VIEW_SCROLLED:
                if (eventTypeCount > 0) {
                    builder.Append(", ");
                }
                builder.Append("TYPE_VIEW_SCROLLED");
                eventTypeCount++;
                break;
            case TYPE_ANNOUNCEMENT:
                if (eventTypeCount > 0) {
                    builder.Append(", ");
                }
                builder.Append("TYPE_ANNOUNCEMENT");
                eventTypeCount++;
                break;
            case TYPE_VIEW_ACCESSIBILITY_FOCUSED:
                if (eventTypeCount > 0) {
                    builder.Append(", ");
                }
                builder.Append("TYPE_VIEW_ACCESSIBILITY_FOCUSED");
                eventTypeCount++;
                break;
            case TYPE_VIEW_ACCESSIBILITY_FOCUS_CLEARED:
                if (eventTypeCount > 0) {
                    builder.Append(", ");
                }
                builder.Append("TYPE_VIEW_ACCESSIBILITY_FOCUS_CLEARED");
                eventTypeCount++;
                break;
            case TYPE_VIEW_TEXT_TRAVERSED_AT_MOVEMENT_GRANULARITY:
                if (eventTypeCount > 0) {
                    builder.Append(", ");
                }
                builder.Append("TYPE_VIEW_TEXT_TRAVERSED_AT_MOVEMENT_GRANULARITY");
                eventTypeCount++;
                break;
            case TYPE_GESTURE_DETECTION_START:
                if (eventTypeCount > 0) {
                    builder.Append(", ");
                }
                builder.Append("TYPE_GESTURE_DETECTION_START");
                eventTypeCount++;
                break;
            case TYPE_GESTURE_DETECTION_END:
                if (eventTypeCount > 0) {
                    builder.Append(", ");
                }
                builder.Append("TYPE_GESTURE_DETECTION_END");
                eventTypeCount++;
                break;
            case TYPE_TOUCH_INTERACTION_START:
                if (eventTypeCount > 0) {
                    builder.Append(", ");
                }
                builder.Append("TYPE_TOUCH_INTERACTION_START");
                eventTypeCount++;
                break;
            case TYPE_TOUCH_INTERACTION_END:
                if (eventTypeCount > 0) {
                    builder.Append(", ");
                }
                builder.Append("TYPE_TOUCH_INTERACTION_END");
                eventTypeCount++;
                break;
            case TYPE_WINDOWS_CHANGED:
                if (eventTypeCount > 0) {
                    builder.Append(", ");
                }
                builder.Append("TYPE_WINDOWS_CHANGED");
                eventTypeCount++;
                break;
            default:
                break;
        }
    }
    if (eventTypeCount > 1) {
        builder.InsertChar(0, '[');
        builder.AppendChar(']');
    }
    return builder.ToString();
}

} // Accessibility
} // View
} // Droid
} // Elastos
