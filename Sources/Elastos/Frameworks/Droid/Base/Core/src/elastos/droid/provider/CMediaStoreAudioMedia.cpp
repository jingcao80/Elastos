
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/net/CUriHelper.h"
#include "elastos/droid/os/Environment.h"
#include "elastos/droid/provider/CMediaStoreAudioMedia.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Os::Environment;
using Elastos::IO::IFile;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CMediaStoreAudioMedia)

CAR_INTERFACE_IMPL(CMediaStoreAudioMedia, Singleton, IMediaStoreAudioMedia)

AutoPtr<ArrayOf<String> > CMediaStoreAudioMedia::initEXTERNAL_PATHS()
{
    String secondary_storage;
    AutoPtr<ISystem> system;
    // CSystem::AcquireSingleton((ISystem**)&system);
    system->GetEnv(String("SECONDARY_STORAGE"), &secondary_storage);
    AutoPtr<ArrayOf<String> > str;
    if (!secondary_storage.IsNullOrEmpty()) {
        // str = secondary_storage.split(":");
    } else {
        str = NULL;
    }
    return str;
}

const AutoPtr<ArrayOf<String> > CMediaStoreAudioMedia::EXTERNAL_PATHS = CMediaStoreAudioMedia::initEXTERNAL_PATHS();

ECode CMediaStoreAudioMedia::GetINTERNAL_CONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    return GetContentUri(String("internal"), uri);
}

ECode CMediaStoreAudioMedia::GetEXTERNAL_CONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    return GetContentUri(String("external"), uri);
}

ECode CMediaStoreAudioMedia::GetContentUri(
    /* [in] */ const String& volumeName,
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    StringBuilder builder;
    builder += IMediaStore::CONTENT_AUTHORITY_SLASH;
    builder += volumeName;
    builder += "/audio/media";
    String str = builder.ToString();
    return helper->Parse(str, uri);
}

ECode CMediaStoreAudioMedia::GetContentUriForPath(
    /* [in] */ const String& path,
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    Int32 length;
    length = EXTERNAL_PATHS->GetLength();
    for (Int32 i = 0; i < length; ++i) {
        if(path.StartWith((*EXTERNAL_PATHS)[i])) {
            return GetEXTERNAL_CONTENT_URI(uri);
        }
    }

    String str;
    AutoPtr<IFile> file = Environment::GetExternalStorageDirectory();
    file->GetPath(&str);

    if(str.StartWith(path)) {
        return GetINTERNAL_CONTENT_URI(uri);
    } else {
        return GetEXTERNAL_CONTENT_URI(uri);
    }
}

} //namespace Provider
} //namespace Droid
} //namespace Elastos
