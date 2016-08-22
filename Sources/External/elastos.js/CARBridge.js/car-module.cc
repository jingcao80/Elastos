
#include <cstdio>

#include <map>

#include <node.h>

#include <nan.h>

#include <elastos.h>

#include "macros.h"

#include "nan-ext.h"

#include "elastos-ext.h"

#include "car-constant.h"
#include "car-data-type.h"
#include "car-imported-module.h"
#include "car-namespace.h"
#include "car-type-alias.h"
#include "error.h"
#include "js-2-car.h"



using namespace std;

using namespace node;

using namespace Nan;

using namespace v8;

_ELASTOS_NAMESPACE_USING

CAR_BRIDGE_NAMESPACE_BEGIN

static map<AutoPtr<IModuleInfo const>, CopyablePersistent<ObjectTemplate>> _mapModuleInfoToCARModule;

Local<ObjectTemplate> CARModuleTemplate(IModuleInfo const *moduleInfo)
{
    ::Nan::EscapableHandleScope scope;

    Local<ObjectTemplate> moduleTemplate;

    ECode ec;

    _ELASTOS String path;

    _ELASTOS Int32 major, minor, build, revision;

    _ELASTOS Int32 nImportedModules;

    _ELASTOS Int32 nConstants;

    _ELASTOS Int32 nStructs;

    _ELASTOS Int32 nTypeAliases;

    auto &_moduleTemplate = _mapModuleInfoToCARModule[moduleInfo];
    if (!_moduleTemplate.IsEmpty())
        return scope.Escape(New(_moduleTemplate));

    moduleTemplate = CARNamespaceTemplate(moduleInfo, "");

    ec = moduleInfo->GetPath(&path);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    SetTemplate(moduleTemplate,
            New("$path").ToLocalChecked(),
            ToValue(path),
            static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

    ec = moduleInfo->GetVersion(&major, &minor, &build, &revision);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    char version[48];

    sprintf(version, "%d.%d.%d.%d", major, minor, build, revision);

    SetTemplate(moduleTemplate,
            New("$version").ToLocalChecked(),
            New(version).ToLocalChecked(),
            static_cast<enum PropertyAttribute>(ReadOnly | DontDelete | DontEnum));

    ec = GetImportedModuleCount(moduleInfo, &nImportedModules);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    if (nImportedModules > 0) {
        AutoPtr<ArrayOf<IModuleInfo const *> > importedModuleInfos;

        importedModuleInfos = ArrayOf<IModuleInfo const *>::Alloc(nImportedModules);
        if (importedModuleInfos == 0)
            throw Error(Error::NO_MEMORY, "");

        ec = GetAllImportedModuleInfos(moduleInfo, importedModuleInfos);
        if (FAILED(ec))
            throw Error(Error::TYPE_ELASTOS, ec, "");

        for (_ELASTOS Int32 i = 0; i < nImportedModules; ++i) {
            ::Nan::HandleScope scope;

            IModuleInfo const *importedModuleInfo;

            _ELASTOS String importedModulePath;

            importedModuleInfo = (*importedModuleInfos)[i];

            ec = importedModuleInfo->GetPath(&importedModulePath);
            if (FAILED(ec))
                throw Error(Error::TYPE_ELASTOS, ec, "");

            SetTemplate(moduleTemplate,
                    ToValue(importedModulePath).As<::v8::String>(),
                    CARImportedModuleTemplate(importedModuleInfo),
                    static_cast<enum PropertyAttribute>(ReadOnly | DontDelete));
        }
    }

    ec = moduleInfo->GetConstantCount(&nConstants);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    if (nConstants > 0) {
        AutoPtr<ArrayOf<IConstantInfo const *> > constantInfos;

        Local<Object> constants;

        constantInfos = ArrayOf<IConstantInfo const *>::Alloc(nConstants);
        if (constantInfos == 0)
            throw Error(Error::NO_MEMORY, "");

        ec = moduleInfo->GetAllConstantInfos(reinterpret_cast<ArrayOf<IConstantInfo *> *>(constantInfos.Get()));
        if (FAILED(ec))
            throw Error(Error::TYPE_ELASTOS, ec, "");

        constants = New<Object>();

        for (_ELASTOS Int32 i = 0; i < nConstants; ++i) {
            ::Nan::HandleScope scope;

            IConstantInfo const *constantInfo;

            _ELASTOS String constantName;

            constantInfo = (*constantInfos)[i];

            ec = constantInfo->GetName(&constantName);
            if (FAILED(ec))
                throw Error(Error::TYPE_ELASTOS, ec, "");

            DefineOwnProperty(constants,
                    ToValue(constantName).As<::v8::String>(),
                    CARConstant(constantInfo),
                    static_cast<enum PropertyAttribute>(ReadOnly | DontDelete));
        }

        SetTemplate(moduleTemplate,
                New("$const").ToLocalChecked(),
                constants,
                static_cast<enum PropertyAttribute>(ReadOnly | DontDelete));
    }

    ec = moduleInfo->GetStructCount(&nStructs);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    if (nStructs > 0) {
        AutoPtr<ArrayOf<IStructInfo const *> > structInfos;

        structInfos = ArrayOf<IStructInfo const *>::Alloc(nStructs);
        if (structInfos == 0)
            throw Error(Error::NO_MEMORY, "");

        ec = moduleInfo->GetAllStructInfos(reinterpret_cast<ArrayOf<IStructInfo *> *>(structInfos.Get()));
        if (FAILED(ec))
            throw Error(Error::TYPE_ELASTOS, ec, "");

        for (_ELASTOS Int32 i = 0; i < nStructs; ++i) {
            ::Nan::HandleScope scope;

            IStructInfo const *structInfo;

            _ELASTOS String structName;

            structInfo = (*structInfos)[i];

            ec = structInfo->GetName(&structName);
            if (FAILED(ec))
                throw Error(Error::TYPE_ELASTOS, ec, "");

            SetTemplate(moduleTemplate,
                    ToValue(structName).As<::v8::String>(),
                    CARStruct(structInfo),
                    static_cast<enum PropertyAttribute>(ReadOnly | DontDelete));
        }
    }

    ec = moduleInfo->GetTypeAliasCount(&nTypeAliases);
    if (FAILED(ec))
        throw Error(Error::TYPE_ELASTOS, ec, "");

    if (nTypeAliases > 0) {
        AutoPtr<ArrayOf<ITypeAliasInfo const *> > typeAliasInfos;

        typeAliasInfos = ArrayOf<ITypeAliasInfo const *>::Alloc(nTypeAliases);
        if (typeAliasInfos == 0)
            throw Error(Error::NO_MEMORY, "");

        ec = moduleInfo->GetAllTypeAliasInfos(reinterpret_cast<ArrayOf<ITypeAliasInfo *> *>(typeAliasInfos.Get()));
        if (FAILED(ec))
            throw Error(Error::TYPE_ELASTOS, ec, "");

        for (_ELASTOS Int32 i = 0; i < nTypeAliases; ++i) {
            ::Nan::HandleScope scope;

            ITypeAliasInfo const *typeAliasInfo;

            _ELASTOS String typeAliasName;

            typeAliasInfo = (*typeAliasInfos)[i];

            ec = typeAliasInfo->GetName(&typeAliasName);
            if (FAILED(ec))
                throw Error(Error::TYPE_ELASTOS, ec, "");

            SetTemplate(moduleTemplate,
                    ToValue(typeAliasName).As<::v8::String>(),
                    CARTypeAlias(typeAliasInfo),
                    static_cast<enum PropertyAttribute>(ReadOnly | DontDelete));
        }
    }

    _moduleTemplate.Reset(moduleTemplate);

    return scope.Escape(moduleTemplate);
}

CAR_BRIDGE_NAMESPACE_END

