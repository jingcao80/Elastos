
#ifndef __ELASTOS_DROID_UTILITY_CNATIVETEXTHELPER_H__
#define __ELASTOS_DROID_UTILITY_CNATIVETEXTHELPER_H__

#include "Elastos.Droid.Utility.h"
#include "_Elastos_Droid_Utility_CNativeTextHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::INativeTextHelper;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Utility {

CarClass(CNativeTextHelper)
    , public Singleton
    , public INativeTextHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * parse the string to current language string in public resources.
     *
     * @param context base context of the application
     * @param originalString original string
     * @param originNamesId the id of the original string array.
     * @param localNamesId the id of the local string keys.
     * @return local language string
     */
    CARAPI GetLocalString(
        /* [in] */ IContext* context,
        /* [in] */ const String& originalString,
        /* [in] */ Int32 originNamesId,
        /* [in] */ Int32 localNamesId,
        /* [out] */ String* result);

    /**
     * parse the string to current language string in current resources.
     *
     * @param context base context of the application
     * @param originalString original string
     * @param originNamesId the id of the original string array.
     * @param localNamesId the id of the local string keys.
     * @return local language string
     */
    CARAPI GetInternalLocalString(
        /* [in] */ IContext* context,
        /* [in] */ const String& originalString,
        /* [in] */ Int32 originNamesId,
        /* [in] */ Int32 localNamesId,
        /* [out] */ String* result);
};

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_UTILITY_CNATIVETEXTHELPER_H__
