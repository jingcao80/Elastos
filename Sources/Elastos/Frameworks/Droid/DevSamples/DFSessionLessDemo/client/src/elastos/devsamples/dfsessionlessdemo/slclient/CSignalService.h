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

#ifndef __ELASTOS_DEVSAMPLES_DFSESSIONLESSDEMO_SLCLIENT_CSIMPLECLIENT_H__
#define __ELASTOS_DEVSAMPLES_DFSESSIONLESSDEMO_SLCLIENT_CSIMPLECLIENT_H__

#include "CClient.h"
#include "_Elastos_DevSamples_DFSessionLessDemo_SLClient_CSignalService.h"

namespace Elastos {
namespace DevSamples {
namespace DFSessionLessDemo {
namespace SLClient {

CarClass(CSignalService)
    , public CClient::SignalService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace SLClient
} // namespace DFSessionLessDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __ELASTOS_DEVSAMPLES_DFSESSIONLESSDEMO_SLCLIENT_CSIMPLECLIENT_H__
