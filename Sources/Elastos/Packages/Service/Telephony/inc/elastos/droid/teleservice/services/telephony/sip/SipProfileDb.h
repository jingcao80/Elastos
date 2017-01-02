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

#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_SIP_SIPPROFILEDB_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_SIP_SIPPROFILEDB_H__

#include "_Elastos.Droid.TeleService.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/teleservice/services/telephony/sip/SipSharedPreferences.h"
#include "Elastos.Droid.Content.h"
#include <elastos/core/Object.h>
#include <elastos/core/StringBuffer.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Droid::Content::IContext;
using Elastos::IO::IFile;
using Elastos::Core::Object;
using Elastos::Core::StringBuffer;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {
namespace Sip {

class SipProfileDb
    : public Object
{
public:
    TO_STRING_IMPL("SipProfileDb")

    SipProfileDb(
        /* [in] */ IContext* context);

    // CARAPI DeleteProfile(
    //     /* [in] */ SipProfile p);

    // CARAPI SaveProfile(
    //     /* [in] */ SipProfile p);

    CARAPI GetProfilesCount(
        /* [out] */ Int32* count);

    CARAPI RetrieveSipProfileList(
        /* [out] */ IList** list);

private:
    CARAPI_(void) DeleteProfile(
        /* [in] */ IFile* file);

    CARAPI_(AutoPtr<IList>) RetrieveSipProfileListInternal();

    // CARAPI Deserialize(
    //     /* [in] */ IFile* profileObjectFile,
    //     /* [out] */ SipProfile** profile);

    static CARAPI_(void) Log(
        /* [in] */ const String& msg);

private:
    static const String PREFIX;
    static const Boolean VERBOSE; /* STOP SHIP if true */

    static const String PROFILES_DIR;
    static const String PROFILE_OBJ_FILE;

    String mProfilesDirectory;
    AutoPtr<SipSharedPreferences> mSipSharedPreferences;
    Int32 mProfilesCount;

    static Object sLock;
};

} // namespace Sip
} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_SIP_SIPPROFILEDB_H__