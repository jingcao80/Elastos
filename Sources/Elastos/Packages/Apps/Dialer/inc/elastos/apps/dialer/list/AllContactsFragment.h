#ifndef __ELASTOS_APPS_DIALER_LIST_ALLCONTACTSFRAGMENT_H__
#define __ELASTOS_APPS_DIALER_LIST_ALLCONTACTSFRAGMENT_H__

#include "ContactEntryListFragment.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace List {

/**
 * Fragments to show all contacts with phone numbers.
 */
class AllContactsFragment
    : public ContactEntryListFragment
    , public IAllContactsFragment
{
private:
    class MyDefaultContactListAdapter()
        : public DefaultContactListAdapter
    {
    public:
        MyDefaultContactListAdapter(
            /* [in] */ AllContactsFragment* host);

    protected:
        // @Override
        CARAPI BindView(
            /* [in] */ IView* itemView,
            /* [in] */ Int32 partition,
            /* [in] */ ICursor* cursor,
            /* [in] */ Int32 position);

    private:
        AllContactsFragment* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    AllContactsFragment()

    // @Override
    CARAPI OnViewCreated(
        /* [in] */ IView* view,
        /* [in] */ IBundle* savedInstanceState);

    // @Override
    CARAPI OnItemClick(
        /* [in] */ IAdapterView* parent,
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

protected:
    // @Override
    CARAPI CreateListAdapter(
        /* [out] */ IContactEntryListAdapter** adapter);

    // @Override
    CARAPI InflateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [out] */ IView** view);

    // @Override
    CARAPI OnItemClick(
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);
};

} // List
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_LIST_ALLCONTACTSFRAGMENT_H__
