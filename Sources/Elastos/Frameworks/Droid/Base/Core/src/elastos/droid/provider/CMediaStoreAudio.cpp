
#include "Elastos.Droid.Database.h"
#include "elastos/droid/database/DatabaseUtils.h"
#include "elastos/droid/provider/CMediaStoreAudio.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Database::DatabaseUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CMediaStoreAudio)

CAR_INTERFACE_IMPL(CMediaStoreAudio, Singleton, IMediaStoreAudio)

ECode CMediaStoreAudio::KeyFor(
    /* [in] */ const String& cname,
    /* [out] */ String* keyvalue)
{
    String name = cname;
    if (name != NULL) {
        Boolean sortfirst = FALSE;
        if (name.Equals(IMediaStore::UNKNOWN_STRING)) {
            *keyvalue = String("\001");
            return NOERROR;
        }
        // Check if the first character is \001. We use this to
        // force sorting of certain special files, like the silent ringtone.
        if (name.StartWith("\001")) {
            sortfirst = TRUE;
        }
        name = name.Trim().ToLowerCase();
        if (name.StartWith("the ")) {
            name = name.Substring(4);
        }
        if (name.StartWith("an ")) {
            name = name.Substring(3);
        }
        if (name.StartWith("a ")) {
            name = name.Substring(2);
        }
        if (name.EndWith(", the") || name.EndWith(",the") ||
            name.EndWith(", an") || name.EndWith(",an") ||
            name.EndWith(", a") || name.EndWith(",a")) {
            name = name.Substring(0, name.LastIndexOf(','));
        }

        String result;
        StringUtils::ReplaceAll(name, String("[\\[\\]\\(\\)\"'.,?!]"), String(""), &result);
        name = result.Trim();
        if (name.GetLength() > 0) {
            // Insert a separator between the characters to avoid
            // matches on a partial character. If we ever change
            // to start-of-word-only matches, this can be removed.
            StringBuilder b;// = new StringBuilder();
            b += '.';
            Int32 nl = name.GetLength();
            for (Int32 i = 0; i < nl; i++) {
                b += name[i];
                b += '.';
            }
            name = b.ToString();
            String key = DatabaseUtils::GetCollationKey(name);
            if (sortfirst) {
                key = String("\001") + key;
            }
            *keyvalue = key;
            return NOERROR;
       } else {
            *keyvalue = String("");
            return NOERROR;
        }
    }

    *keyvalue = NULL;
    return NOERROR;
}

} //namespace Provider
} //namespace Droid
} //namespace Elastos
