
#ifndef __ELASTOS_DROID_UTILITY_NATIVETEXTHELPER_H__
#define __ELASTOS_DROID_UTILITY_NATIVETEXTHELPER_H__

#include "Elastos.Droid.Utility.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

using Elastos::Droid::Content::IContext;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Utility {

/**
 *@hide
 */
class NativeTextHelper
    : public Object
{
public:
    /**
     * parse the string to current language string in public resources.
     *
     * @param context base context of the application
     * @param originalString original string
     * @param originNamesId the id of the original string array.
     * @param localNamesId the id of the local string keys.
     * @return local language string
     */
    static CARAPI GetLocalString(
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
    static CARAPI GetInternalLocalString(
        /* [in] */ IContext* context,
        /* [in] */ const String& originalString,
        /* [in] */ Int32 originNamesId,
        /* [in] */ Int32 localNamesId,
        /* [out] */ String* result);

private:
    /**
     * parse the string to current language.
     *
     * @param context base context of the application
     * @param originalString original string
     * @param defPackage the target package where the local language strings
     *            defined
     * @param originNamesId the id of the original string array.
     * @param localNamesId the id of the local string keys.
     * @return local language string
     */
    static CARAPI GetLocalString(
        /* [in] */ IContext* context,
        /* [in] */ const String& originalString,
        /* [in] */ const String& defPackage,
        /* [in] */ Int32 originNamesId,
        /* [in] */ Int32 localNamesId,
        /* [out] */ String* result);
};

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_UTILITY_NATIVETEXTHELPER_H__
