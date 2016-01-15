
#include "elastos/droid/server/pm/PackageKeySetData.h"
#include "elastos/droid/internal/utility/ArrayUtils.h"

using Elastos::Droid::Internal::Utility::ArrayUtils;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

const Int64 PackageKeySetData::KEYSET_UNASSIGNED;

PackageKeySetData::PackageKeySetData()
    : mProperSigningKeySet(KEYSET_UNASSIGNED)
{}

PackageKeySetData::PackageKeySetData(
    /* [in] */ PackageKeySetData* original)
{
    mProperSigningKeySet = original->mProperSigningKeySet;
    mSigningKeySets = ArrayUtils::CloneOrNull(original->mSigningKeySets);
    mUpgradeKeySets = ArrayUtils::CloneOrNull(original->mUpgradeKeySets);
    mDefinedKeySets = ArrayUtils::CloneOrNull(original->mDefinedKeySets);
    HashMap<String, Int64>::Iterator it = original->mKeySetAliases.Begin();
    for (; it != original->mKeySetAliases.End(); ++it) {
        mKeySetAliases[it->mFirst] = it->mSecond;
    }
}

void PackageKeySetData::SetProperSigningKeySet(
    /* [in] */ Int64 ks)
{
    if (ks == mProperSigningKeySet) {
        /* nothing to change */
        return;
    }

    /* otherwise, our current signing keysets are likely invalid */
    RemoveAllSigningKeySets();
    mProperSigningKeySet = ks;
    AddSigningKeySet(ks);
    return;
}

Int64 PackageKeySetData::GetProperSigningKeySet()
{
    return mProperSigningKeySet;
}

void PackageKeySetData::AddSigningKeySet(
    /* [in] */ Int64 ks)
{
    mSigningKeySets = ArrayUtils::AppendInt64(mSigningKeySets, ks);
}

void PackageKeySetData::RemoveSigningKeySet(
    /* [in] */ Int64 ks)
{
    mSigningKeySets = ArrayUtils::RemoveInt64(mSigningKeySets, ks);
}

void PackageKeySetData::AddUpgradeKeySet(
    /* [in] */ const String& alias)
{
    /* must have previously been defined */
    Int64 ks;
    HashMap<String, Int64>::Iterator it = mKeySetAliases.Find();
    for (it != mKeySetAliases.End()) {
        ks = it->mSecond;
    }
    // if (ks != null) {
    mUpgradeKeySets = ArrayUtils::AppendInt64(mUpgradeKeySets, ks);
    // } else {
    //     throw new IllegalArgumentException("Upgrade keyset alias " + alias
    //             + "does not refer to a defined keyset alias!");
    // }
}

void PackageKeySetData::AddUpgradeKeySetById(
    /* [in] */ Int64 ks)
{
    mSigningKeySets = ArrayUtils::AppendInt64(mSigningKeySets, ks);
}

void PackageKeySetData::AddDefinedKeySet(
    /* [in] */ Int64 ks,
    /* [in] */ const String& alias)
{
    mDefinedKeySets = ArrayUtils::AppendInt64(mDefinedKeySets, ks);
    mKeySetAliases[alias] = ks;
}

void PackageKeySetData::RemoveAllSigningKeySets()
{
    mProperSigningKeySet = KEYSET_UNASSIGNED;
    mSigningKeySets = NULL;
}

void PackageKeySetData::RemoveAllUpgradeKeySets()
{
    mUpgradeKeySets = NULL;
}

void PackageKeySetData::RemoveAllDefinedKeySets()
{
    mDefinedKeySets = NULL;
    mKeySetAliases.Clear();
}

Boolean PackageKeySetData::PackageIsSignedBy(
    /* [in] */ Int64 ks)
{
    return ArrayUtils::Contains(mSigningKeySets, ks);
}

ArrayOf<ArrayOf<Int64> > PackageKeySetData::GetSigningKeySets()
{
    return mSigningKeySets;
}

ArrayOf<ArrayOf<Int64> > PackageKeySetData::GetUpgradeKeySets()
{
    return mUpgradeKeySets;
}

ArrayOf<ArrayOf<Int64> > PackageKeySetData::GetDefinedKeySets()
{
    return mDefinedKeySets;
}

HashMap<String, Int64>& PackageKeySetData::GetAliases()
{
    return mKeySetAliases;
}

Boolean PackageKeySetData::IsUsingDefinedKeySets()
{
    /* should never be the case that mDefinedKeySets.length == 0 */
    return (mDefinedKeySets != NULL && mDefinedKeySets->GetLength() > 0);
}

Boolean PackageKeySetData::IsUsingUpgradeKeySets()
{
    /* should never be the case that mUpgradeKeySets.length == 0 */
    return (mUpgradeKeySets != NULL && mUpgradeKeySets->GetLength() > 0);
}

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
