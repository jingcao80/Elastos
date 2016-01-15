#include "Pipe.h"
#include "SelectorProvider.h"

using Elastos::IO::Channels::Spi::ISelectorProvider;
using Elastos::IO::Channels::Spi::SelectorProvider;

namespace Elastos{
namespace IO{
namespace Channels{

Pipe::Pipe()
{
}

CAR_INTERFACE_IMPL(Pipe, Object, IPipe)

ECode Pipe::Open(
    /* [out] */ IPipe** pipe)
{
    AutoPtr<ISelectorProvider> sp;
    FAIL_RETURN(SelectorProvider::GetProvider((ISelectorProvider**)&sp));
    return sp->OpenPipe(pipe);
}

} // namespace Channels
} // namespace IO
} // namespace Elastoss
