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

#include "CUUIDHelper.h"
#include "CUUID.h"

namespace Elastos {
namespace Utility {

CAR_INTERFACE_IMPL(CUUIDHelper, Singleton, IUUIDHelper)

CAR_SINGLETON_IMPL(CUUIDHelper)

ECode CUUIDHelper::RandomUUID(
    /* [out] */ IUUID** uuid)
{
    return CUUID::RandomUUID(uuid);
}

ECode CUUIDHelper::NameUUIDFromBytes(
    /* [in] */ ArrayOf<Byte>* name,
    /* [out] */ IUUID** uuid)
{
    return CUUID::NameUUIDFromBytes(name, uuid);
}

ECode CUUIDHelper::FromString(
    /* [in] */ const String& name,
    /* [out] */ IUUID** uuid)
{
    return CUUID::FromString(name, uuid);
}

} // namespace Utility
} // namespace Elastos
