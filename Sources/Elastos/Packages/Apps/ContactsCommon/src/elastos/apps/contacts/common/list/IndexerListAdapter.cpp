
#include "IndexerListAdapter.h"

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace List {

//=================================================================
// IndexerListAdapter::Placement
//=================================================================
CAR_INTERFACE_IMPL(Placement, object, IIndexerListAdapterPlacement)

IndexerListAdapter::Placement::Placement()
    : mPosition(IListView::INVALID_POSITION)
{}

ECode IndexerListAdapter::Placement::Invalidate()
{
    mPosition = IListView::INVALID_POSITION;
    return NOERROR;
}

//=================================================================
// IndexerListAdapter
//=================================================================
CAR_INTERFACE_IMPL_2(IndexerListAdapter, PinnedHeaderListAdapter, IIndexerListAdapter, ISectionIndexer)

CAR_OBJECT_IMPL(IndexerListAdapter)

IndexerListAdapter::IndexerListAdapter()
    : mIndexedPartition(0)
{
    mPlacementCache = new Placement();
}

ECode IndexerListAdapter::constructor(
    /* [in] */ IContext* context)
{
    return CPinnedHeaderListAdapter::constructor(context);
}

ECode IndexerListAdapter::IsSectionHeaderDisplayEnabled(
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);
    *result = mSectionHeaderDisplayEnable;
    return NOERROR;
}

ECode IndexerListAdapter::SetSectionHeaderDisplayEnabled(
    /* [in] */ Boolean flag)
{
    mSectionHeaderDisplayEnabled = flag;
    return NOERROR;
}

ECode IndexerListAdapter::GetIndexedPartition(
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);
    *result = mIndexedPartition;
    return NOERROR;
}

ECode IndexerListAdapter::SetIndexedPartition(
    /* [in] */ Int32 partition)
{
    mIndexedPartition = partition;
    return NOERROR;
}

ECode IndexerListAdapter::GetIndexer(
    /* [out] */ ISectionIndexer** indexer)
{
    VALUE_NOT_NULL(indexer);
    *indexer = mIndexer;
    REFCOUNT_ADD(*indexer);
    return NOERROR;
}

ECode IndexerListAdapter::SetIndexer(
    /* [in] */ ISectionIndexer* indexer)
{
    mIndexer = indexer;
    mPlacementCache->Invalidate();
    return NOERROR;
}

ECode IndexerListAdapter::GetSections(
    /* [out, callee] */ ArrayOf<IInterface*>** sections)
{
    VALUE_NOT_NULL(sections);

    if (mIndexer == NULL) {
        AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(1);
        array->Set(0, CoreUtils::Convert(String(" ")));
        *sections = array;
        REFCOUNT_ADD(*sections);
        return NOERROR;
    }
    else {
        return mIndexer->GetSections(sections);
    }
}

ECode IndexerListAdapter::GetPositionForSection(
    /* [in] */ Int32 sectionIndex,
    /* [out] */ Int32* result)
{
    VALUE_NOT_NULL(result);
    if (mIndexer == NULL) {
        *result = -1;
        return NOERROR;
    }

    return mIndexer->GetPositionForSection(sectionIndex, result);
}

ECode IndexerListAdapter::GetSectionForPosition(
    /* [in] */ Int32 position,
    /* [out] */ Int32* result)
{
    VALUE_NOT_NULL(result);
    if (mIndexer == NULL) {
        *result = -1;
        return NOERROR;
    }

    return mIndexer->GetSectionForPosition(position, result);
}

ECode IndexerListAdapter::GetPinnedHeaderCount(
    /* [out] */ Int32* count)
{
    VALUE_NOT_NULL(count);

    Boolean result = FALSE;
    if (IsSectionHeaderDisplayEnabled(&result), result) {
        Int32 headerCount;
        CPinnedHeaderListAdapter::GetPinnedHeaderCount(&headerCount);
        *count = headerCount + 1;
        return NOERROR;
    }
    else {
        return CPinnedHeaderListAdapter::getPinnedHeaderCount(count);
    }
}

ECode IndexerListAdapter::GetPinnedHeaderView(
    /* [in] */ Int32 partition,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** result)
{
    VALUE_NOT_NULL(result);

    Boolean enabled = FALSE;
    Int32 count;
    if (IsSectionHeaderDisplayEnabled(&enabled), enabled
            && GetPinnedHeaderCount(&count), viewIndex == count - 1) {
        if (mHeader == NULL) {
             CreatePinnedSectionHeaderView(mContext, parent, &mHeader);
        }
        *result = mHeader;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    else {
        return CPinnedHeaderListAdapter::GetPinnedHeaderView(
                viewIndex, convertView, parent, result);
    }
}

ECode IndexerListAdapter::ConfigurePinnedHeaders(
    /* [in] */ IPinnedHeaderListView* listView)
{
    CPinnedHeaderListAdapter::ConfigurePinnedHeaders(listView);

    Boolean enabled = FALSE;
    if (isSectionHeaderDisplayEnabled(&enabled), !enabled) {
        return NOERROR;
    }

    Int32 index;
    GetPinnedHeaderCount(&index);
    index -= 1;

    Int32 count;
    if (mIndexer == NULL || GetCount(&count), count == 0) {
        listView->SetHeaderInvisible(index, FALSE);
    }
    else {
        Int32 height;
        listView->GetTotalTopPinnedHeaderHeight(&height);
        Int32 listPosition;
        listView->GetPositionAt(height, &listPosition);
        listView->GetHeaderViewsCount(&count);
        Int32 position = listPosition - count;

        Int32 section = -1;
        Int32 partition;
        GetPartitionForPosition(position, &partition);
        if (partition == mIndexedPartition) {
            Int32 offset;
            GetOffsetInPartition(position, &offset);
            if (offset != -1) {
                GetSectionForPosition(offset, &section);
            }
        }

        if (section == -1) {
            listView->SetHeaderInvisible(index, FALSE);
        }
        else {
            AutoPtr<IView> topChild;
            listView->GetChildAt(listPosition, (IView**)&topChild);
            if (topChild != NULL) {
                // Match the pinned header's height to the height of the list item.
                topChild->GetMeasuredHeight(&height);
                mHeader->SetMinimumHeight(height);
            }

            AutoPtr<ArrayOf<IInterface*> > sections;
            mIndexer->GetSections((ArrayOf<IInterface*>**)&sections);
            SetPinnedSectionTitle(mHeader, CoreUtils::Unbox(sections[section]));

            // Compute the item position where the current partition begins
            Int32 partitionStart;
            GetPositionForPartition(mIndexedPartition, &partitionStart);
            if (HasHeader(mIndexedPartition)) {
                partitionStart++;
            }

            // Compute the item position where the next section begins
            Int32 positionForSection;
            GetPositionForSection(section + 1, &positionForSection);
            Int32 nextSectionPosition = partitionStart + positionForSection;
            Boolean isLastInSection = position == nextSectionPosition - 1;
            listView->SetFadingHeader(index, listPosition, isLastInSection);
        }
    }

    return NOERROR;
}

ECode IndexerListAdapter::GetItemPlacementInSection(
    /* [in] */ Int32 position,
    /* [out] */ IIndexerListAdapterPlacement** placement)
{
    VALUE_NOT_NULL(placement);

    if (mPlacementCache->mPosition == position) {
        *placement = (IIndexerListAdapterPlacement*)mPlacementCache;
        REFCOUNT_ADD(*placement);
        return NOERROR;
    }

    mPlacementCache->mPosition = position;
    Boolean enabled = FALSE;
    if (IsSectionHeaderDisplayEnabled(&enabled), enabled) {
        Int32 section;
        GetSectionForPosition(position, &section);
        Int32 positionForSection;
        if (section != -1 &&
                GetPositionForSection(section, &positionForSection), positionForSection == position) {
            mPlacementCache->mFirstInSection = TRUE;
            AutoPtr<ArrayOf<IInterface*> > sections;
            GetSections((ArrayOf<IInterface*>**)&sections);
            mPlacementCache->mSectionHeader = CoreUtils::Unbox(sections[section]);
        }
        else {
            mPlacementCache->mFirstInSection = FALSE;
            mPlacementCache->mSectionHeader = String(NULL);
        }

        GetPositionForSection(section + 1, &positionForSection);
        mPlacementCache->mLastInSection = (positionForSection - 1 == position);
    }
    else {
        mPlacementCache->mFirstInSection = FALSE;
        mPlacementCache->mLastInSection = FALSE;
        mPlacementCache->mSectionHeader = String(NULL);
    }

    *placement = (IIndexerListAdapterPlacement*)mPlacementCache;
    REFCOUNT_ADD(*placement);
    return NOERROR;
}

} // List
} // Common
} // Contacts
} // Apps
} // Elastos
