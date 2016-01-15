
#ifndef __ELASTOS_DROID_SERVER_DISPLAY_PERSISTENTDATASTORE_H__
#define __ELASTOS_DROID_SERVER_DISPLAY_PERSISTENTDATASTORE_H__

#include "_Elastos.Droid.Server.h"
#include <Elastos.Droid.Hardware.h>
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Hardware::Display::IWifiDisplay;
using Elastos::Droid::Utility::IAtomicFile;
using Elastos::Utility::Etl::List;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlSerializer;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

/**
 * Manages persistent state recorded by the display manager service as an XML file.
 * Caller must acquire lock on the data store before accessing it.
 *
 * File format:
 * <code>
 * &lt;display-manager-state>
 *   &lt;remembered-wifi-displays>
 *     &lt;wifi-display deviceAddress="00:00:00:00:00:00" deviceName="XXXX" deviceAlias="YYYY" />
 *   &gt;remembered-wifi-displays>
 * &gt;/display-manager-state>
 * </code>
 *
 * TODO: refactor this to extract common code shared with the input manager's data store
 */
class PersistentDataStore : public Object
{
public:
    PersistentDataStore();

    CARAPI_(void) SaveIfNeeded();

    CARAPI_(AutoPtr<IWifiDisplay>) GetRememberedWifiDisplay(
        /* [in] */ const String& deviceAddress);

    CARAPI_(AutoPtr< ArrayOf<IWifiDisplay*> >) GetRememberedWifiDisplays();

    CARAPI_(AutoPtr<IWifiDisplay>) ApplyWifiDisplayAlias(
        /* [in] */ IWifiDisplay* display);

    CARAPI_(AutoPtr< ArrayOf<IWifiDisplay*> >) ApplyWifiDisplayAliases(
        /* [in] */ ArrayOf<IWifiDisplay*>* displays);

    CARAPI_(Boolean) RememberWifiDisplay(
        /* [in] */ IWifiDisplay* display);

    CARAPI_(Boolean) ForgetWifiDisplay(
        /* [in] */ const String& deviceAddress);

private:
    CARAPI_(List< AutoPtr<IWifiDisplay> >::Iterator) FindRememberedWifiDisplay(
        /* [in] */ const String& deviceAddress);

    CARAPI_(void) LoadIfNeeded();

    CARAPI_(void) SetDirty();

    CARAPI_(void) ClearState();

    CARAPI_(void) Load();

    CARAPI_(void) Save();

    CARAPI LoadFromXml(
        /* [in] */ IXmlPullParser* parser);

    CARAPI LoadRememberedWifiDisplaysFromXml(
        /* [in] */ IXmlPullParser* parser);

    CARAPI SaveToXml(
        /* [in] */ IXmlSerializer* serializer);

protected:
    static const String TAG;

private:
    // Remembered Wifi display devices.
    List< AutoPtr<IWifiDisplay> > mRememberedWifiDisplays;

    // The atomic file used to safely read or write the file.
    AutoPtr<IAtomicFile> mAtomicFile;

    // True if the data has been loaded.
    Boolean mLoaded;

    // True if there are changes to be saved.
    Boolean mDirty;
};

} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_DISPLAY_PERSISTENTDATASTORE_H__
