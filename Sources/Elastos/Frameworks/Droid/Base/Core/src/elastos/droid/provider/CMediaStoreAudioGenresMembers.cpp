#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/net/CUriHelper.h"
#include "elastos/droid/provider/CMediaStoreAudioGenresMembers.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CMediaStoreAudioGenresMembers)

CAR_INTERFACE_IMPL(CMediaStoreAudioGenresMembers, Singleton, IMediaStoreAudioGenresMembers)

ECode CMediaStoreAudioGenresMembers::GetContentUri(
    /* [in] */ const String& volumeName,
    /* [in] */ Int64 genreId,
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    StringBuilder builder;
    builder += IMediaStore::CONTENT_AUTHORITY_SLASH;
    builder += volumeName;
    builder += "/audio/genres/";
    builder += genreId;
    builder += "/members";
    String str = builder.ToString();
    return helper->Parse(str, uri);
}

} //namespace Provider
} //namespace Droid
} //namespace Elastos
