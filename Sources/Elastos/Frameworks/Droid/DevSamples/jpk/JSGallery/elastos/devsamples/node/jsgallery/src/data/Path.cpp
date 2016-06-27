#include "Path.h"
#include <elastos/Logger.h>
#include <elastos/StringBuilder.h>
#include <elastos/StringUtils.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Gallery {

const String Path::TAG("Path");
AutoPtr<Path> Path::sRoot = new Path(NULL, String("ROOT"));
Mutex Path::sPathLock;
AutoPtr<Path> Path::ITEM_PATH = FromString(String("/local/image/item"));

AutoPtr<Path> Path::FromString(
    /* [in] */ const String& s)
{
    Logger::D(TAG, "FromString--");
    Mutex::Autolock lock(sPathLock);
    AutoPtr< ArrayOf<String> > segments = Split(s);
    AutoPtr<Path> current = sRoot;
    for (Int32 i = 0; i < segments->GetLength(); i++) {
        Logger::D(TAG, "FromString++segment:%s", (*segments)[i].string());
        current = current->GetChild((*segments)[i]);
    }
    return current;
}

AutoPtr< ArrayOf<String> > Path::Split(
    /* [in] */ const String& s)
{
    Logger::D(TAG, "Split--");
    Int32 n = s.GetLength();
    if (n == 0) return ArrayOf<String>::Alloc(0);
    if (s.GetChar(0) != '/') {
        // throw new RuntimeException("malformed path:" + s);
        Logger::E(TAG, "malformed path:%s", s.string());
        return NULL;
    }
    List<String> segments;
    Int32 i = 1;
    while (i < n) {
        Int32 brace = 0;
        Int32 j;
        for (j = i; j < n; j++) {
            Char32 c = s.GetChar(j);
            if (c == '{') ++brace;
            else if (c == '}') --brace;
            else if (brace == 0 && c == '/') break;
        }
        if (brace != 0) {
            // throw new RuntimeException("unbalanced brace in path:" + s);
            Logger::E(TAG, "unbalanced brace in path:", s.string());
            return NULL;
        }
        segments.PushBack(s.Substring(i, j));
        i = j + 1;
    }

    AutoPtr< ArrayOf<String> > result =ArrayOf<String>::Alloc(segments.GetSize());
    List<String>::Iterator it = segments.Begin();
    i = 0;
    for (; it != segments.End(); ++it) {
        (*result)[i] = *it;
        i++;
    }

    return result;
}

AutoPtr< ArrayOf<String> > Path::SplitSequence(
    /* [in] */ const String& s)
{
    Logger::D(TAG, "SplitSequence--");

    Int32 n = s.GetLength();
    if (s.GetChar(0) != '{' || s.GetChar(n-1) != '}') {
        // throw new RuntimeException("bad sequence: " + s);
        Logger::E(TAG, "bad sequence:%s", s.string());
        return NULL;
    }
    List<String> segments;
    Int32 i = 1;
    while (i < n - 1) {
        Int32 brace = 0;
        Int32 j;
        for (j = i; j < n - 1; j++) {
            Char32 c = s.GetChar(j);
            if (c == '{') ++brace;
            else if (c == '}') --brace;
            else if (brace == 0 && c == ',') break;
        }
        if (brace != 0) {
            // throw new RuntimeException("unbalanced brace in path:" + s);
            Logger::E(TAG, "unbalanced brace in path:%s", s.string());
            return NULL;
        }
        segments.PushBack(s.Substring(i, j));
        i = j + 1;
    }

    AutoPtr< ArrayOf<String> > result =ArrayOf<String>::Alloc(segments.GetSize());
    List<String>::Iterator it = segments.Begin();
    i = 0;
    for (; it != segments.End(); ++it) {
        (*result)[i] = *it;
        i++;
    }
    return result;
}

CAR_INTERFACE_IMPL(Path, IInterface)

AutoPtr<Path> Path::GetChild(
    /* [in] */ const String& segment)
{
    Logger::D(TAG, "GetChild--1");
    Mutex::Autolock lock(sPathLock);
    HashMap<String, AutoPtr<Path> >::Iterator it = mChildren.Find(segment);
    if (it != mChildren.End()) {
        return it->mSecond;
    }
    AutoPtr<Path> p = new Path(this, segment);
    mChildren.Insert(HashMap<String, AutoPtr<Path> >::ValueType(segment, p));
    return p;
}

AutoPtr<Path> Path::GetParent()
{
    Logger::D(TAG, "GetParent--");
    Mutex::Autolock lock(sPathLock);
    return mParent;
}

AutoPtr<Path> Path::GetChild(
    /* [in] */ Int32 segment)
{
    Logger::D(TAG, "GetChild--2");
    return GetChild(StringUtils::Int32ToString(segment));
}

AutoPtr<Path> Path::GetChild(
    /* [in] */ Int64 segment)
{
    Logger::D(TAG, "GetChild--3");
    return GetChild(StringUtils::Int64ToString(segment));
}

void Path::SetObject(
    /* [in] */ MediaObject* object)
{
    Logger::D(TAG, "SetObject--");
    Mutex::Autolock lock(sPathLock);
    if (mObject == NULL) {
        mObject = object;
    }
}

AutoPtr<MediaObject> Path::GetObject()
{
    Logger::D(TAG, "GetObject--");
    Mutex::Autolock lock(sPathLock);
    return (mObject == NULL) ? NULL : mObject;
}

String Path::ToString()
{
    Logger::D(TAG, "ToString--");
    Mutex::Autolock lock(sPathLock);
    StringBuilder sb;
    AutoPtr< ArrayOf<String> > segments = Split();
    for (Int32 i = 0; i < segments->GetLength(); i++) {
        sb += "/";
        sb += (*segments)[i];
    }
    return sb.ToString();
}

Boolean Path::EqualsIgnoreCase(
    /* [in] */ const String& p)
{
    Logger::D(TAG, "EqualsIgnoreCase--");
    String path = ToString();
    return path.EqualsIgnoreCase(p);
}

AutoPtr< ArrayOf<String> > Path::Split()
{
    Logger::D(TAG, "Split--");
    Mutex::Autolock lock(sPathLock);
    Int32 n = 0;
    for (AutoPtr<Path> p = this; p != sRoot; p = p->mParent) {
        n++;
    }
    AutoPtr< ArrayOf<String> > segments = ArrayOf<String>::Alloc(n);
    Int32 i = n - 1;
    for (AutoPtr<Path> p = this; p != sRoot; p = p->mParent) {
        (*segments)[i--] = p->mSegment;
    }
    return segments;
}

String Path::GetPrefix()
{
    Logger::D(TAG, "GetPrefix--");
    if (this == sRoot) return String("");
    AutoPtr<Path> p = GetPrefixPath();
    return p->mSegment;
}

AutoPtr<Path> Path::GetPrefixPath()
{
    Logger::D(TAG, "GetPrefixPath--");
    Mutex::Autolock lock(sPathLock);
    AutoPtr<Path> current = this;
    if (current == sRoot) {
        // throw new IllegalStateException();
        Logger::E(TAG, "IllegalStateException");
        return NULL;
    }
    while (current->mParent != sRoot) {
        current = current->mParent;
    }
    return current;
}

String Path::GetSuffix()
{
    Logger::D(TAG, "GetSuffix--");
    return mSegment;
}

Path::Path(
    /* [in] */ Path* parent,
    /* [in] */ const String& segment)
{
    mParent = parent;
    mSegment = segment;
}

} // namespace Gallery
