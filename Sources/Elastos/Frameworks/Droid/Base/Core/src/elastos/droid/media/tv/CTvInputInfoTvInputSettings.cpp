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

#include "elastos/droid/media/tv/CTvInputInfoTvInputSettings.h"
#include "elastos/droid/media/tv/TvInputInfoTvInputSettings.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

CAR_SINGLETON_IMPL(CTvInputInfoTvInputSettings)
CAR_INTERFACE_IMPL(CTvInputInfoTvInputSettings, Singleton, ITvInputInfoTvInputSettings);

ECode CTvInputInfoTvInputSettings::GetHiddenTvInputIds(
    /* [in] */ IContext* context,
    /* [in] */ Int32 userId,
    /* [out] */ ISet** result)
{
    return TvInputInfoTvInputSettings::GetHiddenTvInputIds(context, userId, result);
}

ECode CTvInputInfoTvInputSettings::GetCustomLabels(
    /* [in] */ IContext* context,
    /* [in] */ Int32 userId,
    /* [out] */ IMap** result)
{
    return TvInputInfoTvInputSettings::GetCustomLabels(context, userId, result);
}

ECode CTvInputInfoTvInputSettings::PutHiddenTvInputs(
    /* [in] */ IContext* context,
    /* [in] */ ISet* hiddenInputIds,
    /* [in] */ Int32 userId)
{
    return TvInputInfoTvInputSettings::PutHiddenTvInputs(context, hiddenInputIds, userId);
}

ECode CTvInputInfoTvInputSettings::PutCustomLabels(
    /* [in] */ IContext* context,
    /* [in] */ IMap* customLabels,
    /* [in] */ Int32 userId)
{
    return TvInputInfoTvInputSettings::PutCustomLabels(context, customLabels, userId);
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
