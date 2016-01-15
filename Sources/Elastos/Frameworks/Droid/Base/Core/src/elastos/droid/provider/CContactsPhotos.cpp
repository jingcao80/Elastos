#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CContactsPhotos.h"

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::Uri;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsPhotos)

CAR_INTERFACE_IMPL_4(CContactsPhotos, Singleton, IContactsPhotos, IBaseColumns, IContactsPhotosColumns, ISyncConstValue)

ECode CContactsPhotos::constructor()
{
    return NOERROR;
}

ECode CContactsPhotos::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::Parse(String("content://contacts/photos"), uri);
}

} // Provider
} // Droid
} // Elastos