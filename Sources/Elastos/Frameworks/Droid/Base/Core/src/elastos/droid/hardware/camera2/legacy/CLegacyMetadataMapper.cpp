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

#include "elastos/droid/hardware/camera2/legacy/CLegacyMetadataMapper.h"
#include "elastos/droid/hardware/camera2/legacy/LegacyMetadataMapper.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CAR_INTERFACE_IMPL(CLegacyMetadataMapper, Singleton, ILegacyMetadataMapperHelper)

CAR_SINGLETON_IMPL(CLegacyMetadataMapper)

ECode CLegacyMetadataMapper::CreateCharacteristics(
    /* [in] */ IParameters* parameters,
    /* [in] */ IHardwareCameraInfo* info,
    /* [out] */ ICameraCharacteristics** outcc)
{
    VALIDATE_NOT_NULL(outcc)

    return LegacyMetadataMapper::CreateCharacteristics(parameters, info, outcc);
}

ECode CLegacyMetadataMapper::CreateCharacteristics(
    /* [in] */ const String& parameters,
    /* [in] */ ICameraInfo* info,
    /* [out] */ ICameraCharacteristics** outcc)
{
    VALIDATE_NOT_NULL(outcc)

    return LegacyMetadataMapper::CreateCharacteristics(parameters, info, outcc);
}

ECode CLegacyMetadataMapper::ConvertRequestMetadata(
    /* [in] */ ILegacyRequest* request)
{
    return LegacyMetadataMapper::ConvertRequestMetadata(request);
}

ECode CLegacyMetadataMapper::CreateRequestTemplate(
        /* [in] */ ICameraCharacteristics* c,
        /* [in] */ Int32 templateId,
        /* [out] */ ICameraMetadataNative** outcm)
{
    VALIDATE_NOT_NULL(outcm)

    return LegacyMetadataMapper::CreateRequestTemplate(c, templateId, outcm);
}

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
