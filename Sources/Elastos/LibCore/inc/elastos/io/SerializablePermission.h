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

#ifndef __ELASTOS_IO_SERIALIZABLEPERMISSION_H__
#define __ELASTOS_IO_SERIALIZABLEPERMISSION_H__

#include "Elastos.CoreLibrary.IO.h"
#include "BasicPermission.h"

using Elastos::Security::BasicPermission;

namespace Elastos {
namespace IO {

/**
 * Legacy security code; do not use.
 */
class SerializablePermission
    : public BasicPermission
{
public:
    CARAPI constructor(
        /* [in] */ const String& permissionName);

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& actions);

    // @Override
    CARAPI GetActions(
        /* [out] */ String* result);

    // @Override
    CARAPI Implies(
        /* [in] */ IPermission* permission,
        /* [out] */ Boolean* result);
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_SERIALIZABLEPERMISSION_H__
