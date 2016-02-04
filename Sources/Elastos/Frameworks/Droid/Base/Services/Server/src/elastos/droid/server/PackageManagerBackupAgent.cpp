
#include "PackageManagerBackupAgent.h"
#include "elastos/droid/os/Build.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Logging::Slogger;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::CDataOutputStream;
using Elastos::IO::CDataInputStream;
using Elastos::IO::IDataInput;
using Elastos::IO::IDataOutput;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::Droid::Content::Pm::CApplicationInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::ISignature;
using Elastos::Droid::Content::Pm::CSignature;
using Elastos::Droid::Os::Build;

namespace Elastos {
namespace Droid {
namespace Server {

const String PackageManagerBackupAgent::TAG("PMBA");
const Boolean PackageManagerBackupAgent::DEBUG = FALSE;
const String PackageManagerBackupAgent::GLOBAL_METADATA_KEY("@meta@");

PackageManagerBackupAgent::PackageManagerBackupAgent(
    /* [in] */ IPackageManager* packageMgr,
    /* [in] */ List<AutoPtr<IPackageInfo> >* packages)
    : mAllPackages(packages)
    , mPackageManager(packageMgr)
    , mStoredSdkVersion(0)
    , mHasMetadata(FALSE)
{}

PackageManagerBackupAgent::~PackageManagerBackupAgent()
{}

Boolean PackageManagerBackupAgent::HasMetadata()
{
    return mHasMetadata;
}

AutoPtr<PackageManagerBackupAgent::Metadata> PackageManagerBackupAgent::GetRestoredMetadata(
    /* [in] */ const String& packageName)
{
    if (mRestoredSignatures == NULL) {
        Slogger::W(TAG, "GetRestoredMetadata() before metadata read!");
        return NULL;
    }

    MetadataHashMapIter it = mRestoredSignatures->Find(packageName);
    if (it != mRestoredSignatures->End()) {
        return it->mSecond;
    }
    return NULL;
}

AutoPtr< HashSet<String> > PackageManagerBackupAgent::GetRestoredPackages()
{
    if (mRestoredSignatures == NULL) {
        Slogger::W(TAG, "GetRestoredPackages() before metadata read!");
        return NULL;
    }

    // This is technically the set of packages on the originating handset
    // that had backup agents at all, not limited to the set of packages
    // that had actually contributed a restore dataset, but it's a
    // close enough approximation for our purposes and does not require any
    // additional involvement by the transport to obtain.
    AutoPtr< HashSet<String> > keys = new HashSet<String>();
    MetadataHashMapIter it = mRestoredSignatures->Begin();
    for(; it != mRestoredSignatures->End(); ++it) {
        keys->Insert(it->mFirst);
    }
    return keys;
}

CARAPI PackageManagerBackupAgent::OnBackup(
    /* [in] */ IParcelFileDescriptor* oldState,
    /* [in] */ IBackupDataOutput* data,
    /* [in] */ IParcelFileDescriptor* newState)
{
    if (DEBUG) {
        Slogger::V(TAG, "OnBackup()");
    }

    AutoPtr<IByteArrayOutputStream> outputBuffer;
    CByteArrayOutputStream::New((IByteArrayOutputStream**)&outputBuffer);
    AutoPtr<IDataOutputStream> outputBufferStream;
    CDataOutputStream::New(outputBuffer, (IDataOutputStream**)&outputBufferStream);
    ParseStateFile(oldState);

    // If the stored version string differs, we need to re-backup all
    // of the metadata.  We force this by removing everything from the
    // "already backed up" map built by parseStateFile().
    if (mStoredIncrementalVersion == NULL
            || !mStoredIncrementalVersion.Equals(Build::VERSION::INCREMENTAL)) {
        Slogger::I(TAG, "Previous metadata %s mismatch vs %s -rewriting",  mStoredIncrementalVersion.string(), Build::VERSION::INCREMENTAL.string());
        mExisting.Clear();
    }

    // try {
    /*
     * Global metadata:
     *
     * int SDKversion -- the SDK version of the OS itself on the device
     *                   that produced this backup set.  Used to reject
     *                   backups from later OSes onto earlier ones.
     * String incremental -- the incremental release name of the OS stored in
     *                       the backup set.
     */
    if (mExisting.Find(GLOBAL_METADATA_KEY) == mExisting.End()) {
        if (DEBUG) Slogger::V(TAG, "Storing global metadata key");
        ECode ec = IDataOutput::Probe(outputBufferStream)->WriteInt32(Build::VERSION::SDK_INT);
        if (FAILED(ec)) {
            Slogger::E(TAG, "Unable to write package backup data file!");
            return ec;
        }
        ec = IDataOutput::Probe(outputBufferStream)->WriteUTF(Build::VERSION::INCREMENTAL);
        if (FAILED(ec)) {
            Slogger::E(TAG, "Unable to write package backup data file!");
            return ec;
        }
        AutoPtr<ArrayOf<Byte> > bytes;
        outputBuffer->ToByteArray((ArrayOf<Byte>**)&bytes);
        ec = WriteEntity(data, GLOBAL_METADATA_KEY, bytes);
        if (FAILED(ec)) {
            Slogger::E(TAG, "Unable to write package backup data file!");
            return ec;
        }
    }
    else {
        if (DEBUG) Slogger::V(TAG, "Global metadata key already stored");
        // don't consider it to have been skipped/deleted
        mExisting.Erase(GLOBAL_METADATA_KEY);
    }

    // For each app we have on device, see if we've backed it up yet.  If not,
    // write its signature block to the output, keyed on the package name.
    for (PackageInfoListIter it = mAllPackages->Begin(); it != mAllPackages->End(); ++it) {
        String packName;
        (*it)->GetPackageName(&packName);
        if (packName.Equals(GLOBAL_METADATA_KEY)) {
            // We've already handled the metadata key; skip it here
            continue;
        }
        else {
            AutoPtr<IPackageInfo> info;
            //try {
            ECode ec = mPackageManager->GetPackageInfo(packName, IPackageManager::GET_SIGNATURES, (IPackageInfo**)&info);
            if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
                mExisting.Insert(packName);
                continue;
            }
            //} catch (NameNotFoundException e) {
                // Weird; we just found it, and now are told it doesn't exist.
                // Treat it as having been removed from the device.
            //    mExisting.add(packName);
            //    continue;
            //}
            Int32 versionCode;
            info->GetVersionCode(&versionCode);
            if (mExisting.Find(packName) != mExisting.End()) {
                // We have backed up this app before.  Check whether the version
                // of the backup matches the version of the current app; if they
                // don't match, the app has been updated and we need to store its
                // metadata again.  In either case, take it out of mExisting so that
                // we don't consider it deleted later.
                mExisting.Erase(packName);
                MetadataHashMapIter it = mStateVersions.Find(packName);
                if (it != mStateVersions.End()) {
                    if (versionCode == it->mSecond->mVersionCode) {
                        continue;
                    }
                }
            }

            AutoPtr<ArrayOf<ISignature* > > signatures;
            info->GetSignatures((ArrayOf<ISignature*>**)&signatures);
            if (signatures == NULL || signatures->GetLength() == 0) {
                Slogger::W(TAG, "Not backing up package  %s since it appears to have no signatures", packName.string());
                continue;
            }

            // We need to store this app's metadata
            /*
             * Metadata for each package:
             *
             * int version       -- [4] the package's versionCode
             * byte[] signatures -- [len] flattened Signature[] of the package
             */

            // marshal the version code in a canonical form
            outputBuffer->Reset();
            ec = IDataOutput::Probe(outputBufferStream)->WriteInt32(versionCode);
            if (FAILED(ec)) {
                Slogger::E(TAG, "Unable to write package backup data file!");
                return ec;
            }
            ec = WriteSignatureArray(outputBufferStream, signatures);
            if (FAILED(ec)) {
                Slogger::E(TAG, "Unable to write package backup data file!");
                return ec;
            }

            if (DEBUG) {
                Int32 size = 0;
                outputBuffer->GetSize(&size);
                Slogger::V(TAG, "+ writing metadata for %s version= %d, entityLen=%d", packName.string(), versionCode, size);
            }

            // Now we can write the backup entity for this package
            AutoPtr<ArrayOf<Byte> > bytes;
            outputBuffer->ToByteArray((ArrayOf<Byte>**)&bytes);
            ec = WriteEntity(data, packName, (ArrayOf<Byte>*)bytes);
            if (FAILED(ec)) {
                Slogger::E(TAG, "Unable to write package backup data file!");
                return ec;
            }
        }
    }

    // At this point, the only entries in 'existing' are apps that were
    // mentioned in the saved state file, but appear to no longer be present
    // on the device.  Write a deletion entity for them.
    for (HashSet<String>::Iterator it = mExisting.Begin(); it != mExisting.End(); ++it) {
        if (DEBUG) Slogger::V(TAG, "- removing metadata for deleted pkg %s", (*it).string());
        //try {
        Int32 result = 0;
        ECode ec = data->WriteEntityHeader(*it, -1, &result);
        if (FAILED(ec)) {
            Slogger::E(TAG, "Unable to write package deletions!");
            return ec;
        }
        /*
         *} catch (IOException e) {
         *    Slog.e(TAG, "Unable to write package deletions!");
         *    return;
         *}
         */
    }
    // } catch (IOException e) {
    //     // Real error writing data
    //     Slog.e(TAG, "Unable to write package backup data file!");
    //     return;
    // }

    // Finally, write the new state blob -- just the list of all apps we handled
    WriteStateFile(mAllPackages, newState);
    return NOERROR;
}

ECode PackageManagerBackupAgent::WriteEntity(
    /* [in] */ IBackupDataOutput* data,
    /* [in] */ const String& key,
    /* [in] */ ArrayOf<Byte>* bytes)
{
    Int32 writtenSize;
    FAIL_RETURN(data->WriteEntityHeader(key, bytes->GetLength(), &writtenSize));
    return data->WriteEntityData(bytes, bytes->GetLength(), &writtenSize);
}

CARAPI PackageManagerBackupAgent::OnRestore(
    /* [in] */ IBackupDataInput* data,
    /* [in] */ Int32 appVersionCode,
    /* [in] */ IParcelFileDescriptor* newState)
{
    List< AutoPtr<IApplicationInfo> > restoredApps;
    AutoPtr<MetadataHashMap> sigMap = new MetadataHashMap();
    if (DEBUG) Slogger::V(TAG, "OnRestore()");
    Int32 storedSystemVersion = -1;

    Boolean bNext = FALSE;
    while (data->ReadNextHeader(&bNext), bNext) {
        String key;
        data->GetKey(&key);
        Int32 dataSize;
        data->GetDataSize(&dataSize);

        if (DEBUG) Slogger::V(TAG, "got key=%s, datasize=%d", key.string(), dataSize);

        // generic setup to parse any entity data
        AutoPtr<ArrayOf<Byte> > inputBytes = ArrayOf<Byte>::Alloc(dataSize);
        Int32 readSize;
        data->ReadEntityData((ArrayOf<Byte>*)inputBytes, 0, dataSize, &readSize);
        AutoPtr<IByteArrayInputStream> inputBuffer;
        CByteArrayInputStream::New(inputBytes, (IByteArrayInputStream**)&inputBuffer);
        AutoPtr<IDataInputStream> inputBufferStream;
        CDataInputStream::New(inputBuffer, (IDataInputStream**)&inputBufferStream);

        if (key.Equals(GLOBAL_METADATA_KEY)) {
            Int32 storedSdkVersion;
            FAIL_RETURN(IDataInput::Probe(inputBufferStream)->ReadInt32(&storedSdkVersion));
            if (DEBUG) Slogger::V(TAG, "storedSystemVersion= %d", storedSdkVersion);
            if (storedSdkVersion > Build::VERSION::SDK_INT) {
                // returning before setting the sig map means we rejected the restore set
                Slogger::W(TAG, "Restore set was from a later version of Android; not restoring");
                return NOERROR;
            }
            mStoredSdkVersion = storedSdkVersion;
            FAIL_RETURN(IDataInput::Probe(inputBufferStream)->ReadUTF(&mStoredIncrementalVersion));
            mHasMetadata = TRUE;
            if (DEBUG) {
                Slogger::I(TAG, "Restore set version %d is compatible with OS version %d (%s vs %s)",
                        storedSdkVersion, Build::VERSION::SDK_INT, mStoredIncrementalVersion.string(),
                        Build::VERSION::INCREMENTAL.string());
            }
        }
        else {
            // it's a file metadata record
            Int32 versionCode;
            FAIL_RETURN(IDataInput::Probe(inputBufferStream)->ReadInt32(&versionCode));
            AutoPtr<ArrayOf<ISignature*> > sigs;
            FAIL_RETURN(ReadSignatureArray((IDataInputStream*)inputBufferStream, (ArrayOf<ISignature*>**)&sigs));
            if (DEBUG) {
                Slogger::I(TAG, "read metadata for %s datasize=%d versionCode=%d", key.string(), dataSize, versionCode);
            }

            if (sigs == NULL || sigs->GetLength() == 0) {
                Slogger::W(TAG, "Not restoring package %s since it appears to have no signatures.", key.string());
                continue;
            }

            AutoPtr<IApplicationInfo> app;
            CApplicationInfo::New((IApplicationInfo**)&app);
            app->SetPackageName(key);
            restoredApps.PushBack(app);
            (*sigMap)[key] = new Metadata(versionCode, sigs);
        }
    }
    // On successful completion, cache the signature map for the Backup Manager to use
    mRestoredSignatures = sigMap;
    return NOERROR;
}

ECode PackageManagerBackupAgent::WriteSignatureArray(
    /* [in] */ IDataOutputStream* out,
    /* [in] */ ArrayOf<ISignature*>* sigs)
{
    // write the number of signatures in the array
    FAIL_RETURN(IDataOutput::Probe(out)->WriteInt32(sigs->GetLength()));

    // write the signatures themselves, length + flattened buffer
    for (Int32 i = 0; i < sigs->GetLength(); ++i) {
        AutoPtr<ArrayOf<Byte> > flat;
        (*sigs)[i]->ToByteArray((ArrayOf<Byte>**)&flat);
        FAIL_RETURN(IDataOutput::Probe(out)->WriteInt32(flat->GetLength()));
        FAIL_RETURN(out->Write(flat));
    }
    return NOERROR;
}

ECode PackageManagerBackupAgent::ReadSignatureArray(
    /* [in] */ IDataInputStream* in,
    /* [out] */ ArrayOf<ISignature*>** sigs)
{
    VALIDATE_NOT_NULL(sigs);
    // try {
    Int32 num;
    // try {
    ECode ec = IDataInput::Probe(in)->ReadInt32(&num);
    if (ec == (ECode)E_EOF_EXCEPTION) {
        Slogger::W(TAG, "Read empty signature block");
        *sigs = NULL;
        return NOERROR;
    }
    // } catch (EOFException e) {
    //     // clean termination
    //     Slog.w(TAG, "Read empty signature block");
    //     return null;
    // }

    if (DEBUG) Slogger::V(TAG, ".... unflatten read %d", num);

    // Sensical?
    if (num > 20) {
        Slogger::E(TAG, "Suspiciously large sig count in restore data; aborting");
        //throw new IllegalStateException("Bad restore state");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr< ArrayOf<ISignature*> > _sigs = ArrayOf<ISignature*>::Alloc(num);
    for (Int32 i = 0; i < num; ++i) {
        Int32 len;
        ec = IDataInput::Probe(in)->ReadInt32(&len);
        if (FAILED(ec)) {
            Slogger::E(TAG, "Unable to read signatures");
            *sigs = NULL;
            return NOERROR;
        }
        AutoPtr<ArrayOf<Byte> > flatSig = ArrayOf<Byte>::Alloc(len);
        ec = in->ReadBytes(flatSig, &len);
        if (FAILED(ec)) {
            Slogger::E(TAG, "Unable to read signatures");
            *sigs = NULL;
            return NOERROR;
        }
        AutoPtr<ISignature> sig;
        CSignature::New(flatSig, (ISignature**)&sig);
        _sigs->Set(i, sig);
    }
    *sigs = _sigs;
    REFCOUNT_ADD(*sigs);
    return NOERROR;
    // } catch (IOException e) {
    //     Slog.e(TAG, "Unable to read signatures");
    //     return null;
    // }
}

void PackageManagerBackupAgent::ParseStateFile(
    /* [in] */ IParcelFileDescriptor* stateFile)
{
    mExisting.Clear();
    mStateVersions.Clear();
    mStoredSdkVersion = 0;
    mStoredIncrementalVersion = NULL;

    // The state file is just the list of app names we have stored signatures for
    // with the exception of the metadata block, to which is also appended the
    // version numbers corresponding with the last time we wrote this PM block.
    // If they mismatch the current system, we'll re-store the metadata key.
    AutoPtr<IFileDescriptor> fd;
    stateFile->GetFileDescriptor((IFileDescriptor**)&fd);
    AutoPtr<IFileInputStream> instream;
    CFileInputStream::New(fd, (IFileInputStream**)&instream);
    AutoPtr<IDataInputStream> in;
    CDataInputStream::New(instream, (IDataInputStream**)&in);

    AutoPtr<ArrayOf<Byte> > buf = ArrayOf<Byte>::Alloc(256);
    // try {
    ECode ec = NOERROR;
    String pkg;
    FAIL_GOTO((ec = IDataInput::Probe(in)->ReadUTF(&pkg)), Exit);
    if (pkg.Equals(GLOBAL_METADATA_KEY)) {
        FAIL_GOTO((ec = IDataInput::Probe(in)->ReadInt32(&mStoredSdkVersion)), Exit);
        FAIL_GOTO((ec = IDataInput::Probe(in)->ReadUTF(&mStoredIncrementalVersion)), Exit);
        mExisting.Insert(GLOBAL_METADATA_KEY);
    }
    else {
        Slogger::E(TAG, "No global metadata in state file!");
        return;
    }

    // The global metadata was first; now read all the apps
    while (TRUE) {
        FAIL_GOTO((ec = IDataInput::Probe(in)->ReadUTF(&pkg)), Exit);
        Int32 versionCode;
        FAIL_GOTO((ec = IDataInput::Probe(in)->ReadInt32(&versionCode)), Exit);
        mExisting.Insert(pkg);
        mStateVersions[pkg] = new Metadata(versionCode, NULL);
    }
Exit:
    if (ec == (ECode)E_EOF_EXCEPTION) {
        //safe; we're done
    }
    else if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::E(TAG, "Unable to read Package Manager state file");
    }
    // } catch (EOFException eof) {
    //     // safe; we're done
    // } catch (IOException e) {
    //     // whoops, bad state file.  abort.
    //     Slog.e(TAG, "Unable to read Package Manager state file: " + e);
    // }
}

void PackageManagerBackupAgent::WriteStateFile(
    /* [in] */ List<AutoPtr<IPackageInfo> >* pkgs,
    /* [in] */ IParcelFileDescriptor* stateFile)
{
    AutoPtr<IFileDescriptor> fd;
    stateFile->GetFileDescriptor((IFileDescriptor**)&fd);
    AutoPtr<IFileOutputStream> outstream;
    CFileOutputStream::New(fd, (IFileOutputStream**)&outstream);
    AutoPtr<IDataOutput> out;
    CDataOutputStream::New(outstream, (IDataOutput**)&out);

    // try {
    List<AutoPtr<IPackageInfo> >::Iterator it;
    //try {
    // by the time we get here we know we've stored the global metadata record
    FAIL_GOTO(out->WriteUTF(GLOBAL_METADATA_KEY), Exit);
    FAIL_GOTO(out->WriteInt32(Build::VERSION::SDK_INT), Exit);
    FAIL_GOTO(out->WriteUTF(Build::VERSION::INCREMENTAL), Exit);

    // now write all the app names too
    for (it = pkgs->Begin(); it != pkgs->End(); ++it) {
        String packageName;
        (*it)->GetPackageName(&packageName);
        FAIL_GOTO(out->WriteUTF(packageName), Exit);
        Int32 versionCode;
        (*it)->GetVersionCode(&versionCode);
        FAIL_GOTO(out->WriteInt32(versionCode), Exit);
    }
    return;
Exit:
    Slogger::E(TAG, "Unable to write package manager state file!");
    return;
    // }catch (IOException e) {
    //     Slog.e(TAG, "Unable to write package manager state file!");
    //     return;
    // }
}

} //namespace Droid
} //namespace Server
} //namespace Elastos
