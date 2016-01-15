#ifndef __ELASTOS_DROID_SERVER_WM_DISPLAYSETTINGS_H__
#define __ELASTOS_DROID_SERVER_WM_DISPLAYSETTINGS_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Utility::IAtomicFile;
using Org::Xmlpull::V1::IXmlPullParser;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAtomicFile;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

/**
 * Current persistent settings about a display
 */
class DisplaySettings : public Object
{
public:
    class Entry : public Object
    {
    public:
        Entry(
            /* [in] */ const String& name)
            : mName(name)
            , mOverscanLeft(0)
            , mOverscanTop(0)
            , mOverscanRight(0)
            , mOverscanBottom(0)
        {}

    public:
        String mName;
        Int32 mOverscanLeft;
        Int32 mOverscanTop;
        Int32 mOverscanRight;
        Int32 mOverscanBottom;
    };

public:
    DisplaySettings(
        /* [in] */ IContext* context);

    CARAPI_(void) GetOverscanLocked(
        /* [in] */ const String& name,
        /* [in] */ IRect* outRect);

    CARAPI_(void) SetOverscanLocked(
        /* [in] */ const String* name,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    CARAPI ReadSettingsLocked();

    CARAPI_(void) WriteSettingsLocked();

private:
    CARAPI_(Int32) GetIntAttribute(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ const String& name);

    ECode ReadDisplay(
        /* [in] */ IXmlPullParser* parser);

private:
    static const String TAG;

    AutoPtr<IContext> mContext;
    AutoPtr<IAtomicFile> mFile;
    HashMap<String, AutoPtr<Entry> > mEntries;
};

} // Wm
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_WM_DISPLAYSETTINGS_H__
