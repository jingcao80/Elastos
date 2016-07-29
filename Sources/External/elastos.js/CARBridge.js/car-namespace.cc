
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <node.h>

#include <nan.h>

#include <elastos.h>

#include "macros.h"

#include "libc-ext.h"

#include "nan-ext.h"

#include "car-data-type.h"
#include "car-object.h"
#include "error.h"
#include "map.h"
#include "multimap.h"
#include "new.h"
#include "pair.h"
#include "set.h"
#include "string.h"
#include "unique-ptr.h"



using namespace node;

using namespace Nan;

using namespace v8;

_ELASTOS_NAMESPACE_USING

CAR_BRIDGE_NAMESPACE_BEGIN

class _String: public String {
public:
    _String(void) noexcept
    {}

    _String(char const *s): String(s)
    {}

    _String &operator=(char const *s)
    {
        *static_cast<String *>(this) = s;

        return *this;
    }

    bool operator==(_String const &string) const noexcept
    {
        return !Compare(string);
    }

    bool operator!=(_String const &string) const noexcept
    {
        return !!Compare(string);
    }

    bool operator<(_String const &string) const noexcept
    {
        return Compare(string) < 0;
    }

    bool operator<=(_String const &string) const noexcept
    {
        return Compare(string) <= 0;
    }

    bool operator>=(_String const &string) const noexcept
    {
        return Compare(string) >= 0;
    }

    bool operator>(_String const &string) const noexcept
    {
        return Compare(string) > 0;
    }

private:
    static int strcmp(char const *s1, char const *s2) noexcept
    {
        for (char const *p1 = s1, *p2 = s2; *p1 != '\0' && *p2 != '\0'; ++p1, ++p2)
            if (*p1 != *p2)
                return *p1 - *p2;

        return 0;
    }

    int Compare(_String const &string) const noexcept
    {
        return strcmp(static_cast<char const *>(*this), static_cast<char const *>(string));
    }
};

enum _NamespacedEntryType {
    _NAMESPACED_ENTRY_TYPE_NONE,
    _NAMESPACED_ENTRY_TYPE_ENUM,
    _NAMESPACED_ENTRY_TYPE_INTERFACE,
    _NAMESPACED_ENTRY_TYPE_CLASS
};

struct _NamespacedEntryInfo {
    enum _NamespacedEntryType type;

    AutoPtr<IInterface const> info;
};

typedef Multimap<_String, struct _NamespacedEntryInfo> _MS2NEI;

static Local<ObjectTemplate> _CARNamespaceTemplate(
        _MS2NEI const *mapFullNameToNamespacedEntryInfo, char const *namespace_)
{
    Local<ObjectTemplate> namespaceTemplate;

    size_t offset;

    Pair<_MS2NEI::ConstIterator, _MS2NEI::ConstIterator> range;

    Set<String> namespaces;

    namespaceTemplate = New<ObjectTemplate>();

    offset = strlen(namespace_) + 1;

    range = mapFullNameToNamespacedEntryInfo->EqualRange(namespace_);

    for (auto it = range.first; it != range.second; ++it) {
        ::Nan::HandleScope scope;

        char const *fullName;

        char const *relativeName;

        char const *delimiter;

        fullName = it->first;

        relativeName = fullName + offset;

        delimiter = strchr(relativeName, '.');

        if (delimiter == nullptr) {
            Local<Data> namespacedEntry;

            switch (it->second.type) {
            case _NAMESPACED_ENTRY_TYPE_ENUM:
                namespacedEntry = CAREnum(static_cast<IEnumInfo const *>(it->second.info.Get()));

                break;

            case _NAMESPACED_ENTRY_TYPE_INTERFACE:
                namespacedEntry = CARInterface(static_cast<IInterfaceInfo const *>(it->second.info.Get()));

                break;

            case _NAMESPACED_ENTRY_TYPE_CLASS:
                namespacedEntry = CARObject::NewClassTemplate(static_cast<IClassInfo const *>(it->second.info.Get()));

                break;

            default:
                abort();
            }

            SetTemplate(namespaceTemplate,
                    New(relativeName).ToLocalChecked(),
                    namespacedEntry,
                    static_cast<enum PropertyAttribute>(ReadOnly | DontDelete));
        } else {
            char const *_namespace;

            _namespace = strndup(fullName, delimiter - fullName);
            if (_namespace == nullptr)
                throw Error(Error::NO_MEMORY, "");

            if (namespaces.Count(_namespace) > 0)
                continue;

            SetTemplate(namespaceTemplate,
                    New(_namespace + offset).ToLocalChecked(),
                    _CARNamespaceTemplate(mapFullNameToNamespacedEntryInfo, _namespace),
                    static_cast<enum PropertyAttribute>(ReadOnly | DontDelete));

            namespaces.Insert(_namespace);

            delete [] _namespace;
        }
    }

    return namespaceTemplate;
}

template<class NamespacedEntryInfo>
struct _GetNamespacedEntryType {
    static enum _NamespacedEntryType const VALUE = _NAMESPACED_ENTRY_TYPE_NONE;
};

template<>
struct _GetNamespacedEntryType<IEnumInfo> {
    static enum _NamespacedEntryType const VALUE = _NAMESPACED_ENTRY_TYPE_ENUM;
};

template<>
struct _GetNamespacedEntryType<IInterfaceInfo> {
    static enum _NamespacedEntryType const VALUE = _NAMESPACED_ENTRY_TYPE_INTERFACE;
};

template<>
struct _GetNamespacedEntryType<IClassInfo> {
    static enum _NamespacedEntryType const VALUE = _NAMESPACED_ENTRY_TYPE_CLASS;
};

template<class NamespacedEntryInfo>
static void _AddNamespacedEntryInfos(_MS2NEI *mapFullNameToNamespacedEntryInfo,
        ArrayOf<NamespacedEntryInfo const *> const &namespacedEntryInfos)
{
    for (_ELASTOS Int32 i = 0, nNamespacedEntryInfos = namespacedEntryInfos.GetLength();
            i < nNamespacedEntryInfos;
            ++i) {
        NamespacedEntryInfo const *namespacedEntryInfo;

        ECode ec;

        _ELASTOS String namespace_;

        _ELASTOS String name;

        _ELASTOS String fullName;

        namespacedEntryInfo = namespacedEntryInfos[i];

        ec = namespacedEntryInfo->GetNamespace(&namespace_);
        if (FAILED(ec))
            throw Error(Error::TYPE_ELASTOS, ec, "");

        ec = namespacedEntryInfo->GetName(&name);
        if (FAILED(ec))
            throw Error(Error::TYPE_ELASTOS, ec, "");

        fullName = namespace_ + "." + name;

        mapFullNameToNamespacedEntryInfo->Insert(
                Pair<_String, struct _NamespacedEntryInfo>((char const *)fullName,
                    {_GetNamespacedEntryType<NamespacedEntryInfo>::VALUE, namespacedEntryInfo})
                );
    }
}

typedef Map<_ELASTOS String, CopyablePersistent<ObjectTemplate>> _MS2OT;

static Map<AutoPtr<IModuleInfo const>, _MS2OT> _mapModuleInfoToMapNamespaceToCARNamespace;

static Map<AutoPtr<IModuleInfo const>, UniquePtr<_MS2NEI const>> _mapModuleInfoToMapFullNameToNamespacedEntryInfo;

Local<ObjectTemplate> CARNamespaceTemplate(IModuleInfo const *moduleInfo, char const *namespace_)
{
    UniquePtr<_MS2NEI> mapFullNameToNamespacedEntryInfo;

    ECode ec;

    _ELASTOS Int32 nEnums;

    _ELASTOS Int32 nInterfaces;

    _ELASTOS Int32 nClasses;

    Local<ObjectTemplate> _namespace;

    auto &mapNamespaceToCARNamespace = _mapModuleInfoToMapNamespaceToCARNamespace[moduleInfo];

    auto &__namespace = mapNamespaceToCARNamespace[_ELASTOS String(namespace_)];
    if (!__namespace.IsEmpty())
        return New(__namespace);

    if (_mapModuleInfoToMapFullNameToNamespacedEntryInfo.Count(moduleInfo) > 0)
        return _CARNamespaceTemplate(_mapModuleInfoToMapFullNameToNamespacedEntryInfo[moduleInfo], namespace_);

    mapFullNameToNamespacedEntryInfo = new(NO_THROW) _MS2NEI();
    if (mapFullNameToNamespacedEntryInfo == nullptr)
        throw Error(Error::NO_MEMORY, "");

    ec = moduleInfo->GetEnumCount(&nEnums);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    if (nEnums > 0) {
        AutoPtr<ArrayOf<IEnumInfo const *> > enumInfos;
        ArrayOf<IEnumInfo const *> *_enumInfos;

        _enumInfos = ArrayOf<IEnumInfo const *>::Alloc(nEnums);
        if (_enumInfos == 0)
            throw Error(Error::NO_MEMORY, "");

        enumInfos = _enumInfos, _enumInfos->Release();

        ec = moduleInfo->GetAllEnumInfos(reinterpret_cast<ArrayOf<IEnumInfo *> *>(enumInfos.Get()));
        if (FAILED(ec))
            throw Error(Error::TYPE_ELASTOS, ec, "");

        _AddNamespacedEntryInfos(mapFullNameToNamespacedEntryInfo, *enumInfos);
    }

    ec = moduleInfo->GetInterfaceCount(&nInterfaces);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    if (nInterfaces > 0) {
        AutoPtr<ArrayOf<IInterfaceInfo const *> > interfaceInfos;
        ArrayOf<IInterfaceInfo const *> *_interfaceInfos;

        _interfaceInfos = ArrayOf<IInterfaceInfo const *>::Alloc(nInterfaces);
        if (_interfaceInfos == 0)
            throw Error(Error::NO_MEMORY, "");

        interfaceInfos = _interfaceInfos, _interfaceInfos->Release();

        ec = moduleInfo->GetAllInterfaceInfos(reinterpret_cast<ArrayOf<IInterfaceInfo *> *>(interfaceInfos.Get()));
        if (FAILED(ec))
            throw Error(Error::TYPE_ELASTOS, ec, "");

        _AddNamespacedEntryInfos(mapFullNameToNamespacedEntryInfo, *interfaceInfos);
    }

    ec = moduleInfo->GetClassCount(&nClasses);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    if (nClasses > 0) {
        AutoPtr<ArrayOf<IClassInfo const *> > classInfos;
        ArrayOf<IClassInfo const *> *_classInfos;

        _classInfos = ArrayOf<IClassInfo const *>::Alloc(nClasses);
        if (_classInfos == 0)
            throw Error(Error::NO_MEMORY, "");

        classInfos = _classInfos, _classInfos->Release();

        ec = moduleInfo->GetAllClassInfos(reinterpret_cast<ArrayOf<IClassInfo *> *>(classInfos.Get()));
        if (FAILED(ec))
            throw Error(Error::TYPE_ELASTOS, ec, "");

        _AddNamespacedEntryInfos(mapFullNameToNamespacedEntryInfo, *classInfos);
    }

    auto &_mapFullNameToNamespacedEntryInfo = _mapModuleInfoToMapFullNameToNamespacedEntryInfo[moduleInfo];

    _mapFullNameToNamespacedEntryInfo = mapFullNameToNamespacedEntryInfo.Release();

    _namespace = _CARNamespaceTemplate(_mapFullNameToNamespacedEntryInfo, namespace_);

    __namespace.Reset(_namespace);

    return _namespace;
}

CAR_BRIDGE_NAMESPACE_END

