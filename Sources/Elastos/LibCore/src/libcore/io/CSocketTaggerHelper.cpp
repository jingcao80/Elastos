
#include "CSocketTaggerHelper.h"
#include "SocketTagger.h"

namespace Libcore {
namespace IO {

CAR_SINGLETON_IMPL(CSocketTaggerHelper)

CAR_INTERFACE_IMPL(CSocketTaggerHelper, Singleton, ISocketTaggerHelper)

ECode CSocketTaggerHelper::Set(
    /* [in] */ ISocketTagger* tagger)
{
    return SocketTagger::Set(tagger);
}

ECode CSocketTaggerHelper::Get(
    /* [out] */ ISocketTagger** tagger)
{
    return SocketTagger::Get(tagger);
}

} // namespace IO
} // namespace Libcore