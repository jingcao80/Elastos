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

#include "elastos/droid/utility/CPropertyHelper.h"
#include "elastos/droid/utility/ReflectiveProperty.h"
#include "elastos/droid/ext/frameworkdef.h"

namespace Elastos {
namespace Droid {
namespace Utility {

CPropertyHelper::CPropertyHelper()
{}

ECode CPropertyHelper::constructor()
{
   return NOERROR;
}

ECode CPropertyHelper::Of(
    /* [in] */ const ClassID& clsId,
    /* [in] */ CarDataType dataType,
    /* [in] */ const String& name,
    /* [out] */ IProperty** property)
{
    AutoPtr<IReflectiveProperty> pTemp = new ReflectiveProperty(clsId, dataType, name);
    *property = pTemp;
    REFCOUNT_ADD(*property);
    return NOERROR;
}

} // namespace Utility
} // namepsace Droid
} // namespace Elastos
