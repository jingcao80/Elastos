
#include "Elastos.Droid.Media.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/media/tv/TvContractChannels.h"
#include "elastos/droid/media/tv/TvContract.h"
#include "elastos/droid/net/CUriHelper.h"

using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::CHashMap;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

static AutoPtr<IHashMap> Init()
{
    AutoPtr<IHashMap> map;
    CHashMap::New((IHashMap**)&map);
    AutoPtr<ICharSequence> cs1, cs2;
    CString::New(ITvContractChannels::VIDEO_FORMAT_480I, (ICharSequence**)&cs1);
    CString::New(ITvContractChannels::VIDEO_RESOLUTION_SD, (ICharSequence**)&cs2);
    map->Put(cs1, cs2);
    cs1 = NULL;
    cs2 = NULL;
    CString::New(ITvContractChannels::VIDEO_FORMAT_480P, (ICharSequence**)&cs1);
    CString::New(ITvContractChannels::VIDEO_RESOLUTION_ED, (ICharSequence**)&cs2);
    map->Put(cs1, cs2);
    cs1 = NULL;
    cs2 = NULL;
    CString::New(ITvContractChannels::VIDEO_FORMAT_576I, (ICharSequence**)&cs1);
    CString::New(ITvContractChannels::VIDEO_RESOLUTION_SD, (ICharSequence**)&cs2);
    map->Put(cs1, cs2);
    cs1 = NULL;
    cs2 = NULL;
    CString::New(ITvContractChannels::VIDEO_FORMAT_576P, (ICharSequence**)&cs1);
    CString::New(ITvContractChannels::VIDEO_RESOLUTION_ED, (ICharSequence**)&cs2);
    map->Put(cs1, cs2);
    cs1 = NULL;
    cs2 = NULL;
    CString::New(ITvContractChannels::VIDEO_FORMAT_720P, (ICharSequence**)&cs1);
    CString::New(ITvContractChannels::VIDEO_RESOLUTION_HD, (ICharSequence**)&cs2);
    map->Put(cs1, cs2);
    cs1 = NULL;
    cs2 = NULL;
    CString::New(ITvContractChannels::VIDEO_FORMAT_1080I, (ICharSequence**)&cs1);
    CString::New(ITvContractChannels::VIDEO_RESOLUTION_HD, (ICharSequence**)&cs2);
    map->Put(cs1, cs2);
    cs1 = NULL;
    cs2 = NULL;
    CString::New(ITvContractChannels::VIDEO_FORMAT_1080P, (ICharSequence**)&cs1);
    CString::New(ITvContractChannels::VIDEO_RESOLUTION_FHD, (ICharSequence**)&cs2);
    map->Put(cs1, cs2);
    cs1 = NULL;
    cs2 = NULL;
    CString::New(ITvContractChannels::VIDEO_FORMAT_2160P, (ICharSequence**)&cs1);
    CString::New(ITvContractChannels::VIDEO_RESOLUTION_UHD, (ICharSequence**)&cs2);
    map->Put(cs1, cs2);
    cs1 = NULL;
    cs2 = NULL;
    CString::New(ITvContractChannels::VIDEO_FORMAT_4320P, (ICharSequence**)&cs1);
    CString::New(ITvContractChannels::VIDEO_RESOLUTION_UHD, (ICharSequence**)&cs2);
    map->Put(cs1, cs2);

    return map;
}

AutoPtr<IHashMap> TvContractChannels::VIDEO_FORMAT_TO_RESOLUTION_MAP = Init();

TvContractChannels::TvContractChannels()
{
}

ECode TvContractChannels::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    AutoPtr<IUriHelper> uriHelper;
    CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
    return uriHelper->Parse(String("content://")
         + ITvContract::AUTHORITY + "/" + TvContract::PATH_CHANNEL, uri);
}

ECode TvContractChannels::GetVideoResolution(
    /* [in] */ const String& videoFormat,
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ICharSequence> csq;
    CString::New(videoFormat, (ICharSequence**)&csq);
    AutoPtr<ICharSequence> cs;
    VIDEO_FORMAT_TO_RESOLUTION_MAP->Get(csq, (IInterface**)&cs);
    return cs->ToString(result);
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
