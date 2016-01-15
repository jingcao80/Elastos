#ifndef __ELASTOS_DROID_PROVIDER_VOICEMAILCONTRACT_H__
#define __ELASTOS_DROID_PROVIDER_VOICEMAILCONTRACT_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

class VoicemailContract
{
public:
    class Voicemails
    {
    public:
        static CARAPI BuildSourceUri(
            /* [in] */ const String& packageName,
            /* [out] */ IUri** uri);

    private:
        Voicemails() {}
    public:
        static const AutoPtr<IUri> CONTENT_URI;
    };

    class Status
    {
    public:
        static CARAPI BuildSourceUri(
            /* [in] */ const String& packageName,
            /* [out] */ IUri** uri);

    private:
        Status() {}

    public:
        static const AutoPtr<IUri> CONTENT_URI;
    };

private:
    /** Not instantiable. */
    VoicemailContract() {}
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_VOICEMAILCONTRACT_H__