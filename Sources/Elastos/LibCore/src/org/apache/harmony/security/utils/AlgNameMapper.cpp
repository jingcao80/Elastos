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

#include "AlgNameMapper.h"
#include "CObjectIdentifier.h"
#include "AutoLock.h"
#include "CSecurity.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "org/apache/harmony/security/fortress/Services.h"

using Elastos::Core::AutoLock;
using Elastos::Core::ICharSequence;
using Elastos::Security::CSecurity;
using Elastos::Security::ISecurity;
using Elastos::Utility::IHashTable;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMapEntry;
using Org::Apache::Harmony::Security::Asn1::CObjectIdentifier;
using Org::Apache::Harmony::Security::Fortress::Services;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Utils {

AutoPtr<IAlgNameMapperSource> AlgNameMapper::sSource;

volatile Int32 AlgNameMapper::sCacheVersion = -1;

// Will search OID mappings for these services
const String AlgNameMapper::sServiceName[] = {
        String("Cipher"),
        String("AlgorithmParameters"),
        String("Signature")
};

// These mappings CAN NOT be overridden
// by the ones from available providers
// during maps initialization
// (source: http://asn1.elibel.tm.fr):
String AlgNameMapper::sKnownAlgMappings[19][2] = {
    {String("1.2.840.10040.4.1"),       String("DSA")},
    {String("1.2.840.10040.4.3"),       String("SHA1withDSA")},
    {String("1.2.840.113549.1.1.1"),    String("RSA")},
    // BEGIN android-removed
    // Dropping MD2
    // {"1.2.840.113549.1.1.2",    "MD2withRSA"},
    // END android-removed
    {String("1.2.840.113549.1.1.4"),    String("MD5withRSA")},
    {String("1.2.840.113549.1.1.5"),    String("SHA1withRSA")},
    {String("1.2.840.113549.1.3.1"),    String("DiffieHellman")},
    {String("1.2.840.113549.1.5.3"),    String("pbeWithMD5AndDES-CBC")},
    {String("1.2.840.113549.1.12.1.3"), String("pbeWithSHAAnd3-KeyTripleDES-CBC")},
    {String("1.2.840.113549.1.12.1.6"), String("pbeWithSHAAnd40BitRC2-CBC")},
    {String("1.2.840.113549.3.2"),      String("RC2-CBC")},
    {String("1.2.840.113549.3.3"),      String("RC2-EBC")},
    {String("1.2.840.113549.3.4"),      String("RC4")},
    {String("1.2.840.113549.3.5"),      String("RC4WithMAC")},
    {String("1.2.840.113549.3.6"),      String("DESx-CBC")},
    {String("1.2.840.113549.3.7"),      String("TripleDES-CBC")},
    {String("1.2.840.113549.3.8"),      String("rc5CBC")},
    {String("1.2.840.113549.3.9"),      String("RC5-CBC")},
    {String("1.2.840.113549.3.10"),     String("DESCDMF")},
    {String("2.23.42.9.11.4.1"),        String("ECDSA")},
};

// Maps alg name to OID
HashMap<String, String> AlgNameMapper::sAlg2OidMap;
// Maps OID to alg name
HashMap<String, String> AlgNameMapper::sOid2AlgMap;
// Maps aliases to alg names
HashMap<String, String> AlgNameMapper::sAlgAliasesMap;
Object AlgNameMapper::sLock;
Boolean AlgNameMapper::sInit = InitStatic();

Boolean AlgNameMapper::InitStatic()
{
    for (Int32 i = 0; i < 19 /*sKnownAlgMappings.length*/; i++) {
        String element[2] = { sKnownAlgMappings[i][0], sKnownAlgMappings[i][1] };
        String algUC = element[1].ToUpperCase(/*Locale.US*/);
        sAlg2OidMap[algUC] = element[0];
        sOid2AlgMap[element[0]] = algUC;
        // map upper case alg name to its original name
        sAlgAliasesMap[algUC] = element[1];
    }
    return TRUE;
}

AlgNameMapper::AlgNameMapper()
{}

void AlgNameMapper::CheckCacheVersion()
{
    AutoLock lock(sLock);
    Int32 newCacheVersion = 0;
    Services::GetCacheVersion(&newCacheVersion);
    if (newCacheVersion != sCacheVersion) {
        //
        // Now search providers for mappings like
        // Alg.Alias.<service>.<OID-INTS-DOT-SEPARATED>=<alg-name>
        //  or
        // Alg.Alias.<service>.OID.<OID-INTS-DOT-SEPARATED>=<alg-name>
        //
        AutoPtr<ISecurity> security;
        CSecurity::AcquireSingleton((ISecurity**)&security);
        AutoPtr<ArrayOf<IProvider*> > pl;
        security->GetProviders((ArrayOf<IProvider*>**)&pl);
        for (Int32 i = 0; i < pl->GetLength(); i++) {
            AutoPtr<IProvider> element = (*pl)[i];
            SelectEntries(element);
        }
        sCacheVersion = newCacheVersion;
    }
}

String AlgNameMapper::Map2OID(
    /* [in] */ const String& algName)
{
    CheckCacheVersion();

    // alg2OidMap map contains upper case keys
    String result = sAlg2OidMap[algName.ToUpperCase(/*Locale.US*/)];
    if (result != NULL) {
        return result;
    }

    // Check our external source.
    AutoPtr<IAlgNameMapperSource> s = sSource;
    if (s != NULL) {
        String r;
        s->MapNameToOid(algName, &r);
        return r;
    }

    return String(NULL);
}

String AlgNameMapper::Map2AlgName(
    /* [in] */ const String& oid)
{
    CheckCacheVersion();

    // oid2AlgMap map contains upper case values
    String algUC = sOid2AlgMap[oid];
    // if not NULL there is always map UC->Orig
    if (algUC != NULL) {
        return sAlgAliasesMap[algUC];
    }

    // Check our external source.
    AutoPtr<IAlgNameMapperSource> s = sSource;
    if (s != NULL) {
        String r;
        s->MapOidToName(oid, &r);
        return r;
    }

    return String(NULL);
}

String AlgNameMapper::GetStandardName(
    /* [in] */ const String& algName)
{
    return sAlgAliasesMap[algName.ToUpperCase(/*Locale.US*/)];
}

void AlgNameMapper::SelectEntries(
    /* [in] */ IProvider* p)
{
    AutoPtr<ISet> entrySet;
    IHashTable::Probe(p)->GetEntrySet((ISet**)&entrySet);
    for (Int32 i = 0; i < 3 /*sServiceName.length*/; i++) {
        String service = sServiceName[i];
        String keyPrfix2find = String("Alg.Alias.") + service + ".";

        AutoPtr<IIterator> it;
        entrySet->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IMapEntry> me = IMapEntry::Probe(obj);

            AutoPtr<IInterface> keyObj;
            me->GetKey((IInterface**)&keyObj);
            String key;
            ICharSequence::Probe(keyObj)->ToString(&key);
            if (key.StartWith(keyPrfix2find)) {
                String alias = key.Substring(keyPrfix2find.GetLength());
                AutoPtr<IInterface> value;
                me->GetValue((IInterface**)&value);
                String alg;
                ICharSequence::Probe(value)->ToString(&alg);
                String algUC = alg.ToUpperCase(/*Locale.US*/);
                if (IsOID(alias)) {
                    if (alias.StartWith("OID.")) {
                        alias = alias.Substring(4);
                    }
                    // Do not overwrite already known mappings
                    Boolean oid2AlgContains = FALSE;
                    HashMap<String, String>::Iterator oldIt = sOid2AlgMap.Find(alias);
                    if (oldIt != sOid2AlgMap.End()) {
                        oid2AlgContains = TRUE;
                    }

                    Boolean alg2OidContains = FALSE;
                    HashMap<String, String>::Iterator algIt = sAlg2OidMap.Find(algUC);
                    if (algIt != sAlg2OidMap.End()) {
                        alg2OidContains = TRUE;
                    }

                    if (!oid2AlgContains || !alg2OidContains) {
                        if (!oid2AlgContains) {
                            sOid2AlgMap[alias] = algUC;
                        }
                        if (!alg2OidContains) {
                            sAlg2OidMap[algUC] = alias;
                        }
                        // map upper case alg name to its original name
                        sAlgAliasesMap[algUC] = alg;
                    }
                       // Do not override known standard names
                }
                else {
                    Boolean tmp = FALSE;
                    HashMap<String, String>::Iterator it = sAlgAliasesMap.Find(alias.ToUpperCase(/*Locale.US*/));
                    if (it != sAlgAliasesMap.End()) {
                        tmp = TRUE;
                    }
                    if (!tmp) {
                        sAlgAliasesMap[alias.ToUpperCase(/*Locale.US*/)] = alg;
                    }
                }
            }
        }
    }
}

Boolean AlgNameMapper::IsOID(
    /* [in] */ const String& alias)
{
    Boolean v = FALSE;
    CObjectIdentifier::IsOID(Normalize(alias), &v);
    return v;
}

String AlgNameMapper::Normalize(
    /* [in] */ const String& oid)
{
    return oid.StartWith("OID.")
        ? oid.Substring(4)
        : oid;
}

void AlgNameMapper::SetSource(
    /* [in] */ IAlgNameMapperSource* source)
{
    sSource = source;
}

}
}
}
}
}
