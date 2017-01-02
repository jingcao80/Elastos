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

#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CM_CQSCONSTANTS_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CM_CQSCONSTANTS_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_Internal_Utility_Cm_CQSConstants.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Singleton.h>

using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

CarClass(CQSConstants)
    , public Singleton
    , public IQSConstants
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

public:
    static AutoPtr<IArrayList> TILES_DEFAULT;
    static AutoPtr<IArrayList> TILES_AVAILABLE;
};

} // namespace Cm
} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CM_CQSCONSTANTS_H__
