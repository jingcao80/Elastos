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

#include "elastos/droid/media/CRingtoneManagerHelper.h"
#include "elastos/droid/media/CRingtoneManager.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_SINGLETON_IMPL(CRingtoneManagerHelper)

CAR_INTERFACE_IMPL(CRingtoneManagerHelper, Singleton, IRingtoneManagerHelper)

ECode CRingtoneManagerHelper::GetValidRingtoneUri(
    /* [in] */ IContext* context,
    /* [out] */ IUri** result)
{
    return CRingtoneManager::GetValidRingtoneUri(context, result);
}

ECode CRingtoneManagerHelper::GetRingtone(
    /* [in] */ IContext* context,
    /* [in] */ IUri* ringtoneUri,
    /* [out] */ IRingtone** result)
{
    return CRingtoneManager::GetRingtone(context, ringtoneUri, result);
}

ECode CRingtoneManagerHelper::GetActualDefaultRingtoneUri(
    /* [in] */ IContext* context,
    /* [in] */ Int32 type,
    /* [out] */ IUri** result)
{
    return CRingtoneManager::GetActualDefaultRingtoneUri(context, type, result);
}

ECode CRingtoneManagerHelper::SetActualDefaultRingtoneUri(
    /* [in] */ IContext* context,
    /* [in] */ Int32 type,
    /* [in] */ IUri* ringtoneUri)
{
    return CRingtoneManager::SetActualDefaultRingtoneUri(context, type, ringtoneUri);
}

ECode CRingtoneManagerHelper::IsDefault(
    /* [in] */ IUri* ringtoneUri,
    /* [out] */ Boolean* result)
{
    return CRingtoneManager::IsDefault(ringtoneUri, result);
}

ECode CRingtoneManagerHelper::GetDefaultType(
    /* [in] */ IUri* defaultRingtoneUri,
    /* [out] */ Int32* result)
{
    return CRingtoneManager::GetDefaultType(defaultRingtoneUri, result);
}

ECode CRingtoneManagerHelper::GetDefaultUri(
    /* [in] */ Int32 type,
    /* [out] */ IUri** result)
{
    return CRingtoneManager::GetDefaultUri(type, result);
}

ECode CRingtoneManagerHelper::GetStaticDefaultRingtoneUri(
    /* [in] */ IContext* context,
    /* [out] */ IUri** result)
{
    return CRingtoneManager::GetStaticDefaultRingtoneUri(context, result);
}

ECode CRingtoneManagerHelper::GetDefaultRingtoneSubIdByUri(
    /* [in] */ IUri* defaultRingtoneUri,
    /* [out] */ Int32* result)
{
    return CRingtoneManager::GetDefaultRingtoneSubIdByUri(defaultRingtoneUri, result);
}

ECode CRingtoneManagerHelper::GetActualRingtoneUriBySubId(
    /* [in] */ IContext* context,
    /* [in] */ Int32 subId,
    /* [out] */ IUri** result)
{
    return CRingtoneManager::GetActualRingtoneUriBySubId(context, subId, result);
}

ECode CRingtoneManagerHelper::SetActualRingtoneUriBySubId(
    /* [in] */ IContext* context,
    /* [in] */ Int32 subId,
    /* [in] */ IUri* ringtoneUri)
{
    return CRingtoneManager::SetActualRingtoneUriBySubId(context, subId, ringtoneUri);
}

ECode CRingtoneManagerHelper::GetDefaultRingtoneUriBySubId(
    /* [in] */ Int32 subId,
    /* [out] */ IUri** result)
{
    return CRingtoneManager::GetDefaultRingtoneUriBySubId(subId, result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
