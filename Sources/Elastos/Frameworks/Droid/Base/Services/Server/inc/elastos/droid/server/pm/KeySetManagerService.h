#ifndef __ELASTOS_DROID_SERVER_PM_KEYSETMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_PM_KEYSETMANAGERSERVICE_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/server/pm/CKeySetHandle.h"
#include "elastos/droid/server/pm/PackageSetting.h"
#include "Elastos.CoreLibrary.Security.h"
#include <elastos/core/Object.h>

using Elastos::Security::IPublicKey;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

class KeySetManagerService : public Object
{
public:
    KeySetManagerService(
        /* [in] */ HashMap<String, AutoPtr<PackageSetting> >* packages);

    /**
     * Determine if a package is signed by the given KeySet.
     *
     * Returns false if the package was not signed by all the
     * keys in the KeySet.
     *
     * Returns true if the package was signed by at least the
     * keys in the given KeySet.
     *
     * Note that this can return true for multiple KeySets.
     */
    CARAPI PackageIsSignedByLPr(
        /* [in] */ const String& packageName,
        /* [in] */ CKeySetHandle* ks,
        /* [out] */ Boolean* result);

    /**
     * Determine if a package is signed by the given KeySet.
     *
     * Returns false if the package was not signed by all the
     * keys in the KeySet, or if the package was signed by keys
     * not in the KeySet.
     *
     * Note that this can return only for one KeySet.
     */
    CARAPI PackageIsSignedByExactlyLPr(
        /* [in] */ const String& packageName,
        /* [in] */ CKeySetHandle* ks,
        /* [out] */ Boolean* result);

    /**
     * This informs the system that the given package has defined a KeySet
     * in its manifest that a) contains the given keys and b) is named
     * alias by that package.
     */
    CARAPI AddDefinedKeySetToPackageLPw(
        /* [in] */ const String& packageName,
        /* [in] */ IArraySet* keys,
        /* [in] */ const String& alias);

    /**
     * This informs the system that the given package has defined a KeySet
     * alias in its manifest to be an upgradeKeySet.  This must be called
     * after all of the defined KeySets have been added.
     */
    CARAPI AddUpgradeKeySetToPackageLPw(
        /* [in] */ const String& packageName,
        /* [in] */ const String& alias);

    /**
     * Similar to the above, this informs the system that the given package
     * was signed by the provided KeySet.
     */
    CARAPI AddSigningKeySetToPackageLPw(
        /* [in] */ const String& packageName,
        /* [in] */ IArraySet* signingKeys);

    /**
     * Fetches the KeySet corresponding to the given stable identifier.
     *
     * Returns {@link #KEYSET_NOT_FOUND} if the identifier doesn't
     * identify a {@link KeySet}.
     */
    CARAPI_(AutoPtr<CKeySetHandle>) GetKeySetByIdLPr(
        /* [in] */ Int64 id);

    /**
     * Fetches the {@link KeySetHandle} that a given package refers to by the
     * provided alias. Returns null if the package is unknown or does not have a
     * KeySet corresponding to that alias.
     */
    CARAPI GetKeySetByAliasAndPackageNameLPr(
        /* [in] */ const String& packageName,
        /* [in] */ const String& alias,
        /* [out] */ CKeySetHandle** handle);

    /**
     * Fetches the {@link PublicKey public keys} which belong to the specified
     * KeySet id.
     *
     * Returns {@code null} if the identifier doesn't
     * identify a {@link KeySetHandle}.
     */
    CARAPI_(AutoPtr<IArraySet>) GetPublicKeysFromKeySetLPr(
        /* [in] */ Int64 id);

    /**
     * Fetches the proper {@link KeySetHandle KeySet} that signed the given
     * package.
     *
     * @throws IllegalArgumentException if the package has no keyset data.
     * @throws NullPointerException if the package is unknown.
     */
    CARAPI_(AutoPtr<CKeySetHandle>) GetSigningKeySetByPackageNameLPr(
        /* [in] */ const String& packageName);

    /**
     * Fetches all the known {@link KeySetHandle KeySets} that may upgrade the given
     * package.
     *
     * @throws IllegalArgumentException if the package has no keyset data.
     * @throws NullPointerException if the package is unknown.
     */
    CARAPI GetUpgradeKeySetsByPackageNameLPr(
        /* [in] */ const String& packageName,
        /* [out] */ IArraySet** handles);

    CARAPI RemoveAppKeySetDataLPw(
        /* [in] */ const String& packageName);

    CARAPI EncodePublicKey(
        /* [in] */ IPublicKey* k,
        /* [out] */ String* key);

    // public void dumpLPr(PrintWriter pw, String packageName,
    //                     PackageManagerService.DumpState dumpState);

    CARAPI WriteKeySetManagerServiceLPr(
        /* [in] */ IXmlPullParser* serializer);

    CARAPI WritePublicKeysLPr(
        /* [in] */ IXmlPullParser* serializer);

    CARAPI WriteKeySetsLPr(
        /* [in] */ IXmlPullParser* serializer);

    CARAPI ReadKeySetsLPw(
        /* [in] */ IXmlPullParser* parser);

    CARAPI ReadKeysLPw(
        /* [in] */ IXmlPullParser* parser);

    CARAPI ReadKeySetListLPw(
        /* [in] */ IXmlPullParser* parser);

    CARAPI ReadIdentifierLPw(
        /* [in] */ IXmlPullParser* parser,
        /* [out] */ Int64 value);

    CARAPI ReadPublicKeyLPw(
        /* [in] */ IXmlPullParser* parser);

private:
    /**
     * Fetches the stable identifier associated with the given KeySet. Returns
     * {@link #KEYSET_NOT_FOUND} if the KeySet... wasn't found.
     */
    CARAPI_(Int64) GetIdByKeySetLPr(
        /* [in] */ CKeySetHandle* ks);

    /**
     * Creates a new KeySet corresponding to the given keys.
     *
     * If the {@link PublicKey PublicKeys} aren't known to the system, this
     * adds them. Otherwise, they're deduped.
     *
     * If the KeySet isn't known to the system, this adds that and creates the
     * mapping to the PublicKeys. If it is known, then it's deduped.
     *
     * If the KeySet isn't known to the system, this adds it to all appropriate
     * signingKeySets
     *
     * Throws if the provided set is {@code null}.
     */
    CARAPI AddKeySetLPw(
        /* [in] */ IArraySet* keys,
        /* [out] */ CKeySetHandle** handle);

    /**
     * Adds the given PublicKey to the system, deduping as it goes.
     */
    CARAPI_(Int64) AddPublicKeyLPw(
        /* [in] */ IPublicKey* key);

    /**
     * Finds the stable identifier for a KeySet based on a set of PublicKey stable IDs.
     *
     * Returns KEYSET_NOT_FOUND if there isn't one.
     */
    CARAPI_(Int64) GetIdFromKeyIdsLPr(
        /* [in] */ IArraySet* publicKeyIds);

    /**
     * Finds the stable identifier for a PublicKey or PUBLIC_KEY_NOT_FOUND.
     */
    CARAPI_(Int64) GetIdForPublicKeyLPr(
        /* [in] */ IPublicKey* k);

    /**
     * Gets an unused stable identifier for a KeySet.
     */
    CARAPI_(Int64) GetFreeKeySetIDLPw();

    /**
     * Same as above, but for public keys.
     */
    CARAPI_(Int64) GetFreePublicKeyIdLPw();

    CARAPI_(void) ClearPackageKeySetDataLPw(
        /* [in] */ PackageSetting* p);

    CARAPI GetOriginalKeySetsByPackageNameLPr(
        /* [in] */ const String& packageName,
        /* [out] */ IArraySet** sets);

public:
    static const String TAG;

    /* original keysets implementation had no versioning info, so this is the first */
    static const Int32 FIRST_VERSION = 1;

    static const Int32 CURRENT_VERSION = FIRST_VERSION;

    /** Sentinel value returned when a {@code KeySet} is not found. */
    static const Int64 KEYSET_NOT_FOUND = -1;

protected:
    /** Sentinel value returned when public key is not found. */
    static const Int64 PUBLIC_KEY_NOT_FOUND = -1;

    HashMap<Int64, AutoPtr<IArraySet> > mKeySetMapping;

private:
    HashMap<Int64, AutoPtr<CKeySetHandle> > mKeySets;

    HashMap<Int64, AutoPtr<IPublicKey> > mPublicKeys;

    AutoPtr<HashMap<String, AutoPtr<PackageSetting> > > mPackages;

    static Int64 sLastIssuedKeySetId;

    static Int64 sLastIssuedKeyId;
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_PM_KEYSETMANAGERSERVICE_H__
