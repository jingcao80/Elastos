
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_LIST_INDEXERLISTADAPTER_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_LIST_INDEXERLISTADAPTER_H__

#include "elastos/droid/contacts/common/list/PinnedHeaderListAdapter.h"

using Elastos::Droid::Widget::ISectionIndexer;
using Elastos::Droid::Widget::IListView;

namespace Elastos{
namespace Droid{
namespace Contacts {
namespace Common {
namespace List {

/**
 * A list adapter that supports section indexer and a pinned header.
 */
class IndexerListAdapter
    : public PinnedHeaderListAdapter
    , public IIndexerListAdapter
    , public ISectionIndexer
{
public:
    /**
     * An item view is displayed differently depending on whether it is placed
     * at the beginning, middle or end of a section. It also needs to know the
     * section header when it is at the beginning of a section. This object
     * captures all this configuration.
     */
    class Placement
        : public Object
        , public IIndexerListAdapterPlacement
    {
    public:
        Placement()
            : mFirstInSection(FALSE)
            , mLastInSection(FALSE)
            , mPosition(IListView::INVALID_POSITION)
        {}

        CAR_INTERFACE_DECL()

        CARAPI Invalidate();

    public:
        Boolean mFirstInSection;
        Boolean mLastInSection;
        String mSectionHeader;

    private:
        Int32 mPosition;
    };

public:
    IndexerListAdapter();

    CAR_INTERFACE_DECL()

    virtual ~IndexerListAdapter() {}

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI IsSectionHeaderDisplayEnabled(
        /* [out] */ Boolean* result);

    CARAPI SetSectionHeaderDisplayEnabled(
        /* [in] */ Boolean flag);

    CARAPI GetIndexedPartition(
        /* [out] */ Boolean* result);

    CARAPI SetIndexedPartition(
        /* [in] */ Int32 partition);

    CARAPI GetIndexer(
        /* [out] */ ISectionIndexer** indexer);

    CARAPI SetIndexer(
        /* [in] */ ISectionIndexer* indexer);

    CARAPI GetSections(
        /* [out, callee] */ ArrayOf<IInterface*>** sections);

    /**
     * @return relative position of the section in the indexed partition
     */
    CARAPI GetPositionForSection(
        /* [in] */ Int32 sectionIndex,
        /* [out] */ Int32* result);

    /**
     * @param position relative position in the indexed partition
     */
    CARAPI GetSectionForPosition(
        /* [in] */ Int32 position,
        /* [out] */ Int32* result);

    // @Override
    CARAPI GetPinnedHeaderCount(
        /* [out] */ Int32* count);

    // @Override
    CARAPI GetPinnedHeaderView(
        /* [in] */ Int32 viewIndex,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

    // @Override
    CARAPI ConfigurePinnedHeaders(
        /* [in] */ IPinnedHeaderListView* listView);

    /**
     * Computes the item's placement within its section and populates the {@code placement}
     * object accordingly.  Please note that the returned object is volatile and should be
     * copied if the result needs to be used later.
     */
    CARAPI GetItemPlacementInSection(
        /* [in] */ Int32 position,
        /* [out] */ IIndexerListAdapterPlacement** placement);

protected:
    /**
     * Creates a section header view that will be pinned at the top of the list
     * as the user scrolls.
     */
    virtual CARAPI_(AutoPtr<IView>) CreatePinnedSectionHeaderView(
        /* [in] */ IContext* context,
        /* [in] */ IViewGroup* parent) = 0;

    /**
     * Sets the title in the pinned header as the user scrolls.
     */
    virtual CARAPI_(void) SetPinnedSectionTitle(
        /* [in] */ IView* pinnedHeaderView,
        /* [in] */ const String& title) = 0;

protected:
    AutoPtr<IContext> mContext;

private:
    AutoPtr<ISectionIndexer> mIndexer;
    Int32 mIndexedPartition;
    Boolean mSectionHeaderDisplayEnabled;
    AutoPtr<IView> mHeader;
    AutoPtr<Placement> mPlacementCache;
};

} // List
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_LIST_INDEXERLISTADAPTER_H__
