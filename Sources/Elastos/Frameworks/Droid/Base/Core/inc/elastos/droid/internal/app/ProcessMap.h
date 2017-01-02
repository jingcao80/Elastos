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

#ifndef __ELASTOS_DROID_INTERNAL_APP_PROCESSMAP_H__
#define __ELASTOS_DROID_INTERNAL_APP_PROCESSMAP_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::IArrayMap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

class ProcessMap
    : public Object
    , public IProcessMap
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI Get(
        /* [in] */ const String& name,
        /* [in] */ Int32 uid,
        /* [out] */ IInterface** value);

    CARAPI Put(
        /* [in] */ const String& name,
        /* [in] */ Int32 uid,
        /* [in] */ IInterface* value);

    CARAPI Remove(
        /* [in] */ const String& name,
        /* [in] */ Int32 uid);

    CARAPI GetMap(
        /* [out] */ IArrayMap** map);

public:
    AutoPtr<IArrayMap> mMap;
};

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_PROCESSMAP_H__
