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

#include "elastos/droid/internal/app/ProcessMap.h"
#include "elastos/droid/utility/CArrayMap.h"
#include "elastos/droid/utility/CSparseArray.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CAR_INTERFACE_IMPL(ProcessMap, Object, IProcessMap)

ECode ProcessMap::constructor()
{
    return CArrayMap::New((IArrayMap**)&mMap);
}

ECode ProcessMap::Get(
    /* [in] */ const String& name,
    /* [in] */ Int32 uid,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value)
    *value = NULL;
    AutoPtr<IInterface> item;
    mMap->Get(CoreUtils::Convert(name), (IInterface**)&item);
    AutoPtr<ISparseArray> uids = ISparseArray::Probe(item);
    if (uids == NULL)
        return NOERROR;
    return uids->Get(uid, value);
}

ECode ProcessMap::Put(
    /* [in] */ const String& name,
    /* [in] */ Int32 uid,
    /* [in] */ IInterface* value)
{
    AutoPtr<ICharSequence> key = CoreUtils::Convert(name);
    AutoPtr<IInterface> item;
    mMap->Get(key, (IInterface**)&item);
    AutoPtr<ISparseArray> uids = ISparseArray::Probe(item);
    if (uids == NULL) {
        CSparseArray::New(2, (ISparseArray**)&uids);
        mMap->Put(key, uids);
    }
    uids->Put(uid, value);
    return NOERROR;
}

ECode ProcessMap::Remove(
    /* [in] */ const String& name,
    /* [in] */ Int32 uid)
{
    AutoPtr<ICharSequence> key = CoreUtils::Convert(name);
    AutoPtr<IInterface> item;
    mMap->Get(key, (IInterface**)&item);
    AutoPtr<ISparseArray> uids = ISparseArray::Probe(item);
    if (uids != NULL) {
        uids->Remove(uid);
        Int32 size;
        uids->GetSize(&size);
        if (size == 0) {
            mMap->Remove(key);
        }
    }
    return NOERROR;
}

ECode ProcessMap::GetMap(
    /* [out] */ IArrayMap** map)
{
    VALIDATE_NOT_NULL(map)
    *map = mMap;
    REFCOUNT_ADD(*map)
    return NOERROR;
}

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos
