//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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

    CARAPI constructor();

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
