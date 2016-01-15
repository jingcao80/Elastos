#ifndef __ELASTOS_DROID_SERVER_UPDATES_CCONFIGUPDATEINSTALLRECEIVER_H__
#define __ELASTOS_DROID_SERVER_UPDATES_CCONFIGUPDATEINSTALLRECEIVER_H__

#include "updates/CConfigUpdateInstallReceiver.h"
#include <elstring.h>
#include <eltypes.h>

using Elastos::Core::IThread;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::IO::IFile;
using Elastos::Security::Cert::IX509Certificate;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Updates {

class CConfigUpdateInstallReceiver
: public IBroadcastReceiver
{
public:
    ~CConfigUpdateInstallReceiver();

    CConfigUpdateInstallReceiver(
        /* [in] */ const String& updateDir,
        /* [in] */ const String& updateContentPath,
        /* [in] */ const String& updateMetadataPath,
        /* [in] */ const String& updateVersionPath);

    CARAPI CConfigUpdateInstallReceiver::Init(
        /* [in] */ const String& updateDir,
        /* [in] */ const String& updateContentPath,
        /* [in] */ const String& updateMetadataPath,
        /* [in] */ const String& updateVersionPath);

    //@Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    static const String TAG;// = "ConfigUpdateInstallReceiver";
    static const String EXTRA_CONTENT_PATH;// = "CONTENT_PATH";
    static const String EXTRA_REQUIRED_HASH;// = "REQUIRED_HASH";
    static const String EXTRA_SIGNATURE;// = "SIGNATURE";
    static const String EXTRA_VERSION_NUMBER;// = "VERSION";
    static const String UPDATE_CERTIFICATE_KEY;// = "config_update_certificate";

    /*const*/ AutoPtr<IFile> mUpdateDir;
    /*const*/ AutoPtr<IFile> mUpdateContent;
    /*const*/ AutoPtr<IFile> mUpdateMetadataDir;
    /*const*/ AutoPtr<IFile> mUpdateVersion;

    // get the certificate from Settings.Secure
    AutoPtr<IX509Certificate> cert;// = GetCert(context->GetContentResolver());
    // get the content path from the extras
    String altContent;// = GetAltContent(intent);
    // get the version from the extras
    Int32 altVersion;// = GetVersionFromIntent(intent);
    // get the previous value from the extras
    String altRequiredHash;// = GetRequiredHashFromIntent(intent);
    // get the signature from the extras
    String altSig;// = GetSignatureFromIntent(intent);
    // get the version currently being used
    Int32 currentVersion;// = GetCurrentVersion();
    // get the hash of the currently used value
    String currentHash;// = GetCurrentHash(GetCurrentContent());


    CARAPI Run();

    CARAPI_(AutoPtr<IX509Certificate >) GetCert(
        /* [in] */ IContentResolver* cr);

    CARAPI_(String) GetContentFromIntent(
        /* [in] */ IIntent* i);

    CARAPI_(Int32) GetVersionFromIntent(
        /* [in] */ IIntent* i);// throws NumberFormatException

    CARAPI_(String) GetRequiredHashFromIntent(
        /* [in] */ IIntent* i);

    CARAPI_(String) GetSignatureFromIntent(
        /* [in] */ IIntent* i);

    CARAPI_(Int32) GetCurrentVersion();// throws NumberFormatException

    CARAPI_(String) GetAltContent(
        /* [in] */ IIntent* i); //throws IOException

    CARAPI_(String) GetCurrentContent();

    static CARAPI_(String) GetCurrentHash(
        /* [in] */ const String& content);

    CARAPI_(Boolean) VerifyVersion(
        /* [in] */ Int32 current,
        /* [in] */ Int32 alternative);

    CARAPI_(Boolean) CConfigUpdateInstallReceiver::VerifySignature(
        /* [in] */ const String& content,
        /* [in] */ Int32 version,
        /* [in] */ const String& requiredPrevious,
        /* [in] */ const String& signature,
        /* [in] */ IX509Certificate cert);// throws Exception

    CARAPI_(void) WriteUpdate(
        /* [in] */ IFile* dir,
        /* [in] */ IFile* file,
        /* [in] */ const String& content);// throws IOException

    CARAPI_(void) Install(
        /* [in] */ const String& content,
        /* [in] */ Int32 version);// throws IOException

    CARAPI_(void) StringToByteArray(
        /* [in] */ const String& content,
        /* [out] */ ArrayOf<Byte>** byteArray);
};


//namespace Updates
//namespace Server
//namespace Droid
//namespace Elastos

#endif //__ELASTOS_DROID_SERVER_UPDATES_CCONFIGUPDATEINSTALLRECEIVER_H__
