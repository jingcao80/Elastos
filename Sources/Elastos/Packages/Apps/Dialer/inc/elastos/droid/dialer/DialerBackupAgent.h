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

#ifndef __ELASTOS_DROID_DIALER_DIALERBACKUPAGENT_H__
#define __ELASTOS_DROID_DIALER_DIALERBACKUPAGENT_H__

#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/app/backup/BackupAgentHelper.h"

using Elastos::Droid::App::Backup::BackupAgentHelper;

namespace Elastos {
namespace Droid {
namespace Dialer {

/**
 * The Dialer backup agent backs up the shared preferences settings of the
 * Dialer App. Right now it backs up the whole shared preference file. This
 * can be modified in the future to accommodate partical backup.
 */
class DialerBackupAgent
    : public BackupAgentHelper
    , public IDialerBackupAgent
{
public:
    CAR_INTERFACE_DECL();

    //@Override
    CARAPI OnCreate();

private:
    static const String SHARED_KEY; // = "shared_pref";
};

} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_DIALERBACKUPAGENT_H__
