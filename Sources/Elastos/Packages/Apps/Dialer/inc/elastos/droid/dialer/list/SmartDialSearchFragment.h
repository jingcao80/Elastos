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

#ifndef __ELASTOS_DROID_DIALER_LIST_SMARTDIALSEARCHFRAGMENT_H__
#define __ELASTOS_DROID_DIALER_LIST_SMARTDIALSEARCHFRAGMENT_H__

#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/dialer/list/SearchFragment.h"

using Elastos::Droid::Content::ILoader;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

/**
 * Implements a fragment to load and display SmartDial search results.
 */
class SmartDialSearchFragment
    : public SearchFragment
    , public ISmartDialSearchFragment
{
public:
    CAR_INTERFACE_DECL();

    CARAPI constructor();

    /**
     * Creates a SmartDialCursorLoader object to load query results.
     */
    // @Override
    CARAPI OnCreateLoader(
        /* [in] */ Int32 id,
        /* [in] */ IBundle* args,
        /* [out] */ ILoader** loader);

protected:
    /**
     * Creates a SmartDialListAdapter to display and operate on search results.
     */
    // @Override
    CARAPI_(AutoPtr<IContactEntryListAdapter>) CreateListAdapter();

    /**
     * Gets the Phone Uri of an entry for calling.
     * @param position Location of the data of interest.
     * @return Phone Uri to establish a phone call.
     */
    // @Override
    CARAPI GetPhoneUri(
        /* [in] */ Int32 position,
        /* [out] */ IUri** uri);

private:
    static const String TAG; // = SmartDialSearchFragment.class.getSimpleName();
};

} // List
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_LIST_SMARTDIALSEARCHFRAGMENT_H__
