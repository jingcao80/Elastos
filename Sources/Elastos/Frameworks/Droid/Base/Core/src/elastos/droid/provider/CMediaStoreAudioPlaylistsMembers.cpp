#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/net/CUriHelper.h"
#include "elastos/droid/provider/CMediaStoreAudioPlaylistsMembers.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriBuilder;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Core::CInteger32;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CMediaStoreAudioPlaylistsMembers)

CAR_INTERFACE_IMPL(CMediaStoreAudioPlaylistsMembers, Singleton, IMediaStoreAudioPlaylistsMembers)

ECode CMediaStoreAudioPlaylistsMembers::GetContentUri(
    /* [in] */ const String& volumeName,
    /* [in] */ Int64 playlistId,
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    StringBuilder builder;
    builder += IMediaStore::CONTENT_AUTHORITY_SLASH;
    builder += volumeName;
    builder += "/audio/playlists/";
    builder += playlistId;
    builder += "/members";
    String str = builder.ToString();
    return helper->Parse(str, uri);
}

ECode CMediaStoreAudioPlaylistsMembers::MoveItem(
    /* [in] */ IContentResolver* res,
    /* [in] */ Int64 playlistId,
    /* [in] */ Int32 from,
    /* [in] */ Int32 to,
    /* [out] */ Boolean* result)
{
    AutoPtr<IUri> uri;
    /*AutoPtr<IMediaStoreAudioPlaylistsMembers> members;
    CMediaStoreAudioPlaylistsMembers::AcquireSingleton((IMediaStoreAudioPlaylistsMembers**)&members);
    members->*/
    GetContentUri(String("external"), playlistId, (IUri**)&uri);

    AutoPtr<IUriBuilder> builder;
    uri->BuildUpon((IUriBuilder**)&builder);
    builder->AppendEncodedPath(StringUtils::ToString(from));
    builder->AppendQueryParameter(String("move"), String("true"));
    uri = NULL;
    builder->Build((IUri**)&uri);

    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    AutoPtr<IInteger32> iTo;
    CInteger32::New(to, (IInteger32**)&iTo);
    values->Put(IMediaStoreAudioPlaylistsMembers::PLAY_ORDER, iTo);
    Int32 rows;
    res->Update(uri, values, String(NULL), NULL, &rows);
    *result = rows != 0;
    return NOERROR;
}

} //namespace Provider
} //namespace Droid
} //namespace Elastos
