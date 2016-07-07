#include "elastos/droid/providers/media/MediaScannerReceiver.h"
#include "elastos/droid/providers/media/MediaProvider.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.IO.h"
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/os/Environment.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::Environment;
using Elastos::Utility::Logging::Logger;
using Elastos::IO::CFile;
using Elastos::IO::IFile;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Media {

const String MediaScannerReceiver::TAG("MediaScannerReceiver");

CAR_INTERFACE_IMPL(MediaScannerReceiver, BroadcastReceiver, IMediaScannerReceiver)

ECode MediaScannerReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    AutoPtr<IUri> uri;
    intent->GetData((IUri**)&uri);
    Logger::I(TAG, " >> OnReceive: %s, uri: %s", action.string(), TO_CSTR(uri));

    if (IIntent::ACTION_BOOT_COMPLETED.Equals(action)) {
        // Scan both internal and external storage
        Scan(context, MediaProvider::INTERNAL_VOLUME);
        Scan(context, MediaProvider::EXTERNAL_VOLUME);
    }
    else {
        String scheme;
        uri->GetScheme(&scheme);
        if (scheme.Equals("file")) {
            // handle intents related to external storage
            String path;
            uri->GetPath(&path);
            String externalStoragePath;
            AutoPtr<IFile> file;
            file = Environment::GetExternalStorageDirectory();
            String legacyPath;
            file->GetPath(&legacyPath);

            // try {
                AutoPtr<IFile> tf;
                CFile::New(path, (IFile**)&tf);
                ECode ec = tf->GetCanonicalPath(&path);
            // } catch (IOException e) {
                if (ec == (ECode)E_IO_EXCEPTION) {
                    Logger::E(TAG, "couldn't canonicalize %s", path.string());
                    return NOERROR;
                }
            if (path.StartWith(legacyPath)) {
                Int32 llength = legacyPath.GetLength();
                path = externalStoragePath + path.Substring(llength);
            }

            Logger::D(TAG, "action: %s path: %s", action.string(), path.string());
            if (IIntent::ACTION_MEDIA_MOUNTED.Equals(action)) {
                // scan whenever any volume is mounted
                Scan(context, MediaProvider::EXTERNAL_VOLUME);
            } else if (IIntent::ACTION_MEDIA_SCANNER_SCAN_FILE.Equals(action) &&
                    !path.IsNull() && path.StartWith(externalStoragePath + "/")) {
                ScanFile(context, path);
            }
        }
    }
    return NOERROR;
}

void MediaScannerReceiver::Scan(
    /* [in] */ IContext* context,
    /* [in] */ const String& volume)
{
    AutoPtr<IBundle> args;
    CBundle::New((IBundle**)&args);
    args->PutString(String("volume"), volume);
    AutoPtr<IIntent> intent;
    // CIntent::New(context, ECLSID_CMediaScannerService, (IIntent**)&intent);
    intent->PutExtras(args.Get());
    AutoPtr<IComponentName> cn;
    context->StartService(intent.Get(), (IComponentName**)&cn);
}

void MediaScannerReceiver::ScanFile(
    /* [in] */ IContext* context,
    /* [in] */ const String& path)
{
    AutoPtr<IBundle> args;
    CBundle::New((IBundle**)&args);
    args->PutString(String("filepath"), path);
    AutoPtr<IIntent> intent;
    // CIntent::New(context, ECLSID_CMediaScannerService, (IIntent**)&intent);
    intent->PutExtras(args.Get());
    AutoPtr<IComponentName> cn;
    context->StartService(intent.Get(), (IComponentName**)&cn);
}

} // namespace Media
} // namespace Providers
} // namespace Droid
} // namespace Elastos