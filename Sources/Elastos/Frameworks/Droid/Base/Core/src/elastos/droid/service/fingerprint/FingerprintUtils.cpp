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

#include "Elastos.Droid.Provider.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/service/fingerprint/FingerprintUtils.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Fingerprint {

const Boolean FingerprintUtils::DEBUG = TRUE;
const String FingerprintUtils::TAG("FingerprintUtils");

ECode FingerprintUtils::GetFingerprintIdsForUser(
    /* [in] */ IContentResolver* res,
    /* [in] */ Int32 userId,
    /* [out, callee] */ ArrayOf<Int32>** ids)
{
    VALIDATE_NOT_NULL(ids)
    String fingerIdsRaw;
    Settings::Secure::GetStringForUser(res, ISettingsSecure::USER_FINGERPRINT_IDS, userId, &fingerIdsRaw);

    AutoPtr<ArrayOf<Int32> > result = NULL;

    if (TextUtils::IsEmpty(fingerIdsRaw)) {
        AutoPtr<ArrayOf<String> > fingerStringIds;
        StringUtils::Split(fingerIdsRaw.Replace('[', ' ').Replace(']', ' '), String(", "), (ArrayOf<String>**)&fingerStringIds);

        result = ArrayOf<Int32>::Alloc(fingerStringIds->GetLength());
        for (Int32 i = 0; i < result->GetLength(); i++) {
            //TODO
            // try {
            //     result[i] = Integer.decode(fingerStringIds[i]);
            // } catch (NumberFormatException e) {
            //     if (DEBUG) Log.d(TAG, "Error when parsing finger id " + fingerStringIds[i]);
            // }
        }
    }
    *ids = result.Get();
    REFCOUNT_ADD(*ids);
    return NOERROR;
}

ECode FingerprintUtils::AddFingerprintIdForUser(
    /* [in] */ Int32 fingerId,
    /* [in] */ IContentResolver* res,
    /* [in] */ Int32 userId)
{
    AutoPtr<ArrayOf<Int32> > fingerIds;
    GetFingerprintIdsForUser(res, userId, (ArrayOf<Int32>**)&fingerIds);

    // FingerId 0 has special meaning.
    if (fingerId == 0) return E_NULL_POINTER_EXCEPTION;

    // Don't allow dups
    for (Int32 i = 0; i < fingerIds->GetLength(); i++) {
        if ((*fingerIds)[i] == fingerId) return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<ArrayOf<Int32> > newList;
    Arrays::CopyOf(fingerIds, fingerIds->GetLength() + 1, (ArrayOf<Int32>**)&newList);
    (*newList)[fingerIds->GetLength()] = fingerId;
    Boolean result;
    Settings::Secure::PutStringForUser(res, ISettingsSecure::USER_FINGERPRINT_IDS,
            Arrays::ToString(newList), userId, &result);
    return NOERROR;
}

ECode FingerprintUtils::RemoveFingerprintIdForUser(
    /* [in] */ Int32 fingerId,
    /* [in] */ IContentResolver* res,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* ret)
{
    // FingerId 0 has special meaning. The HAL layer is supposed to remove each finger one
    // at a time and invoke notify() for each fingerId.  If we get called with 0 here, it means
    // something bad has happened.
    if (fingerId == 0) {
        Logger::E(TAG, "Bad fingerId");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<ArrayOf<Int32> > fingerIds;
    GetFingerprintIdsForUser(res, userId, (ArrayOf<Int32>**)&fingerIds);
    AutoPtr<ArrayOf<Int32> > resultIds;
    Arrays::CopyOf(fingerIds, fingerIds->GetLength(), (ArrayOf<Int32>**)&resultIds);
    Int32 resultCount = 0;
    for (Int32 i = 0; i < fingerIds->GetLength(); i++) {
        if (fingerId != (*fingerIds)[i]) {
            (*resultIds)[resultCount++] = (*fingerIds)[i];
        }
    }
    if (resultCount > 0) {
        AutoPtr<ArrayOf<Int32> > copyids;
        Arrays::CopyOf(resultIds, resultCount, (ArrayOf<Int32>**)&copyids);
        Boolean result;
        Settings::Secure::PutStringForUser(res, ISettingsSecure::USER_FINGERPRINT_IDS,
                Arrays::ToString(copyids), userId, &result);
        *ret = TRUE;
        return NOERROR;
    }
    *ret = FALSE;
    return NOERROR;
}

} // namespace Fingerprint
} // namespace Service
} // namepsace Droid
} // namespace Elastos