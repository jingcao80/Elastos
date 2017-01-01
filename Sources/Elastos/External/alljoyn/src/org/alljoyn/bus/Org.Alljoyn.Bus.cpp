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

#include "org/alljoyn/bus/CAboutData.h"
#include "org/alljoyn/bus/CAboutIcon.h"
#include "org/alljoyn/bus/CAboutIconObj.h"
#include "org/alljoyn/bus/CAboutIconProxy.h"
#include "org/alljoyn/bus/CAboutProxy.h"
#include "org/alljoyn/bus/CBusListener.h"
#include "org/alljoyn/bus/CObserver.h"
#include "org/alljoyn/bus/CPropertyChangedEmitter.h"
#include "org/alljoyn/bus/CSignalEmitter.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

CAR_OBJECT_IMPL(CAboutData)
CAR_OBJECT_IMPL(CAboutIcon)
CAR_OBJECT_IMPL(CAboutIconObj)
CAR_OBJECT_IMPL(CAboutIconProxy)
CAR_OBJECT_IMPL(CAboutProxy)
CAR_OBJECT_IMPL(CBusListener)
CAR_OBJECT_IMPL(CObserver)
CAR_OBJECT_IMPL(CPropertyChangedEmitter)
CAR_OBJECT_IMPL(CSignalEmitter)


} // namespace Bus
} // namespace Alljoyn
} // namespace Org
