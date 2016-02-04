
#include "elastos/droid/server/pm/KeySetManagerService.h"
#include "elastos/droid/server/pm/PackageKeySetData.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Utility.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Utility::IBase64;
using Elastos::Droid::Utility::CBase64;
using Elastos::Droid::Utility::CArraySet;
using Elastos::Utility::Logging::Slogger;
using Elastos::Core::IInteger64;
using Elastos::Core::CInteger64;
using Elastos::Core::StringUtils;
using Elastos::Security::IKey;
using Elastos::Utility::ISet;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

const String KeySetManagerService::TAG("KeySetManagerService");
const Int32 KeySetManagerService::FIRST_VERSION;
const Int32 KeySetManagerService::CURRENT_VERSION;
const Int64 KeySetManagerService::KEYSET_NOT_FOUND;
const Int64 KeySetManagerService::PUBLIC_KEY_NOT_FOUND;

Int64 KeySetManagerService::sLastIssuedKeySetId = 0;
Int64 KeySetManagerService::sLastIssuedKeyId = 0;

KeySetManagerService::KeySetManagerService(
    /* [in] */ HashMap<String, AutoPtr<PackageSetting> >* packages)
    : mPackages(packages)
{}

ECode KeySetManagerService::PackageIsSignedByLPr(
    /* [in] */ const String& packageName,
    /* [in] */ CKeySetHandle* ks,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<PackageSetting> pkg;
    HashMap<String, AutoPtr<PackageSetting> >::Iterator it = mPackages->Find(packageName);
    if (it != mPackages->End()) {
        pkg = it->mSecond;
    }
    if (pkg == NULL) {
        Slogger::E(TAG, "Invalid package name");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (pkg->mKeySetData == NULL) {
        Slogger::E(TAG, "Package has no KeySet data");
        return E_NULL_POINTER_EXCEPTION;
    }
    Int64 id = GetIdByKeySetLPr(ks);
    if (id == KEYSET_NOT_FOUND) {
        return NOERROR;
    }
    *result = pkg->mKeySetData->PackageIsSignedBy(id);
    return NOERROR;
}

ECode KeySetManagerService::PackageIsSignedByExactlyLPr(
    /* [in] */ const String& packageName,
    /* [in] */ CKeySetHandle* ks,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<PackageSetting> pkg;
    HashMap<String, AutoPtr<PackageSetting> >::Iterator it = mPackages->Find(packageName);
    if (it != mPackages->End()) {
        pkg = it->mSecond;
    }
    if (pkg == NULL) {
        Slogger::E(TAG, "Invalid package name");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (pkg->mKeySetData == NULL
        || pkg->mKeySetData->GetProperSigningKeySet() == PackageKeySetData::KEYSET_UNASSIGNED) {
        Slogger::E(TAG, "Package has no KeySet data");
        return E_NULL_POINTER_EXCEPTION;
    }
    Int64 id = GetIdByKeySetLPr(ks);
    *result = pkg->mKeySetData->GetProperSigningKeySet() == id;
    return NOERROR;
}

ECode KeySetManagerService::AddDefinedKeySetToPackageLPw(
    /* [in] */ const String& packageName,
    /* [in] */ IArraySet* keys,
    /* [in] */ const String& alias)
{
    if ((packageName.IsNull()) || (keys == NULL) || (alias.IsNull())) {
        Slogger::W(TAG, "Got null argument for a defined keyset, ignoring!");
        return NOERROR;
    }
    AutoPtr<PackageSetting> pkg;
    HashMap<String, AutoPtr<PackageSetting> >::Iterator it = mPackages->Find(packageName);
    if (it != mPackages->End()) {
        pkg = it->mSecond;
    }
    if (pkg == NULL) {
        Slogger::E(TAG, "Unknown package");
        return E_NULL_POINTER_EXCEPTION;
    }
    // Add to KeySets, then to package
    AutoPtr<CKeySetHandle> ks;
    FAIL_RETURN(AddKeySetLPw(keys, (CKeySetHandle**)&ks))
    Int64 id = GetIdByKeySetLPr(ks);
    pkg->mKeySetData->AddDefinedKeySet(id, alias);
    return NOERROR;
}

ECode KeySetManagerService::AddUpgradeKeySetToPackageLPw(
    /* [in] */ const String& packageName,
    /* [in] */ const String& alias)
{
    if (packageName.IsNull() || alias.IsNull()) {
        Slogger::W(TAG, "Got null argument for a defined keyset, ignoring!");
        return NOERROR;
    }
    AutoPtr<PackageSetting> pkg;
    HashMap<String, AutoPtr<PackageSetting> >::Iterator it = mPackages->Find(packageName);
    if (it != mPackages->End()) {
        pkg = it->mSecond;
    }
    if (pkg == NULL) {
        Slogger::E(TAG, "Unknown package");
        return E_NULL_POINTER_EXCEPTION;
    }
    pkg->mKeySetData->AddUpgradeKeySet(alias);
    return NOERROR;
}

ECode KeySetManagerService::AddSigningKeySetToPackageLPw(
    /* [in] */ const String& packageName,
    /* [in] */ IArraySet* signingKeys)
{
    if (packageName.IsNull() || signingKeys == NULL) {
        Slogger::W(TAG, "Got null argument for a defined keyset, ignoring!");
        return NOERROR;
    }
    // add the signing KeySet
    AutoPtr<CKeySetHandle> ks;
    FAIL_RETURN(AddKeySetLPw(signingKeys, (CKeySetHandle**)&ks))
    Int64 id = GetIdByKeySetLPr(ks);
    AutoPtr<IArraySet> publicKeyIds;
    HashMap<Int64, AutoPtr<IArraySet> >::Iterator idIt = mKeySetMapping.Find(id);
    if (idIt != mKeySetMapping.End()) {
        publicKeyIds = idIt->mSecond;
    }
    if (publicKeyIds == NULL) {
        Slogger::E(TAG, "Got invalid KeySet id");
        return E_NULL_POINTER_EXCEPTION;
    }
    // attach it to the package
    AutoPtr<PackageSetting> pkg;
    HashMap<String, AutoPtr<PackageSetting> >::Iterator it = mPackages->Find(packageName);
    if (it != mPackages->End()) {
        pkg = it->mSecond;
    }
    if (pkg == NULL) {
        Slogger::E(TAG, "No such package!");
        return E_NULL_POINTER_EXCEPTION;
    }
    pkg->mKeySetData->SetProperSigningKeySet(id);
    // for each KeySet which is a subset of the one above, add the
    // KeySet id to the package's signing KeySets
    HashMap<Int64, AutoPtr<CKeySetHandle> >::Iterator setIt = mKeySets.Begin();
    for (; setIt != mKeySets.End(); ++setIt) {
        Int64 keySetID = setIt->mFirst;
        AutoPtr<IArraySet> definedKeys;
        idIt = mKeySetMapping.Find(keySetID);
        if (idIt != mKeySetMapping.End()) {
            definedKeys = idIt->mSecond;
        }
        Boolean result;
        if (ISet::Probe(publicKeyIds)->ContainsAll(ICollection::Probe(definedKeys), &result), result) {
            pkg->mKeySetData->AddSigningKeySet(keySetID);
        }
    }
    return NOERROR;
}

Int64 KeySetManagerService::GetIdByKeySetLPr(
    /* [in] */ CKeySetHandle* ks)
{
    HashMap<Int64, AutoPtr<CKeySetHandle> >::Iterator it = mKeySets.Begin();
    for (; it != mKeySets.End(); ++it) {
        AutoPtr<CKeySetHandle> value = it->mSecond;
        Boolean equals;
        if (((IObject*)ks->Probe(EIID_IObject))->Equals((IObject*)value, &equals), equals) {
            return it->mFirst;
        }
    }
    return KEYSET_NOT_FOUND;
}

AutoPtr<CKeySetHandle> KeySetManagerService::GetKeySetByIdLPr(
    /* [in] */ Int64 id)
{
    AutoPtr<CKeySetHandle> handle;
    HashMap<Int64, AutoPtr<CKeySetHandle> >::Iterator it = mKeySets.Find(id);
    if (it != mKeySets.End()) {
        handle = it->mSecond;
    }
    return handle;
}

ECode KeySetManagerService::GetKeySetByAliasAndPackageNameLPr(
    /* [in] */ const String& packageName,
    /* [in] */ const String& alias,
    /* [out] */ CKeySetHandle** handle)
{
    VALIDATE_NOT_NULL(handle)
    *handle = NULL;

    AutoPtr<PackageSetting> p;
    HashMap<String, AutoPtr<PackageSetting> >::Iterator it = mPackages->Find(packageName);
    if (it != mPackages->End()) {
        p = it->mSecond;
    }
    if (p == NULL || p->mKeySetData == NULL) {
        return NOERROR;
    }
    Int64 keySetId;
    HashMap<String, Int64>& aliases = p->mKeySetData->GetAliases();
    HashMap<String, Int64>::Iterator aliasIt = aliases.Find(alias);
    if (aliasIt != aliases.End()) {
        keySetId = aliasIt->mSecond;
    }
    else {
        Slogger::E(TAG, "Unknown KeySet alias: %s", alias.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    HashMap<Int64, AutoPtr<CKeySetHandle> >::Iterator handleIt = mKeySets.Find(keySetId);
    if (handleIt != mKeySets.End()) {
        *handle = handleIt->mSecond;
    }
    REFCOUNT_ADD(*handle)
    return NOERROR;
}

AutoPtr<IArraySet> KeySetManagerService::GetPublicKeysFromKeySetLPr(
    /* [in] */ Int64 id)
{
    AutoPtr<IArraySet> sets;
    HashMap<Int64, AutoPtr<IArraySet> >::Iterator it = mKeySetMapping.Find(id);
    if (it != mKeySetMapping.End()) {
        sets = it->mSecond;
    }
    if(sets == NULL) {
        return NULL;
    }
    AutoPtr<IArraySet> mPubKeys;
    CArraySet::New((IArraySet**)&mPubKeys);
    AutoPtr<IIterator> setIt;
    ISet::Probe(sets)->GetIterator((IIterator**)&setIt);
    Boolean hasNext;
    while (setIt->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        setIt->GetNext((IInterface**)&value);
        AutoPtr<IInteger64> pkId = IInteger64::Probe(value);
        Int64 v;
        pkId->GetValue(&v);
        AutoPtr<IPublicKey> pk;
        HashMap<Int64, AutoPtr<IPublicKey> >::Iterator pkIt = mPublicKeys.Find(v);
        if (pkIt != mPublicKeys.End()) {
            pk = pkIt->mSecond;
        }
        ISet::Probe(mPubKeys)->Add(pk);
    }
    return mPubKeys;
}

AutoPtr<CKeySetHandle> KeySetManagerService::GetSigningKeySetByPackageNameLPr(
    /* [in] */ const String& packageName)
{
    AutoPtr<PackageSetting> p;
    HashMap<String, AutoPtr<PackageSetting> >::Iterator it = mPackages->Find(packageName);
    if (it != mPackages->End()) {
        p = it->mSecond;
    }
    if (p == NULL
        || p->mKeySetData == NULL
        || p->mKeySetData->GetProperSigningKeySet() == PackageKeySetData::KEYSET_UNASSIGNED) {
        return NULL;
    }
    AutoPtr<CKeySetHandle> handle;
    HashMap<Int64, AutoPtr<CKeySetHandle> >::Iterator setit = mKeySets.Find(p->mKeySetData->GetProperSigningKeySet());
    if (setit != mKeySets.End()) {
        handle = setit->mSecond;
    }
    return handle;
}

ECode KeySetManagerService::GetUpgradeKeySetsByPackageNameLPr(
    /* [in] */ const String& packageName,
    /* [out] */ IArraySet** handles)
{
    VALIDATE_NOT_NULL(handles)
    *handles = NULL;

    AutoPtr<IArraySet> upgradeKeySets;
    CArraySet::New((IArraySet**)&upgradeKeySets);
    AutoPtr<PackageSetting> p;
    HashMap<String, AutoPtr<PackageSetting> >::Iterator it = mPackages->Find(packageName);
    if (it != mPackages->End()) {
        p = it->mSecond;
    }
    if (p == NULL) {
        Slogger::E(TAG, "Unknown package");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (p->mKeySetData == NULL) {
        Slogger::E(TAG, "Package has no keySet data");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (p->mKeySetData->IsUsingUpgradeKeySets()) {
        AutoPtr<ArrayOf<Int64> > sets = p->mKeySetData->GetUpgradeKeySets();
        for (Int32 i = 0; i < sets->GetLength(); ++i) {
            AutoPtr<CKeySetHandle> handle;
            HashMap<Int64, AutoPtr<CKeySetHandle> >::Iterator handleIt = mKeySets.Find((*sets)[i]);
            if (handleIt != mKeySets.End()) {
                handle = handleIt->mSecond;
            }
            ISet::Probe(upgradeKeySets)->Add(handle->Probe(EIID_IInterface));
        }
    }
    *handles = upgradeKeySets;
    REFCOUNT_ADD(*handles)
    return NOERROR;
}

ECode KeySetManagerService::AddKeySetLPw(
    /* [in] */ IArraySet* keys,
    /* [out] */ CKeySetHandle** handle)
{
    VALIDATE_NOT_NULL(handle)
    *handle = NULL;

    if (keys == NULL) {
        Slogger::E(TAG, "Provided keys cannot be null");
        return E_NULL_POINTER_EXCEPTION;
    }
    // add each of the keys in the provided set
    Int32 size;
    ISet::Probe(keys)->GetSize(&size);
    AutoPtr<IArraySet> addedKeyIds;
    CArraySet::New(size, (IArraySet**)&addedKeyIds);
    AutoPtr<IIterator> it;
    ISet::Probe(keys)->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        it->GetNext((IInterface**)&value);
        AutoPtr<IPublicKey> k = IPublicKey::Probe(value);
        Int64 id = AddPublicKeyLPw(k);
        AutoPtr<IInteger64> idint;
        CInteger64::New(id, (IInteger64**)&idint);
        ISet::Probe(addedKeyIds)->Add(idint);
    }

    // check to see if the resulting keyset is new
    Int64 existingKeySetId = GetIdFromKeyIdsLPr(addedKeyIds);
    if (existingKeySetId != KEYSET_NOT_FOUND) {
        HashMap<Int64, AutoPtr<CKeySetHandle> >::Iterator handleIt = mKeySets.Find(existingKeySetId);
        if (handleIt != mKeySets.End()) {
            *handle = handleIt->mSecond;
        }
        return NOERROR;
    }

    // create the KeySet object
    AutoPtr<CKeySetHandle> ks;
    CKeySetHandle::NewByFriend((CKeySetHandle**)&ks);
    // get the first unoccupied slot in mKeySets
    Int64 id = GetFreeKeySetIDLPw();
    // add the KeySet object to it
    mKeySets[id] = ks;
    // add the stable key ids to the mapping
    mKeySetMapping[id] = addedKeyIds;
    // add this KeySet id to all packages which are signed by it
    HashMap<String, AutoPtr<PackageSetting> >::Iterator pkgIt = mPackages->Begin();
    for (; pkgIt != mPackages->End(); ++pkgIt) {
        AutoPtr<PackageSetting> p = pkgIt->mSecond;
        if (p->mKeySetData != NULL) {
            Int64 pProperSigning = p->mKeySetData->GetProperSigningKeySet();
            if (pProperSigning != PackageKeySetData::KEYSET_UNASSIGNED) {
                AutoPtr<IArraySet> pSigningKeys;
                HashMap<Int64, AutoPtr<IArraySet> >::Iterator keysIt = mKeySetMapping.Find(pProperSigning);
                if (keysIt != mKeySetMapping.End()) {
                    pSigningKeys = keysIt->mSecond;
                }
                Boolean result;
                if (ISet::Probe(pSigningKeys)->ContainsAll(ICollection::Probe(addedKeyIds), &result), result) {
                    p->mKeySetData->AddSigningKeySet(id);
                }
            }
        }
    }
    // go home
    *handle = ks;
    REFCOUNT_ADD(*handle)
    return NOERROR;
}

Int64 KeySetManagerService::AddPublicKeyLPw(
    /* [in] */ IPublicKey* key)
{
    // check if the public key is new
    Int64 existingKeyId = GetIdForPublicKeyLPr(key);
    if (existingKeyId != PUBLIC_KEY_NOT_FOUND) {
        return existingKeyId;
    }
    // if it's new find the first unoccupied slot in the public keys
    Int64 id = GetFreePublicKeyIdLPw();
    // add the public key to it
    mPublicKeys[id] = key;
    // return the stable identifier
    return id;
}

Int64 KeySetManagerService::GetIdFromKeyIdsLPr(
    /* [in] */ IArraySet* publicKeyIds)
{
    HashMap<Int64, AutoPtr<IArraySet> >::Iterator it = mKeySetMapping.Begin();
    for (; it != mKeySetMapping.End(); ++it) {
        AutoPtr<IArraySet> value = it->mSecond;
        Boolean equals;
        if (IObject::Probe(value)->Equals(publicKeyIds, &equals), equals) {
            return it->mFirst;
        }
    }
    return KEYSET_NOT_FOUND;
}

Int64 KeySetManagerService::GetIdForPublicKeyLPr(
    /* [in] */ IPublicKey* k)
{
    AutoPtr<ArrayOf<Byte> > encoded;
    IKey::Probe(k)->GetEncoded((ArrayOf<Byte>**)&encoded);
    String encodedPublicKey(*encoded);
    HashMap<Int64, AutoPtr<IPublicKey> >::Iterator it = mPublicKeys.Begin();
    for (; it != mPublicKeys.End(); ++it) {
        AutoPtr<IPublicKey> value = it->mSecond;
        encoded = NULL;
        IKey::Probe(k)->GetEncoded((ArrayOf<Byte>**)&encoded);
        String encodedExistingKey(*encoded);
        if (encodedPublicKey.Equals(encodedExistingKey)) {
            return it->mFirst;
        }
    }
    return PUBLIC_KEY_NOT_FOUND;
}

Int64 KeySetManagerService::GetFreeKeySetIDLPw()
{
    sLastIssuedKeySetId += 1;
    return sLastIssuedKeySetId;
}

Int64 KeySetManagerService::GetFreePublicKeyIdLPw()
{
    sLastIssuedKeyId += 1;
    return sLastIssuedKeyId;
}

ECode KeySetManagerService::RemoveAppKeySetDataLPw(
    /* [in] */ const String& packageName)
{
    AutoPtr<IArraySet> deletableKeySets;
    FAIL_RETURN(GetOriginalKeySetsByPackageNameLPr(packageName, (IArraySet**)&deletableKeySets))
    AutoPtr<IArraySet> deletableKeys;
    CArraySet::New((IArraySet**)&deletableKeys);
    AutoPtr<IArraySet> knownKeys;
    AutoPtr<IIterator> it;
    ISet::Probe(deletableKeySets)->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        it->GetNext((IInterface**)&value);
        Int64 ks;
        IInteger64::Probe(value)->GetValue(&ks);
        knownKeys = NULL;
        HashMap<Int64, AutoPtr<IArraySet> >::Iterator setIt = mKeySetMapping.Find(ks);
        if (setIt != mKeySetMapping.End()) {
            knownKeys = setIt->mSecond;
        }
        if (knownKeys != NULL) {
            deletableKeys->AddAll(knownKeys);
        }
    }

    // Now remove the keys and KeySets on which any other package relies
    HashMap<String, AutoPtr<PackageSetting> >::Iterator pkgIt = mPackages->Begin();
    for (; pkgIt != mPackages->End(); ++pkgIt) {
        String pkgName = pkgIt->mFirst;
        if (pkgName.Equals(packageName)) {
            continue;
        }
        AutoPtr<IArraySet> knownKeySets;
        FAIL_RETURN(GetOriginalKeySetsByPackageNameLPr(pkgName, (IArraySet**)&knownKeySets))
        ISet::Probe(deletableKeySets)->RemoveAll(ICollection::Probe(knownKeySets));
        knownKeys = NULL;
        CArraySet::New((IArraySet**)&knownKeys);
        it = NULL;
        ISet::Probe(knownKeySets)->GetIterator((IIterator**)&it);
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> value;
            it->GetNext((IInterface**)&value);
            Int64 ks;
            IInteger64::Probe(value)->GetValue(&ks);
            HashMap<Int64, AutoPtr<IArraySet> >::Iterator setIt = mKeySetMapping.Find(ks);
            knownKeys = NULL;
            if (setIt != mKeySetMapping.End()) {
                knownKeys = setIt->mSecond;
            }
            if (knownKeys != NULL) {
                ISet::Probe(deletableKeys)->RemoveAll(ICollection::Probe(knownKeys));
            }
        }
    }

    // The remaining keys and KeySets are not relied on by any other
    // application and so can be safely deleted.
    it = NULL;
    ISet::Probe(deletableKeySets)->GetIterator((IIterator**)&it);
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        it->GetNext((IInterface**)&value);
        Int64 ks;
        IInteger64::Probe(value)->GetValue(&ks);
        mKeySets.Erase(ks);
        mKeySetMapping.Erase(ks);
    }
    it = NULL;
    ISet::Probe(deletableKeys)->GetIterator((IIterator**)&it);
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        it->GetNext((IInterface**)&value);
        Int64 keyId;
        IInteger64::Probe(value)->GetValue(&keyId);
        mPublicKeys.Erase(keyId);
    }

    // Now remove the deleted KeySets from each package's signingKeySets
    pkgIt = mPackages->Begin();
    for (; pkgIt != mPackages->End(); ++pkgIt) {
        AutoPtr<PackageSetting> p = pkgIt->mSecond;
        it = NULL;
        ISet::Probe(deletableKeySets)->GetIterator((IIterator**)&it);
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> value;
            it->GetNext((IInterface**)&value);
            Int64 ks;
            IInteger64::Probe(value)->GetValue(&ks);
            p->mKeySetData->RemoveSigningKeySet(ks);
        }
    }
    // Finally, remove all KeySets from the original package
    AutoPtr<PackageSetting> p;
    pkgIt = mPackages->Find(packageName);
    if (pkgIt != mPackages->End()) {
        p = pkgIt->mSecond;
    }
    ClearPackageKeySetDataLPw(p);
    return NOERROR;
}

void KeySetManagerService::ClearPackageKeySetDataLPw(
    /* [in] */ PackageSetting* p)
{
    p->mKeySetData->RemoveAllSigningKeySets();
    p->mKeySetData->RemoveAllUpgradeKeySets();
    p->mKeySetData->RemoveAllDefinedKeySets();
    return;
}

ECode KeySetManagerService::GetOriginalKeySetsByPackageNameLPr(
    /* [in] */ const String& packageName,
    /* [out] */ IArraySet** sets)
{
    VALIDATE_NOT_NULL(sets)
    *sets = NULL;

    AutoPtr<PackageSetting> p;
    HashMap<String, AutoPtr<PackageSetting> >::Iterator it = mPackages->Find(packageName);
    if (it != mPackages->End()) {
        p = it->mSecond;
    }
    if (p == NULL) {
        Slogger::E(TAG, "Unknown package");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (p->mKeySetData == NULL) {
        Slogger::E(TAG, "Package has no keySet data");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IArraySet> knownKeySets;
    CArraySet::New((IArraySet**)&knownKeySets);
    AutoPtr<IInteger64> i;
    CInteger64::New(p->mKeySetData->GetProperSigningKeySet(), (IInteger64**)&i);
    ISet::Probe(knownKeySets)->Add(i);
    if (p->mKeySetData->IsUsingDefinedKeySets()) {
        AutoPtr<ArrayOf<Int64> > keysets = p->mKeySetData->GetDefinedKeySets();
        for (Int32 i = 0; i < keysets->GetLength(); ++i) {
            AutoPtr<IInteger64> in;
            CInteger64::New((*keysets)[i], (IInteger64**)&in);
            ISet::Probe(knownKeySets)->Add(in);
        }
    }
    *sets = knownKeySets;
    REFCOUNT_ADD(*sets)
    return NOERROR;
}

ECode KeySetManagerService::EncodePublicKey(
    /* [in] */ IPublicKey* k,
    /* [out] */ String* key)
{
    VALIDATE_NOT_NULL(key)
    AutoPtr<ArrayOf<Byte> > encoded;
    IKey::Probe(k)->GetEncoded((ArrayOf<Byte>**)&encoded);
    AutoPtr<IBase64> base64;
    CBase64::AcquireSingleton((IBase64**)&base64);
    AutoPtr<ArrayOf<Byte> > encoded1;
    base64->Encode(encoded, 0, (ArrayOf<Byte>**)&encoded1);
    *key = String(*encoded1);
    return NOERROR;
}

ECode KeySetManagerService::WriteKeySetManagerServiceLPr(
    /* [in] */ IXmlSerializer* serializer)
{
    FAIL_RETURN(serializer->WriteStartTag(String(NULL), String("keyset-settings")))
    FAIL_RETURN(serializer->WriteAttribute(String(NULL), String("version"), StringUtils::ToString(CURRENT_VERSION)))
    FAIL_RETURN(WritePublicKeysLPr(serializer))
    FAIL_RETURN(WriteKeySetsLPr(serializer))
    FAIL_RETURN(serializer->WriteStartTag(String(NULL), String("lastIssuedKeyId")))
    FAIL_RETURN(serializer->WriteAttribute(String(NULL), String("value"), StringUtils::ToString(sLastIssuedKeyId)))
    FAIL_RETURN(serializer->WriteEndTag(String(NULL), String("lastIssuedKeyId")))
    FAIL_RETURN(serializer->WriteStartTag(String(NULL), String("lastIssuedKeySetId")))
    FAIL_RETURN(serializer->WriteAttribute(String(NULL), String("value"), StringUtils::ToString(sLastIssuedKeySetId)))
    FAIL_RETURN(serializer->WriteEndTag(String(NULL), String("lastIssuedKeySetId")))
    return serializer->WriteEndTag(String(NULL), String("keyset-settings"));
}

ECode KeySetManagerService::WritePublicKeysLPr(
    /* [in] */ IXmlSerializer* serializer)
{
    FAIL_RETURN(serializer->WriteStartTag(String(NULL), String("keys")))
    HashMap<Int64, AutoPtr<IPublicKey> >::Iterator it = mPublicKeys.Begin();
    for (; it != mPublicKeys.End(); ++it) {
        AutoPtr<IPublicKey> key = it->mSecond;
        String encodedKey;
        FAIL_RETURN(EncodePublicKey(key, &encodedKey))
        FAIL_RETURN(serializer->WriteStartTag(String(NULL), String("public-key")))
        FAIL_RETURN(serializer->WriteAttribute(String(NULL), String("identifier"), StringUtils::ToString(it->mFirst)))
        FAIL_RETURN(serializer->WriteAttribute(String(NULL), String("value"), encodedKey))
        FAIL_RETURN(serializer->WriteEndTag(String(NULL), String("public-key")))
    }
    return serializer->WriteEndTag(String(NULL), String("keys"));
}

ECode KeySetManagerService::WriteKeySetsLPr(
    /* [in] */ IXmlSerializer* serializer)
{
    FAIL_RETURN(serializer->WriteStartTag(String(NULL), String("keysets")))
    HashMap<Int64, AutoPtr<IArraySet> >::Iterator it = mKeySetMapping.Begin();
    for (; it != mKeySetMapping.End(); ++it) {
        AutoPtr<IArraySet> keys = it->mSecond;
        FAIL_RETURN(serializer->WriteStartTag(String(NULL), String("keyset")))
        FAIL_RETURN(serializer->WriteAttribute(String(NULL), String("identifier"), StringUtils::ToString(it->mFirst)))
        AutoPtr<IIterator> iter;
        ISet::Probe(keys)->GetIterator((IIterator**)&iter);
        Boolean hasNext;
        while (iter->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> value;
            iter->GetNext((IInterface**)&value);
            Int64 keyId;
            IInteger64::Probe(value)->GetValue(&keyId);
            FAIL_RETURN(serializer->WriteStartTag(String(NULL), String("key-id")))
            FAIL_RETURN(serializer->WriteAttribute(String(NULL), String("identifier"), StringUtils::ToString(keyId)))
            FAIL_RETURN(serializer->WriteEndTag(String(NULL), String("key-id")))
        }
        FAIL_RETURN(serializer->WriteEndTag(String(NULL), String("keyset")))
    }
    return serializer->WriteEndTag(String(NULL), String("keysets"));
}

ECode KeySetManagerService::ReadKeySetsLPw(
    /* [in] */ IXmlPullParser* parser)
{
    Int32 type;
    Int32 outerDepth, depth;
    parser->GetDepth(&outerDepth);
    String recordedVersion;
    FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("version"), &recordedVersion))
    if (recordedVersion.IsNull() || StringUtils::ParseInt32(recordedVersion) != CURRENT_VERSION) {
        while ((parser->Next(&type), type != IXmlPullParser::END_DOCUMENT)
                && (type != IXmlPullParser::END_TAG || (parser->GetDepth(&depth), depth > outerDepth))) {
            // Our version is different than the one which generated the old keyset data.
            // We don't want any of the old data, but we must advance the parser
            continue;
        }
        // The KeySet information read previously from packages.xml is invalid.
        // Destroy it all.
        HashMap<String, AutoPtr<PackageSetting> >::Iterator pkgIt = mPackages->Begin();
        for (; pkgIt != mPackages->End(); ++pkgIt) {
            ClearPackageKeySetDataLPw(pkgIt->mSecond);
        }
        return NOERROR;
    }
    while ((parser->Next(&type), type != IXmlPullParser::END_DOCUMENT)
           && (type != IXmlPullParser::END_TAG || (parser->GetDepth(&depth), depth > outerDepth))) {
        if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
            continue;
        }
        String tagName;
        parser->GetName(&tagName);
        if (tagName.Equals("keys")) {
            FAIL_RETURN(ReadKeysLPw(parser))
        }
        else if (tagName.Equals("keysets")) {
            FAIL_RETURN(ReadKeySetListLPw(parser))
        }
        else if (tagName.Equals("lastIssuedKeyId")) {
            String value;
            FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("value"), &value))
            sLastIssuedKeyId = StringUtils::ParseInt64(value);
        }
        else if (tagName.Equals("lastIssuedKeySetId")) {
            String value;
            FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("value"), &value))
            sLastIssuedKeySetId = StringUtils::ParseInt64(value);
        }
    }
    return NOERROR;
}

ECode KeySetManagerService::ReadKeysLPw(
    /* [in] */ IXmlPullParser* parser)
{
    Int32 outerDepth, depth;
    parser->GetDepth(&outerDepth);
    Int32 type;
    while ((parser->Next(&type), type != IXmlPullParser::END_DOCUMENT)
            && (type != IXmlPullParser::END_TAG || (parser->GetDepth(&depth), depth > outerDepth))) {
        if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
            continue;
        }
        String tagName;
        parser->GetName(&tagName);
        if (tagName.Equals("public-key")) {
            FAIL_RETURN(ReadPublicKeyLPw(parser))
        }
    }
    return NOERROR;
}

ECode KeySetManagerService::ReadKeySetListLPw(
    /* [in] */ IXmlPullParser* parser)
{
    Int32 outerDepth, depth;
    parser->GetDepth(&outerDepth);
    Int32 type;
    Int64 currentKeySetId = 0;
    while ((parser->Next(&type), type != IXmlPullParser::END_DOCUMENT)
            && (type != IXmlPullParser::END_TAG || (parser->GetDepth(&depth), depth > outerDepth))) {
        if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
            continue;
        }
        String tagName;
        parser->GetName(&tagName);
        if (tagName.Equals("keyset")) {
            FAIL_RETURN(ReadIdentifierLPw(parser, &currentKeySetId))
            AutoPtr<CKeySetHandle> handle;
            CKeySetHandle::NewByFriend((CKeySetHandle**)&handle);
            mKeySets[currentKeySetId] = handle;
            AutoPtr<IArraySet> set;
            CArraySet::New((IArraySet**)&set);
            mKeySetMapping[currentKeySetId] = set;
        }
        else if (tagName.Equals("key-id")) {
            Int64 id;
            FAIL_RETURN(ReadIdentifierLPw(parser, &id))
            HashMap<Int64, AutoPtr<IArraySet> >::Iterator it = mKeySetMapping.Find(currentKeySetId);
            if (it != mKeySetMapping.End()) {
                AutoPtr<IInteger64> in;
                CInteger64::New(id, (IInteger64**)&in);
                ISet::Probe(it->mSecond)->Add(in);
            }
        }
    }
    return NOERROR;
}

ECode KeySetManagerService::ReadIdentifierLPw(
    /* [in] */ IXmlPullParser* parser,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)
    value = 0;
    String v;
    FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("identifier"), &v))
    *value = StringUtils::ParseInt64(v);
    return NOERROR;
}

ECode KeySetManagerService::ReadPublicKeyLPw(
    /* [in] */ IXmlPullParser* parser)
{
    String encodedID;
    FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("identifier"), &encodedID))
    Int64 identifier = StringUtils::ParseInt64(encodedID);
    String encodedPublicKey;
    FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("value"), &encodedPublicKey))
    AutoPtr<IPublicKey> pub;
    FAIL_RETURN(PackageParser::ParsePublicKey(encodedPublicKey, (IPublicKey**)&pub))
    if (pub != NULL) {
        mPublicKeys[identifier] = pub;
    }
    return NOERROR;
}

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
