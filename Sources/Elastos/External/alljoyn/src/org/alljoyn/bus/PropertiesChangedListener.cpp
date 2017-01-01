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

#include "org/alljoyn/bus/PropertiesChangedListener.h"
#include "org/alljoyn/bus/NativePropertiesChangedListener.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

CAR_INTERFACE_IMPL(PropertiesChangedListener, Object, IPropertiesChangedListener)

PropertiesChangedListener::PropertiesChangedListener()
    : mHandle(0)
{}

PropertiesChangedListener::~PropertiesChangedListener()
{
    Destroy();
}

ECode PropertiesChangedListener::constructor()
{
    Create();
    return NOERROR;
}

void PropertiesChangedListener::Create()
{
    // changed       Property values that changed as an array of dictionary entries, signature "a{sv}".
    // invalidated   Properties whose values have been invalidated, signature "as".
    AutoPtr<IWeakReference> wr;
    GetWeakReference((IWeakReference**)&wr);
    NativePropertiesChangedListener* npcl = new NativePropertiesChangedListener(wr, String("a{sv}"), String("as"));
    mHandle = reinterpret_cast<Int64>(npcl);
}

/** Release native resources. */
void PropertiesChangedListener::Destroy()
{
    if (mHandle != 0) {
        NativePropertiesChangedListener* npcl = reinterpret_cast<NativePropertiesChangedListener*>(mHandle);
        delete npcl;
        mHandle = 0;
    }
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org