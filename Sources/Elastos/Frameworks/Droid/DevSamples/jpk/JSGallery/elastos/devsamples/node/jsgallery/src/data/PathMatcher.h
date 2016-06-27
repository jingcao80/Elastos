
#ifndef __PATHMATCHER_H__
#define __PATHMATCHER_H__

#include "app/Activity.h"
#include <elastos/HashMap.h>
#include <elastos/List.h>
#include "Path.h"

using Elastos::Utility::HashMap;
using Elastos::Utility::List;

namespace Gallery {

class PathMatcher
    : public ElRefBase
{
private:
    class Node
        : public ElRefBase
        , public IInterface
    {
    public:
        Node();

        CAR_INTERFACE_DECL()

        CARAPI_(AutoPtr<Node>) AddChild(
            /* [in] */ const String& segment);

        CARAPI_(AutoPtr<Node>) GetChild(
            /* [in] */ const String& segment);

        CARAPI_(void) SetKind(
            /* [in] */ Int32 kind);

        CARAPI_(Int32) GetKind();

    private:
        HashMap<String, AutoPtr<Node> > mMap;
        Int32 mKind;

    };

public:
    PathMatcher();

    CAR_INTERFACE_DECL()

    CARAPI_(void) Add(
        /* [in] */ const String& pattern,
        /* [in] */ Int32 kind);

    CARAPI_(Int32) Match(
        /* [in] */ Path* path);

    CARAPI_(String) GetVar(
        /* [in] */ Int32 index);

    CARAPI_(Int32) GetInt32Var(
        /* [in] */ Int32 index);

    CARAPI_(Int64) GetInt64Var(
        /* [in] */ Int32 index);

private:
    static const String TAG;
    static const Int32 NOT_FOUND;

    List<String> mVariables;
    AutoPtr<Node> mRoot;

};

} // namespace Gallery

#endif // __PATHMATCHER_H__
