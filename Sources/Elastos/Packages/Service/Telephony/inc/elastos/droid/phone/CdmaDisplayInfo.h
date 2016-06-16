#ifndef  __ELASTOS_DROID_PHONE_CDMADISPLAYINFO_H__
#define  __ELASTOS_DROID_PHONE_CDMADISPLAYINFO_H__

#include "_Elastos.Droid.Server.Telephony.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
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
    static const String LOG_TAG = "CdmaDisplayInfo";
    static Boolean DBG = (SystemProperties.getInt("ro.debuggable", 0) == 1);

    /** CDMA DisplayInfo dialog */
    static AutoPtr<IAlertDialog> sDisplayInfoDialog;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CDMADISPLAYINFO_H__