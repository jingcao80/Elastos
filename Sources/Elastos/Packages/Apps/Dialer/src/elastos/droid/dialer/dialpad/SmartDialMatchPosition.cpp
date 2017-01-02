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

#include "elastos/droid/dialer/dialpad/SmartDialMatchPosition.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Dialpad {

const String SmartDialMatchPosition::TAG("SmartDialMatchPosition");

CAR_INTERFACE_IMPL(SmartDialMatchPosition, Object, ISmartDialMatchPosition);

ECode SmartDialMatchPosition::constructor(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    mStart = start;
    mEnd = end;
    return NOERROR;
}

void SmartDialMatchPosition::Advance(
    /* [in] */ Int32 toAdvance)
{
    mStart += toAdvance;
    mEnd += toAdvance;
}

void SmartDialMatchPosition::AdvanceMatchPositions(
    /* [in] */ IArrayList* inList,
    /* [in] */ Int32 toAdvance)
{
    Int32 size;
    inList->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> item;
        inList->Get(i, (IInterface**)&item);
        ((SmartDialMatchPosition*)IObject::Probe(item))->Advance(toAdvance);
    }
}

void SmartDialMatchPosition::Print(
    /* [in] */ IArrayList* list)
{
    Int32 size;
    list->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> item;
        list->Get(i, (IInterface**)&item);
        SmartDialMatchPosition* m = (SmartDialMatchPosition*)IObject::Probe(item);
        Logger::D(TAG, "[%d,%d]", m->mStart , m->mEnd);
    }
}

} // Dialpad
} // Dialer
} // Droid
} // Elastos
