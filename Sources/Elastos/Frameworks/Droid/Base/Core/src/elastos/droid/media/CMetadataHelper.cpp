
#include "elastos/droid/media/CMetadataHelper.h"
#include "elastos/droid/media/CMetadata.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_SINGLETON_IMPL(CMetadataHelper)

CAR_INTERFACE_IMPL(CMetadataHelper, Singleton, IMetadataHelper)

ECode CMetadataHelper::LastSytemId(
    /* [out] */ Int32* result)
{
    return CMetadata::LastSytemId(result);
}

ECode CMetadataHelper::FirstCustomId(
    /* [out] */ Int32* result)
{
    return CMetadata::FirstCustomId(result);
}

ECode CMetadataHelper::LastType(
    /* [out] */ Int32* result)
{
    return CMetadata::LastType(result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
