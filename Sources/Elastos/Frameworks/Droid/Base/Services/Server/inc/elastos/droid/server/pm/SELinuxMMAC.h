
#ifndef __ELASTOS_DROID_SERVER_PM_SELINUXMMAC_H__
#define __ELASTOS_DROID_SERVER_PM_SELINUXMMAC_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/pm/PackageParser.h"
#include "Elastos.Droid.Content.h"
#include <Elastos.CoreLibrary.IO.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Content::Pm::ISignature;
using Elastos::Droid::Content::Pm::PackageParser;
using Org::Xmlpull::V1::IXmlPullParser;
using Elastos::IO::IFile;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

/**
 * Centralized access to SELinux MMAC (middleware MAC) implementation.
 * {@hide}
 */
class SELinuxMMAC
{
public:
    // Signature policy stanzas
    class Policy : public Object
    {
    public:
        Policy()
            : mSeinfo(NULL)
        {}

        CARAPI_(void) PutSeinfo(
            /* [in] */ const String& seinfoValue);

        CARAPI_(void) PutPkg(
            /* [in] */ const String& pkg,
            /* [in] */ const String& seinfoValue);

        // Valid policy stanza means there exists a global
        // seinfo value or at least one package policy.
        CARAPI_(Boolean) IsValid();

        CARAPI_(String) CheckPolicy(
            /* [in] */ const String& pkgName);

    private:
        String mSeinfo;
        HashMap<String, String> mPkgMap;
    };

public:
    static CARAPI_(Boolean) ReadInstallPolicy();

    /**
     * Labels a package based on an seinfo tag from install policy.
     * The label is attached to the ApplicationInfo instance of the package.
     * @param pkg object representing the package to be labeled.
     * @return boolean which determines whether a non null seinfo label
     *         was assigned to the package. A null value simply meaning that
     *         no policy matched.
     */
    static CARAPI_(Boolean) AssignSeinfoValue(
        /* [in] */ PackageParser::Package* pkg);

    /**
     * Determines if a recursive restorecon on /data/data and /data/user is needed.
     * It does this by comparing the SHA-1 of the seapp_contexts file against the
     * stored hash at /data/system/seapp_hash.
     *
     * @return Returns true if the restorecon should occur or false otherwise.
     */
    static CARAPI_(Boolean) ShouldRestorecon();

    /**
     * Stores the SHA-1 of the seapp_contexts to /data/system/seapp_hash.
     */
    static CARAPI_(void) SetRestoreconDone();

private:
    static CARAPI_(void) FlushInstallPolicy();

    static CARAPI ReadPolicyTags(
        /* [in] */ IXmlPullParser* parser,
        /* [out] */ Policy** policy);

    static CARAPI ReadSeinfoTag(
        /* [in] */ IXmlPullParser* parser,
        /* [out] */ String* tag);

    static CARAPI_(String) ParseSeinfo(
        /* [in] */ IXmlPullParser* parser);

    /**
     * General validation routine for package names.
     * Returns a boolean indicating if the passed string
     * is a valid android package name.
     */
    static CARAPI_(Boolean) ValidatePackageName(
        /* [in] */ const String& name);

    /**
     * General validation routine for tag values.
     * Returns a boolean indicating if the passed string
     * contains only letters or underscores.
     */
    static CARAPI_(Boolean) ValidateValue(
        /* [in] */ const String& name);

    /**
     * Dump the contents of a byte array to a specified file.
     *
     * @param file The file that receives the byte array content.
     * @param content A byte array that will be written to the specified file.
     * @throws IOException if any failed I/O operation occured.
     *         Included is the failure to atomically rename the tmp
     *         file used in the process.
     */
    static CARAPI DumpHash(
        /* [in] */ IFile* file,
        /* [in] */ ArrayOf<Byte>* content);

    /**
     * Return the SHA-1 of a file.
     *
     * @param file The path to the file given as a string.
     * @return Returns the SHA-1 of the file as a byte array.
     * @throws IOException if any failed I/O operations occured.
     */
    static CARAPI ReturnHash(
        /* [in] */ const String& file,
        /* [out] */ ArrayOf<Byte>** bytes);

    static CARAPI_(Boolean) UseOverridePolicy();

private:
    static const String TAG;

    static const Boolean DEBUG_POLICY = FALSE;
    static const Boolean DEBUG_POLICY_INSTALL = DEBUG_POLICY || FALSE;

    // Signature seinfo values read from policy.
    static HashMap<AutoPtr<ISignature>, AutoPtr<Policy> > sSigSeinfo;

    // Default seinfo read from policy.
    static String sDefaultSeinfo;

    // Data policy override version file.
    static const String DATA_VERSION_FILE;

    // Base policy version file.
    static const String BASE_VERSION_FILE;

    // Whether override security policies should be loaded.
    static const Boolean USE_OVERRIDE_POLICY;

    // Data override mac_permissions.xml policy file.
    static const String DATA_MAC_PERMISSIONS;

    // Base mac_permissions.xml policy file.
    static const String BASE_MAC_PERMISSIONS;

    // Determine which mac_permissions.xml file to use.
    static const String MAC_PERMISSIONS;

    // Data override seapp_contexts policy file.
    static const String DATA_SEAPP_CONTEXTS;

    // Base seapp_contexts policy file.
    static const String BASE_SEAPP_CONTEXTS;

    // Determine which seapp_contexts file to use.
    static const String SEAPP_CONTEXTS;

    // Stores the hash of the last used seapp_contexts file.
    static const String SEAPP_HASH_FILE;
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_PM_SELINUXMMAC_H__
