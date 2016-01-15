
#ifndef __ELASTOS_DROID_CONTENT_CINTENTFILTERHELPER_H__
#define __ELASTOS_DROID_CONTENT_CINTENTFILTERHELPER_H__

#include "_Elastos_Droid_Content_CIntentFilterHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CIntentFilterHelper)
    , public Singleton
    , public IIntentFilterHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Create a new IntentFilter instance with a specified action and MIME
     * type, where you know the MIME type is correctly formatted.  This catches
     * the {@link MalformedMimeTypeException} exception that the constructor
     * can call and turns it into a runtime exception.
     *
     * @param action The action to match, i.e. Intent.ACTION_VIEW.
     * @param dataType The type to match, i.e. "vnd.android.cursor.dir/person".
     *
     * @return A new IntentFilter for the given action and type.
     *
     * @see #IntentFilter(String, String)
     */
    CARAPI Create(
        /* [in] */ const String& action,
        /* [in] */ const String& dataType,
        /* [out] */ IIntentFilter** intentFilter);
};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CINTENTFILTERHELPER_H__
