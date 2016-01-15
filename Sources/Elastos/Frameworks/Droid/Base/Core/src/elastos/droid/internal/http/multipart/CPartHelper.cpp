#include "elastos/droid/internal/http/multipart/Part.h"
#include "elastos/droid/internal/http/multipart/CPartHelper.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Http {
namespace Multipart {

CAR_INTERFACE_IMPL(CPartHelper, Singleton, IPartHelper)

CAR_SINGLETON_IMPL(CPartHelper)

ECode CPartHelper::GetBoundary(
    /* [out] */ String* boundary)
{
    return Part::GetBoundary(boundary);
}

ECode CPartHelper::SendParts(
    /* [in] */ IOutputStream* outStream,
    /* [in] */ ArrayOf<IPart*>* parts)
{
    return Part::SendParts(outStream, parts);
}

ECode CPartHelper::SendParts(
    /* [in] */ IOutputStream* outStream,
    /* [in] */ ArrayOf<IPart*>* parts,
    /* [in] */ ArrayOf<Byte>* partBoundary)
{
    return Part::SendParts(outStream, parts, partBoundary);
}

ECode CPartHelper::GetLengthOfParts(
    /* [in] */ ArrayOf<IPart*>* parts,
    /* [out] */ Int64* length)
{
    return Part::GetLengthOfParts(parts, length);
}

ECode CPartHelper::GetLengthOfParts(
    /* [in] */ ArrayOf<IPart*>* parts,
    /* [in] */ ArrayOf<Byte>* partBoundary,
    /* [out] */ Int64* length)
{
    return Part::GetLengthOfParts(parts, partBoundary, length);
}

} // namespace Multipart
} // namespace Http
} // namespace Internal
} // namespace Droid
} // namespace Elastos
