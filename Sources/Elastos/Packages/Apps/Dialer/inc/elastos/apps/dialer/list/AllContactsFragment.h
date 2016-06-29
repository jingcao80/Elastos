#ifndef __ELASTOS_APPS_DIALER_LIST_ALLCONTACTSFRAGMENT_H__
#define __ELASTOS_APPS_DIALER_LIST_ALLCONTACTSFRAGMENT_H__

#include "_Elastos.Apps.Dialer.h"
#include <elastos/droid/app/Fragment.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::App::Fragment;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IAdapterView;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace List {

/**
 * Fragments to show all contacts with phone numbers.
 */
class AllContactsFragment
    // TODO:
    /*: public ContactEntryListFragment*/
    : public Fragment
    , public IAllContactsFragment
{
private:
    // class MyDefaultContactListAdapter
    //     : public DefaultContactListAdapter
    // {
    // public:
    //     MyDefaultContactListAdapter(
    //         /* [in] */ AllContactsFragment* host);

    // protected:
    //     // @Override
    //     CARAPI BindView(
    //         /* [in] */ IView* itemView,
    //         /* [in] */ Int32 partition,
    //         /* [in] */ ICursor* cursor,
    //         /* [in] */ Int32 position);

    // private:
    //     AllContactsFragment* mHost;
    // };

public:
    CAR_INTERFACE_DECL();

    AllContactsFragment();

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
    // TODO:
    // CARAPI CreateListAdapter(
    //     /* [out] */ IContactEntryListAdapter** adapter);

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
