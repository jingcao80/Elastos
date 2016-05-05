
#include <Elastos.Droid.Content.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Security.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Zip.h>
#include <Elastos.CoreLibrary.Apache.h>
#include <elastos/core/Thread.h>
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/os/FileUtils.h"
#include "elastos/droid/os/RecoverySystem.h"
#include "elastos/droid/os/ConditionVariable.h"
#include "elastos/droid/os/CUserHandleHelper.h"
#include "elastos/droid/os/CUserManagerHelper.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::Thread;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Manifest;
using Elastos::Droid::Os::IConditionVariable;
using Elastos::Droid::Os::ConditionVariable;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::IUserManagerHelper;
using Elastos::Droid::Os::CUserManagerHelper;

using Elastos::IO::CFile;
using Elastos::IO::IRandomAccessFile;
using Elastos::IO::CRandomAccessFile;
using Elastos::IO::ICloseable;
using Elastos::IO::IWriter;
using Elastos::IO::IFileWriter;
using Elastos::IO::CFileWriter;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::IInputStream;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IEnumeration;
using Elastos::Utility::IList;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::Zip::IZipEntry;
using Elastos::Utility::Zip::IZipFile;
using Elastos::Utility::Zip::CZipFile;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Security::Cert::ICertificate;
using Elastos::Security::Cert::ICertificateFactory;
using Elastos::Security::Cert::ICertificateFactoryHelper;
using Elastos::Security::Cert::CCertificateFactoryHelper;
using Elastos::Security::IPublicKey;
using Elastos::Security::ISignature;
using Elastos::Security::ISignatureHelper;

using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;
using Org::Apache::Harmony::Security::Asn1::CBerInputStream;
using Org::Apache::Harmony::Security::Pkcs7::IContentInfo;
using Org::Apache::Harmony::Security::Pkcs7::IContentInfoHelper;
using Org::Apache::Harmony::Security::Pkcs7::CContentInfoHelper;
using Org::Apache::Harmony::Security::Pkcs7::ISignedData;
using Org::Apache::Harmony::Security::Pkcs7::ISignerInfo;

namespace Elastos {
namespace Droid {
namespace Os {

AutoPtr<IFile> CreateFile(
    /* [in] */ IFile* parent,
    /* [in] */ const String& path)
{
    AutoPtr<IFile> file;
    if (!parent) {
        CFile::New(path, (IFile**)&file);
    }
    else {
        CFile::New(parent, path, (IFile**)&file);
    }
    return file;
}

const String RecoverySystem::TAG("RecoverySystem");

const AutoPtr<IFile> RecoverySystem::DEFAULT_KEYSTORE = CreateFile(NULL, String("/system/etc/security/otacerts.zip"));

const Int64 RecoverySystem::PUBLISH_PROGRESS_INTERVAL_MS = 500;

AutoPtr<IFile> RecoverySystem::RECOVERY_DIR = CreateFile(NULL, String("/cache/recovery"));
AutoPtr<IFile> RecoverySystem::COMMAND_FILE = CreateFile(RECOVERY_DIR, String("command"));
AutoPtr<IFile> RecoverySystem::LOG_FILE = CreateFile(RECOVERY_DIR, String("log"));
String RecoverySystem::LAST_PREFIX("last_");

Int32 RecoverySystem::LOG_FILE_MAX_LENGTH = 64 * 1024;

AutoPtr<IHashSet> RecoverySystem::GetTrustedCerts(
    /* [in] */ IFile* keystore)
{
    AutoPtr<IHashSet> trusted;
    CHashSet::New((IHashSet**)&trusted);
    AutoPtr<IFile> tempFile = keystore;
    if (!keystore) {
        tempFile = DEFAULT_KEYSTORE;
    }

    AutoPtr<IZipFile> zip;
    CZipFile::New(tempFile, (IZipFile**)&zip);

    AutoPtr<ICertificateFactory> cf;
    AutoPtr<ICertificateFactoryHelper> helper;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper);
    helper->GetInstance(String("X.509"), (ICertificateFactory**)&cf);

    Boolean hasMoreElements = FALSE;
    AutoPtr<IEnumeration> entries;
    ECode ec = zip->GetEntries((IEnumeration**)&entries);
    FAIL_GOTO(ec, out);

    while(entries->HasMoreElements(&hasMoreElements), hasMoreElements) {
        AutoPtr<IInterface> entry;
        entries->GetNextElement((IInterface**)&entry);
        AutoPtr<IInputStream> is;
        ec = zip->GetInputStream(IZipEntry::Probe(entry), (IInputStream**)&is);
        FAIL_GOTO(ec, out);

        AutoPtr<ICertificate> cert;
        ec = cf->GenerateCertificate(is, (ICertificate**)&cert);
        is->Close();
        FAIL_GOTO(ec, out);
        trusted->Add((IInterface*)cert);
    }

out:
    zip->Close();
    return trusted;
}

ECode RecoverySystem::VerifyPackage(
    /* [in] */ IFile* packageFile,
    /* [in] */ IRecoverySystemProgressListener* listener,
    /* [in] */ IFile* deviceCertsZipFile)
{
    Int64 fileLen;
    packageFile->GetLength(&fileLen);

    AutoPtr<IRandomAccessFile> raf;
    CRandomAccessFile::New(packageFile, String("r"), (IRandomAccessFile**)&raf);
    Int32 lastPercent = 0;
    Int64 lastPublishTime = 0;
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&lastPublishTime);
    if (listener != NULL) {
        listener->OnProgress(lastPercent);
    }

    raf->Seek(fileLen - 6);
    AutoPtr<ArrayOf<Byte> > footer = ArrayOf<Byte>::Alloc(6);
    Int32 number;
    raf->Read((ArrayOf<Byte>*)footer, &number);

    if ((*footer)[2] != (Byte)0xff || (*footer)[3] != (Byte)0xff) {
        // throw new SignatureException("no signature in file (no footer)");
        ICloseable::Probe(raf)->Close();
        return E_SIGNATURE_EXCEPTION;
    }

    Int32 commentSize = ((*footer)[4] & 0xff) | (((*footer)[5] & 0xff) << 8);
    Int32 signatureStart = ((*footer)[0] & 0xff) | (((*footer)[1] & 0xff) << 8);

    AutoPtr<ArrayOf<Byte> > eocd = ArrayOf<Byte>::Alloc(commentSize + 22);
    raf->Seek(fileLen - (commentSize + 22));
    raf->Read((ArrayOf<Byte>*)eocd, &number);

    // Check that we have found the start of the
    // end-of-central-directory record.
    if ((*eocd)[0] != (Byte)0x50 || (*eocd)[1] != (Byte)0x4b ||
        (*eocd)[2] != (Byte)0x05 || (*eocd)[3] != (Byte)0x06) {
        // throw new SignatureException("no signature in file (bad footer)");
        ICloseable::Probe(raf)->Close();
        return E_SIGNATURE_EXCEPTION;
    }

    Int32 length = eocd->GetLength();
    for (Int32 i = 4; i < length - 3; ++i) {
        if ((*eocd)[i  ] == (Byte)0x50 && (*eocd)[i+1] == (Byte)0x4b &&
            (*eocd)[i+2] == (Byte)0x05 && (*eocd)[i+3] == (Byte)0x06) {
            // throw new SignatureException("EOCD marker found after start of EOCD");
            ICloseable::Probe(raf)->Close();
            return E_SIGNATURE_EXCEPTION;
        }
    }

    // The following code is largely copied from
    // JarUtils.verifySignature().  We could just *call* that
    // method here if that function didn't read the entire
    // input (ie, the whole OTA package) into memory just to
    // compute its message digest.

    AutoPtr<IByteArrayInputStream> bais;
    CByteArrayInputStream::New(eocd, commentSize + 22 - signatureStart,
            signatureStart, (IByteArrayInputStream**)&bais);
    AutoPtr<IBerInputStream> bis;
    CBerInputStream::New(IInputStream::Probe(bais), (IBerInputStream**)&bis);

    AutoPtr<IInterface> info;
    AutoPtr<IASN1Sequence> asn1;
    AutoPtr<IContentInfoHelper> helper;
    CContentInfoHelper::AcquireSingleton((IContentInfoHelper**)&helper);
    helper->GetASN1((IASN1Sequence**)&asn1);
    IASN1Type::Probe(asn1)->Decode(IInputStream::Probe(bis), (IInterface**)&info);

    AutoPtr<ISignedData> signedData;
    IContentInfo::Probe(info)->GetSignedData((ISignedData**)&signedData);
    if (!signedData) {
        // throw new IOException("signedData is null");
        ICloseable::Probe(raf)->Close();
        return E_IO_EXCEPTION;
    }

    AutoPtr<IList> encCerts;
    signedData->GetCertificates((IList**)&encCerts);
    Boolean isEmpty = TRUE;
    if (encCerts->IsEmpty(&isEmpty), !isEmpty) {
        // throw new IOException("encCerts is empty");
        ICloseable::Probe(raf)->Close();
        return E_IO_EXCEPTION;
    }

    // Take the first certificate from the signature (packages
    // should contain only one).
    AutoPtr<IIterator> it;
    encCerts->GetIterator((IIterator**)&it);
    AutoPtr<ICertificate> cert;
    Boolean hasNext = FALSE;
    if (it->HasNext(&hasNext), hasNext) {
        AutoPtr<ICertificateFactory> cf;
        AutoPtr<ICertificateFactoryHelper> helper;
        CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper);
        helper->GetInstance(String("X.509"), (ICertificateFactory**)&cf);

        AutoPtr<IInterface> item;
        it->GetNext((IInterface**)&item);
        AutoPtr<ArrayOf<Byte> > encoded;
        ICertificate::Probe(item)->GetEncoded((ArrayOf<Byte>**)&encoded);
        AutoPtr<IByteArrayInputStream> is;
        CByteArrayInputStream::New(encoded, (IByteArrayInputStream**)&is);
        cf->GenerateCertificate(IInputStream::Probe(is), (ICertificate**)&cert);
    }
    else {
        // throw new SignatureException("signature contains no certificates");
        ICloseable::Probe(raf)->Close();
        return E_SIGNATURE_EXCEPTION;
    }

    AutoPtr<IList> sigInfos;
    signedData->GetSignerInfos((IList**)&sigInfos);
    AutoPtr<IInterface> sigInfoInterface;
    if (sigInfos->IsEmpty(&isEmpty), !isEmpty) {
        sigInfos->Get(0, (IInterface**)&sigInfoInterface);
    }
    else {
        // throw new IOException("no signer infos!");
        ICloseable::Probe(raf)->Close();
        return E_IO_EXCEPTION;
    }
    ISignerInfo* sigInfo = ISignerInfo::Probe(sigInfoInterface);

    // Check that the public key of the certificate contained
    // in the package equals one of our trusted public keys.

    AutoPtr<IHashSet> trusted = GetTrustedCerts(
            deviceCertsZipFile == NULL ? (IFile*)DEFAULT_KEYSTORE : deviceCertsZipFile);
    AutoPtr<IPublicKey> signatureKey;
    cert->GetPublicKey((IPublicKey**)&signatureKey);
    Boolean verified = FALSE;
    AutoPtr<ArrayOf<IInterface*> > array;
    trusted->ToArray((ArrayOf<IInterface*>**)&array);
    Int32 size;
    trusted->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<ICertificate> c = ICertificate::Probe((*array)[i]);
        AutoPtr<IPublicKey> publicKey;
        c->GetPublicKey((IPublicKey**)&publicKey);
        Boolean equals = FALSE;
        if (IObject::Probe(publicKey)->Equals(signatureKey, &equals), equals) {
            verified = TRUE;
            break;
        }
    }
    if (!verified) {
        // throw new SignatureException("signature doesn't match any trusted key");
        ICloseable::Probe(raf)->Close();
        return E_SIGNATURE_EXCEPTION;
    }

    // The signature cert matches a trusted key.  Now verify that
    // the digest in the cert matches the actual file data.

    // The verifier in recovery only handles SHA1withRSA and
    // SHA256withRSA signatures.  SignApk chooses which to use
    // based on the signature algorithm of the cert:
    //
    //    "SHA256withRSA" cert -> "SHA256withRSA" signature
    //    "SHA1withRSA" cert   -> "SHA1withRSA" signature
    //    "MD5withRSA" cert    -> "SHA1withRSA" signature (for backwards compatibility)
    //    any other cert       -> SignApk fails
    //
    // Here we ignore whatever the cert says, and instead use
    // whatever algorithm is used by the signature.
    String da;
    sigInfo->GetDigestAlgorithm(&da);
    String dea;
    sigInfo->GetDigestEncryptionAlgorithm(&dea);
    String alg;
    if (da.IsNull() || dea.IsNull()) {
        // fall back to the cert algorithm if the sig one
        // doesn't look right.
        IX509Certificate::Probe(cert)->GetSigAlgName(&alg);
    }
    else {
        alg = da + "with" + dea;
    }

    AutoPtr<ISignature> sig;
    AutoPtr<ISignatureHelper> signHelper;
    assert(0 && "TODO");
    // CSignatureHelper::AcquireSingleton(alg, (ISignatureHelper**)&signHelper);
    signHelper->GetInstance(alg, (ISignature**)&sig);
    sig->InitVerify(cert);

    // The signature covers all of the OTA package except the
    // archive comment and its 2-byte length.
    Int64 toRead = fileLen - commentSize - 2;
    Int64 soFar = 0;
    raf->Seek(0);
    AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(4096);
    Boolean interrupted = false;
    while (soFar < toRead) {
        Thread::GetCurrentThread()->IsInterrupted(&interrupted);
        if (interrupted) break;
        Int32 size = buffer->GetLength();
        if (soFar + size > toRead) {
            size = (Int32)(toRead - soFar);
        }
        Int32 read;
        raf->Read(buffer, 0, size, &read);
        sig->Update(buffer, 0, read);
        soFar += read;

        if (listener != NULL) {
            Int64 now;
            system->GetCurrentTimeMillis(&now);
            Int32 p = (Int32)(soFar * 100 / toRead);
            if (p > lastPercent &&
                now - lastPublishTime > PUBLISH_PROGRESS_INTERVAL_MS) {
                lastPercent = p;
                lastPublishTime = now;
                listener->OnProgress(lastPercent);
            }
        }
    }
    if (listener != NULL) {
        listener->OnProgress(100);
    }

    if (interrupted) {
        // throw new SignatureException("verification was interrupted");
        ICloseable::Probe(raf)->Close();
        return E_SIGNATURE_EXCEPTION;
    }

    AutoPtr<ArrayOf<Byte> > digest;
    sigInfo->GetEncryptedDigest((ArrayOf<Byte>**)&digest);
    Boolean verifyed = FALSE;
    sig->Verify(digest, &verifyed);
    if (!verifyed) {
        // throw new SignatureException("signature digest verification failed");
        ICloseable::Probe(raf)->Close();
        return E_SIGNATURE_EXCEPTION;
    }

    ICloseable::Probe(raf)->Close();
    return NOERROR;
}

ECode RecoverySystem::InstallPackage(
    /* [in] */ IContext* context,
    /* [in] */ IFile* packageFile)
{
    String filename;
    packageFile->GetCanonicalPath(&filename);
    Logger::W(TAG, "!!! REBOOTING TO INSTALL %s !!!", filename.string());

    String filenameArg("--update_package=");
    filenameArg += filename;
    String localeArg("--locale=");
    AutoPtr<ILocale> locale;
    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    helper->GetDefault((ILocale**)&locale);
    String defLocal;
    locale->ToString(&defLocal);
    localeArg += defLocal;
    return BootCommand(context, 2, filenameArg.string(), localeArg.string());
}

ECode RecoverySystem::RebootWipeUserData(
    /* [in] */ IContext* context)
{
    String packageName;
    context->GetPackageName(&packageName);
    return RebootWipeUserData(context, FALSE, packageName, FALSE);
}

ECode RecoverySystem::RebootWipeUserData(
    /* [in] */ IContext* context,
    /* [in] */ const String& reason)
{
    return RebootWipeUserData(context, FALSE, reason, FALSE);
}

ECode RecoverySystem::RebootWipeUserData(
    /* [in] */ IContext* context,
    /* [in] */ Boolean shutdown)
{
    String packageName;
    context->GetPackageName(&packageName);
    return RebootWipeUserData(context, shutdown, packageName, FALSE);
}

ECode RecoverySystem::RebootWipeUserData(
    /* [in] */ IContext* context,
    /* [in] */ Boolean shutdown,
    /* [in] */ const String& reason,
    /* [in] */ Boolean wipeMedia)
{
    AutoPtr<IInterface> service;
    FAIL_RETURN(context->GetSystemService(IContext::USER_SERVICE, (IInterface**)&service));
    IUserManager* um = IUserManager::Probe(service);
    Boolean hasUR = FALSE;
    um->HasUserRestriction(IUserManager::DISALLOW_FACTORY_RESET, &hasUR);
    if (hasUR) {
        // throw new SecurityException("Wiping data is not allowed for this user.");
        return E_SECURITY_EXCEPTION;
    }

    AutoPtr<IConditionVariable> condition = new ConditionVariable();

    AutoPtr<IIntent> intent;
    FAIL_RETURN(CIntent::New(
            String("android.intent.action.MASTER_CLEAR_NOTIFICATION"), (IIntent**)&intent));
    intent->AddFlags(IIntent::FLAG_RECEIVER_FOREGROUND);
    AutoPtr<IUserHandle> owner;
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    helper->GetOWNER((IUserHandle**)&owner);
    AutoPtr<OpenConditionBroadcastReceiver> receiver = new OpenConditionBroadcastReceiver(condition);
    context->SendOrderedBroadcastAsUser(intent, owner,
            Elastos::Droid::Manifest::permission::MASTER_CLEAR,
            receiver, NULL, 0, String(NULL), NULL);

    // Block until the ordered broadcast has completed.
    condition->Block();

    String shutdownArg;
    if (shutdown) {
        shutdownArg = "--shutdown_after";
    }

    String reasonArg;
    if (!reason.IsEmpty()) {
        reasonArg = String("--reason=") + SanitizeArg(reason);
    }

    String localeArg("--locale=");
    AutoPtr<ILocale> locale;
    AutoPtr<ILocaleHelper> localeHelper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelper);
    localeHelper->GetDefault((ILocale**)&locale);
    String defLocal;
    locale->ToString(&defLocal);
    localeArg += defLocal;

    String cmd("--wipe_data\n");
    if (wipeMedia) {
        cmd += "--wipe_media\n";
    }

    return BootCommand(context, 4, shutdownArg.string(),
            cmd.string(), reasonArg.string(), localeArg.string());
}

ECode RecoverySystem::RebootWipeCache(
    /* [in] */ IContext* context)
{
    String packageName;
    context->GetPackageName(&packageName);
    return RebootWipeCache(context, packageName);
}

ECode RecoverySystem::RebootWipeCache(
    /* [in] */ IContext* context,
    /* [in] */ const String& reason)
{
    String reasonArg;
    if (!reason.IsEmpty()) {
        reasonArg = String("--reason=") + SanitizeArg(reason);
    }

    String localeArg("--locale=");
    AutoPtr<ILocale> locale;
    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    helper->GetDefault((ILocale**)&locale);
    String defLocal;
    locale->ToString(&defLocal);
    localeArg += defLocal;
    return BootCommand(context, 3, "--wipe_cache",
            reasonArg.string(), localeArg.string());
}

ECode RecoverySystem::BootCommand(
    /* [in] */ IContext* context, Int32 count, ...)
{
    Boolean succeeded;
    RECOVERY_DIR->Mkdirs(&succeeded);  // In case we need it
    COMMAND_FILE->Delete();  // In case it's not writable
    LOG_FILE->Delete();

    AutoPtr<IFileWriter> command;
    CFileWriter::New(COMMAND_FILE, (IFileWriter**)&command);
    // try {
    va_list args;
    va_start(args, count);
    for (Int32 i = 0; i < count; i++) {
        const char* arg = va_arg(args, const char*);
        if (arg) {
            IWriter::Probe(command)->Write(String(arg));
            IWriter::Probe(command)->Write(String("\n"));
        }
    }
    va_end(args);
    ICloseable::Probe(command)->Close();
    // } finally {
    //     command.close();
    // }

    // Having written the command file, go ahead and reboot
    AutoPtr<IInterface> pm;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&pm);
    IPowerManager::Probe(pm)->Reboot(IPowerManager::REBOOT_RECOVERY);

    // throw new IOException("Reboot failed (no permissions?)");
    return E_IO_EXCEPTION;
}

String RecoverySystem::HandleAftermath()
{
    // Record the tail of the LOG_FILE
    String log;
    ECode ec = FileUtils::ReadTextFile(LOG_FILE,
            -LOG_FILE_MAX_LENGTH, String("...\n"), &log);
    if (ec == (ECode)E_FILE_NOT_FOUND_EXCEPTION) {
        Logger::I(TAG, "No recovery log file");
    }
    else if (ec == (ECode)E_IO_EXCEPTION) {
        Logger::I(TAG, "Error reading recovery log %x", ec);
    }

    // Delete everything in RECOVERY_DIR except those beginning
    // with LAST_PREFIX
    AutoPtr<ArrayOf<String> > names;
    RECOVERY_DIR->List((ArrayOf<String>**)&names);
    Int32 length = names->GetLength();
    for (int i = 0; names != NULL && i < length; i++) {
        if ((*names)[i].StartWith(LAST_PREFIX)) continue;
        AutoPtr<IFile> f;
        CFile::New(RECOVERY_DIR, (*names)[i], (IFile**)&f);
        Boolean deleted = FALSE;
        f->Delete(&deleted);
        String fileStr;
        f->ToString(&fileStr);
        if (!deleted) {
            Logger::E(TAG, "Can't delete: %s", fileStr.string());
        }
        else {
            Logger::I(TAG, "Deleted: %s", fileStr.string());
        }
    }

    return log;
}

String RecoverySystem::SanitizeArg(
    /* [in] */ const String& arg)
{
    String result = arg.Replace('\0', '?');;
    result = result.Replace('\n', '?');
    return result;
}

ECode RecoverySystem::OpenConditionBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    mCondition->Open();
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
