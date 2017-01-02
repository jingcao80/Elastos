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

#include "elastos/droid/ext/frameworkdef.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/app/CApplicationErrorReportHelper.h"
#include "elastos/droid/app/CApplicationErrorReport.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CApplicationErrorReportHelper, Singleton, IApplicationErrorReportHelper)

CAR_SINGLETON_IMPL(CApplicationErrorReportHelper)

ECode CApplicationErrorReportHelper::constructor()
{
    return NOERROR;
}

ECode CApplicationErrorReportHelper::GetErrorReportReceiver(
    /* [in] */ IContext* context,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 appFlags,
    /* [out] */ IComponentName** receiver)
{
    VALIDATE_NOT_NULL(receiver);
    *receiver = NULL;
    VALIDATE_NOT_NULL(context);

    AutoPtr<IComponentName> cn = CApplicationErrorReport::GetErrorReportReceiver(
            context, packageName, appFlags);;
    *receiver = cn;
    REFCOUNT_ADD(*receiver)
    return NOERROR;
}

ECode CApplicationErrorReportHelper::GetErrorReportReceiver(
    /* [in] */ IPackageManager* pm,
    /* [in] */ const String& errorPackage,
    /* [in] */ const String& receiverPackage,
    /* [out] */ IComponentName** receiver)
{
    VALIDATE_NOT_NULL(receiver);
    *receiver = NULL;
    VALIDATE_NOT_NULL(pm);

    AutoPtr<IComponentName> cn = CApplicationErrorReport::GetErrorReportReceiver(
            pm, errorPackage, receiverPackage);;
    *receiver = cn;
    REFCOUNT_ADD(*receiver)
    return NOERROR;
}


} // namespace App
} // namespace Droid
} // namespace Elastos
