
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/contacts/common/list/IndexerListAdapter.h"

using Elastos::Droid::Contacts::Common::List::EIID_IIndexerListAdapter;
using Elastos::Droid::Contacts::Common::List::EIID_IIndexerListAdapterPlacement;
using Elastos::Droid::Widget::EIID_ISectionIndexer;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;

namespace Elastos{
namespace Droid{
namespace Contacts {
namespace Common {
namespace List {

//=================================================================
// IndexerListAdapter::Placement
//=================================================================
CAR_INTERFACE_IMPL(IndexerListAdapter::Placement, Object, IIndexerListAdapterPlacement)

ECode IndexerListAdapter::Placement::Invalidate()
{
    mPosition = IAdapterView::INVALID_POSITION;
    return NOERROR;
}


//=================================================================
// IndexerListAdapter
//=================================================================
IndexerListAdapter::IndexerListAdapter()
    : mIndexedPartition(0)
    , mSectionHeaderDisplayEnabled(FALSE)
{
    mPlacementCache = new Placement();
}

CAR_INTERFACE_IMPL_2(IndexerListAdapter, PinnedHeaderListAdapter, IIndexerListAdapter, ISectionIndexer)

ECode IndexerListAdapter::constructor(
    /* [in] */ IContext* context)
{
    PinnedHeaderListAdapter::constructor(context);
    mContext = context;
    return NOERROR;
}

ECode IndexerListAdapter::IsSectionHeaderDisplayEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSectionHeaderDisplayEnabled;
    return NOERROR;
}

ECode IndexerListAdapter::SetSectionHeaderDisplayEnabled(
    /* [in] */ Boolean flag)
{
    mSectionHeaderDisplayEnabled = flag;
    return NOERROR;
}

ECode IndexerListAdapter::GetIndexedPartition(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
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
    VALIDATE_NOT_NULL(indexer)
    *indexer = mIndexer;
    REFCOUNT_ADD(*indexer)
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
    VALIDATE_NOT_NULL(sections)
    if (mIndexer == NULL) {
        AutoPtr<ArrayOf<IInterface*> > temp = ArrayOf<IInterface*>::Alloc(1);
        AutoPtr<ICharSequence> cs;
        CString::New(String(" "), (ICharSequence**)&cs);
        temp->Set(0, cs);
        *sections = temp;
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
    VALIDATE_NOT_NULL(result)
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
    VALIDATE_NOT_NULL(result)
    if (mIndexer == NULL) {
        *result = -1;
        return NOERROR;
    }

    return mIndexer->GetSectionForPosition(position, result);
}

ECode IndexerListAdapter::GetPinnedHeaderCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    Boolean enabled;
    if (IsSectionHeaderDisplayEnabled(&enabled), enabled) {
        Int32 c;
        PinnedHeaderListAdapter::GetPinnedHeaderCount(&c);
        *count = c + 1;
        return NOERROR;
    }
    else {
        return PinnedHeaderListAdapter::GetPinnedHeaderCount(count);
    }
}

ECode IndexerListAdapter::GetPinnedHeaderView(
    /* [in] */ Int32 viewIndex,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)
    Boolean enabled;
    Int32 count;
    if ((IsSectionHeaderDisplayEnabled(&enabled), enabled) &&
                (GetPinnedHeaderCount(&count), viewIndex == count - 1)) {
        if (mHeader == NULL) {
            mHeader = CreatePinnedSectionHeaderView(mContext, parent);
        }
        *view = mHeader;
        REFCOUNT_ADD(*view)
        return NOERROR;
    }
    else {
        return PinnedHeaderListAdapter::GetPinnedHeaderView(viewIndex, convertView, parent, view);
    }
}

ECode IndexerListAdapter::ConfigurePinnedHeaders(
    /* [in] */ IPinnedHeaderListView* listView)
{
    FAIL_RETURN(PinnedHeaderListAdapter::ConfigurePinnedHeaders(listView))

    Boolean enabled;
    if (IsSectionHeaderDisplayEnabled(&enabled), !enabled) {
        return NOERROR;
    }

    Int32 count;
    GetPinnedHeaderCount(&count);
    Int32 index = count - 1;
    if (mIndexer == NULL || (GetCount(&count), count == 0)) {
        listView->SetHeaderInvisible(index, FALSE);
    }
    else {
        Int32 listPosition;
        Int32 height;
        listView->GetTotalTopPinnedHeaderHeight(&height);
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
            IViewGroup::Probe(listView)->GetChildAt(listPosition, (IView**)&topChild);
            if (topChild != NULL) {
                // Match the pinned header's height to the height of the list item.
                Int32 measuredH;
                topChild->GetMeasuredHeight(&measuredH);
                mHeader->SetMinimumHeight(measuredH);
            }
            AutoPtr<ArrayOf<IInterface*> > sections;
            mIndexer->GetSections((ArrayOf<IInterface*>**)&sections);
            String title;
            ICharSequence::Probe((*sections)[section])->ToString(&title);
            SetPinnedSectionTitle(mHeader, title);

            // Compute the item position where the current partition begins
            Int32 partitionStart;
            GetPositionForPartition(mIndexedPartition, &partitionStart);
            Boolean hasHeader;
            if (HasHeader(mIndexedPartition, &hasHeader), hasHeader) {
                partitionStart++;
            }

            // Compute the item position where the next section begins
            Int32 pos;
            GetPositionForSection(section + 1, &pos);
            Int32 nextSectionPosition = partitionStart + pos;
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
    VALIDATE_NOT_NULL(placement)
    if (mPlacementCache->mPosition == position) {
        *placement = IIndexerListAdapterPlacement::Probe(mPlacementCache);
        REFCOUNT_ADD(*placement)
        return NOERROR;
    }

    mPlacementCache->mPosition = position;
    Boolean enabled;
    if (IsSectionHeaderDisplayEnabled(&enabled), enabled) {
        Int32 section;
        GetSectionForPosition(position, &section);
        Int32 pos;
        if (section != -1 && (GetPositionForSection(section, &pos), pos == position) {
            mPlacementCache->mFirstInSection = TRUE;
            AutoPtr<ArrayOf<IInterface*> > sections;
            GetSections((ArrayOf<IInterface*>**)&sections);
            ICharSequence::Probe((*sections)[section])->ToString(&mPlacementCache->mSectionHeader);
        }
        else {
            mPlacementCache->mFirstInSection = FALSE;
            mPlacementCache->mSectionHeader = String(NULL);
        }

        GetPositionForSection(section + 1, &pos);
        mPlacementCache->mLastInSection = (pos - 1 == position);
    }
    else {
        mPlacementCache->mFirstInSection = FALSE;
        mPlacementCache->mLastInSection = FALSE;
        mPlacementCache->mSectionHeader = String(NULL);
    }
    *placement = IIndexerListAdapterPlacement::Probe(mPlacementCache);
    REFCOUNT_ADD(*placement)
    return NOERROR;
}

} // List
} // Common
} // Contacts
} // Droid
} // Elastos
