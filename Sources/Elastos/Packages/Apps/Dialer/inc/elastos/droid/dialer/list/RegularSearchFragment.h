#ifndef __ELASTOS_DROID_DIALER_LIST_REGULARSEARCHFRAGMENT_H__
#define __ELASTOS_DROID_DIALER_LIST_REGULARSEARCHFRAGMENT_H__

#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/dialer/list/SearchFragment.h"
#include "Elastos.Droid.View.h"

using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Dialer::Service::ICachedNumberLookupService;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

class RegularSearchFragment
    : public SearchFragment
    , public IRegularSearchFragment
{
public:
    RegularSearchFragment();

    CAR_INTERFACE_DECL();

    CARAPI ConfigureDirectorySearch();

protected:
    // @Override
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container);

    // @Override
    CARAPI_(AutoPtr<IContactEntryListAdapter>) CreateListAdapter();

    // @Override
    CARAPI CacheContactInfo(
        /* [in] */ Int32 position);

private:
    static const Int32 SEARCH_DIRECTORY_RESULT_LIMIT; // = 5;

    static const AutoPtr<ICachedNumberLookupService> mCachedNumberLookupService;
};

} // List
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_LIST_REGULARSEARCHFRAGMENT_H__
