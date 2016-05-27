#ifndef __ELASTOS_APPS_DIALER_LIST_REGULARSEARCHFRAGMENT_H__
#define __ELASTOS_APPS_DIALER_LIST_REGULARSEARCHFRAGMENT_H__

#include "SearchFragment.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace List {

class RegularSearchFragment
    : public SearchFragment
    , public IRegularSearchFragment
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI ConfigureDirectorySearch();

protected:
    // @Override
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container);

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
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_LIST_REGULARSEARCHFRAGMENT_H__
