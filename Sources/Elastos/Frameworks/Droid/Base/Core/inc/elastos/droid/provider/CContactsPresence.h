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

#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTSPRESENCE_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTSPRESENCE_H__

#include "_Elastos_Droid_Provider_CContactsPresence.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Widget::IImageView;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContactsPresence)
    , public Singleton
    , public IContactsPresence
    , public IBaseColumns
    , public IContactsPresenceColumns
    , public IContactsPeopleColumns
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    /**
     * The content:// style URL for this table
     * @deprecated see {@link android.provider.ContactsContract}
     */
    //@Deprecated
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * Gets the resource ID for the proper presence icon.
     *
     * @param status the status to get the icon for
     * @return the resource ID for the proper presence icon
     * @deprecated see {@link android.provider.ContactsContract}
     */
    // @Deprecated
    CARAPI GetPresenceIconResourceId(
        /* [in] */ Int32 status,
        /* [out] */ Int32* id);

    /**
     * Sets a presence icon to the proper graphic
     *
     * @param icon the icon to to set
     * @param serverStatus that status
     * @deprecated see {@link android.provider.ContactsContract}
     */
    // @Deprecated
    CARAPI SetPresenceIcon(
        /* [in] */ IImageView* icon,
        /* [in] */ Int32 serverStatus);
};

} // Provider
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCONTACTSPRESENCE_H__
