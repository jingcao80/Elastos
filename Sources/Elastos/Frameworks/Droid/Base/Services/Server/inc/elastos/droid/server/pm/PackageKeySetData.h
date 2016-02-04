#ifndef __ELASTOS_DROID_SERVER_PM_PACKAGEKEYSETDATA_H__
#define __ELASTOS_DROID_SERVER_PM_PACKAGEKEYSETDATA_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

class PackageSettingBase;
class CPackageManagerService;
class Settings;
class KeySetManagerService;

class PackageKeySetData : public Object
{
public:
    PackageKeySetData();

    PackageKeySetData(
        /* [in] */ PackageKeySetData* original);

protected:
    CARAPI_(void) SetProperSigningKeySet(
        /* [in] */ Int64 ks);

    CARAPI_(Int64) GetProperSigningKeySet();

    CARAPI_(void) AddSigningKeySet(
        /* [in] */ Int64 ks);

    CARAPI_(void) RemoveSigningKeySet(
        /* [in] */ Int64 ks);

    CARAPI_(void) AddUpgradeKeySet(
        /* [in] */ const String& alias);

    /*
     * Used only when restoring keyset data from persistent storage.  Must
     * correspond to a defined-keyset.
     */
    CARAPI_(void) AddUpgradeKeySetById(
        /* [in] */ Int64 ks);

    CARAPI_(void) AddDefinedKeySet(
        /* [in] */ Int64 ks,
        /* [in] */ const String& alias);

    CARAPI_(void) RemoveAllSigningKeySets();

    CARAPI_(void) RemoveAllUpgradeKeySets();

    CARAPI_(void) RemoveAllDefinedKeySets();

    CARAPI_(Boolean) PackageIsSignedBy(
        /* [in] */ Int64 ks);

    CARAPI_(AutoPtr<ArrayOf<Int64> >) GetSigningKeySets();

    CARAPI_(AutoPtr<ArrayOf<Int64> >) GetUpgradeKeySets();

    CARAPI_(AutoPtr<ArrayOf<Int64> >) GetDefinedKeySets();

    HashMap<String, Int64>& GetAliases();

    CARAPI_(Boolean) IsUsingDefinedKeySets();

    CARAPI_(Boolean) IsUsingUpgradeKeySets();

public:
    static const Int64 KEYSET_UNASSIGNED = -1;

private:
    /* KeySet containing all signing keys - superset of the others */
    Int64 mProperSigningKeySet;

    AutoPtr<ArrayOf<Int64> > mSigningKeySets;

    AutoPtr<ArrayOf<Int64> > mUpgradeKeySets;

    AutoPtr<ArrayOf<Int64> > mDefinedKeySets;

    HashMap<String, Int64> mKeySetAliases;

    friend class PackageSettingBase;
    friend class CPackageManagerService;
    friend class Settings;
    friend class KeySetManagerService;
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_PM_PACKAGEKEYSETDATA_H__
