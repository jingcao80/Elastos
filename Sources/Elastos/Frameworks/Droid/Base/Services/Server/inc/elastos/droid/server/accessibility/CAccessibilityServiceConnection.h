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

#ifndef __ELASTOS_DROID_SERVER_ACCESSIBILITY_CACCESSIBILITYSERVICECONNECTION_H__
#define __ELASTOS_DROID_SERVER_ACCESSIBILITY_CACCESSIBILITYSERVICECONNECTION_H__

#include "_Elastos_Droid_Server_Accessibility_CAccessibilityServiceConnection.h"
#include "elastos/droid/server/accessibility/AccessibilityManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accessibility {

/**
 * This class represents an accessibility service. It stores all per service
 * data required for the service management, provides API for starting/stopping the
 * service and is responsible for adding/removing the service in the data structures
 * for service management. The class also exposes configuration interface that is
 * passed to the service it represents as soon it is bound. It also serves as the
 * connection for the service.
 */
CarClass(CAccessibilityServiceConnection)
    , public AccessibilityManagerService::Service
{
public:
    CAR_OBJECT_DECL()
};

} // Accessibility
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_ACCESSIBILITY_CACCESSIBILITYSERVICECONNECTION_H__
