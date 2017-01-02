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

#include "elastos/droid/dialer/calllog/CCallLogListItemView.h"
#include "elastos/droid/dialer/calllog/CCallLogActivity.h"
#include "elastos/droid/dialer/calllog/CClearCallLogDialog.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

CAR_INTERFACE_IMPL(CCallLogListItemView, LinearLayout, ICallLogListItemView);

CAR_OBJECT_IMPL(CCallLogListItemView);
CAR_OBJECT_IMPL(CCallLogActivity);
CAR_OBJECT_IMPL(CClearCallLogDialog);



} // CallLog
} // Dialer
} // Droid
} // Elastos
