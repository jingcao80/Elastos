#ifndef __ELASTOS_DROID_MEDIA_TV_CTvStreamConfigBUILDER_H__
#define __ELASTOS_DROID_MEDIA_TV_CTvStreamConfigBUILDER_H__

#include "_Elastos_Droid_Media_Tv_CTvStreamConfigBuilder.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::IInteger32;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

/**
 * A helper class for creating a TvStreamConfig object.
 */
CarClass(CTvStreamConfigBuilder)
    , public Object
    , public ITvStreamConfigBuilder
{
public:
    CTvStreamConfigBuilder();

    virtual ~CTvStreamConfigBuilder();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI StreamId(
        /* [in] */ Int32 streamId);

    CARAPI Type(
        /* [in] */ Int32 type);

    CARAPI MaxWidth(
        /* [in] */ Int32 maxWidth);

    CARAPI MaxHeight(
        /* [in] */ Int32 maxHeight);

    CARAPI Generation(
        /* [in] */ Int32 generation);

    CARAPI Build(
        /* [out] */ ITvStreamConfig** result);

public:
    AutoPtr<IInteger32> mStreamId;
    AutoPtr<IInteger32> mType;
    AutoPtr<IInteger32> mMaxWidth;
    AutoPtr<IInteger32> mMaxHeight;
    AutoPtr<IInteger32> mGeneration;
};

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_TV_CTvStreamConfigBUILDER_H__
