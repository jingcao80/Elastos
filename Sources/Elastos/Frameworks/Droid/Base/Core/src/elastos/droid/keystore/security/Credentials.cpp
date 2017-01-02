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

#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.CoreLibrary.Security.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/keystore/security/Credentials.h"
#include "elastos/droid/keystore/security/KeyChain.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntent;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::CInputStreamReader;
using Elastos::IO::COutputStreamWriter;
using Elastos::IO::IOutputStream;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStreamWriter;
using Elastos::IO::IReader;
using Elastos::IO::IWriter;
using Elastos::IO::Charset::IStandardCharsets;
using Elastos::IO::Charset::CStandardCharsets;
using Elastos::IO::Charset::ICharset;
using Elastos::Security::IKey;
using Elastos::Security::IPublicKey;
using Elastos::Security::Cert::ICertificateFactory;
using Elastos::Security::Cert::ICertificateFactoryHelper;
using Elastos::Security::Cert::CCertificateFactoryHelper;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

CAR_INTERFACE_IMPL(Credentials, Object, ICredentials);

const String Credentials::LOGTAG("Credentials");
AutoPtr<ICredentials> Credentials::singleton;

ECode Credentials::constructor()
{
    return NOERROR;
}

ECode Credentials::ConvertToPem(
    /* [in] */ ArrayOf<ICertificate*>* objects,
    /* [out] */ ArrayOf<Byte>** result)
{
    AutoPtr<IByteArrayOutputStream> bao;
    CByteArrayOutputStream::New((IByteArrayOutputStream**)&bao);

    AutoPtr<IStandardCharsets> scHelper;
    CStandardCharsets::AcquireSingleton((IStandardCharsets**)&scHelper);
    AutoPtr<ICharset> us_ascii;
    scHelper->GetUS_ASCII((ICharset**)&us_ascii);
    AutoPtr<IWriter> writer;
    COutputStreamWriter::New(IOutputStream::Probe(bao), /*TODO us_ascii,*/ (IWriter**)&writer);//TODO elastos/io.car class COutputStreamWriter
    //TODO AutoPtr<IPemWriter> pw;
    //TODO CPemWriter::New(writer, (IPemWriter**)&pw);
    Int32 len = objects->GetLength();
    for (Int32 i = 0; i < len; ++i) {
        ICertificate* o = (*objects)[i];
        assert(o);//TODO remove
        //TODO pw->WriteObject(new PemObject("CERTIFICATE", o.getEncoded()));
    }
    //TODO pw->Close();
    return bao->ToByteArray(result);
}
/**
 * Convert objects from PEM format, which is used for
 * CA_CERTIFICATE and USER_CERTIFICATE entries.
 */
ECode Credentials::ConvertFromPem(
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<IByteArrayInputStream> bai;
    CByteArrayInputStream::New(bytes, (IByteArrayInputStream**)&bai);

    AutoPtr<IStandardCharsets> scHelper;
    CStandardCharsets::AcquireSingleton((IStandardCharsets**)&scHelper);
    AutoPtr<ICharset> us_ascii;
    scHelper->GetUS_ASCII((ICharset**)&us_ascii);
    AutoPtr<IReader> reader;
    CInputStreamReader::New(IInputStream::Probe(bai), us_ascii, (IReader**)&reader);
    //TODO AutoPtr<IPemReader> pr;
    //TODO CPemReader::New(reader, (IPemReader**)&pr);

    AutoPtr<ICertificateFactoryHelper> cfHelper;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&cfHelper);
    AutoPtr<ICertificateFactory> cf;
    cfHelper->GetInstance(String("X509"), (ICertificateFactory**)&cf);

    CArrayList::New(result); // list of X509Certificate
    //TODO AutoPtr<IPemObject> o;
    //TODO pr->ReadPemObject((IPemObject**)&o);
    //TODO while (o != NULL)
    {
        String type;
        //TODO o->GetType(&type);
        if (type.Equals("CERTIFICATE")) {
            AutoPtr<ArrayOf<Byte> > buffer;
            //TODO o->GetContent((ArrayOf<Byte>**)&buffer);
            AutoPtr<IByteArrayInputStream> bais;
            CByteArrayInputStream::New(buffer, (IByteArrayInputStream**)&bais);
            AutoPtr<ICertificate> c;
            cf->GenerateCertificate(IInputStream::Probe(bais), (ICertificate**)&c);
            //result.add((X509Certificate) c);
            (*result)->Add(c);
        } else {
            //throw new IllegalArgumentException("Unknown type " + o.getType());
            Logger::E("Credentials", "ConvertFromPem, unkown type: %s", type.string());
            assert(0);
        }
        //TODO pr->ReadPemObject((IPemObject**)&o);
    }
    //TODO pr->Close();
    return NOERROR;
}

ECode Credentials::GetInstance(
    /* [out] */ ICredentials** instance)
{
    VALIDATE_NOT_NULL(instance);
    if (singleton == NULL) {
        singleton = new Credentials();
    }
    *instance = singleton;
    REFCOUNT_ADD(*instance);
    return NOERROR;
}

ECode Credentials::Unlock(
     /* [in] */ IContext* context)
{
    //try {
    AutoPtr<IIntent> intent;
    CIntent::New(UNLOCK_ACTION, (IIntent**)&intent);
    context->StartActivity(intent);
    //} catch (ActivityNotFoundException e) {
    //    Log.w(LOGTAG, e.toString());
    //}
    return NOERROR;
}

ECode Credentials::Install(
    /* [in] */ IContext* context)
{
    //try {
    AutoPtr<IIntent> intent;
    KeyChain::CreateInstallIntent((IIntent**)&intent);
    context->StartActivity(intent);
    //} catch (ActivityNotFoundException e) {
    //    Log.w(LOGTAG, e.toString());
    //}
    return NOERROR;
}

ECode Credentials::Install(
    /* [in] */ IContext* context,
    /* [in] */ IKeyPair* pair)
{
    //try {
    AutoPtr<IIntent> intent;
    KeyChain::CreateInstallIntent((IIntent**)&intent);

    AutoPtr<IPrivateKey> privateKey;
    pair->GetPrivate((IPrivateKey**)&privateKey);
    AutoPtr<ArrayOf<Byte> > privateEncoded;
    IKey::Probe(privateKey)->GetEncoded((ArrayOf<Byte>**)&privateEncoded);

    AutoPtr<IPublicKey> publicKey;
    pair->GetPublic((IPublicKey**)&publicKey);
    AutoPtr<ArrayOf<Byte> > publicEncoded;
    IKey::Probe(publicKey)->GetEncoded((ArrayOf<Byte>**)&publicEncoded);

    intent->PutByteArrayExtra(EXTRA_PRIVATE_KEY, privateEncoded);
    intent->PutByteArrayExtra(EXTRA_PUBLIC_KEY, publicEncoded);
    context->StartActivity(intent);
    //} catch (ActivityNotFoundException e) {
    //    Log.w(LOGTAG, e.toString());
    //}
    return NOERROR;
}

ECode Credentials::Install(
     /* [in] */ IContext* context,
     /* [in] */ const String& type,
     /* [in] */ ArrayOf<Byte>* value)
{
    //try {
    AutoPtr<IIntent> intent;
    KeyChain::CreateInstallIntent((IIntent**)&intent);
    intent->PutByteArrayExtra(type, value);
    context->StartActivity(intent);
    //} catch (ActivityNotFoundException e) {
    //    Log.w(LOGTAG, e.toString());
    //}
    return NOERROR;
}

ECode Credentials::DeleteAllTypesForAlias(
    /* [in] */ IKeyStore* keystore,
    /* [in] */ const String& alias,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    /*
     * Make sure every type is deleted. There can be all three types, so
     * don't use a conditional here.
     */
    Boolean res1, res2;
    keystore->DelKey(Credentials::USER_PRIVATE_KEY + alias, &res1);
    DeleteCertificateTypesForAlias(keystore, alias, &res2);
    *result = res1 | res2;
    return NOERROR;
}

ECode Credentials::DeleteCertificateTypesForAlias(
    /* [in] */ IKeyStore* keystore,
    /* [in] */ const String& alias,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    /*
     * Make sure every certificate type is deleted. There can be two types,
     * so don't use a conditional here.
     */
    Boolean res1, res2;
    keystore->Delete(Credentials::USER_CERTIFICATE + alias, &res1);
    keystore->Delete(Credentials::CA_CERTIFICATE + alias, &res2);
    *result = res1 | res2;
    return NOERROR;
}

}// namespace Elastos
}// namespace Droid
}// namespace KeyStore
}// namespace Security
