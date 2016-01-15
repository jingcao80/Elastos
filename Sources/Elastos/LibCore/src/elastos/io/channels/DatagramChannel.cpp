
#include "DatagramChannel.h"
#include "SelectionKey.h"
#include "SelectorProvider.h"

using Elastos::IO::Channels::Spi::SelectorProvider;

namespace Elastos {
namespace IO {
namespace Channels {

CAR_INTERFACE_IMPL_4(DatagramChannel, Object, IDatagramChannel, IByteChannel, IScatteringByteChannel, IGatheringByteChannel)

DatagramChannel::DatagramChannel()
{}

ECode DatagramChannel::constructor(
    /* [in] */ ISelectorProvider* provider)
{
    return AbstractSelectableChannel::constructor(provider);
}

DatagramChannel::~DatagramChannel()
{
}

ECode DatagramChannel::Open(
    /* [out] */ IDatagramChannel** channel)
{
    VALIDATE_NOT_NULL(channel)

    AutoPtr<ISelectorProvider> sp;
    FAIL_RETURN(SelectorProvider::GetProvider((ISelectorProvider**)&sp));
    return sp->OpenDatagramChannel(channel);
}

ECode DatagramChannel::GetValidOps(
    /* [out] */ Int32* value)
{
    return (SelectionKey::sOP_READ | SelectionKey::sOP_WRITE);
}

ECode DatagramChannel::Read(
    /* [in] */ ArrayOf<IByteBuffer*>* targets,
    /* [out] */ Int64* nRead)
{
    return Read(targets, 0, targets->GetLength(), nRead);
}

ECode DatagramChannel::Write(
    /* [in] */ ArrayOf<IByteBuffer*>* targets,
    /* [out] */ Int64* nWrite)
{
    return Write(targets, 0, targets->GetLength(), nWrite);
}

} // namespace Channels
} // namespace IO
} // namespace Elastos
