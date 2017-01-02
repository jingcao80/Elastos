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

#include "CStructUtsname.h"

namespace Elastos {
namespace Droid {
namespace System {

CAR_OBJECT_IMPL(CStructUtsname)

CAR_INTERFACE_IMPL(CStructUtsname, Object, IStructUtsname)

ECode CStructUtsname::GetSysname(
    /* [out] */ String* sysname)
{
    VALIDATE_NOT_NULL(sysname);

    *sysname = mSysname;
    return NOERROR;
}

ECode CStructUtsname::GetNodename(
    /* [out] */ String* nodename)
{
    VALIDATE_NOT_NULL(nodename);

    *nodename = mNodename;
    return NOERROR;
}

ECode CStructUtsname::GetRelease(
    /* [out] */ String* release)
{
    VALIDATE_NOT_NULL(release);

    *release = mRelease;
    return NOERROR;
}

ECode CStructUtsname::GetVersion(
    /* [out] */ String* version)
{
    VALIDATE_NOT_NULL(version);

    *version = mVersion;
    return NOERROR;
}

ECode CStructUtsname::GetMachine(
    /* [out] */ String* machine)
{
    VALIDATE_NOT_NULL(machine);

    *machine = mMachine;
    return NOERROR;
}

ECode CStructUtsname::constructor(
    /* [in] */ const String& sysname,
    /* [in] */ const String& nodename,
    /* [in] */ const String& release,
    /* [in] */ const String& version,
    /* [in] */ const String& machine)
{
    mSysname = sysname;
    mNodename = nodename;
    mRelease = release;
    mVersion = version;
    mMachine = machine;
    return NOERROR;
}

} // namespace System
} // namespace Droid
} // namespace Elastos
