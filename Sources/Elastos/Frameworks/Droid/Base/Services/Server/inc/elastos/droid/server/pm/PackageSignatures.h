#ifndef __ELASTOS_DROID_SERVER_PM_PACKAGESIGNATURES_H__
#define __ELASTOS_DROID_SERVER_PM_PACKAGESIGNATURES_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlSerializer;

using Elastos::Droid::Content::Pm::ISignature;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

class PackageSignatures : public ElRefBase
{
public:
    PackageSignatures(
        /* [in] */ PackageSignatures* orig);

    PackageSignatures(
        /* [in] */ ArrayOf<ISignature*>* sigs);

    PackageSignatures();

    CARAPI_(void) WriteXml(
        /* [in] */ IXmlSerializer* serializer,
        /* [in] */ const String& tagName,
        /* [in] */ List< AutoPtr<ISignature> >& pastSignatures);

    CARAPI_(void) ReadXml(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ List< AutoPtr<ISignature> >& pastSignatures);

    CARAPI_(void) AssignSignatures(
        /* [in] */ ArrayOf<ISignature*>* sigs);

public:
    AutoPtr< ArrayOf<ISignature*> > mSignatures;
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_PM_PACKAGESIGNATURES_H__
