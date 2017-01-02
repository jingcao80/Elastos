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

#include "Elastos.Droid.Media.h"
#include "elastos/droid/media/tv/TvContractPrograms.h"
#include "elastos/droid/media/tv/TvContract.h"
#include "elastos/droid/net/CUriHelper.h"

using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

TvContractPrograms::TvContractPrograms()
{
}

ECode TvContractPrograms::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    AutoPtr<IUriHelper> uriHelper;
    CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
    return uriHelper->Parse(String("content://") +
            ITvContract::AUTHORITY + "/" + TvContract::PATH_PROGRAM, uri);
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
