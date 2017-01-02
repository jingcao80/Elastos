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

#include "CInstrumentationWatcher.h"

using Elastos::Droid::App::EIID_IInstrumentationWatcher;
using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Am {

CAR_INTERFACE_IMPL_2(CInstrumentationWatcher, Object, IInstrumentationWatcher, IBinder);

CAR_OBJECT_IMPL(CInstrumentationWatcher)

ECode CInstrumentationWatcher::constructor()
{
    return NOERROR;
}

ECode CInstrumentationWatcher::InstrumentationStatus(
    /* [in] */ IComponentName* name,
    /* [in] */ Int32 resultCode,
    /* [in] */ IBundle* results)
{
    return Am::InstrumentationWatcher::InstrumentationStatus(
            name, resultCode, results);
}

ECode CInstrumentationWatcher::InstrumentationFinished(
    /* [in] */ IComponentName* name,
    /* [in] */ Int32 resultCode,
    /* [in] */ IBundle* results)
{
    return Am::InstrumentationWatcher::InstrumentationFinished(
            name, resultCode, results);
}

ECode CInstrumentationWatcher::ToString(
    /* [out] */ String* string)
{
    return Object::ToString(string);
}

ECode CInstrumentationWatcher::SetRawOutput(
        /* [in] */ Boolean rawMode)
{
    return Am::InstrumentationWatcher::SetRawOutput(rawMode);
}

ECode CInstrumentationWatcher::WaitForFinish(
    /* [out] */ Boolean* result)
{
    return Am::InstrumentationWatcher::WaitForFinish(result);
}

}
}
}
}

