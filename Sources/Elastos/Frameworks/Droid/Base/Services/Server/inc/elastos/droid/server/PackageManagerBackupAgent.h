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

#ifndef __ELASTOS_DROID_SERVER_PACKAGEMANAGERBACKUPAGENT_H__
#define __ELASTOS_DROID_SERVER_PACKAGEMANAGERBACKUPAGENT_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/backup/BackupAgent.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::HashSet;
using Elastos::IO::IDataInputStream;
using Elastos::IO::IDataOutputStream;
using Elastos::Droid::App::Backup::BackupAgent;
using Elastos::Droid::App::Backup::IBackupDataOutput;
using Elastos::Droid::App::Backup::IBackupDataInput;
using Elastos::Droid::App::Backup::IFullBackupDataOutput;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::ISignature;
using Elastos::Droid::Os::IParcelFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * We back up the signatures of each package so that during a system restore,
 * we can verify that the app whose data we think we have matches the app
 * actually resident on the device.
 *
 * Since the Package Manager isn't a proper "application" we just provide a
 * direct IBackupAgent implementation and hand-construct it at need.
 */
class PackageManagerBackupAgent: public BackupAgent
{
public:
    class Metadata : public ElLightRefBase
    {
    public:
        Metadata(
            /* [in] */ Int32 version,
            /* [in] */ AutoPtr<ArrayOf<ISignature*> > signatures)
            : mVersionCode(version)
            , mSignatures(signatures)
        {}

    public:
        Int32 mVersionCode;
        AutoPtr<ArrayOf<ISignature*> > mSignatures;
    };

public:

    PackageManagerBackupAgent(
        /* [in] */ IPackageManager* packageMgr,
        /* [in] */ List<AutoPtr<IPackageInfo> >* packages);

    virtual ~PackageManagerBackupAgent();

    CARAPI_(Boolean) HasMetadata();

    CARAPI_(AutoPtr<Metadata>) GetRestoredMetadata(
        /* [in] */ const String& packageName);

    CARAPI_(AutoPtr< HashSet<String> >) GetRestoredPackages();

    CARAPI OnBackup(
        /* [in] */ IParcelFileDescriptor* oldState,
        /* [in] */ IBackupDataOutput* data,
        /* [in] */ IParcelFileDescriptor* newState);

    CARAPI OnRestore(
        /* [in] */ IBackupDataInput* data,
        /* [in] */ Int32 appVersionCode,
        /* [in] */ IParcelFileDescriptor* newState);

private:
    static CARAPI WriteEntity(
        /* [in] */ IBackupDataOutput* data,
        /* [in] */ const String& key,
        /* [in] */ ArrayOf<Byte>* bytes);

    static CARAPI WriteSignatureArray(
        /* [in] */ IDataOutputStream* out,
        /* [in] */ ArrayOf<ISignature*>* sigs);

    static CARAPI ReadSignatureArray(
        /* [in] */ IDataInputStream* in,
        /* [out] */ ArrayOf<ISignature*>** sigs);

    // Util: parse out an existing state file into a usable structure
    CARAPI_(void) ParseStateFile(
        /* [in] */ IParcelFileDescriptor* stateFile);

    CARAPI_(void) WriteStateFile(
        /* [in] */ List<AutoPtr<IPackageInfo> >* pkgs,
        /* [in] */ IParcelFileDescriptor* stateFile);

private:
    static const String TAG;
    static const Boolean DEBUG;

    static const String GLOBAL_METADATA_KEY;

    typedef List<AutoPtr<IPackageInfo> > PackageInfoList;
    typedef List<AutoPtr<IPackageInfo> >::Iterator PackageInfoListIter;
    AutoPtr<PackageInfoList> mAllPackages;

    AutoPtr<IPackageManager> mPackageManager;

    // version & signature info of each app in a restore set
    typedef HashMap<String, AutoPtr<Metadata> > MetadataHashMap;
    typedef HashMap<String, AutoPtr<Metadata> >::Iterator MetadataHashMapIter;
    AutoPtr<MetadataHashMap> mRestoredSignatures;
    // The version info of each backed-up app as read from the state file
    MetadataHashMap mStateVersions;

    HashSet<String> mExisting;
    Int32 mStoredSdkVersion;
    String mStoredIncrementalVersion;
    Boolean mHasMetadata;
};

} //namespace Droid
} //namespace Server
} //namespace Elastos

#endif
