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
