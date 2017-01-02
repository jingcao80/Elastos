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

#ifndef __ELASTOS_DROID_DIALER_LIST_REGULARSEARCHLISTADAPTER_H__
#define __ELASTOS_DROID_DIALER_LIST_REGULARSEARCHLISTADAPTER_H__

#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/dialer/list/DialerPhoneNumberListAdapter.h"
#include "Elastos.Droid.Content.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Dialer::Service::ICachedNumberLookupService;
using Elastos::Droid::Dialer::Service::ICachedContactInfo;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

/**
 * List adapter to display regular search results.
 */
class RegularSearchListAdapter
    : public DialerPhoneNumberListAdapter
    , public IRegularSearchListAdapter
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI GetContactInfo(
        /* [in] */ ICachedNumberLookupService* lookupService,
        /* [in] */ Int32 position,
        /* [out] */ ICachedContactInfo** info);

    // @Override
    CARAPI SetQueryString(
        /* [in] */ const String& queryString);
};

} // List
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_LIST_REGULARSEARCHLISTADAPTER_H__
