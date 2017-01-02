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

#ifndef __ELASTOS_DROID_CONTACTS_COMMON_CONTACTSTATUSNUTIL_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_CONTACTSTATUSNUTIL_H__

#include "Elastos.Droid.Content.h"

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {

/**
 * Provides static function to get default contact status message.
 */
class ContactStatusUtil
{
public:
    /**
     * Get the presence icon resource according the status.
     *
     * @return null means don't show the status icon.
     */
    static CARAPI_(String) GetStatusString(
        /* [in] */ IContext* context,
        /* [in] */ Int32 presence);

private:
    static const String TAG;
};

} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_CONTACTSTATUSNUTIL_H__
