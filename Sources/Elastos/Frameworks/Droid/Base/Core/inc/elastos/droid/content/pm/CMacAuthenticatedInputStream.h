
#ifndef __ELASTOS_DROID_CONTENT_PM_CMACAUTHENTICATEDINPUTSTREAM_H__
#define __ELASTOS_DROID_CONTENT_PM_CMACAUTHENTICATEDINPUTSTREAM_H__

#include "_Elastos_Droid_Content_Pm_CMacAuthenticatedInputStream.h"
#include "elastos/io/FilterInputStream.h"

using Elastosx::Crypto::IMac;
using Elastos::IO::IInputStream;
using Elastos::IO::FilterInputStream;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * An input stream filter that applies a MAC to the data passing through it. At
 * the end of the data that should be authenticated, the tag can be calculated.
 * After that, the stream should not be used.
 *
 * @hide
 */
CarClass(CMacAuthenticatedInputStream)
    , public FilterInputStream
    , public IMacAuthenticatedInputStream
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CMacAuthenticatedInputStream();

    virtual ~CMacAuthenticatedInputStream();

    CARAPI constructor(
        /* [in] */ IInputStream* inStream,
        /* [in] */ IMac* mac);

    CARAPI IsTagEqual(
        /* [in] */ ArrayOf<Byte>* tag,
        /* [out] */ Boolean* result);

    CARAPI Read(
        /* [out] */ Int32* value);

    CARAPI Read(
        /* [out] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [out] */ Int32* number);

private:
    AutoPtr<IMac> mMac;
};

}
}
}
}

#endif // __ELASTOS_DROID_CONTENT_PM_CMACAUTHENTICATEDINPUTSTREAM_H__
