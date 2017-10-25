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
    Int64 ks = 0;
    HashMap<String, Int64>::Iterator it = mKeySetAliases.Find(alias);
    if (it != mKeySetAliases.End()) {
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
    for (Int32 i = 0; i < mSigningKeySets->GetLength(); ++i) {
        if ((*mSigningKeySets)[i] == ks) return TRUE;
    }
    return FALSE;
}

AutoPtr<ArrayOf<Int64> > PackageKeySetData::GetSigningKeySets()
{
    return mSigningKeySets;
}

AutoPtr<ArrayOf<Int64> > PackageKeySetData::GetUpgradeKeySets()
{
    return mUpgradeKeySets;
}

AutoPtr<ArrayOf<Int64> > PackageKeySetData::GetDefinedKeySets()
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
