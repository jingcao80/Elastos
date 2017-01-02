//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
