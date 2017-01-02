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

#ifndef __ELASTOS_DROID_SERVER_CSYSTEMBACKUPAGENT_H__
#define __ELASTOS_DROID_SERVER_CSYSTEMBACKUPAGENT_H__

#include "elastos/droid/app/backup/BackupAgentHelper.h"
#include "_Elastos_Droid_Server_CSystemBackupAgent.h"

using Elastos::Droid::App::Backup::BackupAgentHelper;

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * Backup agent for various system-managed data, currently just the system wallpaper
 */
class CSystemBackupAgent
    : public BackupAgentHelper
{
public:

    //@Override
    CARAPI OnBackup(
        /* [in] */ IParcelFileDescriptor* oldState,
        /* [in] */ IBackupDataOutput* data,
        /* [in] */ IParcelFileDescriptor* newState);

    //@Override
    CARAPI OnRestore(
        /* [in] */ IBackupDataInput* data,
        /* [in] */ Int32 appVersionCode,
        /* [in] */ IParcelFileDescriptor* newState);

    CARAPI OnFullBackup(
        /* [in] */ IFullBackupDataOutput* data);

    CARAPI OnRestoreFile(
        /* [in] */ IParcelFileDescriptor* data,
        /* [in] */ Int64 size,
        /* [in] */ Int32 type,
        /* [in] */ const String& domain,
        /* [in] */ const String& path,
        /* [in] */ Int64 mode,
        /* [in] */ Int64 mtime);

private:

    CARAPI_(void) FullWallpaperBackup(
        /* [in] */ IFullBackupDataOutput* output);

    static CARAPI_(String) GetWALLPAPER_IMAGE_DIR();
    static CARAPI_(String) GetWALLPAPER_INFO_DIR();
    static CARAPI_(String) GetWALLPAPER_IMAGE();
    static CARAPI_(String) GetWALLPAPER_INFO();

private:

    static const String TAG;
    // These paths must match what the WallpaperManagerService uses.  The leaf *_FILENAME
    // are also used in the full-backup file format, so must not change unless steps are
    // taken to support the legacy backed-up datasets.
    static const String WALLPAPER_IMAGE_FILENAME;
    static const String WALLPAPER_INFO_FILENAME;
    static String WALLPAPER_IMAGE_DIR;
    static String WALLPAPER_IMAGE;
    static String WALLPAPER_INFO_DIR;
    static String WALLPAPER_INFO;
    static const String WALLPAPER_IMAGE_KEY;
    static const String WALLPAPER_INFO_KEY;
};

}// namespace Server
}// namespace Droid
}// namespace Elastos
#endif
