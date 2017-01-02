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

#include "CCollapser.h"
#include "Collapser.h"

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {

CAR_INTERFACE_IMPL(CCollapser, Singleton, ICollapser)

CAR_SINGLETON_IMPL(CCollapser)

ECode CCollapser::CollapseList(
    /* [in] */ IList* list,
    /* [in] */ IContext* context)
{
    Collapser::CollapseList(list, context);
    return NOERROR;
}

} // Common
} // Contacts
} // Apps
} // Elastos
