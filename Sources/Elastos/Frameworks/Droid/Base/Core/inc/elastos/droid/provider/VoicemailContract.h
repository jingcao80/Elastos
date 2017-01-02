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