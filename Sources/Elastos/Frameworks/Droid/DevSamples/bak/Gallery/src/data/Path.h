
#ifndef __PATH_H__
#define __PATH_H__

#include "elastos/droid/app/Activity.h"
#include <elastos/utility/etl/HashMap.h>
#include "MediaObject.h"

using Elastos::Core::HashMap;
using Elastos::Utility::Etl::HashMap;

namespace Gallery {

class MediaObject;

class Path
    : public ElRefBase
    , public IInterface
{
public:
    static CARAPI_(AutoPtr<Path>) FromString(
        /* [in] */ const String& s);

    static CARAPI_(AutoPtr< ArrayOf<String> >) Split(
        /* [in] */ const String& s);

    // Splits a string to an array of strings.
    // For example, "{foo,bar,baz}" -> {"foo","bar","baz"}.
    static CARAPI_(AutoPtr< ArrayOf<String> >) SplitSequence(
        /* [in] */ const String& s);

public:
    CAR_INTERFACE_DECL()

    CARAPI_(AutoPtr<Path>) GetChild(
        /* [in] */ const String& segment);

    CARAPI_(AutoPtr<Path>) GetParent();

    CARAPI_(AutoPtr<Path>) GetChild(
        /* [in] */ Int32 segment);

    CARAPI_(AutoPtr<Path>) GetChild(
        /* [in] */ Int64 segment);

    CARAPI_(void) SetObject(
        /* [in] */ MediaObject* object);

    CARAPI_(AutoPtr<MediaObject>) GetObject();

    CARAPI_(String) ToString();

    CARAPI_(Boolean) EqualsIgnoreCase(
        /* [in] */ const String& p);

    CARAPI_(AutoPtr< ArrayOf<String> >) Split();

    CARAPI_(String) GetPrefix();

    CARAPI_(AutoPtr<Path>) GetPrefixPath();

    CARAPI_(String) GetSuffix();

private:
    Path(
        /* [in] */ Path* parent,
        /* [in] */ const String& segment);

public:
    static AutoPtr<Path> ITEM_PATH;

private:
    static const String TAG;
    static AutoPtr<Path> sRoot;
    static Object sPathLock;

    AutoPtr<Path> mParent;
    String mSegment;
    // WeakReference<MediaObject> mObject;
    AutoPtr<MediaObject> mObject;
    // IdentityCache<String, Path> mChildren;
    HashMap<String, AutoPtr<Path> > mChildren;

};

} // namespace Gallery

#endif // __PATH_H__
