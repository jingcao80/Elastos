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

#include "elastos/droid/server/lights/LightsManager.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Lights {

CAR_INTERFACE_IMPL(LightsManager, Object, ILightsManager);

const Int32 LightsManager::LIGHT_ID_BACKLIGHT = 0;
const Int32 LightsManager::LIGHT_ID_KEYBOARD = 1;
const Int32 LightsManager::LIGHT_ID_BUTTONS = 2;
const Int32 LightsManager::LIGHT_ID_BATTERY = 3;
const Int32 LightsManager::LIGHT_ID_NOTIFICATIONS = 4;
const Int32 LightsManager::LIGHT_ID_ATTENTION = 5;
const Int32 LightsManager::LIGHT_ID_BLUETOOTH = 6;
const Int32 LightsManager::LIGHT_ID_WIFI = 7;
const Int32 LightsManager::LIGHT_ID_CAPS = 8;
const Int32 LightsManager::LIGHT_ID_FUNC = 9;
const Int32 LightsManager::LIGHT_ID_COUNT = 10;

LightsManager::~LightsManager()
{
}

} // Lights
} // Server
} // Droid
} // Elastos
