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

#include "elastos/droid/internal/utility/cm/CQSConstants.h"

using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

static AutoPtr<IArrayList> InitTILES_DEFAULT()
{
    AutoPtr<IArrayList> list;
    CArrayList::New((IArrayList**)&list);

    AutoPtr<ICharSequence> cs;
    CString::New(IQSConstants::TILE_WIFI, (ICharSequence**)&cs);
    list->Add(cs);
    cs = NULL;
    CString::New(IQSConstants::TILE_BLUETOOTH, (ICharSequence**)&cs);
    list->Add(cs);
    cs = NULL;
    CString::New(IQSConstants::TILE_CELLULAR, (ICharSequence**)&cs);
    list->Add(cs);
    cs = NULL;
    CString::New(IQSConstants::TILE_AIRPLANE, (ICharSequence**)&cs);
    list->Add(cs);
    cs = NULL;
    CString::New(IQSConstants::TILE_ROTATION, (ICharSequence**)&cs);
    list->Add(cs);
    cs = NULL;
    CString::New(IQSConstants::TILE_FLASHLIGHT, (ICharSequence**)&cs);
    list->Add(cs);
    cs = NULL;
    CString::New(IQSConstants::TILE_LOCATION, (ICharSequence**)&cs);
    list->Add(cs);
    cs = NULL;
    CString::New(IQSConstants::TILE_CAST, (ICharSequence**)&cs);
    list->Add(cs);

    return list;
}

static AutoPtr<IArrayList> InitTILES_AVAILABLE()
{
    AutoPtr<IArrayList> list;
    CArrayList::New((IArrayList**)&list);

    list->AddAll(ICollection::Probe(CQSConstants::TILES_DEFAULT));

    AutoPtr<ICharSequence> cs;
    CString::New(IQSConstants::TILE_INVERSION, (ICharSequence**)&cs);
    list->Add(cs);
    cs = NULL;
    CString::New(IQSConstants::TILE_HOTSPOT, (ICharSequence**)&cs);
    list->Add(cs);
    cs = NULL;
    CString::New(IQSConstants::TILE_NOTIFICATIONS, (ICharSequence**)&cs);
    list->Add(cs);
    cs = NULL;
    CString::New(IQSConstants::TILE_DATA, (ICharSequence**)&cs);
    list->Add(cs);
    cs = NULL;
    CString::New(IQSConstants::TILE_ROAMING, (ICharSequence**)&cs);
    list->Add(cs);
    cs = NULL;
    CString::New(IQSConstants::TILE_DDS, (ICharSequence**)&cs);
    list->Add(cs);
    cs = NULL;
    CString::New(IQSConstants::TILE_APN, (ICharSequence**)&cs);
    list->Add(cs);
    cs = NULL;
    CString::New(IQSConstants::TILE_PROFILES, (ICharSequence**)&cs);
    list->Add(cs);
    cs = NULL;
    CString::New(IQSConstants::TILE_PERFORMANCE, (ICharSequence**)&cs);
    list->Add(cs);
    cs = NULL;
    CString::New(IQSConstants::TILE_ADB_NETWORK, (ICharSequence**)&cs);
    list->Add(cs);
    cs = NULL;
    CString::New(IQSConstants::TILE_NFC, (ICharSequence**)&cs);
    list->Add(cs);
    cs = NULL;
    CString::New(IQSConstants::TILE_COMPASS, (ICharSequence**)&cs);
    list->Add(cs);
    cs = NULL;
    CString::New(IQSConstants::TILE_LOCKSCREEN, (ICharSequence**)&cs);
    list->Add(cs);
    cs = NULL;
    CString::New(IQSConstants::TILE_LTE, (ICharSequence**)&cs);
    list->Add(cs);
    cs = NULL;
    CString::New(IQSConstants::TILE_VISUALIZER, (ICharSequence**)&cs);
    list->Add(cs);
    cs = NULL;
    CString::New(IQSConstants::TILE_SCREEN_TIMEOUT, (ICharSequence**)&cs);
    list->Add(cs);
    cs = NULL;
    CString::New(IQSConstants::TILE_LIVE_DISPLAY, (ICharSequence**)&cs);
    list->Add(cs);
    cs = NULL;
    CString::New(IQSConstants::TILE_USB_TETHER, (ICharSequence**)&cs);
    list->Add(cs);

    return list;
}

AutoPtr<IArrayList> CQSConstants::TILES_DEFAULT = InitTILES_DEFAULT();
AutoPtr<IArrayList> CQSConstants::TILES_AVAILABLE = InitTILES_AVAILABLE();

CAR_INTERFACE_IMPL(CQSConstants, Singleton, IQSConstants)

CAR_SINGLETON_IMPL(CQSConstants)

} //namespace Cm
} //namespace Utility
} //namespace Internal
} //namespace Droid
} //namespace Elastos
