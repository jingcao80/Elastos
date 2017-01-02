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
#include "elastos/droid/view/accessibility/CAccessibilityNodeInfo.h"
#include "elastos/droid/view/accessibility/AccessibilityNodeInfoCollectionInfo.h"
#include "elastos/droid/view/accessibility/AccessibilityNodeInfoCollectionItemInfo.h"
#include "elastos/droid/view/accessibility/AccessibilityNodeInfoRangeInfo.h"
#include "elastos/droid/view/accessibility/CAccessibilityNodeInfoAccessibilityAction.h"
#include "elastos/droid/view/accessibility/CAccessibilityInteractionClient.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::IInputType;
using Elastos::Droid::View::Accessibility::IAccessibilityInteractionClient;
using Elastos::Core::Math;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollection;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

const String CAccessibilityNodeInfo::TAG("AccessibilityNodeInfo");
const Boolean CAccessibilityNodeInfo::DEBUG = FALSE;
const Int32 CAccessibilityNodeInfo::LAST_LEGACY_STANDARD_ACTION = ACTION_SET_TEXT;
const Int32 CAccessibilityNodeInfo::ACTION_TYPE_MASK = 0xFF000000;

const Int32 CAccessibilityNodeInfo::BOOLEAN_PROPERTY_CHECKABLE = 0x00000001;
const Int32 CAccessibilityNodeInfo::BOOLEAN_PROPERTY_CHECKED = 0x00000002;
const Int32 CAccessibilityNodeInfo::BOOLEAN_PROPERTY_FOCUSABLE = 0x00000004;
const Int32 CAccessibilityNodeInfo::BOOLEAN_PROPERTY_FOCUSED = 0x00000008;
const Int32 CAccessibilityNodeInfo::BOOLEAN_PROPERTY_SELECTED = 0x00000010;
const Int32 CAccessibilityNodeInfo::BOOLEAN_PROPERTY_CLICKABLE = 0x00000020;
const Int32 CAccessibilityNodeInfo::BOOLEAN_PROPERTY_LONG_CLICKABLE = 0x00000040;
const Int32 CAccessibilityNodeInfo::BOOLEAN_PROPERTY_ENABLED = 0x00000080;
const Int32 CAccessibilityNodeInfo::BOOLEAN_PROPERTY_PASSWORD = 0x00000100;
const Int32 CAccessibilityNodeInfo::BOOLEAN_PROPERTY_SCROLLABLE = 0x00000200;
const Int32 CAccessibilityNodeInfo::BOOLEAN_PROPERTY_ACCESSIBILITY_FOCUSED = 0x00000400;
const Int32 CAccessibilityNodeInfo::BOOLEAN_PROPERTY_VISIBLE_TO_USER = 0x00000800;
const Int32 CAccessibilityNodeInfo::BOOLEAN_PROPERTY_EDITABLE = 0x00001000;
const Int32 CAccessibilityNodeInfo::BOOLEAN_PROPERTY_OPENS_POPUP = 0x00002000;
const Int32 CAccessibilityNodeInfo::BOOLEAN_PROPERTY_DISMISSABLE = 0x00004000;
const Int32 CAccessibilityNodeInfo::BOOLEAN_PROPERTY_MULTI_LINE = 0x00008000;
const Int32 CAccessibilityNodeInfo::BOOLEAN_PROPERTY_CONTENT_INVALID = 0x00010000;
const Int64 CAccessibilityNodeInfo::VIRTUAL_DESCENDANT_ID_MASK = 0xffffffff00000000ll;

const Int32 CAccessibilityNodeInfo::VIRTUAL_DESCENDANT_ID_SHIFT = 32;

const Int32 CAccessibilityNodeInfo::MAX_POOL_SIZE = 50;
AutoPtr<Pools::SynchronizedPool<IAccessibilityNodeInfo> > CAccessibilityNodeInfo::sPool =
        new Pools::SynchronizedPool<IAccessibilityNodeInfo>(MAX_POOL_SIZE);

CAR_INTERFACE_IMPL_2(CAccessibilityNodeInfo, Object, IAccessibilityNodeInfo, IParcelable)

CAR_OBJECT_IMPL(CAccessibilityNodeInfo)

CAccessibilityNodeInfo::CAccessibilityNodeInfo()
    : mSealed(FALSE)
    , mWindowId(UNDEFINED_ITEM_ID)
    , mSourceNodeId(ROOT_NODE_ID)
    , mParentNodeId(ROOT_NODE_ID)
    , mLabelForId(ROOT_NODE_ID)
    , mLabeledById(ROOT_NODE_ID)
    , mBooleanProperties(0)
    , mViewIdResourceName(NULL)
    , mMaxTextLength(-1)
    , mMovementGranularities(0)
    , mTextSelectionStart(UNDEFINED_SELECTION_INDEX)
    , mTextSelectionEnd(UNDEFINED_SELECTION_INDEX)
    , mInputType(IInputType::TYPE_NULL)
    , mLiveRegion(IView::ACCESSIBILITY_LIVE_REGION_NONE)
    , mConnectionId(UNDEFINED_CONNECTION_ID)
{
    ASSERT_SUCCEEDED(CRect::New((IRect**)&mBoundsInParent));
    ASSERT_SUCCEEDED(CRect::New((IRect**)&mBoundsInScreen));
}

CAccessibilityNodeInfo::~CAccessibilityNodeInfo()
{
}

ECode CAccessibilityNodeInfo::constructor()
{
    return NOERROR;
}

Int32 CAccessibilityNodeInfo::GetAccessibilityViewId(
    /* [in] */ Int64 accessibilityNodeId)
{
    return (Int32)accessibilityNodeId;
}

Int32 CAccessibilityNodeInfo::GetVirtualDescendantId(
    /* [in] */ Int64 accessibilityNodeId)
{
    return (Int32)((accessibilityNodeId & VIRTUAL_DESCENDANT_ID_MASK)
            >> VIRTUAL_DESCENDANT_ID_SHIFT);
}

Int64 CAccessibilityNodeInfo::MakeNodeId(
    /* [in] */ Int32 accessibilityViewId,
    /* [in] */ Int32 virtualDescendantId)
{
    // We changed the value for undefined node to positive due to wrong
    // global id composition (two 32-bin ints into one 64-bit long) but
    // the value used for the host node provider view has id -1 so we
    // remap it here.
    if (virtualDescendantId == IAccessibilityNodeProvider::HOST_VIEW_ID) {
        virtualDescendantId = UNDEFINED_ITEM_ID;
    }

    return (((Int64)virtualDescendantId) << VIRTUAL_DESCENDANT_ID_SHIFT)
            | accessibilityViewId;
}

ECode CAccessibilityNodeInfo::SetSource(
    /* [in] */ IView* source)
{
    return SetSource(source, UNDEFINED_ITEM_ID);
}

ECode CAccessibilityNodeInfo::SetSource(
    /* [in] */ IView* root,
    /* [in] */ Int32 virtualDescendantId)
{
    FAIL_RETURN(EnforceNotSealed());
    mWindowId = UNDEFINED_ITEM_ID;
    Int32 rootAccessibilityViewId = UNDEFINED_ITEM_ID;
    if (root != NULL) {
        root->GetAccessibilityWindowId(&mWindowId);
        root->GetAccessibilityViewId(&rootAccessibilityViewId);
    }
    mSourceNodeId = MakeNodeId(rootAccessibilityViewId, virtualDescendantId);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::FindFocus(
    /* [in] */ Int32 focus,
    /* [out] */ IAccessibilityNodeInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

    FAIL_RETURN(EnforceSealed());
    FAIL_RETURN(EnforceValidFocusType(focus));
    if (!CanPerformRequestOverConnection(mSourceNodeId)) {
        return NOERROR;
    }

    AutoPtr<IAccessibilityInteractionClient> client = CAccessibilityInteractionClient::GetInstance();
    return  client->FindFocus(mConnectionId, mWindowId, mSourceNodeId, focus, info);
}

ECode CAccessibilityNodeInfo::FocusSearch(
    /* [in] */ Int32 direction,
    /* [out] */ IAccessibilityNodeInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

    FAIL_RETURN(EnforceSealed());
    FAIL_RETURN(EnforceValidFocusDirection(direction));
    if (!CanPerformRequestOverConnection(mSourceNodeId)) {
        return NOERROR;
    }

    AutoPtr<IAccessibilityInteractionClient> client = CAccessibilityInteractionClient::GetInstance();
    return client->FocusSearch(mConnectionId, mWindowId, mSourceNodeId, direction, info);
}

ECode CAccessibilityNodeInfo::GetWindowId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mWindowId;
    return NOERROR;
}

ECode CAccessibilityNodeInfo::Refresh(
    /* [in] */ Boolean bypassCache,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    FAIL_RETURN(EnforceSealed());
    if (!CanPerformRequestOverConnection(mSourceNodeId)) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IAccessibilityInteractionClient> client = CAccessibilityInteractionClient::GetInstance();
    AutoPtr<IAccessibilityNodeInfo> refreshedInfo;
    client->FindAccessibilityNodeInfoByAccessibilityId(mConnectionId,
            mWindowId, mSourceNodeId, bypassCache, 0, (IAccessibilityNodeInfo**)&refreshedInfo);
    if (refreshedInfo == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    Init(refreshedInfo);
    refreshedInfo->Recycle();
    *result = TRUE;

    return NOERROR;
}

ECode CAccessibilityNodeInfo::Refresh(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return Refresh(FALSE, result);
}

ECode CAccessibilityNodeInfo::GetChildNodeIds(
    /* [out] */ IInt64Array** ids)
{
    VALIDATE_NOT_NULL(ids);
    *ids = mChildNodeIds;
    REFCOUNT_ADD(*ids);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::GetChildId(
    /* [in] */ Int32 index,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);
    *id = 0;
    if (mChildNodeIds == NULL) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    return mChildNodeIds->Get(index, id);
}

ECode CAccessibilityNodeInfo::GetChildCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = 0;

    if (mChildNodeIds != NULL) {
        return mChildNodeIds->GetSize(count);
    }

    return NOERROR;
}

ECode CAccessibilityNodeInfo::GetChild(
    /* [in] */ Int32 index,
    /* [out] */ IAccessibilityNodeInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;
    FAIL_RETURN(EnforceSealed());

    if (mChildNodeIds == NULL) {
        return NOERROR;
    }

    if (!CanPerformRequestOverConnection(mSourceNodeId)) {
        return NOERROR;
    }

    Int64 childId;
    mChildNodeIds->Get(index, &childId);

    AutoPtr<IAccessibilityInteractionClient> client = CAccessibilityInteractionClient::GetInstance();
    return client->FindAccessibilityNodeInfoByAccessibilityId(mConnectionId,
            mWindowId, childId, FALSE, FLAG_PREFETCH_DESCENDANTS, info);
}

ECode CAccessibilityNodeInfo::AddChild(
    /* [in] */ IView* child)
{
    return AddChildInternal(child, UNDEFINED_ITEM_ID, TRUE);
}

ECode CAccessibilityNodeInfo::AddChildUnchecked(
    /* [in] */ IView* child)
{
    return AddChildInternal(child, UNDEFINED_ITEM_ID, FALSE);
}

ECode CAccessibilityNodeInfo::RemoveChild(
    /* [in] */ IView* child,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return RemoveChild(child, UNDEFINED_ITEM_ID, result);
}

ECode CAccessibilityNodeInfo::AddChild(
    /* [in] */ IView* root,
    /* [in] */ Int32 virtualDescendantId)
{
    return AddChildInternal(root, virtualDescendantId, TRUE);
}

ECode CAccessibilityNodeInfo::AddChildInternal(
    /* [in] */ IView* root,
    /* [in] */ Int32 virtualDescendantId,
    /* [in] */ Boolean checked)
{
    FAIL_RETURN(EnforceNotSealed());
    if (mChildNodeIds == NULL) {
        CInt64Array::New((IInt64Array**)&mChildNodeIds);
    }

    Int32 rootAccessibilityViewId = UNDEFINED_ITEM_ID;
    if (root != NULL) {
        root->GetAccessibilityViewId(&rootAccessibilityViewId);
    }
    Int64 childNodeId = MakeNodeId(rootAccessibilityViewId, virtualDescendantId);
    // If we're checking uniqueness and the ID already exists, abort.
    Int32 key;
    mChildNodeIds->IndexOf(childNodeId, &key);

    if (checked && key >= 0) {
        return NOERROR;
    }

    mChildNodeIds->Add(childNodeId);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::RemoveChild(
    /* [in] */ IView* root,
    /* [in] */ Int32 virtualDescendantId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(EnforceNotSealed());
    AutoPtr<IInt64Array> childIds = mChildNodeIds;
    if (childIds == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    Int32 rootAccessibilityViewId = UNDEFINED_ITEM_ID;
    if (root != NULL) {
        root->GetAccessibilityViewId(&rootAccessibilityViewId);
    }

    Int64 childNodeId = MakeNodeId(rootAccessibilityViewId, virtualDescendantId);
    Int32 index;
    childIds->IndexOf(childNodeId, &index);

    if (index < 0) {
        *result = FALSE;
        return NOERROR;
    }

    childIds->Remove(index);
    *result = TRUE;
    return NOERROR;
}

ECode CAccessibilityNodeInfo::GetActionList(
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list);
    if (mActions == NULL) {
        AutoPtr<ICollections> coll;
        CCollections::AcquireSingleton((ICollections**)&coll);
        return coll->GetEmptyList(list);
    }

    *list = IList::Probe(mActions);
    REFCOUNT_ADD(*list);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::GetActions(
    /* [out] */ Int32* actions)
{
    VALIDATE_NOT_NULL(actions);
    Int32 returnValue = 0;

    if (mActions == NULL) {
        *actions = returnValue;
        return NOERROR;
    }

    Int32 actionSize;
    mActions->GetSize(&actionSize);
    for (Int32 i = 0; i < actionSize; i++) {
        AutoPtr<IInterface> obj;
        mActions->Get(i, (IInterface**)&obj);
        Int32 actionId;
        IAccessibilityNodeInfoAccessibilityAction::Probe(obj)->GetId(&actionId);
        if (actionId <= LAST_LEGACY_STANDARD_ACTION) {
            returnValue |= actionId;
        }
    }

    *actions = returnValue;
    return NOERROR;
}

ECode CAccessibilityNodeInfo::AddAction(
    /* [in] */ IAccessibilityNodeInfoAccessibilityAction* action)
{
    FAIL_RETURN(EnforceNotSealed());

    if (action == NULL) {
        return NOERROR;
    }

    if (mActions == NULL) {
        CArrayList::New((IArrayList**)&mActions);
    }

    mActions->Remove(action);
    mActions->Add(action);

    return NOERROR;
}

ECode CAccessibilityNodeInfo::AddAction(
    /* [in] */ Int32 action)
{
    FAIL_RETURN(EnforceNotSealed());
    if ((action & ACTION_TYPE_MASK) != 0) {
        // throw new IllegalArgumentException("Action is not a combination of the standard " +
        //         "actions: " + action);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AddLegacyStandardActions(action);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::RemoveAction(
    /* [in] */ Int32 action)
{
    FAIL_RETURN(EnforceNotSealed());

    Boolean res;
    return RemoveAction(GetActionSingleton(action), &res);
}

ECode CAccessibilityNodeInfo::RemoveAction(
    /* [in] */ IAccessibilityNodeInfoAccessibilityAction* action,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(EnforceNotSealed());

    if (mActions == NULL || action == NULL ) {
        *result = FALSE;
        return NOERROR;
    }

    return mActions->Remove(action, result);
}

ECode CAccessibilityNodeInfo::SetMaxTextLength(
    /* [in] */ Int32 max)
{
    FAIL_RETURN(EnforceNotSealed());
    mMaxTextLength = max;
    return NOERROR;
}

ECode CAccessibilityNodeInfo::GetMaxTextLength(
    /* [out] */ Int32* max)
{
    VALIDATE_NOT_NULL(max);
    *max = mMaxTextLength;
    return NOERROR;
}

ECode CAccessibilityNodeInfo::SetMovementGranularities(
    /* [in] */ Int32 granularities)
{
    FAIL_RETURN(EnforceNotSealed());
    mMovementGranularities = granularities;
    return NOERROR;
}

ECode CAccessibilityNodeInfo::GetMovementGranularities(
    /* [out] */ Int32* granularities)
{
    VALIDATE_NOT_NULL(granularities);
    *granularities = mMovementGranularities;
    return NOERROR;
}

ECode CAccessibilityNodeInfo::PerformAction(
    /* [in] */ Int32 action,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    FAIL_RETURN(EnforceSealed());
    if (!CanPerformRequestOverConnection(mSourceNodeId)) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IAccessibilityInteractionClient> client = CAccessibilityInteractionClient::GetInstance();
    return client->PerformAccessibilityAction(mConnectionId, mWindowId, mSourceNodeId,
            action, NULL, result);
}

ECode CAccessibilityNodeInfo::PerformAction(
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    FAIL_RETURN(EnforceSealed());
    if (!CanPerformRequestOverConnection(mSourceNodeId)) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IAccessibilityInteractionClient> client = CAccessibilityInteractionClient::GetInstance();
    return client->PerformAccessibilityAction(mConnectionId, mWindowId, mSourceNodeId,
            action, arguments, result);
}

ECode CAccessibilityNodeInfo::FindAccessibilityNodeInfosByText(
    /* [in] */ const String& text,
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list);

    FAIL_RETURN(EnforceSealed());
    if (!CanPerformRequestOverConnection(mSourceNodeId)) {
        AutoPtr<ICollections> coll;
        CCollections::AcquireSingleton((ICollections**)&coll);
        return coll->GetEmptyList(list);
    }

    AutoPtr<IAccessibilityInteractionClient> client = CAccessibilityInteractionClient::GetInstance();
    return client->FindAccessibilityNodeInfosByText(
            mConnectionId, mWindowId, mSourceNodeId, text, list);
}

ECode CAccessibilityNodeInfo::FindAccessibilityNodeInfosByViewId(
    /* [in] */ const String& viewId,
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list);

    FAIL_RETURN(EnforceSealed());
    if (!CanPerformRequestOverConnection(mSourceNodeId)) {
        AutoPtr<ICollections> coll;
        CCollections::AcquireSingleton((ICollections**)&coll);
        return coll->GetEmptyList(list);
    }

    AutoPtr<IAccessibilityInteractionClient> client = CAccessibilityInteractionClient::GetInstance();
    return client->FindAccessibilityNodeInfosByViewId(
            mConnectionId, mWindowId, mSourceNodeId, viewId, list);
}

ECode CAccessibilityNodeInfo::GetWindow(
    /* [out] */ IAccessibilityWindowInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

    FAIL_RETURN(EnforceSealed());
    if (!CanPerformRequestOverConnection(mSourceNodeId)) {
        return NOERROR;
    }

    AutoPtr<IAccessibilityInteractionClient> client = CAccessibilityInteractionClient::GetInstance();
    return client->GetWindow(mConnectionId, mWindowId, info);
}

ECode CAccessibilityNodeInfo::GetParent(
    /* [out] */ IAccessibilityNodeInfo** parent)
{
    VALIDATE_NOT_NULL(parent);

    FAIL_RETURN(EnforceSealed());
    if (!CanPerformRequestOverConnection(mParentNodeId)) {
        *parent = NULL;
        return NOERROR;
    }

    AutoPtr<IAccessibilityInteractionClient> client = CAccessibilityInteractionClient::GetInstance();
    return client->FindAccessibilityNodeInfoByAccessibilityId(
            mConnectionId, mWindowId, mParentNodeId, FALSE,
            FLAG_PREFETCH_PREDECESSORS | FLAG_PREFETCH_DESCENDANTS | FLAG_PREFETCH_SIBLINGS, parent);
}

ECode CAccessibilityNodeInfo::GetParentNodeId(
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mParentNodeId;
    return NOERROR;
}

ECode CAccessibilityNodeInfo::SetParent(
    /* [in] */ IView* parent)
{
    return SetParent(parent, UNDEFINED_ITEM_ID);
}

ECode CAccessibilityNodeInfo::SetParent(
    /* [in] */ IView* root,
    /* [in] */ Int32 virtualDescendantId)
{
    FAIL_RETURN(EnforceNotSealed());
    Int32 rootAccessibilityViewId = UNDEFINED_ITEM_ID;
    if (root != NULL) {
        root->GetAccessibilityViewId(&rootAccessibilityViewId);
    }
    mParentNodeId = MakeNodeId(rootAccessibilityViewId, virtualDescendantId);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::GetBoundsInParent(
    /* [in] */ IRect* outBounds)
{
    VALIDATE_NOT_NULL(outBounds);
    Int32 left, top, right, bottom;
    mBoundsInParent->GetLeft(&left);
    mBoundsInParent->GetTop(&top);
    mBoundsInParent->GetRight(&right);
    mBoundsInParent->GetBottom(&bottom);
    outBounds->Set(left, top, right, bottom);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::SetBoundsInParent(
    /* [in] */ IRect* bounds)
{
    FAIL_RETURN(EnforceNotSealed());
    Int32 left, top, right, bottom;
    bounds->GetLeft(&left);
    bounds->GetTop(&top);
    bounds->GetRight(&right);
    bounds->GetBottom(&bottom);
    return mBoundsInParent->Set(left, top, right, bottom);
}

ECode CAccessibilityNodeInfo::GetBoundsInScreen(
    /* [in] */ IRect* outBounds)
{
    VALIDATE_NOT_NULL(outBounds);

    Int32 left, top, right, bottom;
    mBoundsInScreen->GetLeft(&left);
    mBoundsInScreen->GetTop(&top);
    mBoundsInScreen->GetRight(&right);
    mBoundsInScreen->GetBottom(&bottom);
    outBounds->Set(left, top, right, bottom);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::SetBoundsInScreen(
    /* [in] */ IRect* bounds)
{
    FAIL_RETURN(EnforceNotSealed());
    Int32 left, top, right, bottom;
    bounds->GetLeft(&left);
    bounds->GetTop(&top);
    bounds->GetRight(&right);
    bounds->GetBottom(&bottom);
    return mBoundsInScreen->Set(left, top, right, bottom);
}

ECode CAccessibilityNodeInfo::IsCheckable(
    /* [out] */ Boolean* checkable)
{
    VALIDATE_NOT_NULL(checkable);
    *checkable = GetBooleanProperty(BOOLEAN_PROPERTY_CHECKABLE);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::SetCheckable(
    /* [in] */ Boolean checkable)
{
    return SetBooleanProperty(BOOLEAN_PROPERTY_CHECKABLE, checkable);
}

ECode CAccessibilityNodeInfo::IsChecked(
    /* [out] */ Boolean* checked)
{
    VALIDATE_NOT_NULL(checked);
    *checked = GetBooleanProperty(BOOLEAN_PROPERTY_CHECKED);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::SetChecked(
    /* [in] */ Boolean checked)
{
    return SetBooleanProperty(BOOLEAN_PROPERTY_CHECKED, checked);
}

ECode CAccessibilityNodeInfo::IsFocusable(
    /* [out] */ Boolean* focusable)
{
    VALIDATE_NOT_NULL(focusable);
    *focusable = GetBooleanProperty(BOOLEAN_PROPERTY_FOCUSABLE);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::SetFocusable(
    /* [in] */ Boolean focusable)
{
    return SetBooleanProperty(BOOLEAN_PROPERTY_FOCUSABLE, focusable);
}

ECode CAccessibilityNodeInfo::IsFocused(
    /* [out] */ Boolean* focused)
{
    VALIDATE_NOT_NULL(focused);
    *focused = GetBooleanProperty(BOOLEAN_PROPERTY_FOCUSED);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::SetFocused(
    /* [in] */ Boolean focused)
{
    return SetBooleanProperty(BOOLEAN_PROPERTY_FOCUSED, focused);
}

ECode CAccessibilityNodeInfo::IsVisibleToUser(
    /* [out] */ Boolean* visible)
{
    VALIDATE_NOT_NULL(visible);
    *visible = GetBooleanProperty(BOOLEAN_PROPERTY_VISIBLE_TO_USER);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::SetVisibleToUser(
    /* [in] */ Boolean visible)
{
    return SetBooleanProperty(BOOLEAN_PROPERTY_VISIBLE_TO_USER, visible);
}

ECode CAccessibilityNodeInfo::IsAccessibilityFocused(
    /* [out] */ Boolean* focused)
{
    VALIDATE_NOT_NULL(focused);
    *focused = GetBooleanProperty(BOOLEAN_PROPERTY_ACCESSIBILITY_FOCUSED);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::SetAccessibilityFocused(
    /* [in] */ Boolean focused)
{
    return SetBooleanProperty(BOOLEAN_PROPERTY_ACCESSIBILITY_FOCUSED, focused);
}

ECode CAccessibilityNodeInfo::IsSelected(
    /* [out] */ Boolean* selected)
{
    VALIDATE_NOT_NULL(selected);
    *selected = GetBooleanProperty(BOOLEAN_PROPERTY_SELECTED);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::SetSelected(
    /* [in] */ Boolean selected)
{
    return SetBooleanProperty(BOOLEAN_PROPERTY_SELECTED, selected);
}

ECode CAccessibilityNodeInfo::IsClickable(
    /* [out] */ Boolean* clickable)
{
    VALIDATE_NOT_NULL(clickable);
    *clickable = GetBooleanProperty(BOOLEAN_PROPERTY_CLICKABLE);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::SetClickable(
    /* [in] */ Boolean clickable)
{
    return SetBooleanProperty(BOOLEAN_PROPERTY_CLICKABLE, clickable);
}

ECode CAccessibilityNodeInfo::IsLongClickable(
    /* [out] */ Boolean* longClickable)
{
    VALIDATE_NOT_NULL(longClickable);
    *longClickable = GetBooleanProperty(BOOLEAN_PROPERTY_LONG_CLICKABLE);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::SetLongClickable(
    /* [in] */ Boolean longClickable)
{
    return SetBooleanProperty(BOOLEAN_PROPERTY_LONG_CLICKABLE, longClickable);
}

ECode CAccessibilityNodeInfo::IsEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = GetBooleanProperty(BOOLEAN_PROPERTY_ENABLED);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::SetEnabled(
    /* [in] */ Boolean enabled)
{
    return SetBooleanProperty(BOOLEAN_PROPERTY_ENABLED, enabled);
}

ECode CAccessibilityNodeInfo::IsPassword(
    /* [out] */ Boolean* isPassword)
{
    VALIDATE_NOT_NULL(isPassword);
    *isPassword = GetBooleanProperty(BOOLEAN_PROPERTY_PASSWORD);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::SetPassword(
    /* [in] */ Boolean isPassword)
{
    return SetBooleanProperty(BOOLEAN_PROPERTY_PASSWORD, isPassword);
}

ECode CAccessibilityNodeInfo::IsScrollable(
    /* [out] */ Boolean* scrollable)
{
    VALIDATE_NOT_NULL(scrollable);
    *scrollable = GetBooleanProperty(BOOLEAN_PROPERTY_SCROLLABLE);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::SetScrollable(
    /* [in] */ Boolean scrollable)
{
    return SetBooleanProperty(BOOLEAN_PROPERTY_SCROLLABLE, scrollable);
}

ECode CAccessibilityNodeInfo::IsEditable(
    /* [out] */ Boolean* editable)
{
    VALIDATE_NOT_NULL(editable);
    *editable = GetBooleanProperty(BOOLEAN_PROPERTY_EDITABLE);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::SetEditable(
    /* [in] */ Boolean editable)
{
    return SetBooleanProperty(BOOLEAN_PROPERTY_EDITABLE, editable);
}

ECode CAccessibilityNodeInfo::GetCollectionInfo(
    /* [out] */ IAccessibilityNodeInfoCollectionInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = mCollectionInfo;
    REFCOUNT_ADD(*info);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::SetCollectionInfo(
    /* [in] */ IAccessibilityNodeInfoCollectionInfo* collectionInfo)
{
    FAIL_RETURN(EnforceNotSealed())
    mCollectionInfo = collectionInfo;
    return NOERROR;
}

ECode CAccessibilityNodeInfo::GetCollectionItemInfo(
    /* [out] */ IAccessibilityNodeInfoCollectionItemInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = mCollectionItemInfo;
    REFCOUNT_ADD(*info);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::SetCollectionItemInfo(
    /* [in] */ IAccessibilityNodeInfoCollectionItemInfo* collectionItemInfo)
{
    FAIL_RETURN(EnforceNotSealed())
    mCollectionItemInfo = collectionItemInfo;
    return NOERROR;
}

ECode CAccessibilityNodeInfo::GetRangeInfo(
    /* [out] */ IAccessibilityNodeInfoRangeInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = mRangeInfo;
    REFCOUNT_ADD(*info);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::SetRangeInfo(
    /* [in] */ IAccessibilityNodeInfoRangeInfo* rangeInfo)
{
    FAIL_RETURN(EnforceNotSealed())
    mRangeInfo = rangeInfo;
    return NOERROR;
}

ECode CAccessibilityNodeInfo::IsContentInvalid(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = GetBooleanProperty(BOOLEAN_PROPERTY_CONTENT_INVALID);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::SetContentInvalid(
    /* [in] */ Boolean contentInvalid)
{
    return SetBooleanProperty(BOOLEAN_PROPERTY_CONTENT_INVALID, contentInvalid);
}

ECode CAccessibilityNodeInfo::GetLiveRegion(
    /* [out] */ Int32* region)
{
    VALIDATE_NOT_NULL(region);
    *region = mLiveRegion;
    return NOERROR;
}

ECode CAccessibilityNodeInfo::SetLiveRegion(
    /* [in] */ Int32 mode)
{
    FAIL_RETURN(EnforceNotSealed())
    mLiveRegion = mode;
    return NOERROR;
}

ECode CAccessibilityNodeInfo::IsMultiLine(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = GetBooleanProperty(BOOLEAN_PROPERTY_MULTI_LINE);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::SetMultiLine(
    /* [in] */ Boolean multiLine)
{
    return SetBooleanProperty(BOOLEAN_PROPERTY_MULTI_LINE, multiLine);
}

ECode CAccessibilityNodeInfo::CanOpenPopup(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = GetBooleanProperty(BOOLEAN_PROPERTY_OPENS_POPUP);
    return NOERROR;
}


ECode CAccessibilityNodeInfo::SetCanOpenPopup(
    /* [in] */ Boolean opensPopup)
{
    FAIL_RETURN(EnforceNotSealed());
    return SetBooleanProperty(BOOLEAN_PROPERTY_OPENS_POPUP, opensPopup);
}

ECode CAccessibilityNodeInfo::IsDismissable(
    /* [out] */ Boolean* dismissable)
{
    VALIDATE_NOT_NULL(dismissable);
    *dismissable = GetBooleanProperty(BOOLEAN_PROPERTY_DISMISSABLE);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::SetDismissable(
    /* [in] */ Boolean dismissable)
{
    return SetBooleanProperty(BOOLEAN_PROPERTY_DISMISSABLE, dismissable);
}

ECode CAccessibilityNodeInfo::GetPackageName(
    /* [out] */ ICharSequence** packageName)
{
    VALIDATE_NOT_NULL(packageName);
    *packageName = mPackageName;
    REFCOUNT_ADD(*packageName);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::SetPackageName(
    /* [in] */ ICharSequence* packageName)
{
    FAIL_RETURN(EnforceNotSealed());
    mPackageName = packageName;
    return NOERROR;
}

ECode CAccessibilityNodeInfo::GetClassName(
    /* [out] */ ICharSequence** className)
{
    VALIDATE_NOT_NULL(className);
    *className = mClassName;
    REFCOUNT_ADD(*className);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::SetClassName(
    /* [in] */ ICharSequence* className)
{
    FAIL_RETURN(EnforceNotSealed());
    mClassName = className;
    return NOERROR;
}

ECode CAccessibilityNodeInfo::GetText(
    /* [out] */ ICharSequence** text)
{
    VALIDATE_NOT_NULL(text);
    *text = mText;
    REFCOUNT_ADD(*text);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::SetText(
    /* [in] */ ICharSequence* text)
{
    FAIL_RETURN(EnforceNotSealed());
    mText = text;
    return NOERROR;
}

ECode CAccessibilityNodeInfo::SetError(
    /* [in] */ ICharSequence* error)
{
    FAIL_RETURN(EnforceNotSealed());
    mError = error;
    return NOERROR;
}

ECode CAccessibilityNodeInfo::GetError(
    /* [out] */ ICharSequence** error)
{
    VALIDATE_NOT_NULL(error);
    *error = mError;
    REFCOUNT_ADD(*error);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::GetContentDescription(
    /* [out] */ ICharSequence** description)
{
    VALIDATE_NOT_NULL(description);
    *description = mContentDescription;
    REFCOUNT_ADD(*description);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::SetContentDescription(
    /* [in] */ ICharSequence* description)
{
    FAIL_RETURN(EnforceNotSealed());
    mContentDescription = description;
    return NOERROR;
}

ECode CAccessibilityNodeInfo::SetLabelFor(
    /* [in] */ IView* labeled)
{
    return SetLabelFor(labeled, UNDEFINED_ITEM_ID);
}

ECode CAccessibilityNodeInfo::SetLabelFor(
    /* [in] */ IView* root,
    /* [in] */ Int32 virtualDescendantId)
{
    FAIL_RETURN(EnforceNotSealed());
    Int32 rootAccessibilityViewId = UNDEFINED_ITEM_ID;
    if (root != NULL) {
        root->GetAccessibilityViewId(&rootAccessibilityViewId);
    }
    mLabelForId = MakeNodeId(rootAccessibilityViewId, virtualDescendantId);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::GetLabelFor(
    /* [out] */ IAccessibilityNodeInfo** info)
{
    VALIDATE_NOT_NULL(info);
    FAIL_RETURN(EnforceSealed());
    if (!CanPerformRequestOverConnection(mLabelForId)) {
        *info = NULL;
        return NOERROR;
    }

    AutoPtr<IAccessibilityInteractionClient> client = CAccessibilityInteractionClient::GetInstance();
    return client->FindAccessibilityNodeInfoByAccessibilityId(
            mConnectionId, mWindowId, mLabelForId, FALSE,
            FLAG_PREFETCH_PREDECESSORS | FLAG_PREFETCH_DESCENDANTS | FLAG_PREFETCH_SIBLINGS, info);
}

ECode CAccessibilityNodeInfo::SetLabeledBy(
    /* [in] */ IView* label)
{
    return SetLabeledBy(label, UNDEFINED_ITEM_ID);
}

ECode CAccessibilityNodeInfo::SetLabeledBy(
    /* [in] */ IView* root,
    /* [in] */ Int32 virtualDescendantId)
{
    FAIL_RETURN(EnforceNotSealed());
    Int32 rootAccessibilityViewId = UNDEFINED_ITEM_ID;
    if (root != NULL) {
        root->GetAccessibilityViewId(&rootAccessibilityViewId);
    }
    mLabeledById = MakeNodeId(rootAccessibilityViewId, virtualDescendantId);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::GetLabeledBy(
    /* [out] */ IAccessibilityNodeInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

    FAIL_RETURN(EnforceSealed());
    if (!CanPerformRequestOverConnection(mLabeledById)) {

        return NOERROR;
    }

    AutoPtr<IAccessibilityInteractionClient> client = CAccessibilityInteractionClient::GetInstance();
    return client->FindAccessibilityNodeInfoByAccessibilityId(
            mConnectionId, mWindowId, mLabeledById, FALSE,
            FLAG_PREFETCH_PREDECESSORS | FLAG_PREFETCH_DESCENDANTS | FLAG_PREFETCH_SIBLINGS, info);
}

ECode CAccessibilityNodeInfo::SetViewIdResourceName(
    /* [in] */ const String& viewIdResName)
{
    FAIL_RETURN(EnforceNotSealed());
    mViewIdResourceName = viewIdResName;
    return NOERROR;
}

ECode CAccessibilityNodeInfo::GetViewIdResourceName(
    /* [out] */ String* viewIdResName)
{
    VALIDATE_NOT_NULL(viewIdResName);
    *viewIdResName = mViewIdResourceName;
    return NOERROR;
}

ECode CAccessibilityNodeInfo::GetTextSelectionStart(
    /* [out] */ Int32* start)
{
    VALIDATE_NOT_NULL(start);
    *start = mTextSelectionStart;
    return NOERROR;
}

ECode CAccessibilityNodeInfo::GetTextSelectionEnd(
    /* [out] */ Int32* end)
{
    VALIDATE_NOT_NULL(end);
    *end = mTextSelectionEnd;
    return NOERROR;
}

ECode CAccessibilityNodeInfo::SetTextSelection(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    FAIL_RETURN(EnforceNotSealed());
    mTextSelectionStart = start;
    mTextSelectionEnd = end;
    return NOERROR;
}

ECode CAccessibilityNodeInfo::GetInputType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mInputType;
    return NOERROR;
}

ECode CAccessibilityNodeInfo::SetInputType(
    /* [in] */ Int32 inputType)
{
    FAIL_RETURN(EnforceNotSealed());
    mInputType = inputType;
    return NOERROR;
}

ECode CAccessibilityNodeInfo::GetExtras(
    /* [out] */ IBundle** bundle)
{
    VALIDATE_NOT_NULL(bundle);
    if (mExtras == NULL) {
        CBundle::New((IBundle**)&mExtras);
    }

    *bundle = mExtras;
    REFCOUNT_ADD(*bundle);
    return NOERROR;
}

Boolean CAccessibilityNodeInfo::GetBooleanProperty(
    /* [in] */ Int32 property)
{
    return (mBooleanProperties & property) != 0;
}

ECode CAccessibilityNodeInfo::SetBooleanProperty(
    /* [in] */ Int32 property,
    /* [in] */ Boolean value)
{
    FAIL_RETURN(EnforceNotSealed());
    if (value) {
        mBooleanProperties |= property;
    }
    else {
        mBooleanProperties &= ~property;
    }
    return NOERROR;
}

ECode CAccessibilityNodeInfo::SetConnectionId(
    /* [in] */ Int32 connectionId)
{
    FAIL_RETURN(EnforceNotSealed());
    mConnectionId = connectionId;
    return NOERROR;
}

ECode CAccessibilityNodeInfo::GetSourceNodeId(
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mSourceNodeId;
    return NOERROR;
}

ECode CAccessibilityNodeInfo::SetSealed(
    /* [in] */ Boolean sealed)
{
    mSealed = sealed;
    return NOERROR;
}

ECode CAccessibilityNodeInfo::IsSealed(
    /* [out] */ Boolean* sealed)
{
    VALIDATE_NOT_NULL(sealed);
    *sealed = mSealed;
    return NOERROR;
}

ECode CAccessibilityNodeInfo::EnforceSealed()
{
    Boolean sealed;
    if (IsSealed(&sealed), !sealed) {
        Slogger::E(TAG, "Cannot perform this action on a not sealed instance.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalStateException("Cannot perform this "
        //         + "action on a not sealed instance.");
    }
    return NOERROR;
}

ECode CAccessibilityNodeInfo::EnforceValidFocusDirection(
    /* [in] */ Int32 direction)
{
    switch (direction) {
        case IView::FOCUS_DOWN:
        case IView::FOCUS_UP:
        case IView::FOCUS_LEFT:
        case IView::FOCUS_RIGHT:
        case IView::FOCUS_FORWARD:
        case IView::FOCUS_BACKWARD:
            return NOERROR;
        default:
            Slogger::E(TAG, "Unknown direction: %d", direction);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
            // throw new IllegalArgumentException("Unknown direction: " + direction);
    }
}

ECode CAccessibilityNodeInfo::EnforceValidFocusType(
    /* [in] */ Int32 focusType)
{
    switch (focusType) {
        case FOCUS_INPUT:
        case FOCUS_ACCESSIBILITY:
            return NOERROR;
        default:
            Slogger::E(TAG, "Unknown focus type: %d", focusType);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
            // throw new IllegalArgumentException("Unknown focus type: " + focusType);
    }
}

ECode CAccessibilityNodeInfo::EnforceNotSealed()
{
    Boolean sealed;
    if (IsSealed(&sealed), sealed) {
        Slogger::E(TAG, "Cannot perform this action on a sealed instance.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalStateException("Cannot perform this "
        //         + "action on a sealed instance.");
    }
    return NOERROR;
}

ECode CAccessibilityNodeInfo::Obtain(
    /* [in] */ IView* source,
    /* [out] */ IAccessibilityNodeInfo** info)
{
    VALIDATE_NOT_NULL(info);
    Obtain(info);
    return (*info)->SetSource(source);
}

ECode CAccessibilityNodeInfo::Obtain(
    /* [in] */ IView* root,
    /* [in] */ Int32 virtualDescendantId,
    /* [out] */ IAccessibilityNodeInfo** info)
{
    VALIDATE_NOT_NULL(info);
    Obtain(info);
    return (*info)->SetSource(root, virtualDescendantId);
}

ECode CAccessibilityNodeInfo::Obtain(
    /* [out] */ IAccessibilityNodeInfo** info)
{
    VALIDATE_NOT_NULL(info);
    AutoPtr<IAccessibilityNodeInfo> _info = sPool->AcquireItem();
    if (_info == NULL) {
        AutoPtr<IAccessibilityNodeInfo> accInfo;
        CAccessibilityNodeInfo::New((IAccessibilityNodeInfo**)&accInfo);
        *info = accInfo;
        REFCOUNT_ADD(*info);
        return NOERROR;
    }

    *info = _info;
    REFCOUNT_ADD(*info);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::Obtain(
    /* [in] */ IAccessibilityNodeInfo* info,
    /* [out] */ IAccessibilityNodeInfo** infoClone)
{
    VALIDATE_NOT_NULL(infoClone);
    Obtain(infoClone);
    ((CAccessibilityNodeInfo*)(*infoClone))->Init(info);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::Recycle()
{
    Clear();
    sPool->ReleaseItem(this);
    return NOERROR;
}

ECode CAccessibilityNodeInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    Boolean sealed;
    IsSealed(&sealed);
    dest->WriteBoolean(sealed);
    dest->WriteInt64(mSourceNodeId);
    dest->WriteInt32(mWindowId);
    dest->WriteInt64(mParentNodeId);
    dest->WriteInt64(mLabelForId);
    dest->WriteInt64(mLabeledById);
    dest->WriteInt32(mConnectionId);

    AutoPtr<IInt64Array> childIds = mChildNodeIds;
    if (childIds == NULL) {
        dest->WriteInt32(0);
    }
    else {
        Int32 childIdsSize;
        childIds->GetSize(&childIdsSize);
        dest->WriteInt32(childIdsSize);
        for (Int32 i = 0; i < childIdsSize; ++i) {
            Int64 id;
            childIds->Get(i, &id);
            dest->WriteInt64(id);
        }
    }

    AutoPtr<CRect> inParentCls = (CRect*)mBoundsInParent.Get();
    AutoPtr<CRect> inScreenCls = (CRect*)mBoundsInScreen.Get();
    dest->WriteInt32(inParentCls->mTop);
    dest->WriteInt32(inParentCls->mBottom);
    dest->WriteInt32(inParentCls->mLeft);
    dest->WriteInt32(inParentCls->mRight);

    dest->WriteInt32(inScreenCls->mTop);
    dest->WriteInt32(inScreenCls->mBottom);
    dest->WriteInt32(inScreenCls->mLeft);
    dest->WriteInt32(inScreenCls->mRight);

    Boolean res;
    if (mActions != NULL && (mActions->IsEmpty(&res), !res)) {
        Int32 actionCount;
        mActions->GetSize(&actionCount);
        dest->WriteInt32(actionCount);

        Int32 defaultLegacyStandardActions = 0;
        for (Int32 i = 0; i < actionCount; i++) {
            AutoPtr<IInterface> obj;
            mActions->Get(i, (IInterface**)&obj);
            AutoPtr<IAccessibilityNodeInfoAccessibilityAction> action = IAccessibilityNodeInfoAccessibilityAction::Probe(obj);
            if (IsDefaultLegacyStandardAction(action)) {
                Int32 id;
                action->GetId(&id);
                defaultLegacyStandardActions |= id;
            }
        }
        dest->WriteInt32(defaultLegacyStandardActions);

        for (Int32 i = 0; i < actionCount; i++) {
            AutoPtr<IInterface> obj;
            mActions->Get(i, (IInterface**)&obj);
            AutoPtr<IAccessibilityNodeInfoAccessibilityAction> action = IAccessibilityNodeInfoAccessibilityAction::Probe(obj);
            if (!IsDefaultLegacyStandardAction(action)) {
                Int32 id;
                action->GetId(&id);
                dest->WriteInt32(id);
                AutoPtr<ICharSequence> cs;
                action->GetLabel((ICharSequence**)&cs);
                dest->WriteInterfacePtr(cs);
            }
        }
    }
    else {
        dest->WriteInt32(0);
    }

    dest->WriteInt32(mMaxTextLength);
    dest->WriteInt32(mMovementGranularities);
    dest->WriteInt32(mBooleanProperties);

    dest->WriteInterfacePtr(mPackageName);
    dest->WriteInterfacePtr(mClassName);
    dest->WriteInterfacePtr(mText);
    dest->WriteInterfacePtr(mError);
    dest->WriteInterfacePtr(mContentDescription);
    dest->WriteString(mViewIdResourceName);

    dest->WriteInt32(mTextSelectionStart);
    dest->WriteInt32(mTextSelectionEnd);
    dest->WriteInt32(mInputType);
    dest->WriteInt32(mLiveRegion);

    if (mExtras != NULL) {
        dest->WriteInt32(1);
        dest->WriteInterfacePtr(mExtras);
    }
    else {
        dest->WriteInt32(0);
    }

    if (mRangeInfo != NULL) {
        dest->WriteInt32(1);
        Int32 type;
        mRangeInfo->GetType(&type);
        Float min, max, current;
        mRangeInfo->GetMin(&min);
        mRangeInfo->GetMax(&max);
        mRangeInfo->GetCurrent(&current);
        dest->WriteInt32(type);
        dest->WriteFloat(min);
        dest->WriteFloat(max);
        dest->WriteFloat(current);
    }
    else {
        dest->WriteInt32(0);
    }

    if (mCollectionInfo != NULL) {
        dest->WriteInt32(1);
        Int32 rowCount, columnCount, selectionMode;
        Boolean hierarchical;
        mCollectionInfo->GetRowCount(&rowCount);
        mCollectionInfo->GetColumnCount(&columnCount);
        mCollectionInfo->IsHierarchical(&hierarchical);
        mCollectionInfo->GetSelectionMode(&selectionMode);
        dest->WriteInt32(rowCount);
        dest->WriteInt32(columnCount);
        dest->WriteBoolean(hierarchical);
        dest->WriteInt32(selectionMode);
    }
    else {
        dest->WriteInt32(0);
    }

    if (mCollectionItemInfo != NULL) {
        dest->WriteInt32(1);
        Int32 rowIndex, rowSpan, columnIndex, columnSpan;
        Boolean heading, selected;
        mCollectionItemInfo->GetRowIndex(&rowIndex);
        mCollectionItemInfo->GetRowSpan(&rowSpan);
        mCollectionItemInfo->GetColumnIndex(&columnIndex);
        mCollectionItemInfo->GetColumnSpan(&columnSpan);
        mCollectionItemInfo->IsHeading(&heading);
        mCollectionItemInfo->IsSelected(&selected);
        dest->WriteInt32(rowIndex);
        dest->WriteInt32(rowSpan);
        dest->WriteInt32(columnIndex);
        dest->WriteInt32(columnSpan);
        dest->WriteBoolean(heading);
        dest->WriteBoolean(selected);
    }
    else {
        dest->WriteInt32(0);
    }

    // Since instances of this class are fetched via synchronous i.e. blocking
    // calls in IPCs we always recycle as soon as the instance is marshaled.
    return Recycle();
}

ECode CAccessibilityNodeInfo::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    parcel->ReadBoolean(&mSealed);
    parcel->ReadInt64(&mSourceNodeId);
    parcel->ReadInt32(&mWindowId);
    parcel->ReadInt64(&mParentNodeId);
    parcel->ReadInt64(&mLabelForId);
    parcel->ReadInt64(&mLabeledById);
    parcel->ReadInt32(&mConnectionId);

    Int32 childrenSize;
    parcel->ReadInt32(&childrenSize);
    if (childrenSize <= 0) {
        mChildNodeIds = NULL;
    } else {
        CInt64Array::New(childrenSize, (IInt64Array**)&mChildNodeIds);
        for (Int32 i = 0; i < childrenSize; i++) {
            Int64 childId;
            parcel->ReadInt64(&childId);
            mChildNodeIds->Add(childId);
        }
    }

    AutoPtr<CRect> inParentCls = (CRect*)mBoundsInParent.Get();
    AutoPtr<CRect> inScreenCls = (CRect*)mBoundsInScreen.Get();
    parcel->ReadInt32(&inParentCls->mTop);
    parcel->ReadInt32(&inParentCls->mBottom);
    parcel->ReadInt32(&inParentCls->mLeft);
    parcel->ReadInt32(&inParentCls->mRight);

    parcel->ReadInt32(&inScreenCls->mTop);
    parcel->ReadInt32(&inScreenCls->mBottom);
    parcel->ReadInt32(&inScreenCls->mLeft);
    parcel->ReadInt32(&inScreenCls->mRight);

    Int32 actionCount;
    parcel->ReadInt32(&actionCount);
    if (actionCount > 0) {
        Int32 legacyStandardActions;
        parcel->ReadInt32(&legacyStandardActions);
        AddLegacyStandardActions(legacyStandardActions);
        Int32 nonLegacyActionCount = actionCount - Elastos::Core::Math::BitCount(legacyStandardActions);
        for (Int32 i = 0; i < nonLegacyActionCount; ++i) {
            Int32 data;
            parcel->ReadInt32(&data);
            AutoPtr<IInterface> obj;
            parcel->ReadInterfacePtr((Handle32*)&obj);
            AutoPtr<ICharSequence> cs = ICharSequence::Probe(obj);
            AutoPtr<IAccessibilityNodeInfoAccessibilityAction> action;
            CAccessibilityNodeInfoAccessibilityAction::New(data, cs, (IAccessibilityNodeInfoAccessibilityAction**)&action);
            AddAction(action);
        }
    }

    parcel->ReadInt32(&mMaxTextLength);
    parcel->ReadInt32(&mMovementGranularities);
    parcel->ReadInt32(&mBooleanProperties);

    AutoPtr<IInterface> obj;
    parcel->ReadInterfacePtr((Handle32*)&obj);
    mPackageName = ICharSequence::Probe(obj);
    parcel->ReadInterfacePtr((Handle32*)&obj);
    mClassName = ICharSequence::Probe(obj);
    parcel->ReadInterfacePtr((Handle32*)&obj);
    mText = ICharSequence::Probe(obj);
    parcel->ReadInterfacePtr((Handle32*)&obj);
    mError = ICharSequence::Probe(obj);
    parcel->ReadInterfacePtr((Handle32*)&obj);
    mContentDescription = ICharSequence::Probe(obj);
    parcel->ReadString(&mViewIdResourceName);

    parcel->ReadInt32(&mTextSelectionStart);
    parcel->ReadInt32(&mTextSelectionEnd);

    parcel->ReadInt32(&mInputType);
    parcel->ReadInt32(&mLiveRegion);

    Int32 data;
    parcel->ReadInt32(&data);
    if (data == 1) {
        AutoPtr<IInterface> obj;
        parcel->ReadInterfacePtr((Handle32*)&obj);
        AutoPtr<IBundle> btmp = IBundle::Probe(obj);
        AutoPtr<IBundle> bundle;
        GetExtras((IBundle**)&bundle);
        bundle->PutAll(btmp);
    }

    parcel->ReadInt32(&data);
    if (data == 1) {
        Int32 type;
        Float min, max, current;
        parcel->ReadInt32(&type);
        parcel->ReadFloat(&min);
        parcel->ReadFloat(&max);
        parcel->ReadFloat(&current);
        AccessibilityNodeInfoRangeInfo::Obtain(type, min, max, current, (IAccessibilityNodeInfoRangeInfo**)&mRangeInfo);
    }

    parcel->ReadInt32(&data);
    if (data == 1) {
        Int32 rowCount, columnCount, selectionMode;
        Boolean hierarchical;
        parcel->ReadInt32(&rowCount);
        parcel->ReadInt32(&columnCount);
        parcel->ReadBoolean(&hierarchical);
        parcel->ReadInt32(&selectionMode);
        AccessibilityNodeInfoCollectionInfo::Obtain(rowCount, columnCount, hierarchical, selectionMode, (IAccessibilityNodeInfoCollectionInfo**)&mCollectionInfo);
    }

    parcel->ReadInt32(&data);
    if (data == 1) {
        Int32 rowIndex, rowSpan, columnIndex, columnSpan;
        Boolean heading, selected;
        parcel->ReadInt32(&rowIndex);
        parcel->ReadInt32(&rowSpan);
        parcel->ReadInt32(&columnIndex);
        parcel->ReadInt32(&columnSpan);
        parcel->ReadBoolean(&heading);
        parcel->ReadBoolean(&selected);
        AccessibilityNodeInfoCollectionItemInfo::Obtain(rowIndex, rowSpan, columnIndex, columnSpan,
                heading, selected, (IAccessibilityNodeInfoCollectionItemInfo**)&mCollectionItemInfo);
    }
    return NOERROR;
}

void CAccessibilityNodeInfo::Init(
    /* [in] */ IAccessibilityNodeInfo* other)
{
    AutoPtr<CAccessibilityNodeInfo> cOther = (CAccessibilityNodeInfo*)other;
    mSealed = cOther->mSealed;
    mSourceNodeId = cOther->mSourceNodeId;
    mParentNodeId = cOther->mParentNodeId;
    mLabelForId = cOther->mLabelForId;
    mLabeledById = cOther->mLabeledById;
    mWindowId = cOther->mWindowId;
    mConnectionId = cOther->mConnectionId;
    mBoundsInParent->Set(cOther->mBoundsInParent);
    mBoundsInScreen->Set(cOther->mBoundsInScreen);
    mPackageName = cOther->mPackageName;
    mClassName = cOther->mClassName;
    mText = cOther->mText;
    mText = cOther->mError;
    mContentDescription = cOther->mContentDescription;
    mViewIdResourceName = cOther->mViewIdResourceName;

    AutoPtr<IArrayList> otherActions = cOther->mActions;
    Int32 size;
    if (otherActions != NULL && (otherActions->GetSize(&size), size) > 0) {
        if (mActions == NULL) {
            CArrayList::New(ICollection::Probe(otherActions), (IArrayList**)&mActions);
        }
        else {
            mActions->Clear();
            mActions->AddAll(ICollection::Probe(mActions));
        }
    }
    mBooleanProperties = cOther->mBooleanProperties;
    mMaxTextLength = cOther->mMaxTextLength;
    mMovementGranularities = cOther->mMovementGranularities;

    AutoPtr<IInt64Array> otherChildNodeIds = cOther->mChildNodeIds;
    if (otherChildNodeIds != NULL && (otherChildNodeIds->GetSize(&size), size) > 0) {
        if (mChildNodeIds == NULL) {
            AutoPtr<IInterface> obj;
            ICloneable::Probe(otherChildNodeIds)->Clone((IInterface**)&obj);
            mChildNodeIds = IInt64Array::Probe(obj);
        }
        else {
            mChildNodeIds->Clear();
            mChildNodeIds->AddAll(otherChildNodeIds);
        }
    }

    mTextSelectionStart = cOther->mTextSelectionStart;
    mTextSelectionEnd = cOther->mTextSelectionEnd;
    mInputType = cOther->mInputType;
    mLiveRegion = cOther->mLiveRegion;

    Boolean res;
    if (cOther->mExtras != NULL && (cOther->mExtras->IsEmpty(&res), !res)) {
        AutoPtr<IBundle> bundle;
        GetExtras((IBundle**)&bundle);
        bundle->PutAll(cOther->mExtras);
    }

    mRangeInfo = NULL;
    if (cOther->mRangeInfo != NULL) {
        AccessibilityNodeInfoRangeInfo::Obtain(cOther->mRangeInfo, (IAccessibilityNodeInfoRangeInfo**)&mRangeInfo);
    }

    mCollectionInfo = NULL;
    if (cOther->mCollectionInfo != NULL) {
        AccessibilityNodeInfoCollectionInfo::Obtain(cOther->mCollectionInfo, (IAccessibilityNodeInfoCollectionInfo**)&mCollectionInfo);
    }

    mCollectionItemInfo = NULL;
    if (cOther->mCollectionItemInfo != NULL) {
        AccessibilityNodeInfoCollectionItemInfo::Obtain(cOther->mCollectionItemInfo, (IAccessibilityNodeInfoCollectionItemInfo**)&mCollectionItemInfo);
    }
}

void CAccessibilityNodeInfo::Clear()
{
    mSealed = FALSE;
    mSourceNodeId = ROOT_NODE_ID;
    mParentNodeId = ROOT_NODE_ID;
    mLabelForId = ROOT_NODE_ID;
    mLabeledById = ROOT_NODE_ID;
    mWindowId = UNDEFINED_ITEM_ID;
    mConnectionId = UNDEFINED_ITEM_ID;
    mMaxTextLength = -1;
    mMovementGranularities = 0;
    if (mChildNodeIds != NULL) {
        mChildNodeIds->Clear();
    }
    mBoundsInParent->Set(0, 0, 0, 0);
    mBoundsInScreen->Set(0, 0, 0, 0);
    mBooleanProperties = 0;
    mPackageName = NULL;
    mClassName = NULL;
    mText = NULL;
    mError = NULL;
    mContentDescription = NULL;
    mViewIdResourceName = NULL;
    if (mActions != NULL) {
        mActions->Clear();
    }
    mTextSelectionStart = UNDEFINED_SELECTION_INDEX;
    mTextSelectionEnd = UNDEFINED_SELECTION_INDEX;
    mInputType = IInputType::TYPE_NULL;
    mLiveRegion = IView::ACCESSIBILITY_LIVE_REGION_NONE;
    if (mExtras != NULL) {
        mExtras->Clear();
    }
    if (mRangeInfo != NULL) {
        mRangeInfo->Recycle();
        mRangeInfo = NULL;
    }
    if (mCollectionInfo != NULL) {
        mCollectionInfo->Recycle();
        mCollectionInfo = NULL;
    }
    if (mCollectionItemInfo != NULL) {
        mCollectionItemInfo->Recycle();
        mCollectionItemInfo = NULL;
    }
}

Boolean CAccessibilityNodeInfo::IsDefaultLegacyStandardAction(
    /* [in] */ IAccessibilityNodeInfoAccessibilityAction* action)
{
    Int32 id;
    action->GetId(&id);
    AutoPtr<ICharSequence> cs;
    action->GetLabel((ICharSequence**)&cs);
    return id <= LAST_LEGACY_STANDARD_ACTION && TextUtils::IsEmpty(cs);
}

AutoPtr<IAccessibilityNodeInfoAccessibilityAction> CAccessibilityNodeInfo::GetActionSingleton(
    /* [in] */ Int32 actionId)
{
    Int32 actions;
    ICollection::Probe(CAccessibilityNodeInfoAccessibilityAction::sStandardActions)->GetSize(&actions);
    for (Int32 i = 0; i < actions; i++) {
        AutoPtr<IInterface> obj;
        CAccessibilityNodeInfoAccessibilityAction::sStandardActions->GetValueAt(i, (IInterface**)&obj);
        AutoPtr<IAccessibilityNodeInfoAccessibilityAction> currentAction = IAccessibilityNodeInfoAccessibilityAction::Probe(obj);
        Int32 id;
        currentAction->GetId(&id);
        if (actionId == id) {
            return currentAction;
        }
    }

    return NULL;
}

void CAccessibilityNodeInfo::AddLegacyStandardActions(
    /* [in] */ Int32 actionMask)
{
    Int32 remainingIds = actionMask;
    while (remainingIds > 0) {
        Int32 id = 1 << Elastos::Core::Math::NumberOfTrailingZeros(remainingIds);
        remainingIds &= ~id;
        AutoPtr<IAccessibilityNodeInfoAccessibilityAction> action = GetActionSingleton(id);
        AddAction(action);
    }
}

String CAccessibilityNodeInfo::GetActionSymbolicName(
    /* [in] */ Int32 action)
{
    switch (action) {
        case ACTION_FOCUS:
            return String("ACTION_FOCUS");
        case ACTION_CLEAR_FOCUS:
            return String("ACTION_CLEAR_FOCUS");
        case ACTION_SELECT:
            return String("ACTION_SELECT");
        case ACTION_CLEAR_SELECTION:
            return String("ACTION_CLEAR_SELECTION");
        case ACTION_CLICK:
            return String("ACTION_CLICK");
        case ACTION_LONG_CLICK:
            return String("ACTION_LONG_CLICK");
        case ACTION_ACCESSIBILITY_FOCUS:
            return String("ACTION_ACCESSIBILITY_FOCUS");
        case ACTION_CLEAR_ACCESSIBILITY_FOCUS:
            return String("ACTION_CLEAR_ACCESSIBILITY_FOCUS");
        case ACTION_NEXT_AT_MOVEMENT_GRANULARITY:
            return String("ACTION_NEXT_AT_MOVEMENT_GRANULARITY");
        case ACTION_PREVIOUS_AT_MOVEMENT_GRANULARITY:
            return String("ACTION_PREVIOUS_AT_MOVEMENT_GRANULARITY");
        case ACTION_NEXT_HTML_ELEMENT:
            return String("ACTION_NEXT_HTML_ELEMENT");
        case ACTION_PREVIOUS_HTML_ELEMENT:
            return String("ACTION_PREVIOUS_HTML_ELEMENT");
        case ACTION_SCROLL_FORWARD:
            return String("ACTION_SCROLL_FORWARD");
        case ACTION_SCROLL_BACKWARD:
            return String("ACTION_SCROLL_BACKWARD");
        case ACTION_CUT:
            return String("ACTION_CUT");
        case ACTION_COPY:
            return String("ACTION_COPY");
        case ACTION_PASTE:
            return String("ACTION_PASTE");
        case ACTION_SET_SELECTION:
            return String("ACTION_SET_SELECTION");
        default:
            return String("ACTION_UNKNOWN");
    }
}

String CAccessibilityNodeInfo::GetMovementGranularitySymbolicName(
    /* [in] */ Int32 granularity)
{
    switch (granularity) {
        case MOVEMENT_GRANULARITY_CHARACTER:
            return String("MOVEMENT_GRANULARITY_CHARACTER");
        case MOVEMENT_GRANULARITY_WORD:
            return String("MOVEMENT_GRANULARITY_WORD");
        case MOVEMENT_GRANULARITY_LINE:
            return String("MOVEMENT_GRANULARITY_LINE");
        case MOVEMENT_GRANULARITY_PARAGRAPH:
            return String("MOVEMENT_GRANULARITY_PARAGRAPH");
        case MOVEMENT_GRANULARITY_PAGE:
            return String("MOVEMENT_GRANULARITY_PAGE");
        default:
            Slogger::E(TAG, "Unknown movement granularity: %d", granularity);
            return String(NULL);
            // throw new IllegalArgumentException("Unknown movement granularity: " + granularity);
    }
}

Boolean CAccessibilityNodeInfo::CanPerformRequestOverConnection(
    /* [in] */ Int64 accessibilityNodeId)
{
    return (mWindowId != UNDEFINED_ITEM_ID
            && GetAccessibilityViewId(accessibilityNodeId) != UNDEFINED_ITEM_ID
            && mConnectionId != UNDEFINED_CONNECTION_ID);
}

ECode CAccessibilityNodeInfo::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (other == NULL) {
        return NOERROR;
    }

    IAccessibilityNodeInfo* info = IAccessibilityNodeInfo::Probe(other);
    if (info == NULL) {
        return NOERROR;
    }

    CAccessibilityNodeInfo* node = (CAccessibilityNodeInfo*)info;
    if (node == this) {
        *result = TRUE;
        return NOERROR;
    }

    if (mSourceNodeId != node->mSourceNodeId) {
        return NOERROR;
    }
    if (mWindowId != node->mWindowId) {
        return NOERROR;
    }

    *result = TRUE;
    return NOERROR;
}

ECode CAccessibilityNodeInfo::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash);

    const Int32 prime = 31;
    Int32 result = 1;
    result = prime * result + GetAccessibilityViewId(mSourceNodeId);
    result = prime * result + GetVirtualDescendantId(mSourceNodeId);
    result = prime * result + mWindowId;
    *hash = result;
    return NOERROR;
}

ECode CAccessibilityNodeInfo::ToString(
    /* [out] */ String* s)
{
    VALIDATE_NOT_NULL(s);

    AutoPtr<StringBuilder> builder = new StringBuilder();
    // builder.append(super.toString());

    if (DEBUG) {
        builder->Append("; sourceNodeId: ");
        builder->Append(mSourceNodeId);
        builder->Append("; accessibilityViewId: ");
        builder->Append(GetAccessibilityViewId(mSourceNodeId));
        builder->Append("; virtualDescendantId: ");
        builder->Append(GetVirtualDescendantId(mSourceNodeId));
        builder->Append("; mParentNodeId: ");
        builder->Append( + mParentNodeId);

        Int32 granularities = mMovementGranularities;
        builder->Append("; MovementGranularities: [");
        while (granularities != 0) {
            const Int32 granularity = 1 << Elastos::Core::Math::NumberOfTrailingZeros(granularities);
            granularities &= ~granularity;
            builder->Append(GetMovementGranularitySymbolicName(granularity));
            if (granularities != 0) {
                builder->Append(", ");
            }
        }
        builder->Append("]");

        builder->Append("; childAccessibilityIds: [");
        AutoPtr<IInt64Array> childIds = mChildNodeIds;
        if (childIds != NULL) {
            Int32 count;
            childIds->GetSize(&count);
            for (Int32 i = 0; i < count; i++) {
                Int64 id;
                childIds->Get(i, &id);
                builder->Append(id);
                if (i < count - 1) {
                    builder->Append(", ");
                }
            }
        }
        builder->Append("]");
    }

    builder->Append("; boundsInParent: ");
    builder->Append(mBoundsInParent);
    builder->Append("; boundsInScreen: ");
    builder->Append(mBoundsInScreen);

    builder->Append("; packageName: ");
    builder->Append(mPackageName);
    builder->Append("; className: ");
    builder->Append(mClassName);
    builder->Append("; text: ");
    builder->Append(mText);
    builder->Append("; error: ");
    builder->Append(mError);
    builder->Append("; maxTextLength: ");
    builder->Append(mMaxTextLength);
    builder->Append("; contentDescription: ");
    builder->Append(mContentDescription);
    builder->Append("; viewIdResName: ");
    builder->Append(mViewIdResourceName);

    builder->Append("; checkable: ");
    Boolean res;
    builder->Append((IsCheckable(&res), res));
    builder->Append("; checked: ");
    builder->Append((IsChecked(&res), res));
    builder->Append("; focusable: ");
    builder->Append((IsFocusable(&res), res));
    builder->Append("; focused: ");
    builder->Append((IsFocused(&res), res));
    builder->Append("; selected: ");
    builder->Append((IsSelected(&res), res));
    builder->Append("; clickable: ");
    builder->Append((IsClickable(&res), res));
    builder->Append("; longClickable: ");
    builder->Append((IsLongClickable(&res), res));
    builder->Append("; enabled: ");
    builder->Append((IsEnabled(&res), res));
    builder->Append("; password: ");
    builder->Append((IsPassword(&res), res));
    builder->Append("; scrollable: ");
    builder->Append((IsScrollable(&res), res));
    builder->Append("; actions: ");
    builder->Append(mActions);

    *s = builder->ToString();
    return NOERROR;
}

} // Accessibility
} // View
} // Droid
} // Elastos
