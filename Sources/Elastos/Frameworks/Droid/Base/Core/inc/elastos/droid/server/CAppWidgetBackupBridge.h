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

#ifndef __ELASTOS_DROID_SERVER_CAPPWIDGETBACKUPBRIDGE_H__
#define __ELASTOS_DROID_SERVER_CAPPWIDGETBACKUPBRIDGE_H__

#include "_Elastos_Droid_Server_CAppWidgetBackupBridge.h"
#include "Elastos.Droid.Core.Server.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Server::IWidgetBackupProvider;
using Elastos::Core::Singleton;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CAppWidgetBackupBridge)
    , public Singleton
    , public IAppWidgetBackupBridge
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI Register(
        /* [in] */ IWidgetBackupProvider* instance);

    CARAPI GetWidgetParticipants(
        /* [in] */ Int32 userId,
        /* [out] */ IList** wp);

    CARAPI GetWidgetState(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 userId,
        /* [out, callee] */ ArrayOf<Byte>** ws);

    CARAPI RestoreStarting(
        /* [in] */ Int32 userId);

    CARAPI RestoreWidgetState(
        /* [in] */ const String& packageName,
        /* [in] */ ArrayOf<Byte>* restoredState,
        /* [in] */ Int32 userId);

    CARAPI RestoreFinished(
        /* [in] */ Int32 userId);

private:
    static AutoPtr<IWidgetBackupProvider> sAppWidgetService;
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CAPPWIDGETBACKUPBRIDGE_H__
