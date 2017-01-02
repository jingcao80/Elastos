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

#ifndef __ELASTOS_DROID_SERVER_TELECOM_BLACKLIST_H__
#define __ELASTOS_DROID_SERVER_TELECOM_BLACKLIST_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Content::IContext;
using Elastos::IO::IObjectInput;
using Elastos::IO::IObjectOutput;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * This class used to handle the blacklist data. Its
 * only remaining purpose is legacy data migration
 */
class Blacklist
    : public Object
{
private:
    class PhoneNumber
        : public Object
        // , public IExternalizable
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor();

        CARAPI WriteExternal(
            /* [in] */ IObjectOutput* out);

        CARAPI ReadExternal(
            /* [in] */ IObjectInput* in);

        // @Override
        CARAPI HashCode(
            /* [out] */ Int32* result);

        static const Int64 serialVersionUID;

        String mPhone;
    };

public:
    static CARAPI MigrateOldDataIfPresent(
        /* [in] */ IContext* context);

private:
    static const String BLFILE;

    static const Int32 BLFILE_VER;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_BLACKLIST_H__
