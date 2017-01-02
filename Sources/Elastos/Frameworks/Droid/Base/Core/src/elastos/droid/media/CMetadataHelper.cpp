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

#include "elastos/droid/media/CMetadataHelper.h"
#include "elastos/droid/media/CMetadata.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_SINGLETON_IMPL(CMetadataHelper)

CAR_INTERFACE_IMPL(CMetadataHelper, Singleton, IMetadataHelper)

ECode CMetadataHelper::LastSytemId(
    /* [out] */ Int32* result)
{
    return CMetadata::LastSytemId(result);
}

ECode CMetadataHelper::FirstCustomId(
    /* [out] */ Int32* result)
{
    return CMetadata::FirstCustomId(result);
}

ECode CMetadataHelper::LastType(
    /* [out] */ Int32* result)
{
    return CMetadata::LastType(result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
