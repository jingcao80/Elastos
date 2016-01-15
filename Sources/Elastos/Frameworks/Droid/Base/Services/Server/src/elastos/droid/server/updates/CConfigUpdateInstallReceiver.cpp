#include "updates/CConfigUpdateInstallReceiver.h"
#include <elastos/core/Thread.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/CInteger32.h>
#include <elastos/core/StringUtils.h>
#include <elastos/io/CIoUtils.h>
#include <elastos/io/CFileDescriptor.h>
#include <elastos/security/CMessageDigest.h>
#include <elastos/security/CSignature.h>


/*
import android.content.BroadcastReceiver;
import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.provider.Settings;
import android.os.FileUtils;
import android.util.Base64;
import android.util.EventLog;
import android.util.Slog;

import com.android.server.EventLogTags;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.IOException;
import java.security.cert.Certificate;
import java.security.cert.CertificateException;
import java.security.cert.CertificateFactory;
import java.security.cert.X509Certificate;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.security.Signature;
import java.security.SignatureException;

import libcore.io.IoUtils;
*/

using Elastos::Core::IInteger32;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IInputStream;
using Elastos::IO::IIoUtils;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::ICloseable;
using Elastos::Security::IMessageDigest;
using Elastos::Security::IMessageDigestHelper;
using Elastos::Security::ISignature;
using Elastos::Security::ISignatureHelper;
using Elastos::Security::Cert::ICertificateFactory;
using Elastos::Security::Cert::ICertificateFactoryHelper;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::SLogger;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Updates {

const String CConfigUpdateInstallReceiver::TAG = "ConfigUpdateInstallReceiver";
const String CConfigUpdateInstallReceiver::EXTRA_CONTENT_PATH = "CONTENT_PATH";
const String CConfigUpdateInstallReceiver::EXTRA_REQUIRED_HASH = "REQUIRED_HASH";
const String CConfigUpdateInstallReceiver::EXTRA_SIGNATURE = "SIGNATURE";
const String CConfigUpdateInstallReceiver::EXTRA_VERSION_NUMBER = "VERSION";
const String CConfigUpdateInstallReceiver::UPDATE_CERTIFICATE_KEY = "config_update_certificate";

CConfigUpdateInstallReceiver::~CConfigUpdateInstallReceiver()
{}

CConfigUpdateInstallReceiver::CConfigUpdateInstallReceiver(
    /* [in] */ const String& updateDir,
    /* [in] */ const String& updateContentPath,
    /* [in] */ const String& updateMetadataPath,
    /* [in] */ const String& updateVersionPath)
{
    CFile::New(updateDir, (IFile**)&mUpdateDir);
    CFile::New(updateDir, updateContentPath, (IFile**)&mUpdateContent);
    CFile::New(updateDir, updateMetadataPath, (IFile**)&mUpdateMetadataDir);
    CFile::New(mUpdateMetadataDir, updateVersionPath, (IFile**)&mUpdateVersion);
    return NOERROR;
}

//@Override
ECode CConfigUpdateInstallReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    // get the certificate from Settings.Secure
    cert = GetCert(context->GetContentResolver(resolver);
    // get the content path from the extras
    altContent = GetAltContent(intent);
    // get the version from the extras
    altVersion = GetVersionFromIntent(intent);
    // get the previous value from the extras
    altRequiredHash = GetRequiredHashFromIntent(intent);
    // get the signature from the extras
    altSig = GetSignatureFromIntent(intent);
    // get the version currently being used
    currentVersion = GetCurrentVersion();
    // get the hash of the currently used value
    currentHash = GetCurrentHash(GetCurrentContent());

    AutoPtr<IThread> thread;
    CThread::New((IRunnable*)this->Probe(EIID_IRunnable), TAG, (IThread**)&thread);
    thread->Start();

    return NOERROR;
}

ECode CConfigUpdateInstallReceiver::Run()
{
    if (FALSE == VerifyVersion(currentVersion, altVersion))
    {
        Slogger::I(TAG, "Not installing, new version is <= current version");
    }
    else if (FALSE == VerifyPreviousHash(currentHash, altRequiredHash))
    {
        //EventLog.writeEvent(EventLogTags.CONFIG_INSTALL_FAILED,
        //                    "Current hash did not match required value");
        Slogger::I(TAG, "Current hash did not match required value");
    }
    else if (FALSE == VerifySignature(altContent, altVersion, altRequiredHash, altSig,
               cert))
    {
        //EventLog.writeEvent(EventLogTags.CONFIG_INSTALL_FAILED,
        //                    "Signature did not verify");
        Slogger::I(TAG, "Signature did not verify");
    }
    else
    {
        // install the new content
        Slogger::I(TAG, "Found new update, installing...");
        Install(altContent, altVersion);
        Slogger::I(TAG, "Installation successful");
    }
    return NOERROR;
}


AutoPtr<IX509Certificate> CConfigUpdateInstallReceiver::GetCert(
    /* [in] */ IContentResolver* cr)
{
    // get the cert from settings
    String cert;
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->GetString(cr, UPDATE_CERTIFICATE_KEY, &cert);
    // convert it into a real certificate
    //try {
        //cert.GetBytes((ArrayOf<Byte>**)&temp);

        AutoPtr<ArrayOf<Byte> > derCert;
        //IBase64::Decode(temp, IBase64::DEFAULT, (ArrayOf<Byte>**)&derCert);
        IBase64::Decode(cert, IBase64::DEFAULT, (ArrayOf<Byte>**)&derCert);

        AutoPtr<IInputStream> istream;
        CByteArrayInputStream::New(derCert, (IByteArrayInputStream**)&istream);

        AutoPtr<ICertificateFactory> cf;
        AutoPtr<ICertificateFactoryHelper> cf_helper;
        CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&cf_helper);
        cf_helper->GetInstance("X.509", (ICertificateFactory**)&cf);

        AutoPtr<IX509Certificate> x509cf;
        cf->GenerateCertificate(istream, (ICertificate**)&x509cf);
        return x509cf;
    //} catch (CertificateException e) {
    //    throw new IllegalStateException("Got malformed certificate from settings, ignoring");
    //}
}

String CConfigUpdateInstallReceiver::GetContentFromIntent(
    /* [in] */ IIntent* i)
{
    String extraValue;
    i->GetStringExtra(EXTRA_CONTENT_PATH, &extraValue);

    if (extraValue.IsNull())
    {
        //throw new IllegalStateException("Missing required content path, ignoring.");

    }
    return extraValue;
}

Int32 CConfigUpdateInstallReceiver::GetVersionFromIntent(
    /* [in] */ IIntent* i)// throws NumberFormatException
{
    String extraValue;
    i->GetStringExtra(EXTRA_VERSION_NUMBER, &extraValue);

    if (extraValue.IsNull())
    {
        //throw new IllegalStateException("Missing required version number, ignoring.");
    }

    String strTemp = extraValue.Trim();

    Int32 result = StringUtils::ParseInt32(strTemp);

    //IInteger::ParseInt(strTemp, &result);

    return result;
}

String CConfigUpdateInstallReceiver::GetRequiredHashFromIntent(
    /* [in] */ IIntent* i)
{
    String extraValue;
    i->GetStringExtra(EXTRA_REQUIRED_HASH, &extraValue);

    if (extraValue.IsNull())
    {
        //throw new IllegalStateException("Missing required previous hash, ignoring.");
    }
    return extraValue.Trim();
}

String CConfigUpdateInstallReceiver::GetSignatureFromIntent(
    /* [in] */ IIntent* i)
{
    String extraValue;
    i->GetStringExtra(EXTRA_SIGNATURE, &extraValue);

    if (extraValue.IsNull())
    {
        //throw new IllegalStateException("Missing required signature, ignoring.");
    }
    return extraValue.Trim();
}

Int32 CConfigUpdateInstallReceiver::GetCurrentVersion()// throws NumberFormatException
{
    //try {
        String path;
        mUpdateVersion->GetCanonicalPath(&path);
        String strTemp;
        AutoPtr<IIoUtils> ioUtils;
        CIoUtils::New((IIoUtils**)&ioUtils);
        ioUtils->ReadFileAsString(path, &strTemp)

        String strVersion = strTemp.Trim();
        Int32 result = StringUtils::ParseInt32(strVersion);
        //IInteger::ParseInt(strVersion, &result);
        return result;
    //} catch (IOException e) {
    //    Slog.i(TAG, "Couldn't find current metadata, assuming first update");
    //    return 0;
    //}
}

String CConfigUpdateInstallReceiver::GetAltContent(
    /* [in] */ IIntent* i) //throws IOException
{
    String contents;
    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::New((IIoUtils**)&ioUtils);
    ioUtils->ReadFileAsString(GetContentFromIntent(i), &contents);
    return contents.Trim();
}

String CConfigUpdateInstallReceiver::GetCurrentContent()
{
    //try {
        String path;
        mUpdateContent->GetCanonicalPath(&path);
        String strBuf;
        AutoPtr<IIoUtils> ioUtils;
        CIoUtils::New((IIoUtils**)&ioUtils);
        ioUtils->ReadFileAsString(path, &strBuf);
        return strBuf.Trim();
    //} catch (IOException e) {
    //    Slog.i(TAG, "Failed to read current content, assuming first update!");
    //    return null;
    //}
}

static String CConfigUpdateInstallReceiver::GetCurrentHash(
    /* [in] */ const String& content)
{
    if (content == NULL)
    {
        return "0";
    }
    //try {
        AutoPtr<IMessageDigest> dgst;
        AutoPtr<IMessageDigestHelper> dgst_helper;
        CMessageDigestHelper::AcquireSingleton((IMessageDigestHelper**)&dgst_helper);
        dgst_helper->GetInstance("SHA512", (IMessageDigest**)&dgst);

        AutoPtr<ArrayOf<Byte> > encoded;
        StringToByteArray(content, (ArrayOf<Byte>**)&encoded);
        AutoPtr<ArrayOf<Byte> > fingerprint;
        // To be check...IMessageDigest is not implemented in Elastos...
        dgst->Digest(encoded, (ArrayOf<Byte>**)&fingerprint);

        String hexString;
        StringUtils::BytesToHexString(fingerprint, FALSE, &hexString);

        return hexString;
    //} catch (NoSuchAlgorithmException e) {
    //    throw new AssertionError(e);
    //}
}

Boolean CConfigUpdateInstallReceiver::VerifyVersion(
    /* [in] */ Int32 current,
    /* [in] */ Int32 alternative)
{
    return (current < alternative);
}

Boolean CConfigUpdateInstallReceiver::VerifyPreviousHash(
    /* [in] */ const String& current,
    /* [in] */ const String& required)
{
    Boolean result;

    // this is an optional value- if the required field is NONE then we ignore it
    required.Equals("NONE", &result);
    if (result == TRUE)
    {
        return TRUE;
    }
    // otherwise, verify that we match correctly
    current.Equals(required, &result);
    return result;
}

Boolean CConfigUpdateInstallReceiver::VerifySignature(
    /* [in] */ const String& content,
    /* [in] */ Int32 version,
    /* [in] */ const String& requiredPrevious,
    /* [in] */ const String& signature,
    /* [in] */ IX509Certificate* cert)// throws Exception
{
    //To be modified: ISignature is not yet implemented in Elastos
    AutoPtr<ISignature> signer;
    AutoPtr<ISignatureHelper> signer_helper;
    CSignatureHelper::AcquireSingleton((ISignatureHelper**)&signer_helper);
    signer_helper->GetInstance("SHA512withRSA", (ISignature**)&signer);
    signer->InitVerify(cert);

    AutoPtr<ArrayOf<Byte> > tempBuf1;
    StringToByteArray(content, (ArrayOf<Byte>**)&tempBuf1);
    signer->Update(tempBuf1);

    String strBuf;
    StringUtils::Int32ToString(version, &strBuf);

    AutoPtr<ArrayOf<Byte> > tempBuf2;
    StringToByteArray(strBuf, (ArrayOf<Byte>**)&tempBuf2);
    signer->Update(tempBuf2);

    AutoPtr<ArrayOf<Byte> > tempBuf3;
    StringToByteArray(requiredPrevious, (ArrayOf<Byte>**)&tempBuf3);
    signer->Update(tempBuf3);

    //ArrayOf<Byte>* tempBuf4;
    //signature.GetBytes((ArrayOf<Byte>**)&tempBuf4);
    AutoPtr<ArrayOf<Byte> > resBuf;
    //IBase64::Decode(*tempBuf4, IBase64::DEFAULT, (ArrayOf<Byte>**)&resBuf);
    IBase64::Decode(signature, IBase64::DEFAULT, (ArrayOf<Byte>**)&resBuf);
    Boolean result;
    signer->Verify(resBuf, &result);

    return result;
}

void CConfigUpdateInstallReceiver::WriteUpdate(
    /* [in] */ IFile* dir,
    /* [in] */ IFile* file,
    /* [in] */ String* content)// throws IOException
{
    AutoPtr<IFileOutputStream> out = NULL;
    AutoPtr<IFile> tmp = NULL;
    //try {
        // create the temporary file
        IFile::CreateTempFile("journal", "", dir, (IFile**)&tmp);
        // create the parents for the destination file
        AutoPtr<IFile> parent;
        file->GetParentFile((IFile**)&parent);
        Boolean succeeded;
        parent->Mkdirs(&succeeded);
        // check that they were created correctly

        Boolean doesExist;
        parent->Exists(&doesExist);
        if (FALSE == doesExist)
        {
            //throw new IOException("Failed to create directory " + parent.getCanonicalPath());
            return;
        }
        // mark tmp -rw-r--r--
        tmp->SetReadable(TRUE, FALSE, &succeeded);
        // write to it
        CFileOutputStream::New(tmp, (IFileOutputStream**)&out);

        AutoPtr<ArrayOf<Byte> > tempBuf;
        StringToByteArray(content, (ArrayOf<Byte>**)&tempBuf);
        out->WriteBytes(*tempBuf);
        // sync to disk
        AutoPtr<IFileDescriptor> fd;
        out->GetFD((IFileDescriptor**)&fd);
        fd->Sync();
        // atomic rename
        if (NOERROR != tmp->RenameTo(file, &succeeded))
        {
            //throw new IOException("Failed to atomically rename " + file.getCanonicalPath());
        }
    //} finally {
        if (tmp != NULL) {
            tmp->Delete(&succeeded);
        }
        AutoPtr<IIoUtils> ioUtils;
        CIoUtils::New((IIoUtils**)&ioUtils);
        ioUtils->CloseQuietly((ICloseable*)out);
    }
}

void CConfigUpdateInstallReceiver::Install(
    /* [in] */ const String& content,
    /* [in] */ Int32 version)// throws IOException
{
    WriteUpdate(mUpdateDir, mUpdateContent, &content);

    String strBuf;
    StringUtils::Int32ToString(version, &strBuf);
    WriteUpdate(mUpdateDir, mUpdateVersion, &strBuf);
}

void CConfigUpdateInstallReceiver::StringToByteArray(
    /* [in] */ const String& content,
    /* [out] */ ArrayOf<Byte>** byteArray)
{
    *byteArray = ArrayOf<Byte>::Alloc(content.GetLength());
    REFCOUNT_ADD(*byteArray);

    for(Int32 i = 0; i < content.GetLength(); i++) {
        (*byteArray)[i] = (Byte) content[i];
    }

    REFCOUNT_ADD(*byteArray);
}

//namespace Updates
//namespace Server
//namespace Droid
//namespace Elastos
