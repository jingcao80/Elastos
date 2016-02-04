/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.server.backup;

using Elastos::Droid::App::Backup::IBackupAgent;
using Elastos::Droid::App::Backup::IBackupDataInput;
using Elastos::Droid::App::Backup::IBackupDataOutput;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IPackageManager.NameNotFoundException;
using Elastos::Droid::Content::Pm::ISignature;
using Elastos::Droid::Os::IBuild;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Utility::ISlog;

using Elastos::IO::IBufferedInputStream;
using Elastos::IO::IBufferedOutputStream;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::IDataInputStream;
using Elastos::IO::IDataOutputStream;
using Elastos::IO::IEOFException;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IIOException;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;

using Elastos::Utility::IObjects;

/**
 * We back up the signatures of each package so that during a system restore,
 * we can verify that the app whose data we think we have matches the app
 * actually resident on the device.
 *
 * Since the Package Manager isn't a proper "application" we just provide a
 * direct IBackupAgent implementation and hand-construct it at need.
 */
public class PackageManagerBackupAgent extends BackupAgent {
    private static const String TAG = "PMBA";
    private static const Boolean DEBUG = FALSE;

    // key under which we store global metadata (individual app metadata
    // is stored using the package name as a key)
    private static const String GLOBAL_METADATA_KEY = "@meta@";

    // key under which we store the identity of the user's chosen default home app
    private static const String DEFAULT_HOME_KEY = "@home@";

    // Sentinel: start of state file, followed by a version number
    private static const String STATE_FILE_HEADER = "=state=";
    private static const Int32 STATE_FILE_VERSION = 2;

    // Current version of the saved ancestral-dataset file format
    private static const Int32 ANCESTRAL_RECORD_VERSION = 1;

    private List<PackageInfo> mAllPackages;
    private PackageManager mPackageManager;
    // version & signature info of each app in a restore set
    private HashMap<String, Metadata> mRestoredSignatures;
    // The version info of each backed-up app as read from the state file
    private HashMap<String, Metadata> mStateVersions = new HashMap<String, Metadata>();

    private final HashSet<String> mExisting = new HashSet<String>();
    private Int32 mStoredSdkVersion;
    private String mStoredIncrementalVersion;
    private ComponentName mStoredHomeComponent;
    private Int64 mStoredHomeVersion;
    private ArrayList<Byte[]> mStoredHomeSigHashes;

    private Boolean mHasMetadata;
    private ComponentName mRestoredHome;
    private Int64 mRestoredHomeVersion;
    private String mRestoredHomeInstaller;
    private ArrayList<Byte[]> mRestoredHomeSigHashes;

    // For compactness we store the SHA-256 hash of each app's Signatures
    // rather than the Signature blocks themselves.
    public class Metadata {
        public Int32 versionCode;
        public ArrayList<Byte[]> sigHashes;

        Metadata(Int32 version, ArrayList<Byte[]> hashes) {
            versionCode = version;
            sigHashes = hashes;
        }
    }

    // We're constructed with the set of applications that are participating
    // in backup.  This set changes as apps are installed & removed.
    PackageManagerBackupAgent(PackageManager packageMgr, List<PackageInfo> packages) {
        Init(packageMgr, packages);
    }

    PackageManagerBackupAgent(PackageManager packageMgr) {
        Init(packageMgr, NULL);

        EvaluateStorablePackages();
    }

    private void Init(PackageManager packageMgr, List<PackageInfo> packages) {
        mPackageManager = packageMgr;
        mAllPackages = packages;
        mRestoredSignatures = NULL;
        mHasMetadata = FALSE;

        mStoredSdkVersion = Build.VERSION.SDK_INT;
        mStoredIncrementalVersion = Build.VERSION.INCREMENTAL;
    }

    // We will need to refresh our understanding of what is eligible for
    // backup periodically; this entry point serves that purpose.
    CARAPI EvaluateStorablePackages() {
        mAllPackages = GetStorableApplications(mPackageManager);
    }

    public static List<PackageInfo> GetStorableApplications(PackageManager pm) {
        List<PackageInfo> pkgs;
        pkgs = pm->GetInstalledPackages(PackageManager.GET_SIGNATURES);
        Int32 N = pkgs->Size();
        for (Int32 a = N-1; a >= 0; a--) {
            PackageInfo pkg = pkgs->Get(a);
            if (!BackupManagerService->AppIsEligibleForBackup(pkg.applicationInfo)) {
                pkgs->Remove(a);
            }
        }
        return pkgs;
    }

    public Boolean HasMetadata() {
        return mHasMetadata;
    }

    public Metadata GetRestoredMetadata(String packageName) {
        if (mRestoredSignatures == NULL) {
            Slogger::W(TAG, "GetRestoredMetadata() before metadata read!");
            return NULL;
        }

        return mRestoredSignatures->Get(packageName);
    }

    public Set<String> GetRestoredPackages() {
        if (mRestoredSignatures == NULL) {
            Slogger::W(TAG, "GetRestoredPackages() before metadata read!");
            return NULL;
        }

        // This is technically the set of packages on the originating handset
        // that had backup agents at all, not limited to the set of packages
        // that had actually contributed a restore dataset, but it's a
        // close enough approximation for our purposes and does not require any
        // additional involvement by the transport to obtain.
        return mRestoredSignatures->KeySet();
    }
    
    // The backed up data is the signature block for each app, keyed by
    // the package name.
    CARAPI OnBackup(ParcelFileDescriptor oldState, BackupDataOutput data,
            ParcelFileDescriptor newState) {
        if (DEBUG) Slogger::V(TAG, "OnBackup()");

        ByteArrayOutputStream outputBuffer = new ByteArrayOutputStream();  // we'll reuse these
        DataOutputStream outputBufferStream = new DataOutputStream(outputBuffer);
        ParseStateFile(oldState);

        // If the stored version string differs, we need to re-backup all
        // of the metadata.  We force this by removing everything from the
        // "already backed up" map built by ParseStateFile().
        if (mStoredIncrementalVersion == NULL
                || !mStoredIncrementalVersion->Equals(Build.VERSION.INCREMENTAL)) {
            Slogger::I(TAG, "Previous metadata " + mStoredIncrementalVersion + " mismatch vs "
                    + Build.VERSION.INCREMENTAL + " - rewriting");
            mExisting->Clear();
        }

        Int64 homeVersion = 0;
        ArrayList<Byte[]> homeSigHashes = NULL;
        PackageInfo homeInfo = NULL;
        String homeInstaller = NULL;
        ComponentName home = GetPreferredHomeComponent();
        if (home != NULL) {
            try {
                homeInfo = mPackageManager->GetPackageInfo(home->GetPackageName(),
                        PackageManager.GET_SIGNATURES);
                homeInstaller = mPackageManager->GetInstallerPackageName(home->GetPackageName());
                homeVersion = homeInfo.versionCode;
                homeSigHashes = HashSignatureArray(homeInfo.signatures);
            } catch (NameNotFoundException e) {
                Slogger::W(TAG, "Can't access preferred home info");
                // proceed as though there were no preferred home set
                home = NULL;
            }
        }

        try {
            // We need to push a new preferred-home-app record if:
            //    1. the version of the home app has changed since our last backup;
            //    2. the home app [or absence] we now use differs from the prior state,
            // OR 3. it looks like we use the same home app + version as before, but
            //       the signatures don't match so we treat them as different apps.
            final Boolean needHomeBackup = (homeVersion != mStoredHomeVersion)
                    || Objects->Equals(home, mStoredHomeComponent)
                    || (home != NULL
                        && !BackupManagerService->SignaturesMatch(mStoredHomeSigHashes, homeInfo));
            if (needHomeBackup) {
                if (DEBUG) {
                    Slogger::I(TAG, "Home preference changed; backing up new state " + home);
                }
                if (home != NULL) {
                    outputBufferStream->WriteUTF(home->FlattenToString());
                    outputBufferStream->WriteLong(homeVersion);
                    outputBufferStream->WriteUTF(homeInstaller != NULL ? homeInstaller : "" );
                    WriteSignatureHashArray(outputBufferStream, homeSigHashes);
                    WriteEntity(data, DEFAULT_HOME_KEY, outputBuffer->ToByteArray());
                } else {
                    data->WriteEntityHeader(DEFAULT_HOME_KEY, -1);
                }
            }

            /*
             * Global metadata:
             *
             * Int32 SDKversion -- the SDK version of the OS itself on the device
             *                   that produced this backup set.  Used to reject
             *                   backups from later OSes onto earlier ones.
             * String incremental -- the incremental release name of the OS stored in
             *                       the backup set.
             */
            outputBuffer->Reset();
            if (!mExisting->Contains(GLOBAL_METADATA_KEY)) {
                if (DEBUG) Slogger::V(TAG, "Storing global metadata key");
                outputBufferStream->WriteInt(Build.VERSION.SDK_INT);
                outputBufferStream->WriteUTF(Build.VERSION.INCREMENTAL);
                WriteEntity(data, GLOBAL_METADATA_KEY, outputBuffer->ToByteArray());
            } else {
                if (DEBUG) Slogger::V(TAG, "Global metadata key already stored");
                // don't consider it to have been skipped/deleted
                mExisting->Remove(GLOBAL_METADATA_KEY);
            }

            // For each app we have on device, see if we've backed it up yet.  If not,
            // write its signature block to the output, keyed on the package name.
            for (PackageInfo pkg : mAllPackages) {
                String packName = pkg.packageName;
                if (packName->Equals(GLOBAL_METADATA_KEY)) {
                    // We've already handled the metadata key; skip it here
                    continue;
                } else {
                    PackageInfo info = NULL;
                    try {
                        info = mPackageManager->GetPackageInfo(packName,
                                PackageManager.GET_SIGNATURES);
                    } catch (NameNotFoundException e) {
                        // Weird; we just found it, and now are told it doesn't exist.
                        // Treat it as having been removed from the device.
                        mExisting->Add(packName);
                        continue;
                    }

                    if (mExisting->Contains(packName)) {
                        // We have backed up this app before.  Check whether the version
                        // of the backup matches the version of the current app; if they
                        // don't match, the app has been updated and we need to store its
                        // metadata again.  In either case, take it out of mExisting so that
                        // we don't consider it deleted later.
                        mExisting->Remove(packName);
                        if (info.versionCode == mStateVersions->Get(packName).versionCode) {
                            continue;
                        }
                    }
                    
                    if (info.signatures == NULL || info.signatures.length == 0)
                    {
                        Slogger::W(TAG, "Not backing up package " + packName
                                + " since it appears to have no signatures.");
                        continue;
                    }

                    // We need to store this app's metadata
                    /*
                     * Metadata for each package:
                     *
                     * Int32 version       -- [4] the package's versionCode
                     * Byte[] signatures -- [len] flattened signature hash array of the package
                     */

                    // marshal the version code in a canonical form
                    outputBuffer->Reset();
                    outputBufferStream->WriteInt(info.versionCode);
                    WriteSignatureHashArray(outputBufferStream,
                            HashSignatureArray(info.signatures));

                    if (DEBUG) {
                        Slogger::V(TAG, "+ writing metadata for " + packName
                                + " version=" + info.versionCode
                                + " entityLen=" + outputBuffer->Size());
                    }
                    
                    // Now we can write the backup entity for this package
                    WriteEntity(data, packName, outputBuffer->ToByteArray());
                }
            }

            // At this point, the only entries in 'existing' are apps that were
            // mentioned in the saved state file, but appear to no longer be present
            // on the device.  Write a deletion entity for them.
            for (String app : mExisting) {
                if (DEBUG) Slogger::V(TAG, "- removing metadata for deleted pkg " + app);
                try {
                    data->WriteEntityHeader(app, -1);
                } catch (IOException e) {
                    Slogger::E(TAG, "Unable to write package deletions!");
                    return;
                }
            }
        } catch (IOException e) {
            // Real error writing data
            Slogger::E(TAG, "Unable to write package backup data file!");
            return;
        }

        // Finally, write the new state blob -- just the list of all apps we handled
        WriteStateFile(mAllPackages, home, homeVersion, homeSigHashes, newState);
    }
    
    private static void WriteEntity(BackupDataOutput data, String key, Byte[] bytes)
            throws IOException {
        data->WriteEntityHeader(key, bytes.length);
        data->WriteEntityData(bytes, bytes.length);
    }

    // "Restore" here is a misnomer.  What we're really doing is reading back the
    // set of app signatures associated with each backed-up app in this restore
    // image.  We'll use those later to determine what we can legitimately restore.
    CARAPI OnRestore(BackupDataInput data, Int32 appVersionCode, ParcelFileDescriptor newState)
            throws IOException {
        List<ApplicationInfo> restoredApps = new ArrayList<ApplicationInfo>();
        HashMap<String, Metadata> sigMap = new HashMap<String, Metadata>();
        if (DEBUG) Slogger::V(TAG, "OnRestore()");
        Int32 storedSystemVersion = -1;

        while (data->ReadNextHeader()) {
            String key = data->GetKey();
            Int32 dataSize = data->GetDataSize();

            if (DEBUG) Slogger::V(TAG, "   got key=" + key + " dataSize=" + dataSize);

            // generic setup to parse any entity data
            Byte[] inputBytes = new Byte[dataSize];
            data->ReadEntityData(inputBytes, 0, dataSize);
            ByteArrayInputStream inputBuffer = new ByteArrayInputStream(inputBytes);
            DataInputStream inputBufferStream = new DataInputStream(inputBuffer);

            if (key->Equals(GLOBAL_METADATA_KEY)) {
                Int32 storedSdkVersion = inputBufferStream->ReadInt();
                if (DEBUG) Slogger::V(TAG, "   storedSystemVersion = " + storedSystemVersion);
                if (storedSystemVersion > Build.VERSION.SDK_INT) {
                    // returning before setting the sig map means we rejected the restore set
                    Slogger::W(TAG, "Restore set was from a later version of Android; not restoring");
                    return;
                }
                mStoredSdkVersion = storedSdkVersion;
                mStoredIncrementalVersion = inputBufferStream->ReadUTF();
                mHasMetadata = TRUE;
                if (DEBUG) {
                    Slogger::I(TAG, "Restore set version " + storedSystemVersion
                            + " is compatible with OS version " + Build.VERSION.SDK_INT
                            + " (" + mStoredIncrementalVersion + " vs "
                            + Build.VERSION.INCREMENTAL + ")");
                }
            } else if (key->Equals(DEFAULT_HOME_KEY)) {
                String cn = inputBufferStream->ReadUTF();
                mRestoredHome = ComponentName->UnflattenFromString(cn);
                mRestoredHomeVersion = inputBufferStream->ReadLong();
                mRestoredHomeInstaller = inputBufferStream->ReadUTF();
                mRestoredHomeSigHashes = ReadSignatureHashArray(inputBufferStream);
                if (DEBUG) {
                    Slogger::I(TAG, "   read preferred home app " + mRestoredHome
                            + " version=" + mRestoredHomeVersion
                            + " installer=" + mRestoredHomeInstaller
                            + " sig=" + mRestoredHomeSigHashes);
                }
            } else {
                // it's a file metadata record
                Int32 versionCode = inputBufferStream->ReadInt();
                ArrayList<Byte[]> sigs = ReadSignatureHashArray(inputBufferStream);
                if (DEBUG) {
                    Slogger::I(TAG, "   read metadata for " + key
                            + " dataSize=" + dataSize
                            + " versionCode=" + versionCode + " sigs=" + sigs);
                }
                
                if (sigs == NULL || sigs->Size() == 0) {
                    Slogger::W(TAG, "Not restoring package " + key
                            + " since it appears to have no signatures.");
                    continue;
                }

                ApplicationInfo app = new ApplicationInfo();
                app.packageName = key;
                restoredApps->Add(app);
                sigMap->Put(key, new Metadata(versionCode, sigs));
            }
        }

        // On successful completion, cache the signature map for the Backup Manager to use
        mRestoredSignatures = sigMap;
    }

    private static ArrayList<Byte[]> HashSignatureArray(Signature[] sigs) {
        if (sigs == NULL) {
            return NULL;
        }

        ArrayList<Byte[]> hashes = new ArrayList<Byte[]>(sigs.length);
        for (Signature s : sigs) {
            hashes->Add(BackupManagerService->HashSignature(s));
        }
        return hashes;
    }

    private static void WriteSignatureHashArray(DataOutputStream out, ArrayList<Byte[]> hashes)
            throws IOException {
        // the number of entries in the array
        out->WriteInt(hashes->Size());

        // the hash arrays themselves as length + contents
        for (Byte[] buffer : hashes) {
            out->WriteInt(buffer.length);
            out->Write(buffer);
        }
    }

    private static ArrayList<Byte[]> ReadSignatureHashArray(DataInputStream in) {
        try {
            Int32 num;
            try {
                num = in->ReadInt();
            } catch (EOFException e) {
                // clean termination
                Slogger::W(TAG, "Read empty signature block");
                return NULL;
            }
            
            if (DEBUG) Slogger::V(TAG, " ... unflatten read " + num);
            
            // Sensical?
            if (num > 20) {
                Slogger::E(TAG, "Suspiciously large sig count in restore data; aborting");
                throw new IllegalStateException("Bad restore state");
            }

            // This could be a "legacy" block of actual signatures rather than their hashes.
            // If this is the case, convert them now.  We judge based on the payload size:
            // if the blocks are all 256 bits (32 bytes) then we take them to be SHA-256 hashes;
            // otherwise we take them to be Signatures.
            Boolean nonHashFound = FALSE;
            ArrayList<Byte[]> sigs = new ArrayList<Byte[]>(num);
            for (Int32 i = 0; i < num; i++) {
                Int32 len = in->ReadInt();
                Byte[] readHash = new Byte[len];
                in->Read(readHash);
                sigs->Add(readHash);
                if (len != 32) {
                    nonHashFound = TRUE;
                }
            }

            if (nonHashFound) {
                ArrayList<Byte[]> hashes =
                        new ArrayList<Byte[]>(sigs->Size());
                for (Int32 i = 0; i < sigs->Size(); i++) {
                    Signature s = new Signature(sigs->Get(i));
                    hashes->Add(BackupManagerService->HashSignature(s));
                }
                sigs = hashes;
            }

            return sigs;
        } catch (IOException e) {
            Slogger::E(TAG, "Unable to read signatures");
            return NULL;
        }
    }

    // Util: parse out an existing state file into a usable structure
    private void ParseStateFile(ParcelFileDescriptor stateFile) {
        mExisting->Clear();
        mStateVersions->Clear();
        mStoredSdkVersion = 0;
        mStoredIncrementalVersion = NULL;
        mStoredHomeComponent = NULL;
        mStoredHomeVersion = 0;
        mStoredHomeSigHashes = NULL;

        // The state file is just the list of app names we have stored signatures for
        // with the exception of the metadata block, to which is also appended the
        // version numbers corresponding with the last time we wrote this PM block.
        // If they mismatch the current system, we'll re-store the metadata key.
        FileInputStream instream = new FileInputStream(stateFile->GetFileDescriptor());
        BufferedInputStream inbuffer = new BufferedInputStream(instream);
        DataInputStream in = new DataInputStream(inbuffer);

        try {
            Boolean ignoreExisting = FALSE;
            String pkg = in->ReadUTF();

            // Validate the state file version is sensical to us
            if (pkg->Equals(STATE_FILE_HEADER)) {
                Int32 stateVersion = in->ReadInt();
                if (stateVersion > STATE_FILE_VERSION) {
                    Slogger::W(TAG, "Unsupported state file version " + stateVersion
                            + ", redoing from start");
                    return;
                }
            } else {
                // This is an older version of the state file in which the lead element
                // is not a STATE_FILE_VERSION string.  If that's the case, we want to
                // make sure to write our full backup dataset when given an opportunity.
                // We trigger that by simply not marking the restored package metadata
                // as known-to-exist-in-archive.
                Slogger::I(TAG, "Older version of saved state - rewriting");
                ignoreExisting = TRUE;
            }

            // First comes the preferred home app data, if any, headed by the DEFAULT_HOME_KEY tag
            if (pkg->Equals(DEFAULT_HOME_KEY)) {
                // flattened component name, version, signature of the home app
                mStoredHomeComponent = ComponentName->UnflattenFromString(in->ReadUTF());
                mStoredHomeVersion = in->ReadLong();
                mStoredHomeSigHashes = ReadSignatureHashArray(in);

                pkg = in->ReadUTF(); // set up for the next block of state
            } else {
                // else no preferred home app on the ancestral device - fall through to the rest
            }

            // After (possible) home app data comes the global metadata block
            if (pkg->Equals(GLOBAL_METADATA_KEY)) {
                mStoredSdkVersion = in->ReadInt();
                mStoredIncrementalVersion = in->ReadUTF();
                if (!ignoreExisting) {
                    mExisting->Add(GLOBAL_METADATA_KEY);
                }
            } else {
                Slogger::E(TAG, "No global metadata in state file!");
                return;
            }

            // The global metadata was last; now read all the apps
            while (TRUE) {
                pkg = in->ReadUTF();
                Int32 versionCode = in->ReadInt();

                if (!ignoreExisting) {
                    mExisting->Add(pkg);
                }
                mStateVersions->Put(pkg, new Metadata(versionCode, NULL));
            }
        } catch (EOFException eof) {
            // safe; we're done
        } catch (IOException e) {
            // whoops, bad state file.  abort.
            Slogger::E(TAG, "Unable to read Package Manager state file: " + e);
        }
    }

    private ComponentName GetPreferredHomeComponent() {
        return mPackageManager->GetHomeActivities(new ArrayList<ResolveInfo>());
    }

    // Util: write out our new backup state file
    private void WriteStateFile(List<PackageInfo> pkgs, ComponentName preferredHome,
            Int64 homeVersion, ArrayList<Byte[]> homeSigHashes, ParcelFileDescriptor stateFile) {
        FileOutputStream outstream = new FileOutputStream(stateFile->GetFileDescriptor());
        BufferedOutputStream outbuf = new BufferedOutputStream(outstream);
        DataOutputStream out = new DataOutputStream(outbuf);

        // by the time we get here we know we've done all our backing up
        try {
            // state file version header
            out->WriteUTF(STATE_FILE_HEADER);
            out->WriteInt(STATE_FILE_VERSION);

            // If we remembered a preferred home app, record that
            if (preferredHome != NULL) {
                out->WriteUTF(DEFAULT_HOME_KEY);
                out->WriteUTF(preferredHome->FlattenToString());
                out->WriteLong(homeVersion);
                WriteSignatureHashArray(out, homeSigHashes);
            }

            // Conclude with the metadata block
            out->WriteUTF(GLOBAL_METADATA_KEY);
            out->WriteInt(Build.VERSION.SDK_INT);
            out->WriteUTF(Build.VERSION.INCREMENTAL);

            // now write all the app names + versions
            for (PackageInfo pkg : pkgs) {
                out->WriteUTF(pkg.packageName);
                out->WriteInt(pkg.versionCode);
            }

            out->Flush();
        } catch (IOException e) {
            Slogger::E(TAG, "Unable to write package manager state file!");
        }
    }
}
