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

#ifndef __ELASTOS_DROID_COMMANDS_AM_CAM_H__
#define __ELASTOS_DROID_COMMANDS_AM_CAM_H__

#include "_Elastos_Droid_Commands_Am_CAm.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Am {

CarClass(CAm)
    , public Singleton
    , public IAm
{
public:
    CAR_SINGLETON_DECL();

    CAR_INTERFACE_DECL()

    /**
     * Command-line entry point.
     *
     * @param args The command-line arguments
     */
    CARAPI Main(
        /* [in] */ const ArrayOf<String>& args);
};

}
}
}
}

#endif // __ELASTOS_DROID_COMMANDS_AM_CAM_H__
