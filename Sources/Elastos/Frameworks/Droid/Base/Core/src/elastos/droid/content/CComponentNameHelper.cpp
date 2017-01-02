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

#include "elastos/droid/content/CComponentNameHelper.h"
#include "elastos/droid/content/CComponentName.h"

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL(CComponentNameHelper, Singleton, IComponentNameHelper)

CAR_SINGLETON_IMPL(CComponentNameHelper)

ECode CComponentNameHelper::UnflattenFromString(
    /* [in] */ const String& str,
    /* [out] */ IComponentName** componentName)
{
    return CComponentName::UnflattenFromString(str, componentName);
}

ECode CComponentNameHelper::WriteToParcel(
    /* [in] */ IComponentName* cn,
    /* [in] */ IParcel* dest)
{
    return CComponentName::WriteToParcel(cn, dest);
}

ECode CComponentNameHelper::ReadFromParcel(
    /* [in] */ IParcel* source,
    /* [out] */ IComponentName** cn)
{
    return CComponentName::ReadFromParcel(source, cn);
}

ECode CComponentNameHelper::AppendShortClassName(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ const String& packageName,
    /* [in] */ const String& className)
{
    return CComponentName::AppendShortClassName(sb, packageName, className);
}

ECode CComponentNameHelper::PrintShortClassName(
    /* [in] */ IPrintWriter* sb,
    /* [in] */ const String& packageName,
    /* [in] */ const String& className)
{
    return CComponentName::PrintShortClassName(sb, packageName, className);
}

ECode CComponentNameHelper::AppendShortString(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ const String& packageName,
    /* [in] */ const String& className)
{
    return CComponentName::AppendShortString(sb, packageName, className);
}

ECode CComponentNameHelper::PrintShortString(
    /* [in] */ IPrintWriter* sb,
    /* [in] */ const String& packageName,
    /* [in] */ const String& className)
{
    return CComponentName::PrintShortString(sb, packageName, className);
}

} // namespace Content
} // namespace Droid
} // namespace Elastos
