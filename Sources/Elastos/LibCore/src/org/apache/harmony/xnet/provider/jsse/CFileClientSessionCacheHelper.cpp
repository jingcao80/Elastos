
#include "CFileClientSessionCacheHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CAR_SINGLETON_IMPL(CFileClientSessionCacheHelper)

CAR_INTERFACE_IMPL(CFileClientSessionCacheHelper, Singleton, IFileClientSessionCacheHelper)

ECode CFileClientSessionCacheHelper::UsingDirectory(
    /* [in] */ Elastos::IO::IFile * pDirectory,
    /* [out] */ Org::Apache::Harmony::Xnet::Provider::Jsse::ISSLClientSessionCache ** ppCache)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}
}

