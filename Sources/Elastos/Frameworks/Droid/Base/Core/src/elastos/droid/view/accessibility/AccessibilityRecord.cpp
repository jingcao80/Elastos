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
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/accessibility/AccessibilityRecord.h"
#include "elastos/droid/view/accessibility/CAccessibilityRecord.h"
#include "elastos/droid/view/accessibility/CAccessibilityNodeInfo.h"
#include "elastos/droid/view/accessibility/CAccessibilityInteractionClient.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::EIID_IList;
using Elastos::Utility::ICollection;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

// {657EA1A2-91E4-4414-9514-CE42594D9BC5}
extern const InterfaceID EIID_AccessibilityRecord =
        { 0x657ea1a2, 0x91e4, 0x4414, { 0x95, 0x14, 0xce, 0x42, 0x59, 0x4d, 0x9b, 0xc5 } };

const String AccessibilityRecord::TAG("AccessibilityRecord");
const Int32 AccessibilityRecord::UNDEFINED = -1;
const Int32 AccessibilityRecord::PROPERTY_CHECKED = 0x00000001;
const Int32 AccessibilityRecord::PROPERTY_ENABLED = 0x00000002;
const Int32 AccessibilityRecord::PROPERTY_PASSWORD = 0x00000004;
const Int32 AccessibilityRecord::PROPERTY_FULL_SCREEN = 0x00000080;
const Int32 AccessibilityRecord::PROPERTY_SCROLLABLE = 0x00000100;
const Int32 AccessibilityRecord::PROPERTY_IMPORTANT_FOR_ACCESSIBILITY = 0x00000200;
const Int32 AccessibilityRecord::GET_SOURCE_PREFETCH_FLAGS =
        IAccessibilityNodeInfo::FLAG_PREFETCH_PREDECESSORS
        | IAccessibilityNodeInfo::FLAG_PREFETCH_SIBLINGS
        | IAccessibilityNodeInfo::FLAG_PREFETCH_DESCENDANTS;
const Int32 AccessibilityRecord::MAX_POOL_SIZE = 10;

Object AccessibilityRecord::sPoolLock;
AutoPtr<AccessibilityRecord> AccessibilityRecord::sPool;
Int32 AccessibilityRecord::sPoolSize = 0;

CAR_INTERFACE_IMPL(AccessibilityRecord, Object, IAccessibilityRecord)

AccessibilityRecord::AccessibilityRecord()
    : mIsInPool(FALSE)
    , mSealed(FALSE)
    , mBooleanProperties(0)
    , mCurrentItemIndex(UNDEFINED)
    , mItemCount(UNDEFINED)
    , mFromIndex(UNDEFINED)
    , mToIndex(UNDEFINED)
    , mScrollX(UNDEFINED)
    , mScrollY(UNDEFINED)
    , mMaxScrollX(UNDEFINED)
    , mMaxScrollY(UNDEFINED)
    , mAddedCount(UNDEFINED)
    , mRemovedCount(UNDEFINED)
    , mSourceNodeId(UNDEFINED)
    , mSourceWindowId(UNDEFINED)
    , mConnectionId(UNDEFINED)
{
    mSourceNodeId = CAccessibilityNodeInfo::MakeNodeId(UNDEFINED, UNDEFINED);
    CArrayList::New((IList**)&mText);
}

AccessibilityRecord::~AccessibilityRecord()
{}

ECode AccessibilityRecord::constructor()
{
    return NOERROR;
}

ECode AccessibilityRecord::SetSource(
    /* [in] */ IView* source)
{
    return SetSource(source, UNDEFINED);
}

ECode AccessibilityRecord::SetSource(
    /* [in] */ IView* root,
    /* [in] */ Int32 virtualDescendantId)
{
    FAIL_RETURN(EnforceNotSealed());
    Boolean important = TRUE;
    if (virtualDescendantId == UNDEFINED) {
        if (root != NULL) {
            root->IsImportantForAccessibility(&important);
        }
    }
    SetBooleanProperty(PROPERTY_IMPORTANT_FOR_ACCESSIBILITY, important);
    mSourceWindowId = UNDEFINED;
    if (root != NULL) {
        root->GetAccessibilityWindowId(&mSourceWindowId);
    }
    Int32 rootViewId = UNDEFINED;
    if (root != NULL) {
        root->GetAccessibilityViewId(&rootViewId);
    }
    mSourceNodeId = CAccessibilityNodeInfo::MakeNodeId(rootViewId, virtualDescendantId);

    return NOERROR;
}

ECode AccessibilityRecord::GetSource(
    /* [out] */ IAccessibilityNodeInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

    FAIL_RETURN(EnforceSealed());
    if (mConnectionId == UNDEFINED || mSourceWindowId == UNDEFINED
            || CAccessibilityNodeInfo::GetAccessibilityViewId(mSourceNodeId) == UNDEFINED) {
        return NOERROR;
    }

    AutoPtr<IAccessibilityInteractionClient> client = CAccessibilityInteractionClient::GetInstance();
    return client->FindAccessibilityNodeInfoByAccessibilityId(
            mConnectionId, mSourceWindowId,
            mSourceNodeId, FALSE, GET_SOURCE_PREFETCH_FLAGS, info);
}

ECode AccessibilityRecord::SetWindowId(
    /* [in] */ Int32 windowId)
{
    mSourceWindowId = windowId;
    return NOERROR;
}

ECode AccessibilityRecord::GetWindowId(
    /* [out] */ Int32* windowId)
{
    VALIDATE_NOT_NULL(windowId);
    *windowId = mSourceWindowId;
    return NOERROR;
}

ECode AccessibilityRecord::IsChecked(
    /* [out] */ Boolean* checked)
{
    VALIDATE_NOT_NULL(checked);
    *checked = GetBooleanProperty(PROPERTY_CHECKED);
    return NOERROR;
}

ECode AccessibilityRecord::SetChecked(
    /* [in] */ Boolean isChecked)
{
    FAIL_RETURN(EnforceNotSealed());
    SetBooleanProperty(PROPERTY_CHECKED, isChecked);
    return NOERROR;
}

ECode AccessibilityRecord::IsEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = GetBooleanProperty(PROPERTY_ENABLED);
    return NOERROR;
}

ECode AccessibilityRecord::SetEnabled(
    /* [in] */ Boolean enabled)
{
    FAIL_RETURN(EnforceNotSealed());
    SetBooleanProperty(PROPERTY_ENABLED, enabled);
    return NOERROR;
}

ECode AccessibilityRecord::IsPassword(
    /* [out] */ Boolean* password)
{
    VALIDATE_NOT_NULL(password);
    *password = GetBooleanProperty(PROPERTY_PASSWORD);
    return NOERROR;
}

ECode AccessibilityRecord::SetPassword(
    /* [in] */ Boolean password)
{
    FAIL_RETURN(EnforceNotSealed());
    SetBooleanProperty(PROPERTY_PASSWORD, password);
    return NOERROR;
}

ECode AccessibilityRecord::IsFullScreen(
    /* [out] */ Boolean* isFull)
{
    VALIDATE_NOT_NULL(isFull);
    *isFull = GetBooleanProperty(PROPERTY_FULL_SCREEN);
    return NOERROR;
}

ECode AccessibilityRecord::SetFullScreen(
    /* [in] */ Boolean full)
{
    FAIL_RETURN(EnforceNotSealed());
    SetBooleanProperty(PROPERTY_FULL_SCREEN, full);
    return NOERROR;
}

ECode AccessibilityRecord::IsScrollable(
    /* [out] */ Boolean* scrollable)
{
    VALIDATE_NOT_NULL(scrollable);
    *scrollable = GetBooleanProperty(PROPERTY_SCROLLABLE);
    return NOERROR;
}

ECode AccessibilityRecord::SetScrollable(
    /* [in] */ Boolean scrollable)
{
    FAIL_RETURN(EnforceNotSealed());
    SetBooleanProperty(PROPERTY_SCROLLABLE, scrollable);
    return NOERROR;
}

ECode AccessibilityRecord::IsImportantForAccessibility(
    /* [out] */ Boolean* important)
{
    VALIDATE_NOT_NULL(important);
    *important = GetBooleanProperty(PROPERTY_IMPORTANT_FOR_ACCESSIBILITY);
    return NOERROR;
}

ECode AccessibilityRecord::GetItemCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = mItemCount;
    return NOERROR;
}

ECode AccessibilityRecord::SetItemCount(
    /* [in] */ Int32 count)
{
    FAIL_RETURN(EnforceNotSealed());
    mItemCount = count;
    return NOERROR;
}

ECode AccessibilityRecord::GetCurrentItemIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    *index = mCurrentItemIndex;
    return NOERROR;
}

ECode AccessibilityRecord::SetCurrentItemIndex(
    /* [in] */ Int32 index)
{
    FAIL_RETURN(EnforceNotSealed());
    mCurrentItemIndex = index;
    return NOERROR;
}

ECode AccessibilityRecord::GetFromIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    *index = mFromIndex;
    return NOERROR;
}

ECode AccessibilityRecord::SetFromIndex(
    /* [in] */ Int32 index)
{
    FAIL_RETURN(EnforceNotSealed());
    mFromIndex = index;
    return NOERROR;
}

ECode AccessibilityRecord::GetToIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    *index = mToIndex;
    return NOERROR;
}

ECode AccessibilityRecord::SetToIndex(
    /* [in] */ Int32 index)
{
    FAIL_RETURN(EnforceNotSealed());
    mToIndex = index;
    return NOERROR;
}

ECode AccessibilityRecord::GetScrollX(
    /* [out] */ Int32* x)
{
    VALIDATE_NOT_NULL(x);
    *x = mScrollX;
    return NOERROR;
}

ECode AccessibilityRecord::SetScrollX(
    /* [in] */ Int32 x)
{
    FAIL_RETURN(EnforceNotSealed());
    mScrollX = x;
    return NOERROR;
}

ECode AccessibilityRecord::GetScrollY(
    /* [out] */ Int32* y)
{
    VALIDATE_NOT_NULL(y);
    *y = mScrollY;
    return NOERROR;
}

ECode AccessibilityRecord::SetScrollY(
    /* [in] */ Int32 y)
{
    FAIL_RETURN(EnforceNotSealed());
    mScrollY = y;
    return NOERROR;
}

ECode AccessibilityRecord::GetMaxScrollX(
    /* [out] */ Int32* x)
{
    VALIDATE_NOT_NULL(x);
    *x = mMaxScrollX;
    return NOERROR;
}

ECode AccessibilityRecord::SetMaxScrollX(
    /* [in] */ Int32 x)
{
    FAIL_RETURN(EnforceNotSealed());
    mMaxScrollX = x;
    return NOERROR;
}

ECode AccessibilityRecord::GetMaxScrollY(
    /* [out] */ Int32* y)
{
    VALIDATE_NOT_NULL(y);
    *y = mMaxScrollY;
    return NOERROR;
}

ECode AccessibilityRecord::SetMaxScrollY(
    /* [in] */ Int32 y)
{
    FAIL_RETURN(EnforceNotSealed());
    mMaxScrollY = y;
    return NOERROR;
}

ECode AccessibilityRecord::GetAddedCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = mAddedCount;
    return NOERROR;
}

ECode AccessibilityRecord::SetAddedCount(
    /* [in] */ Int32 count)
{
    FAIL_RETURN(EnforceNotSealed());
    mAddedCount = count;
    return NOERROR;
}

ECode AccessibilityRecord::GetRemovedCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = mRemovedCount;
    return NOERROR;
}

ECode AccessibilityRecord::SetRemovedCount(
    /* [in] */ Int32 count)
{
    FAIL_RETURN(EnforceNotSealed());
    mRemovedCount = count;
    return NOERROR;
}

ECode AccessibilityRecord::GetClassName(
    /* [out] */ ICharSequence** name)
{
    VALIDATE_NOT_NULL(name);
    *name = mClassName;
    REFCOUNT_ADD(*name);
    return NOERROR;
}

ECode AccessibilityRecord::SetClassName(
    /* [in] */ ICharSequence* name)
{
    FAIL_RETURN(EnforceNotSealed());
    mClassName = name;
    return NOERROR;
}

ECode AccessibilityRecord::GetText(
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list);
    *list = mText;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

ECode AccessibilityRecord::GetBeforeText(
    /* [out] */ ICharSequence** text)
{
    VALIDATE_NOT_NULL(text);
    *text = mBeforeText;
    REFCOUNT_ADD(*text);
    return NOERROR;
}

ECode AccessibilityRecord::SetBeforeText(
    /* [in] */ ICharSequence* text)
{
    FAIL_RETURN(EnforceNotSealed());
    mBeforeText = text;
    return NOERROR;
}

ECode AccessibilityRecord::GetContentDescription(
    /* [out] */ ICharSequence** contentDescription)
{
    VALIDATE_NOT_NULL(contentDescription);
    *contentDescription = mContentDescription;
    REFCOUNT_ADD(*contentDescription);
    return NOERROR;
}

ECode AccessibilityRecord::SetContentDescription(
    /* [in] */ ICharSequence* contentDescription)
{
    FAIL_RETURN(EnforceNotSealed());
    mContentDescription = contentDescription;
    return NOERROR;
}

ECode AccessibilityRecord::GetParcelableData(
    /* [out] */ IParcelable** parcelableData)
{
    VALIDATE_NOT_NULL(parcelableData);
    *parcelableData = mParcelableData;
    REFCOUNT_ADD(*parcelableData);
    return NOERROR;
}

ECode AccessibilityRecord::SetParcelableData(
    /* [in] */ IParcelable* parcelableData)
{
    FAIL_RETURN(EnforceNotSealed());
    mParcelableData = parcelableData;
    return NOERROR;
}

ECode AccessibilityRecord::GetSourceNodeId(
    /* [out] */ Int64* nodeId)
{
    VALIDATE_NOT_NULL(nodeId);
    *nodeId = mSourceNodeId;
    return NOERROR;
}

ECode AccessibilityRecord::SetConnectionId(
    /* [in] */ Int32 connectionId)
{
    FAIL_RETURN(EnforceNotSealed());
    mConnectionId = connectionId;
    return NOERROR;
}

ECode AccessibilityRecord::SetSealed(
    /* [in] */ Boolean sealed)
{
    FAIL_RETURN(EnforceNotSealed());
    mSealed = sealed;
    return NOERROR;
}

Boolean AccessibilityRecord::IsSealed()
{
    return mSealed;
}

ECode AccessibilityRecord::EnforceSealed()
{
    if (!IsSealed()) {
        Slogger::E(TAG, "Cannot perform this action on a not sealed instance.");
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException("Cannot perform this "
        //         + "action on a not sealed instance.");
    }
    return NOERROR;
}

ECode AccessibilityRecord::EnforceNotSealed()
{
    if (IsSealed()) {
        Slogger::E(TAG, "Cannot perform this action on a sealed instance.");
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException("Cannot perform this "
        //         + "action on a sealed instance.");
    }
    return NOERROR;
}

Boolean AccessibilityRecord::GetBooleanProperty(
    /* [in] */ Int32 property)
{
    return (mBooleanProperties & property) == property;
}

void AccessibilityRecord::SetBooleanProperty(
    /* [in] */ Int32 property,
    /* [in] */ Boolean value)
{
    if (value) {
        mBooleanProperties |= property;
    }
    else {
        mBooleanProperties &= ~property;
    }
}

ECode AccessibilityRecord::Obtain(
    /* [in] */ IAccessibilityRecord* record,
    /* [out] */ IAccessibilityRecord** newInstance)
{
    VALIDATE_NOT_NULL(newInstance);
    AutoPtr<IAccessibilityRecord> clone;
    Obtain((IAccessibilityRecord**)&clone);
    ((AccessibilityRecord*)clone.Get())->Init(record);
    *newInstance = clone;
    REFCOUNT_ADD(*newInstance);
    return NOERROR;
}

ECode AccessibilityRecord::Obtain(
    /* [out] */ IAccessibilityRecord** newInstance)
{
    VALIDATE_NOT_NULL(newInstance);
    {    AutoLock syncLock(sPoolLock);
        if (sPool != NULL) {
            AutoPtr<AccessibilityRecord> record = sPool;
            sPool = sPool->mNext;
            sPoolSize--;
            record->mNext = NULL;
            record->mIsInPool = FALSE;
            *newInstance = (IAccessibilityRecord*)record.Get();
            REFCOUNT_ADD(*newInstance);
            return NOERROR;
        }

        AutoPtr<CAccessibilityRecord> record;
        CAccessibilityRecord::NewByFriend((CAccessibilityRecord**)&record);
        *newInstance = (IAccessibilityRecord*)record.Get();
        REFCOUNT_ADD(*newInstance);
    }
    return NOERROR;
}

ECode AccessibilityRecord::Recycle()
{
    if (mIsInPool) {
        Slogger::E(TAG, "Record already recycled!");
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException("Record already recycled!");
    }
    Clear();
    {    AutoLock syncLock(sPoolLock);
        if (sPoolSize <= MAX_POOL_SIZE) {
            mNext = sPool;
            sPool = this;
            mIsInPool = TRUE;
            sPoolSize++;
        }
    }
    return NOERROR;
}

void AccessibilityRecord::Init(
    /* [in] */ IAccessibilityRecord* record)
{
    AutoPtr<AccessibilityRecord> _record = (AccessibilityRecord*)record;
    mSealed = _record->mSealed;
    mBooleanProperties = _record->mBooleanProperties;
    mCurrentItemIndex = _record->mCurrentItemIndex;
    mItemCount = _record->mItemCount;
    mFromIndex = _record->mFromIndex;
    mToIndex = _record->mToIndex;
    mScrollX = _record->mScrollX;
    mScrollY = _record->mScrollY;
    mMaxScrollX = _record->mMaxScrollX;
    mMaxScrollY = _record->mMaxScrollY;
    mAddedCount = _record->mAddedCount;
    mRemovedCount = _record->mRemovedCount;
    mClassName = _record->mClassName;
    mContentDescription = _record->mContentDescription;
    mBeforeText = _record->mBeforeText;
    mParcelableData = _record->mParcelableData;
    mText->AddAll(ICollection::Probe(_record->mText));
    mSourceWindowId = _record->mSourceWindowId;
    mSourceNodeId = _record->mSourceNodeId;
    mConnectionId = _record->mConnectionId;
}

void AccessibilityRecord::Clear()
{
    mSealed = FALSE;
    mBooleanProperties = 0;
    mCurrentItemIndex = UNDEFINED;
    mItemCount = UNDEFINED;
    mFromIndex = UNDEFINED;
    mToIndex = UNDEFINED;
    mScrollX = UNDEFINED;
    mScrollY = UNDEFINED;
    mMaxScrollX = UNDEFINED;
    mMaxScrollY = UNDEFINED;
    mAddedCount = UNDEFINED;
    mRemovedCount = UNDEFINED;
    mClassName = NULL;
    mContentDescription = NULL;
    mBeforeText = NULL;
    mParcelableData = NULL;
    mText->Clear();
    mSourceNodeId = CAccessibilityNodeInfo::MakeNodeId(UNDEFINED, UNDEFINED);
    mSourceWindowId = UNDEFINED;
    mConnectionId = UNDEFINED;
}

String AccessibilityRecord::ToString()
{
    StringBuilder builder;

    builder.Append(" [ ClassName: ");
    builder.Append(mClassName);
    builder.Append("; Text: ");
    builder.Append((IInterface*)mText);
    builder.Append("; ContentDescription: ");
    builder.Append(mContentDescription);
    builder.Append("; ItemCount: ");
    builder.Append(mItemCount);
    builder.Append("; CurrentItemIndex: ");
    builder.Append(mCurrentItemIndex);
    builder.Append("; IsEnabled: ");
    builder.Append(GetBooleanProperty(PROPERTY_ENABLED));
    builder.Append("; IsPassword: ");
    builder.Append(GetBooleanProperty(PROPERTY_PASSWORD));
    builder.Append("; IsChecked: ");
    builder.Append(GetBooleanProperty(PROPERTY_CHECKED));
    builder.Append("; IsFullScreen: ");
    builder.Append(GetBooleanProperty(PROPERTY_FULL_SCREEN));
    builder.Append("; Scrollable: ");
    builder.Append(GetBooleanProperty(PROPERTY_SCROLLABLE));
    builder.Append("; BeforeText: ");
    builder.Append(mBeforeText);
    builder.Append("; FromIndex: ");
    builder.Append(mFromIndex);
    builder.Append("; ToIndex: ");
    builder.Append(mToIndex);
    builder.Append("; ScrollX: ");
    builder.Append(mScrollX);
    builder.Append("; ScrollY: ");
    builder.Append(mScrollY);
    builder.Append("; MaxScrollX: ");
    builder.Append(mMaxScrollX);
    builder.Append("; MaxScrollY: ");
    builder.Append(mMaxScrollY);
    builder.Append("; AddedCount: ");
    builder.Append(mAddedCount);
    builder.Append("; RemovedCount: ");
    builder.Append(mRemovedCount);
    builder.Append("; ParcelableData: ");
    builder.Append((IInterface*)mParcelableData);
    builder.Append(" ]");

    return builder.ToString();
}

} // Accessibility
} // View
} // Droid
} // Elastos
