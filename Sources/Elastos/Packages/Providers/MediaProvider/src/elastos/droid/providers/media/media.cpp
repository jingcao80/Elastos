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

#include "elastos/droid/providers/media/CMediaDocumentsProvider.h"
#include "elastos/droid/providers/media/CMediaProvider.h"
#include "elastos/droid/providers/media/CMediaScannerReceiver.h"
#include "elastos/droid/providers/media/CMediaScannerService.h"
#include "elastos/droid/providers/media/CMediaUpgradeReceiver.h"
#include "elastos/droid/providers/media/CMtpReceiver.h"
#include "elastos/droid/providers/media/CMtpService.h"
#include "elastos/droid/providers/media/CRingtonePickerActivity.h"
#include "elastos/droid/providers/media/CMediaThumbRequest.h"
#include "elastos/droid/providers/media/CIMtpService.h"
#include "elastos/droid/providers/media/CIMediaScannerService.h"

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Media {

CAR_OBJECT_IMPL(CMediaDocumentsProvider)
CAR_OBJECT_IMPL(CMediaProvider)
CAR_OBJECT_IMPL(CMediaScannerReceiver)
CAR_OBJECT_IMPL(CMediaScannerService)
CAR_OBJECT_IMPL(CMediaUpgradeReceiver)
CAR_OBJECT_IMPL(CMtpReceiver)
CAR_OBJECT_IMPL(CMtpService)
CAR_OBJECT_IMPL(CRingtonePickerActivity)
CAR_OBJECT_IMPL(CMediaThumbRequest)
CAR_OBJECT_IMPL(CIMtpService)
CAR_OBJECT_IMPL(CIMediaScannerService)

} // namespace Media
} // namespace Providers
} // namespace Droid
} // namespace Elastos