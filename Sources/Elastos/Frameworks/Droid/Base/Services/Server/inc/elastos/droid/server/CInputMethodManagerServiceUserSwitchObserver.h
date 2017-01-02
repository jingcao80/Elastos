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

#ifndef __ELASTOS_DROID_SERVER_CINPUTMETHODMANAGERSERVICEUSERSWITCHOBSERVER_H__
#define __ELASTOS_DROID_SERVER_CINPUTMETHODMANAGERSERVICEUSERSWITCHOBSERVER_H__

#include "_Elastos_Droid_Server_CInputMethodManagerServiceUserSwitchObserver.h"
#include <elastos/core/Object.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Internal.h>

using Elastos::Droid::Os::IIRemoteCallback;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::App::IIUserSwitchObserver;
using Elastos::Droid::Internal::View::IIInputMethodManager;

namespace Elastos {
namespace Droid {
namespace Server {

class CInputMethodManagerService;

CarClass(CInputMethodManagerServiceUserSwitchObserver)
    , public Object
    , public IIUserSwitchObserver
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IIInputMethodManager* inputMethodManagerService);

    CARAPI OnUserSwitching(
        /* [in] */ Int32 newUserId,
        /* [in] */ IIRemoteCallback* reply);

    CARAPI OnUserSwitchComplete(
        /* [in] */ Int32 newUserId);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CInputMethodManagerService* mHost;
};

} // namespace Elastos;
} // namespace Droid;
} // namespace Server;
#endif
