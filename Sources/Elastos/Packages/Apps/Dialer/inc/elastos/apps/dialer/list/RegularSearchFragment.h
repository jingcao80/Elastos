#ifndef __ELASTOS_APPS_DIALER_LIST_REGULARSEARCHFRAGMENT_H__
#define __ELASTOS_APPS_DIALER_LIST_REGULARSEARCHFRAGMENT_H__

#include "_Elastos.Apps.Dialer.h"
#include "elastos/apps/dialer/list/SearchFragment.h"
#include "Elastos.Droid.View.h"

using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IViewGroup;
using Elastos::Apps::Dialer::Service::ICachedNumberLookupService;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace List {

class RegularSearchFragment
    : public SearchFragment
    , public IRegularSearchFragment
{
public:
    CAR_INTERFACE_DECL();

    CARAPI constructor();

    CARAPI ConfigureDirectorySearch();

protected:
    // @Override
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container);

    // TODO:
    // CARAPI_(AutoPtr<IContactEntryListAdapter>) CreateListAdapter();

    // @Override
    CARAPI CacheContactInfo(
        /* [in] */ Int32 position);

private:
    static const Int32 SEARCH_DIRECTORY_RESULT_LIMIT; // = 5;

    static const AutoPtr<ICachedNumberLookupService> mCachedNumberLookupService;
};

} // List
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_LIST_REGULARSEARCHFRAGMENT_H__
