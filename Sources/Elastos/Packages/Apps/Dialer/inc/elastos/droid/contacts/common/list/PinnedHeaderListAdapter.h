
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_LIST_PINNEDHEADERLISTADAPTER_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_LIST_PINNEDHEADERLISTADAPTER_H__

#include "elastos/droid/common/widget/CompositeCursorAdapter.h"

namespace Elastos{
namespace Droid{
namespace Contacts {
namespace Common {
namespace List {

/**
 * A subclass of {@link CompositeCursorAdapter} that manages pinned partition headers.
 */
class PinnedHeaderListAdapter
    : public CompositeCursorAdapter
    , public IPinnedHeaderListAdapter
    , public IPinnedHeaderAdapter
{
public:
    PinnedHeaderListAdapter()
        : mPinnedPartitionHeadersEnabled(FALSE)
    {}

    CAR_INTERFACE_DECL()

    virtual ~PinnedHeaderListAdapter() {}

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 initialCapacity);

    CARAPI GetPinnedPartitionHeadersEnabled(
        /* [out] */ Boolean* result);

    CARAPI SetPinnedPartitionHeadersEnabled(
        /* [in] */ Boolean flag);

    // @Override
    CARAPI GetPinnedHeaderCount(
        /* [out] */ Int32* count);

    /**
     * The default implementation creates the same type of view as a normal
     * partition header.
     */
    // @Override
    CARAPI GetPinnedHeaderView(
        /* [in] */ Int32 viewIndex,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

    // @Override
    CARAPI ConfigurePinnedHeaders(
        /* [in] */ IPinnedHeaderListView* listView);

    // @Override
    CARAPI GetScrollPositionForHeader(
        /* [in] */ Int32 viewIndex,
        /* [out] */ Int32* position);

protected:
    CARAPI_(Boolean) IsPinnedPartitionHeaderVisible(
        /* [in] */ Int32 partition);

private:
    Boolean mPinnedPartitionHeadersEnabled;
    AutoPtr<ArrayOf<Boolean> > mHeaderVisibility;
};

} // List
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_LIST_PINNEDHEADERLISTADAPTER_H__
