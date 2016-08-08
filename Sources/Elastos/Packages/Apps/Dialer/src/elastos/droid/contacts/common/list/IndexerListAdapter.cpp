
#include "elastos/droid/contacts/common/list/IndexerListAdapter.h"

using Elastos::Droid::Contacts::Common::List::EIID_IIndexerListAdapter;
using Elastos::Droid::Contacts::Common::List::EIID_IIndexerListAdapterPlacement;
using Elastos::Droid::Widget::EIID_ISectionIndexer;

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
    mPosition = IListView::INVALID_POSITION;
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
// begin from this
ECode IndexerListAdapter::GetIndexedPartition(
    /* [out] */ Boolean* result);

ECode IndexerListAdapter::SetIndexedPartition(
    /* [in] */ Int32 partition);

ECode IndexerListAdapter::GetIndexer(
    /* [out] */ ISectionIndexer** indexer);

ECode IndexerListAdapter::SetIndexer(
    /* [in] */ ISectionIndexer* indexer);

ECode IndexerListAdapter::GetSections(
    /* [out, callee] */ ArrayOf<IInterface*>** sections);

/**
 * @return relative position of the section in the indexed partition
 */
ECode IndexerListAdapter::GetPositionForSection(
    /* [in] */ Int32 sectionIndex,
    /* [out] */ Int32* result);

/**
 * @param position relative position in the indexed partition
 */
ECode IndexerListAdapter::GetSectionForPosition(
    /* [in] */ Int32 position,
    /* [out] */ Int32* result);

// @Override
ECode IndexerListAdapter::ECode IndexerListAdapter::(
    /* [out] */ Int32* count);

// @Override
ECode IndexerListAdapter::GetPinnedHeaderView(
    /* [in] */ Int32 viewIndex,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view);

// @Override
ECode IndexerListAdapter::ConfigurePinnedHeaders(
    /* [in] */ IPinnedHeaderListView* listView);

/**
 * Computes the item's placement within its section and populates the {@code placement}
 * object accordingly.  Please note that the returned object is volatile and should be
 * copied if the result needs to be used later.
 */
ECode IndexerListAdapter::GetItemPlacementInSection(
    /* [in] */ Int32 position,
    /* [out] */ IIndexerListAdapterPlacement** placement);

} // List
} // Common
} // Contacts
} // Droid
} // Elastos
