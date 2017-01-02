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

#ifndef __ELASTOS_DROID_SETTINGS_PINNEDHEADERLISTFRAGMENT_H__
#define __ELASTOS_DROID_SETTINGS_PINNEDHEADERLISTFRAGMENT_H__

#include "elastos/droid/app/ListFragment.h"

using Elastos::Droid::App::ListFragment;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Settings {

/**
 * A ListFragment with a pinned header
 */
class PinnedHeaderListFragment
    : public ListFragment
{
public:
    PinnedHeaderListFragment();

    virtual ~PinnedHeaderListFragment();

    CARAPI constructor();

    /**
     * Set the pinned header view. This can only be done when the ListView is already created.
     *
     * @param pinnedHeaderView the view to be used for the pinned header view.
     */
    virtual CARAPI SetPinnedHeaderView(
        /* [in] */ IView* pinnedHeaderView);

    /**
     * Clear the pinned header view. This can only be done when the ListView is already created.
     */
    virtual CARAPI ClearPinnedHeaderView();
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_PINNEDHEADERLISTFRAGMENT_H__