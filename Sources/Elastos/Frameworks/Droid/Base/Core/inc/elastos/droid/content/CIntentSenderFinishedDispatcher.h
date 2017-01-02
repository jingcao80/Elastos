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

#ifndef __ELASTOS_DROID_CONTENT_CINTENTSENDER_FINISHEDDISPATCHER_H__
#define __ELASTOS_DROID_CONTENT_CINTENTSENDER_FINISHEDDISPATCHER_H__

#include "_Elastos_Droid_Content_CIntentSenderFinishedDispatcher.h"
#include "elastos/droid/content/CIntentSender.h"

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CIntentSenderFinishedDispatcher)
    , public CIntentSender::FinishedDispatcher
{
public:
    CAR_OBJECT_DECL()
};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CINTENTSENDER_FINISHEDDISPATCHER_H__
