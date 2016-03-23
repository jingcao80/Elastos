#ifndef __ELASTOS_DROID_SERVER_SERVICE_CONFIG_H__
#define __ELASTOS_DROID_SERVER_SERVICE_CONFIG_H__

#include <elastos/droid/ext/frameworkext.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.External.h>
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/HashSet.h>

using Elastos::Droid::Content::Pm::IFeatureInfo;
using Elastos::Droid::Content::Pm::ISignature;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::HashSet;
using Elastos::IO::IFile;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * Loads global system configuration info.
 */
class SystemConfig
    : public Object
{
public:
    class PermissionEntry
        : public Object
    {
    public:
        PermissionEntry(
            /* [in] */ const String& name)
            : mName(name)
        {
        }

    public:
        String mName;
        AutoPtr<ArrayOf<Int32> > mGids;
    };

public:

    static AutoPtr<SystemConfig> GetInstance();

    AutoPtr<ArrayOf<Int32> > GetGlobalGids();

    AutoPtr< HashMap<Int32, AutoPtr<HashSet<String> > > > GetSystemPermissions();

    AutoPtr<HashMap<String, String> > GetSharedLibraries();

    AutoPtr<HashMap<String, AutoPtr<IFeatureInfo> > > GetAvailableFeatures();

    AutoPtr<HashMap<String, AutoPtr<PermissionEntry> > > GetPermissions();

    AutoPtr<HashSet<String> > GetAllowInPowerSave();

    AutoPtr<HashSet<String> > GetFixedImeApps();

    AutoPtr< HashMap<AutoPtr<ISignature>, AutoPtr<HashSet<String> > > > GetSignatureAllowances();

    SystemConfig();

    CARAPI ReadPermissions(
        /* [in] */ IFile* libraryDir,
        /* [in] */ Boolean onlyFeatures);

    CARAPI ReadPermission(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ const String& name);

private:

    CARAPI ReadPermissionsFromXml(
        /* [in] */ IFile* permFile,
        /* [in] */ Boolean onlyFeatures);

public:
    static const String TAG;
    static Object sLock;

    static AutoPtr<SystemConfig> sInstance;

    // Group-ids that are given to all packages as read from etc/permissions/*.xml.
    AutoPtr<ArrayOf<Int32> > mGlobalGids;

    // These are the built-in uid -> permission mappings that were read from the
    // system configuration files.
    AutoPtr< HashMap<Int32, AutoPtr<HashSet<String> > > > mSystemPermissions;

    // These are the built-in shared libraries that were read from the
    // system configuration files.  Keys are the library names; strings are the
    // paths to the libraries.
    AutoPtr< HashMap<String, String> > mSharedLibraries;

    // These are the features this devices supports that were read from the
    // system configuration files.
    AutoPtr< HashMap<String, AutoPtr<IFeatureInfo> > > mAvailableFeatures;

    // These are the permission -> gid mappings that were read from the
    // system configuration files.
    AutoPtr< HashMap<String, AutoPtr<PermissionEntry> > > mPermissions;

    // These are the packages that are white-listed to be able to run in the
    // background while in power save mode, as read from the configuration files.
    AutoPtr< HashSet<String> > mAllowInPowerSave;

    // These are the app package names that should not allow IME switching.
    AutoPtr< HashSet<String> > mFixedImeApps;
};

} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_SERVICE_CONFIG_H__