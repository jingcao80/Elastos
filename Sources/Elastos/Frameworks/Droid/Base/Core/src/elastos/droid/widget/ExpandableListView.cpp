#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/widget/ExpandableListView.h"
#include "elastos/droid/widget/CExpandableListContextMenuInfo.h"
#include "elastos/droid/widget/CExpandableListViewSavedState.h"
#include "elastos/droid/widget/CExpandableListConnector.h"
#include "elastos/droid/widget/ExpandableListPosition.h"
#include "elastos/droid/widget/GroupMetadata.h"
#include "elastos/droid/widget/PositionMetadata.h"
#include "elastos/droid/view/SoundEffectConstants.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Os::Build;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::SoundEffectConstants;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::R;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Widget {

const Int64 ExpandableListView::PACKED_POSITION_MASK_CHILD = 0x00000000FFFFFFFFL;
const Int64 ExpandableListView::PACKED_POSITION_MASK_GROUP = 0x7FFFFFFF00000000L;
const Int64 ExpandableListView::PACKED_POSITION_MASK_TYPE = 0x8000000000000000L;
const Int64 ExpandableListView::PACKED_POSITION_SHIFT_GROUP = 32;
const Int64 ExpandableListView::PACKED_POSITION_SHIFT_TYPE = 63;
const Int64 ExpandableListView::PACKED_POSITION_INT_MASK_CHILD = 0xFFFFFFFF;
const Int64 ExpandableListView::PACKED_POSITION_INT_MASK_GROUP = 0x7FFFFFFF;

const Int32 ExpandableListView::INDICATOR_UNDEFINED = -2;

static AutoPtr< ArrayOf<Int32> > InitEMPTY_STATE_SET()
{
    AutoPtr< ArrayOf<Int32> > arg = ArrayOf<Int32>::Alloc(0);
    return arg;
}

const AutoPtr< ArrayOf<Int32> > ExpandableListView::EMPTY_STATE_SET = InitEMPTY_STATE_SET();

static AutoPtr< ArrayOf<Int32> > InitGROUP_EXPANDED_STATE_SET()
{
    AutoPtr< ArrayOf<Int32> > arg = ArrayOf<Int32>::Alloc(1);
    (*arg)[0] = R::attr::state_expanded;
    return arg;
}

const AutoPtr< ArrayOf<Int32> > ExpandableListView::GROUP_EXPANDED_STATE_SET = InitGROUP_EXPANDED_STATE_SET();

static AutoPtr< ArrayOf<Int32> > InitGROUP_EMPTY_STATE_SET()
{
    AutoPtr< ArrayOf<Int32> > arg = ArrayOf<Int32>::Alloc(1);
    (*arg)[0] = R::attr::state_empty;
    return arg;
}

const AutoPtr< ArrayOf<Int32> > ExpandableListView::GROUP_EMPTY_STATE_SET = InitGROUP_EMPTY_STATE_SET();

static AutoPtr< ArrayOf<Int32> > InitGROUP_EXPANDED_EMPTY_STATE_SET()
{
    AutoPtr< ArrayOf<Int32> > arg = ArrayOf<Int32>::Alloc(2);
    (*arg)[0] = R::attr::state_expanded;
    (*arg)[1] = R::attr::state_empty;
    return arg;
}

const AutoPtr< ArrayOf<Int32> > ExpandableListView::GROUP_EXPANDED_EMPTY_STATE_SET = InitGROUP_EXPANDED_EMPTY_STATE_SET();

AutoPtr< ArrayOf< ArrayOf<Int32>* > > ExpandableListView::InitGROUP_STATE_SETS()
{
    AutoPtr< ArrayOf< ArrayOf<Int32>* > > arg = ArrayOf< ArrayOf<Int32>* >::Alloc(4);
    (*arg)[0] = EMPTY_STATE_SET; //00
    (*arg)[1] = GROUP_EXPANDED_STATE_SET; //01
    (*arg)[2] = GROUP_EMPTY_STATE_SET; //10
    (*arg)[3] = GROUP_EXPANDED_EMPTY_STATE_SET; //11
    return arg;
}

const AutoPtr< ArrayOf< ArrayOf<Int32>* > > ExpandableListView::GROUP_STATE_SETS = InitGROUP_STATE_SETS();

static AutoPtr< ArrayOf<Int32> > InitCHILD_LAST_STATE_SET()
{
    AutoPtr< ArrayOf<Int32> > arg = ArrayOf<Int32>::Alloc(1);
    (*arg)[0] = R::attr::state_last;
    return arg;
}

const AutoPtr< ArrayOf<Int32> > ExpandableListView::CHILD_LAST_STATE_SET = InitCHILD_LAST_STATE_SET();

//==============================================================================
//          ExpandableListView::ExpandableListContextMenuInfo
//==============================================================================

CAR_INTERFACE_IMPL(ExpandableListView::ExpandableListContextMenuInfo, Object, IExpandableListContextMenuInfo)

ExpandableListView::ExpandableListContextMenuInfo::ExpandableListContextMenuInfo()
    : mPackedPosition(0)
    , mId(0)
{}

ExpandableListView::ExpandableListContextMenuInfo::~ExpandableListContextMenuInfo()
{}

ECode ExpandableListView::ExpandableListContextMenuInfo::constructor(
    /* [in] */ IView* targetView,
    /* [in] */ Int64 packedPosition,
    /* [in] */ Int64 id)
{
    mTargetView = targetView;
    mPackedPosition = packedPosition;
    mId = id;
    return NOERROR;
}

ECode ExpandableListView::ExpandableListContextMenuInfo::SetTargetView(
    /* [in] */ IView* targetView)
{
    mTargetView = targetView;
    return NOERROR;
}

ECode ExpandableListView::ExpandableListContextMenuInfo::GetTargetView(
    /* [out] */ IView** targetView)
{
    VALIDATE_NOT_NULL(targetView);
    *targetView = mTargetView;
    REFCOUNT_ADD(*targetView);
    return NOERROR;
}

ECode ExpandableListView::ExpandableListContextMenuInfo::SetPackedPosition(
    /* [in] */ Int64 packedPosition)
{
    mPackedPosition = packedPosition;
    return NOERROR;
}

ECode ExpandableListView::ExpandableListContextMenuInfo::GetPackedPosition(
    /* [out] */ Int64* packedPosition)
{
    VALIDATE_NOT_NULL(packedPosition);
    *packedPosition = mPackedPosition;
    return NOERROR;
}

ECode ExpandableListView::ExpandableListContextMenuInfo::SetId(
    /* [in] */ Int64 id)
{
    mId = id;
    return NOERROR;
}

ECode ExpandableListView::ExpandableListContextMenuInfo::GetId(
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mId;
    return NOERROR;
}

//==============================================================================
//          ExpandableListView::SavedState
//==============================================================================

CAR_INTERFACE_IMPL(ExpandableListView::SavedState, View::BaseSavedState, IExpandableListViewSavedState);

ExpandableListView::SavedState::SavedState()
{}

ExpandableListView::SavedState::~SavedState()
{}

ECode ExpandableListView::SavedState::constructor()
{
    return View::BaseSavedState::constructor();
}

ECode ExpandableListView::SavedState::constructor(
    /* [in] */ IParcelable* superState,
    /* [in] */ IArrayList* expandedGroupMetadataList)
{
    View::BaseSavedState::constructor(superState);
    mExpandedGroupMetadataList = expandedGroupMetadataList;
    return NOERROR;
}

ECode ExpandableListView::SavedState::WriteToParcel(
    /* [in] */ IParcel* out)
{
    FAIL_RETURN(View::BaseSavedState::WriteToParcel(out));

    Int32 size = mExpandedGroupMetadataList != NULL ? (mExpandedGroupMetadataList->GetSize(&size), size) : 0;
    out->WriteInt32(size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> value;
        mExpandedGroupMetadataList->Get(i, (IInterface**)&value);
        out->WriteInterfacePtr(IGroupMetadata::Probe(value));
    }

    return NOERROR;
}

ECode ExpandableListView::SavedState::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    FAIL_RETURN(View::BaseSavedState::ReadFromParcel(in));

    Int32 size;
    in->ReadInt32(&size);
    if (size > 0) {
        if (!mExpandedGroupMetadataList) {
            CArrayList::New((IArrayList**)&mExpandedGroupMetadataList);
        }
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> value;
            in->ReadInterfacePtr((Handle32*)(IGroupMetadata**)&value);
            mExpandedGroupMetadataList->Add(value);
        }
    }

    return NOERROR;
}

//==============================================================================
//          ExpandableListView
//==============================================================================

CAR_INTERFACE_IMPL(ExpandableListView, ListView, IExpandableListView);

ExpandableListView::ExpandableListView()
    : mIndicatorLeft(0)
    , mIndicatorRight(0)
    , mIndicatorStart(0)
    , mIndicatorEnd(0)
    , mChildIndicatorLeft(0)
    , mChildIndicatorRight(0)
    , mChildIndicatorStart(0)
    , mChildIndicatorEnd(0)
{}

ExpandableListView::~ExpandableListView()
{}

ECode ExpandableListView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode ExpandableListView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::expandableListViewStyle);
}

ECode ExpandableListView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode ExpandableListView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    ListView::constructor(context, attrs, defStyleAttr, defStyleRes);

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::ExpandableListView),
            ArraySize(R::styleable::ExpandableListView));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(
            attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a);

    a->GetDrawable(R::styleable::ExpandableListView_groupIndicator, (IDrawable**)&mGroupIndicator);
    a->GetDrawable(R::styleable::ExpandableListView_childIndicator, (IDrawable**)&mChildIndicator);
    a->GetDimensionPixelSize(R::styleable::ExpandableListView_indicatorLeft, 0, &mIndicatorLeft);
    a->GetDimensionPixelSize(R::styleable::ExpandableListView_indicatorRight, 0, &mIndicatorRight);
    if (mIndicatorRight == 0 && mGroupIndicator != NULL) {
        Int32 width = 0;
        mGroupIndicator->GetIntrinsicWidth(&width);
        mIndicatorRight = mIndicatorLeft + width;
    }

    a->GetDimensionPixelSize(R::styleable::ExpandableListView_childIndicatorLeft, CHILD_INDICATOR_INHERIT, &mChildIndicatorLeft);
    a->GetDimensionPixelSize(
            R::styleable::ExpandableListView_childIndicatorRight, CHILD_INDICATOR_INHERIT, &mChildIndicatorRight);
    a->GetDrawable(R::styleable::ExpandableListView_childDivider, (IDrawable**)&mChildDivider);

    if (!IsRtlCompatibilityMode()) {
        a->GetDimensionPixelSize(
                R::styleable::ExpandableListView_indicatorStart,
                INDICATOR_UNDEFINED, &mIndicatorStart);
        a->GetDimensionPixelSize(
                R::styleable::ExpandableListView_indicatorEnd,
                INDICATOR_UNDEFINED, &mIndicatorEnd);

        a->GetDimensionPixelSize(
                R::styleable::ExpandableListView_childIndicatorStart,
                CHILD_INDICATOR_INHERIT, &mChildIndicatorStart);
        a->GetDimensionPixelSize(
                R::styleable::ExpandableListView_childIndicatorEnd,
                CHILD_INDICATOR_INHERIT, &mChildIndicatorEnd);
    }

    a->Recycle();
    return NOERROR;
}

Boolean ExpandableListView::IsRtlCompatibilityMode()
{
    AutoPtr<IApplicationInfo> info;
    mContext->GetApplicationInfo((IApplicationInfo**)&info);

    Int32 targetSdkVersion;
    info->GetTargetSdkVersion(&targetSdkVersion);
    return targetSdkVersion < Build::VERSION_CODES::JELLY_BEAN_MR1 || !HasRtlSupport();
}

Boolean ExpandableListView::HasRtlSupport()
{
    AutoPtr<IApplicationInfo> info;
    mContext->GetApplicationInfo((IApplicationInfo**)&info);

    Boolean res;
    info->HasRtlSupport(&res);
    return res;
}

ECode ExpandableListView::OnRtlPropertiesChanged(
    /* [in] */ Int32 layoutDirection)
{
    ResolveIndicator();
    ResolveChildIndicator();
    return NOERROR;
}

void ExpandableListView::ResolveIndicator()
{
    Boolean isLayoutRtl;
    IsLayoutRtl(&isLayoutRtl);
    if (isLayoutRtl) {
        if (mIndicatorStart >= 0) {
            mIndicatorRight = mIndicatorStart;
        }
        if (mIndicatorEnd >= 0) {
            mIndicatorLeft = mIndicatorEnd;
        }
    }
    else {
        if (mIndicatorStart >= 0) {
            mIndicatorLeft = mIndicatorStart;
        }
        if (mIndicatorEnd >= 0) {
            mIndicatorRight = mIndicatorEnd;
        }
    }
    if (mIndicatorRight == 0 && mGroupIndicator != NULL) {
        Int32 width;
        mGroupIndicator->GetIntrinsicWidth(&width);
        mIndicatorRight = mIndicatorLeft + width;
    }
}

void ExpandableListView::ResolveChildIndicator()
{
    Boolean isLayoutRtl;
    IsLayoutRtl(&isLayoutRtl);
    if (isLayoutRtl) {
        if (mChildIndicatorStart >= CHILD_INDICATOR_INHERIT) {
            mChildIndicatorRight = mChildIndicatorStart;
        }
        if (mChildIndicatorEnd >= CHILD_INDICATOR_INHERIT) {
            mChildIndicatorLeft = mChildIndicatorEnd;
        }
    }
    else {
        if (mChildIndicatorStart >= CHILD_INDICATOR_INHERIT) {
            mChildIndicatorLeft = mChildIndicatorStart;
        }
        if (mChildIndicatorEnd >= CHILD_INDICATOR_INHERIT) {
            mChildIndicatorRight = mChildIndicatorEnd;
        }
    }
}

void ExpandableListView::DispatchDraw(
    /* [in] */ ICanvas* canvas)
{
    ListView::DispatchDraw(canvas);

    if ((mChildIndicator == NULL) && (mGroupIndicator == NULL)) {
        return;
    }

    Int32 saveCount = 0;
    Boolean clipToPadding = (mGroupFlags & ViewGroup::CLIP_TO_PADDING_MASK) == ViewGroup::CLIP_TO_PADDING_MASK;
    if (clipToPadding) {
        canvas->Save(&saveCount);
        Int32 scrollX = mScrollX;
        Int32 scrollY = mScrollY;
        Boolean isNotEmpty = FALSE;
        canvas->ClipRect(scrollX + mPaddingLeft, scrollY + mPaddingTop,
                scrollX + mRight - mLeft - mPaddingRight,
                scrollY + mBottom - mTop - mPaddingBottom, &isNotEmpty);
    }

    Int32 headerViewsCount = GetHeaderViewsCount();
    Int32 lastChildFlPos = mItemCount - GetFooterViewsCount() - headerViewsCount - 1;
    Int32 myB = mBottom;

    AutoPtr<IView> item;
    AutoPtr<IDrawable> indicator;
    Int32 t = 0;
    Int32 b = 0;

    Int32 lastItemType = ~(ExpandableListPosition::CHILD | ExpandableListPosition::GROUP);
    AutoPtr<IRect> indicatorRect = mIndicatorRect;

    Int32 childCount;
    GetChildCount(&childCount);
    for (Int32 i = 0, childFlPos = mFirstPosition - headerViewsCount; i < childCount;
            i++, childFlPos++) {

        if (childFlPos < 0) {
            continue;
        }
        else if (childFlPos > lastChildFlPos) {
            break;
        }

        item =NULL;
        GetChildAt(i, (IView**)&item);
        item->GetTop(&t);
        item->GetBottom(&b);

        if ((b < 0) || (t > myB)) continue;

        AutoPtr<IPositionMetadata> pos;
        mConnector->GetUnflattenedPos(childFlPos, (IPositionMetadata**)&pos);

        Boolean isLayoutRtl;
        IsLayoutRtl(&isLayoutRtl);
        Int32 width;
        GetWidth(&width);

        AutoPtr<ExpandableListPosition> elpos = (ExpandableListPosition*)(((PositionMetadata*)pos.Get())->mPosition.Get());
        AutoPtr<CRect> rect = (CRect*)indicatorRect.Get();
        if (elpos->mType != lastItemType) {
            if (elpos->mType == ExpandableListPosition::CHILD) {
                indicatorRect->SetLeft((mChildIndicatorLeft == CHILD_INDICATOR_INHERIT) ?
                        mIndicatorLeft : mChildIndicatorLeft);
                indicatorRect->SetRight((mChildIndicatorRight == CHILD_INDICATOR_INHERIT) ?
                        mIndicatorRight : mChildIndicatorRight);
            }
            else {
                indicatorRect->SetLeft(mIndicatorLeft);
                indicatorRect->SetRight(mIndicatorRight);
            }

            if (isLayoutRtl) {
                const Int32 temp = rect->mLeft;
                rect->mLeft = width - rect->mRight;
                rect->mRight = width - temp;

                rect->mLeft -= mPaddingRight;
                rect->mRight -= mPaddingRight;
            }
            else {
                rect->mLeft += mPaddingLeft;
                rect->mRight += mPaddingLeft;
            }

            lastItemType = elpos->mType;
        }

        if (rect->mLeft != rect->mRight) {
            if (mStackFromBottom) {
                rect->mTop = t;
                rect->mBottom = b;
            }
            else {
                rect->mTop = t;
                rect->mBottom = b;
            }

            indicator = GetIndicator(pos);
            if (indicator != NULL) {
                indicator->SetBounds(indicatorRect);
                indicator->Draw(canvas);
            }
        }

        pos->Recycle();
    }

    if (clipToPadding) {
        canvas->RestoreToCount(saveCount);
    }
}

AutoPtr<IDrawable> ExpandableListView::GetIndicator(
    /* [in] */ IPositionMetadata* pos)
{
    AutoPtr<IDrawable> indicator = NULL;
    AutoPtr<ExpandableListPosition> elpos = (ExpandableListPosition*)(((PositionMetadata*)pos)->mPosition.Get());
    if (elpos->mType == ExpandableListPosition::GROUP) {
        indicator = mGroupIndicator;

        Boolean stateful = FALSE;
        if (indicator.Get() != NULL && (indicator->IsStateful(&stateful), stateful)) {
            // Empty check based on availability of data.  If the groupMetadata isn't NULL,
            // we do a check on it. Otherwise, the group is collapsed so we consider it
            // empty for performance reasons.
            Boolean isEmpty = (((PositionMetadata*)pos)->mGroupMetadata == NULL) ||
                    (((GroupMetadata*)((PositionMetadata*)pos)->mGroupMetadata.Get())->mLastChildFlPos == ((GroupMetadata*)((PositionMetadata*)pos)->mGroupMetadata.Get())->mFlPos);

            Boolean expanded;
            pos->IsExpanded(&expanded);
            Int32 stateSetIndex =
                (expanded ? 1 : 0) | // Expanded?
                (isEmpty ? 2 : 0); // Empty?

            Boolean res;
            indicator->SetState((*GROUP_STATE_SETS)[stateSetIndex], &res);
        }
    }
    else {
        indicator = mChildIndicator;

        Boolean stateful = FALSE;
        if (indicator != NULL && (indicator->IsStateful(&stateful), stateful)) {
            // No need for a state sets array for the child since it only has two states
            AutoPtr<GroupMetadata> gmData = (GroupMetadata*)(((PositionMetadata*)pos)->mGroupMetadata.Get());
            AutoPtr< ArrayOf<Int32> > stateSet;
            if (elpos->mFlatListPos == gmData->mLastChildFlPos) {
                stateSet = CHILD_LAST_STATE_SET;
            }
            else {
                stateSet = EMPTY_STATE_SET;
            }

            Boolean res;
            indicator->SetState(stateSet, &res);
        }
    }

    return indicator;
}

ECode ExpandableListView::SetChildDivider(
    /* [in] */ IDrawable* childDivider)
{
    mChildDivider = childDivider;

    return NOERROR;
}

void ExpandableListView::DrawDivider(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IRect* bounds,
    /* [in] */ Int32 childIndex)
{
    Int32 flatListPosition = childIndex + mFirstPosition;

    // Only proceed as possible child if the divider isn't above all items (if it is above
    // all items, then the item below it has to be a group)
    if (flatListPosition >= 0) {
        Int32 adjustedPosition = GetFlatPositionForConnector(flatListPosition);
        AutoPtr<IPositionMetadata> pos;
        mConnector->GetUnflattenedPos(adjustedPosition, (IPositionMetadata**)&pos);
        // If this item is a child, or it is a non-empty group that is expanded

        Boolean expanded;
        pos->IsExpanded(&expanded);
        AutoPtr<ExpandableListPosition> elpos = (ExpandableListPosition*)(((PositionMetadata*)pos.Get())->mPosition.Get());
        if ((elpos->mType == ExpandableListPosition::CHILD) || (expanded &&
                ((GroupMetadata*)((PositionMetadata*)pos.Get())->mGroupMetadata.Get())->mLastChildFlPos != ((GroupMetadata*)((PositionMetadata*)pos.Get())->mGroupMetadata.Get())->mFlPos)) {
            // These are the cases where we draw the child divider
            AutoPtr<IDrawable> divider = mChildDivider;
            divider->SetBounds(bounds);
            divider->Draw(canvas);
            pos->Recycle();
            return;
        }
        pos->Recycle();
    }

    // Otherwise draw the default divider
    ListView::DrawDivider(canvas, bounds, flatListPosition);
}

ECode ExpandableListView::SetAdapter(
    /* [in] */ IAdapter* adapter)
{
    Logger::E("ExpandableListView", "For ExpandableListView, use setAdapter(ExpandableListAdapter) instead of setAdapter(ListAdapter)");
    return E_RUNTIME_EXCEPTION;
}

ECode ExpandableListView::GetAdapter(
    /* [out] */ IAdapter** adapter)
{
    VALIDATE_NOT_NULL(adapter);
    return ListView::GetAdapter(adapter);
}

ECode ExpandableListView::SetOnItemClickListener(
    /* [in] */ IAdapterViewOnItemClickListener* l)
{
    return ListView::SetOnItemClickListener(l);
}

ECode ExpandableListView::SetAdapter(
    /* [in] */ IExpandableListAdapter* adapter)
{
    mAdapter = adapter;
    mConnector = NULL;
    if (adapter != NULL) {
        CExpandableListConnector::New(adapter, (IExpandableListConnector**)&mConnector);
    }

    return ListView::SetAdapter(IAdapter::Probe(mConnector));
}

ECode ExpandableListView::GetExpandableListAdapter(
    /* [out] */ IExpandableListAdapter** adapter)
{
    VALIDATE_NOT_NULL(adapter);
    *adapter = mAdapter;
    return NOERROR;
}

Boolean ExpandableListView::IsHeaderOrFooterPosition(
    /* [in] */ Int32 position)
{
    Int32 footerViewsStart = mItemCount - GetFooterViewsCount();
    return (position < GetHeaderViewsCount() || position >= footerViewsStart);
}

Int32 ExpandableListView::GetFlatPositionForConnector(
    /* [in] */ Int32 flatListPosition)
{
    return flatListPosition - GetHeaderViewsCount();
}

Int32 ExpandableListView::GetAbsoluteFlatPosition(
    /* [in] */ Int32 flatListPosition)
{
    return flatListPosition + GetHeaderViewsCount();
}

ECode ExpandableListView::PerformItemClick(
    /* [in] */ IView* v,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (IsHeaderOrFooterPosition(position)) {
        return ListView::PerformItemClick(v, position, id, result);
    }

    Int32 adjustedPosition = GetFlatPositionForConnector(position);
    *result = HandleItemClick(v, adjustedPosition, id);
    return NOERROR;
}

Boolean ExpandableListView::HandleItemClick(
    /* [in] */ IView* v,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    AutoPtr<IPositionMetadata> posMetadata;
    mConnector->GetUnflattenedPos(position, (IPositionMetadata**)&posMetadata);

    id = GetChildOrGroupId(((PositionMetadata*)posMetadata.Get())->mPosition);

    Boolean returnValue = FALSE;
    AutoPtr<ExpandableListPosition> listPosition = (ExpandableListPosition*)((PositionMetadata*)posMetadata.Get())->mPosition.Get();

    if (listPosition->mType == ExpandableListPosition::GROUP) {

        if (mOnGroupClickListener.Get() != NULL) {
            Boolean res = FALSE;
            mOnGroupClickListener->OnGroupClick(THIS_PROBE(IExpandableListView), v,
                listPosition->mGroupPos, id, &res);
            if (res) {
                posMetadata->Recycle();
                return TRUE;
            }
        }

        Boolean expanded = FALSE;
        posMetadata->IsExpanded(&expanded);
        if (expanded) {
            Boolean res = FALSE;
            mConnector->CollapseGroup(posMetadata, &res);

            PlaySoundEffect(SoundEffectConstants::CLICK);

            if (mOnGroupCollapseListener.Get() != NULL) {
                mOnGroupCollapseListener->OnGroupCollapse(listPosition->mGroupPos);
            }
        }
        else {
            Boolean rush = FALSE;
            mConnector->CollapseGroup(posMetadata, &rush);

            PlaySoundEffect(SoundEffectConstants::CLICK);

            if (mOnGroupExpandListener.Get() != NULL) {
                mOnGroupExpandListener->OnGroupExpand(listPosition->mGroupPos);
            }

            Int32 groupPos = listPosition->mGroupPos;
            Int32 groupFlatPos = listPosition->mFlatListPos;

            Int32 shiftedGroupPosition = groupFlatPos + GetHeaderViewsCount();

            Int32 count;
            mAdapter->GetChildrenCount(groupPos, &count);
            AbsListView::SmoothScrollToPosition(shiftedGroupPosition + count, shiftedGroupPosition);
        }

        returnValue = TRUE;
    }
    else {
        if (mOnChildClickListener.Get() != NULL) {
            PlaySoundEffect(SoundEffectConstants::CLICK);
            mOnChildClickListener->OnChildClick(THIS_PROBE(IExpandableListView), v,
                    listPosition->mGroupPos, listPosition->mChildPos, id, &returnValue);
            return returnValue;
        }

        returnValue = FALSE;
    }

    posMetadata->Recycle();

    return returnValue;
}

ECode ExpandableListView::ExpandGroup(
    /* [in] */ Int32 groupPos,
    /* [out] */ Boolean* expanded)
{
    VALIDATE_NOT_NULL(expanded);
    return ExpandGroup(groupPos, FALSE, expanded);
}

ECode ExpandableListView::ExpandGroup(
    /* [in] */ Int32 groupPos,
    /* [in] */ Boolean animate,
    /* [out] */ Boolean* expanded)
{
    VALIDATE_NOT_NULL(expanded);
    AutoPtr<IExpandableListPosition> elGroupPos = ExpandableListPosition::Obtain(
            ExpandableListPosition::GROUP, groupPos, -1, -1);
    AutoPtr<IPositionMetadata> pm;
    mConnector->GetFlattenedPos(elGroupPos, (IPositionMetadata**)&pm);
    elGroupPos->Recycle();

    Boolean retValue = FALSE;
    mConnector->ExpandGroup(pm, &retValue);

    if(mOnGroupExpandListener != NULL) {
        mOnGroupExpandListener->OnGroupExpand(groupPos);
    }

    if(animate) {
        Int32 groupFlatPos = ((ExpandableListPosition*)((PositionMetadata*)pm.Get())->mPosition.Get())->mFlatListPos;
        Int32 shiftedGroupPosition = groupFlatPos + GetHeaderViewsCount();
        Int32 childCount = 0;
        mAdapter->GetChildrenCount(groupPos, &childCount);
        AbsListView::SmoothScrollToPosition(shiftedGroupPosition + childCount, shiftedGroupPosition);
    }

    pm->Recycle();
    *expanded = retValue;
    return NOERROR;
}

ECode ExpandableListView::CollapseGroup(
    /* [in] */ Int32 groupPos,
    /* [out] */ Boolean* collapse)
{
    VALIDATE_NOT_NULL(collapse);

    Boolean retValue = FALSE;
    Boolean res = FALSE;
    retValue = ((CExpandableListConnector*)mConnector.Get())->CollapseGroup(groupPos, &res);

    if (mOnGroupCollapseListener.Get() != NULL) {
        mOnGroupCollapseListener->OnGroupCollapse(groupPos);
    }

    *collapse = retValue;
    return NOERROR;
}

ECode ExpandableListView::SetOnGroupCollapseListener(
    /* [in] */ IExpandableListViewOnGroupCollapseListener* onGroupCollapseListener)
{
    mOnGroupCollapseListener = onGroupCollapseListener;
    return NOERROR;
}

ECode ExpandableListView::SetOnGroupExpandListener(
    /* [in] */ IExpandableListViewOnGroupExpandListener* onGroupExpandListener)
{
    mOnGroupExpandListener = onGroupExpandListener;
    return NOERROR;
}

ECode ExpandableListView::SetOnGroupClickListener(
    /* [in] */ IExpandableListViewOnGroupClickListener* onGroupClickListener)
{
    mOnGroupClickListener = onGroupClickListener;
    return NOERROR;
}

ECode ExpandableListView::SetOnChildClickListener(
    /* [in] */ IExpandableListViewOnChildClickListener* onChildClickListener)
{
    mOnChildClickListener = onChildClickListener;
    return NOERROR;
}

ECode ExpandableListView::GetExpandableListPosition(
    /* [in] */ Int32 flatListPosition,
    /* [out] */ Int64* position)
{
    VALIDATE_NOT_NULL(position);

    if (IsHeaderOrFooterPosition(flatListPosition)) {
        *position = PACKED_POSITION_VALUE_NULL;
        return NOERROR;
    }

    Int32 adjustedPosition = GetFlatPositionForConnector(flatListPosition);
    AutoPtr<IPositionMetadata> pm;
    mConnector->GetUnflattenedPos(adjustedPosition, (IPositionMetadata**)&pm);
    Int64 packedPos = 0;
    ((ExpandableListPosition*)((PositionMetadata*)pm.Get())->mPosition.Get())->GetPackedPosition(&packedPos);
    pm->Recycle();
    *position = packedPos;
    return NOERROR;
}

ECode ExpandableListView::GetFlatListPosition(
    /* [in] */ Int64 packedPosition,
    /* [out] */ Int32* position)
{
    VALIDATE_NOT_NULL(position);

    AutoPtr<IExpandableListPosition> elPackedPos =
            ExpandableListPosition::ObtainPosition(packedPosition);
    AutoPtr<IPositionMetadata> pm;
    mConnector->GetFlattenedPos(elPackedPos, (IPositionMetadata**)&pm);
    elPackedPos->Recycle();

    Int32 flatListPosition = ((ExpandableListPosition*)((PositionMetadata*)pm.Get())->mPosition.Get())->mFlatListPos;
    pm->Recycle();
    *position = GetAbsoluteFlatPosition(flatListPosition);
    return NOERROR;
}

ECode ExpandableListView::GetSelectedPosition(
    /* [out] */ Int64* position)
{
    VALIDATE_NOT_NULL(position);

    Int32 selectedPos;
    GetSelectedItemPosition(&selectedPos);

    return GetExpandableListPosition(selectedPos, position);
}

ECode ExpandableListView::GetSelectedId(
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);

    Int64 packedPos;
    GetSelectedPosition(&packedPos);

    if (packedPos == PACKED_POSITION_VALUE_NULL) {
        *id = -1;
        return NOERROR;
    }

    Int32 groupPos = GetPackedPositionGroup(packedPos);

    Int64 _id = 0;
    if (GetPackedPositionType(packedPos) == PACKED_POSITION_TYPE_GROUP) {
        // It's a group
        mAdapter->GetGroupId(groupPos, &_id);
        *id = _id;
        return NOERROR;
    }
    else {
        // It's a child
        mAdapter->GetChildId(groupPos, GetPackedPositionChild(packedPos), &_id);
        *id = _id;
        return NOERROR;
    }
}

ECode ExpandableListView::SetSelectedGroup(
    /* [in] */ Int32 groupPosition)
{
    AutoPtr<IExpandableListPosition> elGroupPos =
            ExpandableListPosition::ObtainGroupPosition(groupPosition);
    AutoPtr<IPositionMetadata> pm = NULL;
    mConnector->GetFlattenedPos(elGroupPos, (IPositionMetadata**)&pm);
    elGroupPos->Recycle();
    Int32 absoluteFlatPosition = GetAbsoluteFlatPosition(((ExpandableListPosition*)((PositionMetadata*)pm.Get())->mPosition.Get())->mFlatListPos);
    ListView::SetSelection(absoluteFlatPosition);
    pm->Recycle();
    return NOERROR;
}

ECode ExpandableListView::SetSelectedChild(
    /* [in] */ Int32 groupPosition,
    /* [in] */ Int32 childPosition,
    /* [in] */ Boolean shouldExpandGroup,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    AutoPtr<IExpandableListPosition> elChildPos = ExpandableListPosition::ObtainChildPosition(
            groupPosition, childPosition);
    AutoPtr<IPositionMetadata> flatChildPos;
    mConnector->GetFlattenedPos(elChildPos, (IPositionMetadata**)&flatChildPos);

    if (flatChildPos.Get() == NULL) {
        if (!shouldExpandGroup) {
            *res = FALSE;
            return NOERROR;
        }

        Boolean result;
        ExpandGroup(groupPosition, &result);

        mConnector->GetFlattenedPos(elChildPos, (IPositionMetadata**)&flatChildPos);

        if (flatChildPos.Get() == NULL) {
            Logger::E("ExpandableListView", "Could not find child");
            return E_ILLEGAL_STATE_EXCEPTION;
            //throw new IllegalStateException("Could not find child");
        }
    }

    Int32 absoluteFlatPosition = GetAbsoluteFlatPosition(((ExpandableListPosition*)((PositionMetadata*)flatChildPos.Get())->mPosition.Get())->mFlatListPos);
    ListView::SetSelection(absoluteFlatPosition);

    elChildPos->Recycle();
    flatChildPos->Recycle();

    *res = TRUE;
    return NOERROR;
}

ECode ExpandableListView::IsGroupExpanded(
    /* [in] */ Int32 groupPosition,
    /* [out] */ Boolean* expanded)
{
    VALIDATE_NOT_NULL(expanded);

    return mConnector->IsGroupExpanded(groupPosition, expanded);
}

Int32 ExpandableListView::GetPackedPositionType(
    /* [in] */ Int64 packedPosition)
{
    if (packedPosition == PACKED_POSITION_VALUE_NULL) {
        return PACKED_POSITION_TYPE_NULL;
    }

    return (packedPosition & PACKED_POSITION_MASK_TYPE) == PACKED_POSITION_MASK_TYPE
            ? PACKED_POSITION_TYPE_CHILD
            : PACKED_POSITION_TYPE_GROUP;
}

Int32 ExpandableListView::GetPackedPositionGroup(
    /* [in] */ Int64 packedPosition)
{
    if (packedPosition == PACKED_POSITION_VALUE_NULL) return -1;

    return (Int32) ((packedPosition & PACKED_POSITION_MASK_GROUP) >> PACKED_POSITION_SHIFT_GROUP);
}

Int32 ExpandableListView::GetPackedPositionChild(
    /* [in] */ Int64 packedPosition)
{
    if (packedPosition == PACKED_POSITION_VALUE_NULL) return -1;

    if ((packedPosition & PACKED_POSITION_MASK_TYPE) != PACKED_POSITION_MASK_TYPE) return -1;

    return (Int32) (packedPosition & PACKED_POSITION_MASK_CHILD);
}

Int64 ExpandableListView::GetPackedPositionForChild(
    /* [in] */ Int32 groupPosition,
    /* [in] */ Int32 childPosition)
{
    return (((Int64)PACKED_POSITION_TYPE_CHILD) << PACKED_POSITION_SHIFT_TYPE)
            | ((((Int64)groupPosition) & PACKED_POSITION_INT_MASK_GROUP)
                    << PACKED_POSITION_SHIFT_GROUP)
            | (childPosition & PACKED_POSITION_INT_MASK_CHILD);
}

Int64 ExpandableListView::GetPackedPositionForGroup(
    /* [in] */ Int32 groupPosition)
{
    return ((((Int64)groupPosition) & PACKED_POSITION_INT_MASK_GROUP)
            << PACKED_POSITION_SHIFT_GROUP);
}

AutoPtr<IContextMenuInfo> ExpandableListView::CreateContextMenuInfo(
    /* [in] */ IView* view,
    /* [in] */ Int32 flatListPosition,
    /* [in] */ Int64 id)
{
    if (IsHeaderOrFooterPosition(flatListPosition)) {
        return new AdapterContextMenuInfo(view, flatListPosition, id);
    }

    Int32 adjustedPosition = GetFlatPositionForConnector(flatListPosition);
    AutoPtr<IPositionMetadata> pm;
    mConnector->GetUnflattenedPos(adjustedPosition, (IPositionMetadata**)&pm);
    AutoPtr<IExpandableListPosition> pos = ((PositionMetadata*)pm.Get())->mPosition;

    id = GetChildOrGroupId(pos);
    Int64 packedPosition;
    pos->GetPackedPosition(&packedPosition);
    pm->Recycle();

    AutoPtr<IContextMenuInfo> info;
    CExpandableListContextMenuInfo::New(view, packedPosition, id, (IContextMenuInfo**)&info);
    return info;
}

Int64 ExpandableListView::GetChildOrGroupId(
    /* [in] */ IExpandableListPosition* position)
{
    Int64 id = 0;
    if (((ExpandableListPosition*)position)->mType == ExpandableListPosition::CHILD) {
        mAdapter->GetChildId(((ExpandableListPosition*)position)->mGroupPos, ((ExpandableListPosition*)position)->mChildPos, &id);
        return id;
    }
    else {
        mAdapter->GetGroupId(((ExpandableListPosition*)position)->mGroupPos, &id);
        return id;
    }
}

ECode ExpandableListView::SetChildIndicator(
    /* [in] */ IDrawable* childIndicator)
{
    mChildIndicator = childIndicator;
    return NOERROR;
}

ECode ExpandableListView::SetChildIndicatorBounds(
    /* [in] */ Int32 left,
    /* [in] */ Int32 right)
{
    mChildIndicatorLeft = left;
    mChildIndicatorRight = right;
    ResolveChildIndicator();
    return NOERROR;
}

ECode ExpandableListView::SetChildIndicatorBoundsRelative(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    mChildIndicatorStart = start;
    mChildIndicatorEnd = end;
    ResolveChildIndicator();
    return NOERROR;
}

ECode ExpandableListView::SetGroupIndicator(
    /* [in] */ IDrawable* groupIndicator)
{
    mGroupIndicator = groupIndicator;

    if (mIndicatorRight == 0 && mGroupIndicator != NULL) {
        Int32 res = 0;
        mGroupIndicator->GetIntrinsicWidth(&res);
        mIndicatorRight = mIndicatorLeft + res;
    }
    return NOERROR;
}

ECode ExpandableListView::SetIndicatorBounds(
    /* [in] */ Int32 left,
    /* [in] */ Int32 right)
{
    mIndicatorLeft = left;
    mIndicatorRight = right;
    ResolveIndicator();
    return NOERROR;
}

ECode ExpandableListView::SetIndicatorBoundsRelative(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    mIndicatorStart = start;
    mIndicatorEnd = end;
    ResolveIndicator();
    return NOERROR;
}

AutoPtr<IParcelable> ExpandableListView::OnSaveInstanceState()
{
    AutoPtr<IParcelable> superState = ListView::OnSaveInstanceState();

    AutoPtr<IList> expandedGroupMetadataList;
    if (mConnector.Get() != NULL) {
        ((ExpandableListConnector*)mConnector.Get())->GetExpandedGroupMetadataList((IList**)&expandedGroupMetadataList);
    }

    AutoPtr<IExpandableListViewSavedState> ss = NULL;
    CExpandableListViewSavedState::New(superState, IArrayList::Probe(expandedGroupMetadataList), (IExpandableListViewSavedState**)&ss);

    return IParcelable::Probe(ss);
}

void ExpandableListView::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    if (!(IExpandableListViewSavedState::Probe(state))) {
        ListView::OnRestoreInstanceState(state);
    }

    AutoPtr<IExpandableListView> view = IExpandableListView::Probe(state);
    AutoPtr<SavedState> ss = (SavedState*)view.Get();
    ListView::OnRestoreInstanceState(ss->GetSuperState());

    if (mConnector.Get() != NULL && ss->mExpandedGroupMetadataList != NULL) {
        ((ExpandableListConnector*)mConnector.Get())->SetExpandedGroupMetadataList(IList::Probe(ss->mExpandedGroupMetadataList));
    }
}

ECode ExpandableListView::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    ListView::OnInitializeAccessibilityEvent(event);
    IAccessibilityRecord::Probe(event)->SetClassName(CoreUtils::Convert("CExpandableListView"));
    return NOERROR;
}

ECode ExpandableListView::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    ListView::OnInitializeAccessibilityNodeInfo(info);

    info->SetClassName(CoreUtils::Convert("CExpandableListView"));
    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
