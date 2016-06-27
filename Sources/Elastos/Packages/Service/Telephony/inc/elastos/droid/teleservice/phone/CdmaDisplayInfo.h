#ifndef  __ELASTOS_DROID_PHONE_CDMADISPLAYINFO_H__
#define  __ELASTOS_DROID_PHONE_CDMADISPLAYINFO_H__

#include "_Elastos.Droid.TeleService.h"
#include "Elastos.Droid.App.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * Helper class for displaying the DisplayInfo sent by CDMA network.
 */
class CdmaDisplayInfo
    : public Object
{
public:
    TO_STRING_IMPL("CdmaDisplayInfo")

    /**
     * Handle the DisplayInfo record and display the alert dialog with
     * the network message.
     *
     * @param context context to get strings.
     * @param infoMsg Text message from Network.
     */
    static void DisplayInfoRecord(
        /* [in] */ IContext* context,
        /* [in] */ const String& infoMsg);

    /**
     * Dismiss the DisplayInfo record
     */
    static CARAPI_(void) DismissDisplayInfoRecord();

private:
    static CARAPI_(void) Log(
        /* [in] */ const String& msg);

private:
    static const String TAG;
    static Boolean DBG;

    /** CDMA DisplayInfo dialog */
    static AutoPtr<IAlertDialog> sDisplayInfoDialog;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CDMADISPLAYINFO_H__