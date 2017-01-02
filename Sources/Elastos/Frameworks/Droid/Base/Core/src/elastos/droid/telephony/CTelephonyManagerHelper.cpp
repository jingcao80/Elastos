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

#include "elastos/droid/telephony/CTelephonyManager.h"
#include "elastos/droid/telephony/CTelephonyManagerHelper.h"
#include "elastos/droid/ext/frameworkdef.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

CAR_INTERFACE_IMPL(CTelephonyManagerHelper, Singleton, ITelephonyManagerHelper)

CAR_SINGLETON_IMPL(CTelephonyManagerHelper)

ECode CTelephonyManagerHelper::GetDefault(
    /* [out] */ ITelephonyManager** res)
{
    return CTelephonyManager::GetDefault(res);
}

ECode CTelephonyManagerHelper::From(
    /* [in] */ IContext* ctx,
    /* [out] */ ITelephonyManager** res)
{
    return CTelephonyManager::From(ctx, res);
}

ECode CTelephonyManagerHelper::GetPhoneType(
    /* [in] */ Int32 networkMode,
    /* [out] */ Int32* res)
{
    return CTelephonyManager::GetPhoneType(networkMode, res);
}

ECode CTelephonyManagerHelper::GetLteOnCdmaModeStatic(
    /* [out] */ Int32* res)
{
    return CTelephonyManager::GetLteOnCdmaModeStatic(res);
}

ECode CTelephonyManagerHelper::GetNetworkClass(
    /* [in] */ Int32 networkType,
    /* [out] */ Int32* res)
{
    return CTelephonyManager::GetNetworkClass(networkType, res);
}

ECode CTelephonyManagerHelper::GetNetworkTypeName(
    /* [in] */ Int32 type,
    /* [out] */ String* res)
{
    return CTelephonyManager::GetNetworkTypeName(type, res);
}

ECode CTelephonyManagerHelper::SetTelephonyProperty(
    /* [in] */ const String& property,
    /* [in] */ Int64 subId,
    /* [in] */ const String& value)
{
    return CTelephonyManager::SetTelephonyProperty(property, subId, value);
}

ECode CTelephonyManagerHelper::GetInt32AtIndex(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& name,
    /* [in] */ Int32 index,
    /* [out] */ Int32* val)
{
    return CTelephonyManager::GetInt32AtIndex(resolver, name, index, val);
}

ECode CTelephonyManagerHelper::PutInt32AtIndex(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& name,
    /* [in] */ Int32 index,
    /* [in] */ Int32 val,
    /* [out] */ Boolean* res)
{
    return CTelephonyManager::PutInt32AtIndex(resolver, name, index, val, res);
}

ECode CTelephonyManagerHelper::GetTelephonyProperty(
    /* [in] */ const String& property,
    /* [in] */ Int64 subId,
    /* [in] */ const String& defaultVal,
    /* [out] */ String* res)
{
    return CTelephonyManager::GetTelephonyProperty(property, subId, defaultVal, res);
}

}
}
}

