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

#ifndef __ELASTOS_DROID_SERVER_NOTIFICATION_NOTIFICATIONCOMPARATOR_H__
#define __ELASTOS_DROID_SERVER_NOTIFICATION_NOTIFICATIONCOMPARATOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::IComparator;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

/**
 * Sorts notifications individually into attention-relelvant order.
 */
class NotificationComparator
    : public Object
    , public IComparator
{
public:
    CAR_INTERFACE_DECL()

    NotificationComparator();

    ~NotificationComparator();

    // @Override
    CARAPI Compare(
        /* [in] */ IInterface* lhs,
        /* [in] */ IInterface* rhs,
        /* [out] */ Int32* result);
};

} // Notification
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_NOTIFICATION_NOTIFICATIONCOMPARATOR_H__
