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

#include "elastos/droid/view/textservice/CInternalListener.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::TextService::EIID_IITextServicesSessionListener;

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {
namespace TextService {

//========================================================================================
//              CInternalListener::
//========================================================================================
const String CInternalListener::TAG("SpellCheckerSession");
const Boolean CInternalListener::DBG = FALSE;

CAR_INTERFACE_IMPL_2(CInternalListener, Object, IInternalListener, IITextServicesSessionListener)

CAR_OBJECT_IMPL(CInternalListener)

ECode CInternalListener::constructor(
    /* [in] */ ISpellCheckerSessionListenerImpl* spellCheckerSessionListenerImpl)
{
    mParentSpellCheckerSessionListenerImpl = spellCheckerSessionListenerImpl;
    return NOERROR;
}

ECode CInternalListener::OnServiceConnected(
    /* [in] */ IISpellCheckerSession* session)
{
    if (DBG) {
        Logger::W(TAG, "SpellCheckerSession connected.");
    }
    mParentSpellCheckerSessionListenerImpl->OnServiceConnected(session);
    return NOERROR;
}

} // namespace TextService
} // namespace View
} // namespace Droid
} // namespace Elastos
