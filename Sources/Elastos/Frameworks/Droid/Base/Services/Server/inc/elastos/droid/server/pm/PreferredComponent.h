
#ifndef __ELASTOS_DROID_SERVER_PM_PREFERREDCOMPONENT_H__
#define __ELASTOS_DROID_SERVER_PM_PREFERREDCOMPONENT_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.IO.h>
#include <elastos/core/Object.h>

using Elastos::IO::IPrintWriter;
using Elastos::Utility::IList;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlSerializer;
using Elastos::Droid::Content::IComponentName;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

extern "C" const InterfaceID EIID_IPreferredComponentCallbacks;

class PreferredComponent : public Object
{
public:
    interface ICallbacks : public IInterface
    {
    public:
        virtual CARAPI OnReadTag(
            /* [in] */ const String& tagName,
            /* [in] */ IXmlPullParser* parser,
            /* [out] */ Boolean* result) = 0;
    };

public:
    PreferredComponent(
        /* [in] */ ICallbacks* callbacks,
        /* [in] */ Int32 match,
        /* [in] */ ArrayOf<IComponentName*>* set,
        /* [in] */ IComponentName* component,
        /* [in] */ Boolean always);

    PreferredComponent(
        /* [in] */ ICallbacks* callbacks,
        /* [in] */ IXmlPullParser* parser);

    CARAPI_(String) GetParseError();

    CARAPI WriteToXml(
        /* [in] */ IXmlSerializer* serializer,
        /* [in] */ Boolean full);

    CARAPI_(Boolean) SameSet(
        /* [in] */ IList* query);

    CARAPI_(Boolean) SameSet(
        /* [in] */ ArrayOf<IComponentName*>* comps);

    CARAPI_(void) Dump(
        /* [in] */ IPrintWriter* out,
        /* [in] */ const String& prefix,
        /* [in] */ IInterface* ident);

public:
    Int32 mMatch;
    AutoPtr<IComponentName> mComponent;
    // Whether this is to be the one that's always chosen. If false, it's the most recently chosen.
    Boolean mAlways;

    AutoPtr<ArrayOf<String> > mSetPackages;
    AutoPtr<ArrayOf<String> > mSetClasses;
    AutoPtr<ArrayOf<String> > mSetComponents;
    String mShortComponent;

private:
    static const String TAG_SET;
    static const String ATTR_ALWAYS; // boolean
    static const String ATTR_MATCH; // number
    static const String ATTR_NAME; // component name
    static const String ATTR_SET; // number

    String mParseError;
    AutoPtr<ICallbacks> mCallbacks;
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_PM_PREFERREDCOMPONENT_H__
