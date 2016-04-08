#include "PathMatcher.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Gallery {

const String PathMatcher::TAG("PathMatcher");
const Int32 PathMatcher::NOT_FOUND = -1;

PathMatcher::Node::Node()
    : mKind(NOT_FOUND)
{}

CAR_INTERFACE_IMPL(PathMatcher::Node, IInterface)

AutoPtr<PathMatcher::Node> PathMatcher::Node::AddChild(
    /* [in] */ const String& segment)
{
    HashMap<String, AutoPtr<Node> >::Iterator it = mMap.Find(segment);
    if (it != mMap.End()) {
        return it->mSecond;
    }
    AutoPtr<Node> n = new Node();
    mMap.Insert(HashMap<String, AutoPtr<Node> >::ValueType(segment, n));
    return n;
}

AutoPtr<PathMatcher::Node> PathMatcher::Node::GetChild(
    /* [in] */ const String& segment)
{
    HashMap<String, AutoPtr<Node> >::Iterator it = mMap.Find(segment);
    if (it != mMap.End()) {
        return it->mSecond;
    }
    else {
        return NULL;
    }
}

void PathMatcher::Node::SetKind(
    /* [in] */ Int32 kind)
{
    mKind = kind;
}

Int32 PathMatcher::Node::GetKind()
{
    return mKind;
}

PathMatcher::PathMatcher()
{
    mRoot = new Node();
}

CAR_INTERFACE_IMPL(PathMatcher, IInterface)

void PathMatcher::Add(
    /* [in] */ const String& pattern,
    /* [in] */ Int32 kind)
{
    AutoPtr< ArrayOf<String> > segments = Path::Split(pattern);
    AutoPtr<Node> current = mRoot;
    for (Int32 i = 0; i < segments->GetLength(); i++) {
        current = current->AddChild((*segments)[i]);
    }
    current->SetKind(kind);
}

Int32 PathMatcher::Match(
    /* [in] */ Path* path)
{
    AutoPtr< ArrayOf<String> > segments = path->Split();
    mVariables.Clear();
    AutoPtr<Node> current = mRoot;
    for (Int32 i = 0; i < segments->GetLength(); i++) {
        AutoPtr<Node> next = current->GetChild((*segments)[i]);
        if (next == NULL) {
            next = current->GetChild(String("*"));
            if (next != NULL) {
                mVariables.PushBack((*segments)[i]);
            }
            else {
                return NOT_FOUND;
            }
        }
        current = next;
    }
    return current->GetKind();
}

String PathMatcher::GetVar(
    /* [in] */ Int32 index)
{
    return mVariables[index];
}

Int32 PathMatcher::GetInt32Var(
    /* [in] */ Int32 index)
{
    String str = mVariables[index];
    return StringUtils::ParseInt32(str);
}

Int64 PathMatcher::GetInt64Var(
    /* [in] */ Int32 index)
{
    String str = mVariables[index];
    return StringUtils::ParseInt64(str);
}

} // namespace Gallery
