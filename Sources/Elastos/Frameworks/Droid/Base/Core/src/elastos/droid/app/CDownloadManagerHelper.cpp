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

#include "elastos/droid/app/CDownloadManagerHelper.h"
#include "elastos/droid/app/CDownloadManager.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CDownloadManagerHelper, Singleton, IDownloadManagerHelper)

CAR_SINGLETON_IMPL(CDownloadManagerHelper)

ECode CDownloadManagerHelper::GetUNDERLYINGCOLUMNS(
    /* [out] */ ArrayOf<String>** columns)
{
    VALIDATE_NOT_NULL(columns);

    *columns = CDownloadManager::UNDERLYING_COLUMNS;
    REFCOUNT_ADD(*columns);
    return NOERROR;
}

ECode CDownloadManagerHelper::GetMaxBytesOverMobile(
    /* [in] */ IContext* context,
    /* [out] */ Int64* size)
{
    return CDownloadManager::GetMaxBytesOverMobile(context, size);
}

ECode CDownloadManagerHelper::GetRecommendedMaxBytesOverMobile(
    /* [in] */ IContext* context,
    /* [out] */ Int64* size)
{
    return CDownloadManager::GetRecommendedMaxBytesOverMobile(context, size);
}

ECode CDownloadManagerHelper::IsActiveNetworkExpensive(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* actived)
{
    return CDownloadManager::IsActiveNetworkExpensive(context, actived);
}

ECode CDownloadManagerHelper::GetActiveNetworkWarningBytes(
    /* [in] */ IContext* context,
    /* [out] */ Int64* bytes)
{
    return CDownloadManager::GetActiveNetworkWarningBytes(context, bytes);
}

} // namespace App
} // namespace Droid
} // namespace Elastos
