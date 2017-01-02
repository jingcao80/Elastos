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

#include "elastos/droid/services/securitybridge/api/PackageManagerMonitor.h"

namespace Elastos {
namespace Droid {
namespace Services {
namespace SecurityBridge {
namespace Api {

PackageManagerMonitor::PackageManagerMonitor()
{}

PackageManagerMonitor::~PackageManagerMonitor()
{}

ECode PackageManagerMonitor::constructor()
{
    return NOERROR;
}

CAR_INTERFACE_IMPL(PackageManagerMonitor, Object, IPackageManagerMonitor)

ECode PackageManagerMonitor::ApproveAppInstallRequest(
    /* [in] */ const String& apkFilePath,
    /* [in] */ const String& originalAPKFilePath,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

} // namespace Api
} // namespace SecurityBridge
} // namespace Services
} // namespace Droid
} // namespace Elastos