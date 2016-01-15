
#ifndef __ELASTOS_DROID_CONTENT_CINTENTSHORTCUTICONRESOURCE_H__
#define __ELASTOS_DROID_CONTENT_CINTENTSHORTCUTICONRESOURCE_H__

#include "_Elastos_Droid_Content_CIntentShortcutIconResource.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CIntentShortcutIconResource)
    , public Object
    , public IIntentShortcutIconResource
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI GetPackageName(
        /* [out] */ String* packageName);

    CARAPI SetPackageName(
        /* [in] */ const String& packageName);

    CARAPI GetResourceName(
        /* [out] */ String* resourceName);

    CARAPI SetResourceName(
        /* [in] */ const String& resourceName);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    /**
     * The package name of the application containing the icon.
     */
    String mPackageName;

    /**
     * The resource name of the icon, including package, name and type.
     */
    String mResourceName;

};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CINTENTSHORTCUTICONRESOURCE_H__
