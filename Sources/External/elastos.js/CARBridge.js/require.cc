
#include <cstddef>
#include <cstdint>
#include <cstring>

#include <node.h>

#include <nan.h>

#include <elastos.h>

#include "macros.h"

#include "elastos-ext.h"

#include "car-constant.h"
#include "car-data-type.h"
#include "car-imported-module.h"
#include "car-module.h"
#include "car-namespace.h"
#include "car-object.h"
#include "car-type-alias.h"
#include "error.h"



using namespace node;

using namespace Nan;

using namespace v8;

_ELASTOS_NAMESPACE_USING

CAR_BRIDGE_NAMESPACE_BEGIN

static Local<Value> _Require(IModuleInfo const *moduleInfo, char const *entryId)
{
    ::Nan::EscapableHandleScope scope;

    ECode ec;

    _ELASTOS Boolean hasImportedModule;

    _ELASTOS Boolean hasEnum;

    _ELASTOS Boolean hasStruct;

    _ELASTOS Boolean hasTypeAlias;

    _ELASTOS Boolean hasInterface;

    _ELASTOS Boolean hasClass;

    ec = HasImportedModule(moduleInfo, _ELASTOS String(entryId), &hasImportedModule);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    if (hasImportedModule != FALSE) {
        AutoPtr<IModuleInfo const> importedModuleInfo;
        IModuleInfo const *_importedModuleInfo;

        ec = GetImportedModuleInfo(moduleInfo, _ELASTOS String(entryId), &_importedModuleInfo);
        if (FAILED(ec))
            throw Error(Error::TYPE_ELASTOS, ec, "");

        importedModuleInfo = _importedModuleInfo, _importedModuleInfo->Release();

        return scope.Escape(NewInstance(CARImportedModuleTemplate(importedModuleInfo)).ToLocalChecked());
    }

    if (strncmp(entryId, "$const.", 7) == 0) {
        char const *_entryId;

        _ELASTOS Boolean hasConstant;

        AutoPtr<IConstantInfo const> constantInfo;
        IConstantInfo *_constantInfo;

        _entryId = entryId + 7;

        ec = HasConstant(moduleInfo, _ELASTOS String(_entryId), &hasConstant);
        if (FAILED(ec))
            throw Error(Error::TYPE_ELASTOS, ec, "");

        if (hasConstant == FALSE)
            throw Error(Error::NONENTITY, "");

        ec = moduleInfo->GetConstantInfo(_ELASTOS String(_entryId), &_constantInfo);
        if (FAILED(ec))
            throw Error(Error::TYPE_ELASTOS, ec, "");

        constantInfo = _constantInfo, _constantInfo->Release();

        return CARConstant(constantInfo);
    }

    ec = HasEnum(moduleInfo, _ELASTOS String(entryId), &hasEnum);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    if (hasEnum != FALSE) {
        AutoPtr<IEnumInfo const> enumInfo;
        IEnumInfo *_enumInfo;

        ec = moduleInfo->GetEnumInfo(_ELASTOS String(entryId), &_enumInfo);
        if (FAILED(ec))
            throw Error(Error::TYPE_ELASTOS, ec, "");

        enumInfo = _enumInfo, _enumInfo->Release();

        return CAREnum(enumInfo);
    }

    ec = HasStruct(moduleInfo, _ELASTOS String(entryId), &hasStruct);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    if (hasStruct != FALSE) {
        AutoPtr<IStructInfo const> structInfo;
        IStructInfo *_structInfo;

        ec = moduleInfo->GetStructInfo(_ELASTOS String(entryId), &_structInfo);
        if (FAILED(ec))
            throw Error(Error::TYPE_ELASTOS, ec, "");

        structInfo = _structInfo, _structInfo->Release();

        return CARStruct(structInfo);
    }

    ec = HasTypeAlias(moduleInfo, _ELASTOS String(entryId), &hasTypeAlias);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    if (hasTypeAlias != FALSE) {
        AutoPtr<ITypeAliasInfo const> typeAliasInfo;
        ITypeAliasInfo *_typeAliasInfo;

        ec = moduleInfo->GetTypeAliasInfo(_ELASTOS String(entryId), &_typeAliasInfo);
        if (FAILED(ec))
            throw Error(Error::TYPE_ELASTOS, ec, "");

        typeAliasInfo = _typeAliasInfo, _typeAliasInfo->Release();

        return CARTypeAlias(typeAliasInfo);
    }

    ec = HasInterface(moduleInfo, _ELASTOS String(entryId), &hasInterface);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    if (hasInterface != FALSE) {
        AutoPtr<IInterfaceInfo const> interfaceInfo;
        IInterfaceInfo *_interfaceInfo;

        ec = moduleInfo->GetInterfaceInfo(_ELASTOS String(entryId), &_interfaceInfo);
        if (FAILED(ec))
            throw Error(Error::TYPE_ELASTOS, ec, "");

        interfaceInfo = _interfaceInfo, _interfaceInfo->Release();

        return CARInterface(interfaceInfo);
    }

    ec = HasClass(moduleInfo, _ELASTOS String(entryId), &hasClass);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    if (hasClass != FALSE) {
        AutoPtr<IClassInfo const> classInfo;
        IClassInfo *_classInfo;

        ec = moduleInfo->GetClassInfo(_ELASTOS String(entryId), &_classInfo);
        if (FAILED(ec))
            throw Error(Error::TYPE_ELASTOS, ec, "");

        classInfo = _classInfo, _classInfo->Release();

        return scope.Escape(GetFunction(CARObject::NewClassTemplate(classInfo)).ToLocalChecked());
    }

    return scope.Escape(NewInstance(CARNamespaceTemplate(moduleInfo, entryId)).ToLocalChecked());
}

Local<Value> Require(char const *ecoPath,
        uint32_t major, uint32_t minor, uint32_t build, uint32_t revision,
        size_t nEntryIds, char const * const entryIds[])
{
    ECode ec;

    AutoPtr<IModuleInfo const> moduleInfo;
    IModuleInfo *_moduleInfo;

    _ELASTOS Int32 _major, _minor, _build, _revision;

    Local<Array> entries;

    ec = CReflector::AcquireModuleInfo(_ELASTOS String(ecoPath), &_moduleInfo);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    moduleInfo = _moduleInfo, _moduleInfo->Release();

    ec = moduleInfo->GetVersion(&_major, &_minor, &_build, &_revision);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    if ((uint32_t)_major != major)
        throw Error(Error::INCOMPATIBLE_VERSION, "");
    if ((uint32_t)_minor < minor)
        throw Error(Error::INCOMPATIBLE_VERSION, "");
    if ((uint32_t)_build != build)
        throw Error(Error::INCOMPATIBLE_VERSION, "");
    if ((uint32_t)_minor == minor && (uint32_t)_revision < revision)
        throw Error(Error::INCOMPATIBLE_VERSION, "");

    if (nEntryIds == 0) {
        ::Nan::EscapableHandleScope scope;

        return scope.Escape(NewInstance(CARModuleTemplate(moduleInfo)).ToLocalChecked());
    }

    if (nEntryIds == 1)
        return _Require(moduleInfo, entryIds[0]);

    entries = New<Array>(nEntryIds);

    for (size_t i = 0; i < nEntryIds; ++i) {
        ::Nan::HandleScope scope;

        ::Nan::Set(entries, i, _Require(moduleInfo, entryIds[i]));
    }

    return entries;
}

CAR_BRIDGE_NAMESPACE_END

