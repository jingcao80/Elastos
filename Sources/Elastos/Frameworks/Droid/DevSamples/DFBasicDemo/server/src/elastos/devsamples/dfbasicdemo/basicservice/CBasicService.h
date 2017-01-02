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

#ifndef __ELASTOS_DEVSAMPLES_DFBASICDEMO_BASICSERVICE_CBASICSERVICE_H__
#define __ELASTOS_DEVSAMPLES_DFBASICDEMO_BASICSERVICE_CBASICSERVICE_H__

#include "CService.h"
#include "_Elastos_DevSamples_DFBasicDemo_BasicService_CBasicService.h"

namespace Elastos {
namespace DevSamples {
namespace DFBasicDemo {
namespace BasicService {

CarClass(CBasicService)
    , public CService::BasicService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace BasicService
} // namespace DFBasicDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __ELASTOS_DEVSAMPLES_DFBASICDEMO_BASICSERVICE_CBASICSERVICE_H__
