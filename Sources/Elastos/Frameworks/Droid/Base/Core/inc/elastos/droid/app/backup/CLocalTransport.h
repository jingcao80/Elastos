
#ifndef __ELASTOS_DROID_INTERNAL_BACKUP_CLOCALTRANSPORT_H__
#define __ELASTOS_DROID_INTERNAL_BACKUP_CLOCALTRANSPORT_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Internal_Backup_CLocalTransport.h"

using Elastos::IO::IFile;
using Elastos::Droid::App::Backup::IRestoreSet;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Os::IParcelFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Backup {

CarClass(CLocalTransport)
{
public:

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CLocalTransport();

    ~CLocalTransport();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI ConfigurationIntent(
        /* [out] */ IIntent** intent);

    CARAPI CurrentDestinationString(
        /* [out] */ String* string);

    CARAPI TransportDirName(
        /* [out] */ String* string);

    CARAPI RequestBackupTime(
        /* [out] */ Int64* time);

    CARAPI InitializeDevice(
        /* [out] */ Int32* value);

    CARAPI PerformBackup(
        /* [in] */ IPackageInfo* packageInfo,
        /* [in] */ IParcelFileDescriptor* inFd,
        /* [out] */ Int32* value);

    CARAPI ClearBackupData(
        /* [in] */ IPackageInfo* packageInfo,
        /* [out] */ Int32* value);

    CARAPI FinishBackup(
        /* [out] */ Int32* value);

    CARAPI GetAvailableRestoreSets(
        /* [out, callee] */ ArrayOf<IRestoreSet*>** result);

    CARAPI GetCurrentRestoreSet(
        /* [out] */ Int64* value);

    CARAPI StartRestore(
        /* [in] */ Int64 token,
        /* [in] */ ArrayOf<IPackageInfo*>* packages,
        /* [out] */ Int32* value);

    CARAPI NextRestorePackage(
        /* [out] */ String* result);

    CARAPI GetRestoreData(
        /* [in] */ IParcelFileDescriptor* outFd,
        /* [out] */ Int32* value);

    CARAPI FinishRestore();

private:
    // Deletes the contents but not the given directory
    CARAPI_(void) DeleteContents(
        /* [in] */ IFile* dirname);

private:
    static const String TAG;
    static const Boolean DEBUG;
    static const String TRANSPORT_DIR_NAME;
    static const String TRANSPORT_DESTINATION_STRING;

    // The single hardcoded restore set always has the same (nonzero!) token
    static const Int64 RESTORE_TOKEN;

    AutoPtr<IContext> mContext;
    AutoPtr<IFile> mDataDir;
    AutoPtr<ArrayOf<IPackageInfo*> > mRestorePackages;
    // Index into mRestorePackages
    Int32 mRestorePackage;

};
} // Backup
} // Internal
} // Droid
} // namespace Elastos
#endif //__ELASTOS_DROID_INTERNAL_BACKUP_CLOCALTRANSPORT_H__
