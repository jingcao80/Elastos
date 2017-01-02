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

#ifndef __ELASTOS_DROID_VIEW_TEXTSERVICE_CINTERNALLISTENER_H__
#define __ELASTOS_DROID_VIEW_TEXTSERVICE_CINTERNALLISTENER_H__

#include "_Elastos_Droid_View_TextService_CInternalListener.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Internal::TextService::IITextServicesSessionListener;
using Elastos::Droid::Internal::TextService::IISpellCheckerSession;

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace View {
namespace TextService {

CarClass(CInternalListener)
    , public Object
    , public IInternalListener
    , public IITextServicesSessionListener
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ ISpellCheckerSessionListenerImpl* spellCheckerSessionListenerImpl);

    CARAPI OnServiceConnected(
        /* [in] */ IISpellCheckerSession* session);

private:
    AutoPtr<ISpellCheckerSessionListenerImpl> mParentSpellCheckerSessionListenerImpl;

    static const String TAG;
    static const Boolean DBG;
};

}   //namespace TextService
}   //namespace View
}   //namespace Droid
}   //namespace Elastos

#endif //__ELASTOS_DROID_VIEW_TEXTSERVICE_CINTERNALLISTENER_H__
