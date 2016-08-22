
#include <elastos.h>

#include "macros.h"

#include "elastos-ext.h"



_ELASTOS_NAMESPACE_USING

CAR_BRIDGE_NAMESPACE_BEGIN

char const *GetEName(ECode ecode) noexcept
{
    switch (ERROR(ecode)) {
    case NOERROR:
        return "NOERROR";

    case E_DOES_NOT_EXIST:
        return "E_DOES_NOT_EXIST";

    case E_INVALID_OPERATION:
        return "E_INVALID_OPERATION";

    case E_TIMED_OUT:
        return "E_TIMED_OUT";

    case E_INTERRUPTED:
        return "E_INTERRUPTED";

    case E_FILE_NOT_FOUND:
        return "E_FILE_NOT_FOUND";

    case E_NOT_SUPPORTED:
        return "E_NOT_SUPPORTED";

    case E_OUT_OF_MEMORY:
        return "E_OUT_OF_MEMORY";

    case E_INVALID_ARGUMENT:
        return "E_INVALID_ARGUMENT";

    case E_NOT_IMPLEMENTED:
        return "E_NOT_IMPLEMENTED";

    case E_NO_INTERFACE:
        return "E_NO_INTERFACE";

    case E_ABORTED:
        return "E_ABORTED";

    case E_FAIL:
        return "E_FAIL";

    case E_NO_DEFAULT_CTOR:
        return "E_NO_DEFAULT_CTOR";

    case E_CLASS_NO_AGGREGATION:
        return "E_CLASS_NO_AGGREGATION";

    case E_CLASS_NOT_AVAILABLE:
        return "E_CLASS_NOT_AVAILABLE";

    case E_ASPECT_CANNOT_AGGREGATE_ASPECT:
        return "E_ASPECT_CANNOT_AGGREGATE_ASPECT";

    case E_UNAGGREGATE_FAILED:
        return "E_UNAGGREGATE_FAILED";

    case E_NO_GENERIC:
        return "E_NO_GENERIC";

    case E_WRONG_GENERIC:
        return "E_WRONG_GENERIC";

    case E_ASPECT_REFUSE_TO_ATTACH:
        return "E_ASPECT_REFUSE_TO_ATTACH";

    case E_ZOMBIE_ASPECT:
        return "E_ZOMBIE_ASPECT";

    case E_DUPLICATE_ASPECT:
        return "E_DUPLICATE_ASPECT";

    case E_CONFLICT_WITH_LOCAL_KEYWORD:
        return "E_CONFLICT_WITH_LOCAL_KEYWORD";

    case E_NOT_IN_PROTECTED_ZONE:
        return "E_NOT_IN_PROTECTED_ZONE";

    case E_COMPONENT_CANNOT_UNLOAD_NOW:
        return "E_COMPONENT_CANNOT_UNLOAD_NOW";

    case E_NOT_CAR_COMPONENT:
        return "E_NOT_CAR_COMPONENT";

    case E_COMPONENT_NOT_LOADED:
        return "E_COMPONENT_NOT_LOADED";

    case E_REMOTE_FAIL:
        return "E_REMOTE_FAIL";

    case E_OUT_OF_NUMBER:
        return "E_OUT_OF_NUMBER";

    case E_DATAINFO_EXIST:
        return "E_DATAINFO_EXIST";

    case E_NO_CLASS_INFO:
        return "E_NO_CLASS_INFO";

    case E_NO_EXPORT_OBJECT:
        return "E_NO_EXPORT_OBJECT";

    case E_NO_IMPORT_OBJECT:
        return "E_NO_IMPORT_OBJECT";

    case E_MARSHAL_DATA_TRANSPORT_ERROR:
        return "E_MARSHAL_DATA_TRANSPORT_ERROR";

    case E_NOT_CALLBACK_THREAD:
        return "E_NOT_CALLBACK_THREAD";

    case E_CANCLE_BOTH_EVENTS:
        return "E_CANCLE_BOTH_EVENTS";

    case E_NO_DELEGATE_REGISTERED:
        return "E_NO_DELEGATE_REGISTERED";

    case E_DELEGATE_ALREADY_REGISTERED:
        return "E_DELEGATE_ALREADY_REGISTERED";

    case E_CALLBACK_CANCELED:
        return "E_CALLBACK_CANCELED";

    case E_CALLBACK_IS_BUSY:
        return "E_CALLBACK_IS_BUSY";

    case E_CONNECT_NOCONNECTION:
        return "E_CONNECT_NOCONNECTION";

    case E_CONNECT_ADVISELIMIT:
        return "E_CONNECT_ADVISELIMIT";

    case E_CONNECT_CANNOTCONNECT:
        return "E_CONNECT_CANNOTCONNECT";

    case E_CONNECT_OVERRIDDEN:
        return "E_CONNECT_OVERRIDDEN";

    default:
        return "";
    }
}

template<class NamespacedDataTypeInfo>
static ECode _GetFullName(NamespacedDataTypeInfo const *namespacedDataTypeInfo, String *fullName) noexcept
{
    ECode ec;

    String namespace_;

    String name;

    if (namespacedDataTypeInfo == 0)
        return E_INVALID_ARGUMENT;

    if (fullName == 0)
        return NOERROR;

    ec = namespacedDataTypeInfo->GetNamespace(&namespace_);
    if (FAILED(ec))
        return ec;

    ec = namespacedDataTypeInfo->GetName(&name);
    if (FAILED(ec))
        return ec;

    *fullName = namespace_ + "." + name;

    return NOERROR;
}

ECode GetFullName(IDataTypeInfo const *dataTypeInfo, String *fullName) noexcept
{
    ECode ec;

    CarDataType dataType;

    if (dataTypeInfo == 0)
        return E_INVALID_ARGUMENT;

    if (fullName == 0)
        return NOERROR;

    ec = dataTypeInfo->GetDataType(&dataType);
    if (FAILED(ec))
        return ec;

    switch (dataType) {
    case CarDataType_Enum:
        ec = _GetFullName(static_cast<IEnumInfo const *>(dataTypeInfo), fullName);
        if (FAILED(ec))
            return ec;

        break;

    case CarDataType_CppVector:
        break;

    case CarDataType_Interface:
        ec = _GetFullName(static_cast<IInterfaceInfo const *>(dataTypeInfo), fullName);
        if (FAILED(ec))
            return ec;

        break;

    default:
        ec = dataTypeInfo->GetName(fullName);
        if (FAILED(ec))
            return ec;
    }

    return NOERROR;
}

ECode GetCategory(IClassInfo const *classInfo, enum ClassCategory *category) noexcept
{
    ECode ec;

    Boolean isGeneric;

    Boolean isRegime;

    Boolean isAspect;

    if (classInfo == 0)
        return E_INVALID_ARGUMENT;

    if (category == 0)
        return NOERROR;

    ec = classInfo->IsGeneric(&isGeneric);
    if (FAILED(ec))
        return ec;

    if (isGeneric != FALSE) {
        *category = CLASS_CATEGORY_GENERIC;

        return NOERROR;
    }

    ec = classInfo->IsRegime(&isRegime);
    if (FAILED(ec))
        return ec;

    if (isRegime != FALSE) {
        *category = CLASS_CATEGORY_REGIME;

        return NOERROR;
    }

    ec = classInfo->IsAspect(&isAspect);
    if (FAILED(ec))
        return ec;

    if (isAspect != FALSE) {
        *category = CLASS_CATEGORY_ASPECT;

        return NOERROR;
    }

    *category = CLASS_CATEGORY_CLASS;

    return NOERROR;
}

ECode GetImportedModuleCount(IModuleInfo const *moduleInfo, Int32 *count) noexcept
{
    if (moduleInfo == 0)
        return E_INVALID_ARGUMENT;

    if (count == 0)
        return NOERROR;

    return moduleInfo->GetImportModuleInfoCount(count);
}

ECode GetAllImportedModuleInfos(IModuleInfo const *moduleInfo,
        ArrayOf<IModuleInfo const *> *importedModuleInfos) noexcept
{
    if (moduleInfo == 0)
        return E_INVALID_ARGUMENT;

    if (importedModuleInfos == 0)
        return NOERROR;

    return moduleInfo->GetAllImportModuleInfos(reinterpret_cast<ArrayOf<IModuleInfo *> *>(importedModuleInfos));
}

ECode GetImportedModuleInfo(IModuleInfo const *moduleInfo,
        String const &path,
        IModuleInfo const **importedModuleInfo) noexcept;

ECode HasImportedModule(IModuleInfo const *moduleInfo, String const &path, Boolean *has) noexcept
{
    ECode ec;

    IModuleInfo const *_;

    if (moduleInfo == 0)
        return E_INVALID_ARGUMENT;

    if (has == 0)
        return NOERROR;

    ec = GetImportedModuleInfo(moduleInfo, path, &_);

    if (ec == E_DOES_NOT_EXIST) {
        *has = FALSE;

        return NOERROR;
    }

    if (FAILED(ec))
        return ec;

    *has = TRUE;

    _->Release();

    return ec;
}

ECode GetImportedModuleInfo(IModuleInfo const *moduleInfo,
        String const &path,
        IModuleInfo const **importedModuleInfo) noexcept
{
    ECode ec;

    Int32 nImportedModules;

    AutoPtr<ArrayOf<IModuleInfo const *> > importedModuleInfos;

    if (moduleInfo == 0)
        return E_INVALID_ARGUMENT;

    if (importedModuleInfo == 0)
        return NOERROR;

    ec = moduleInfo->GetImportModuleInfoCount(&nImportedModules);
    if (FAILED(ec))
        return ec;

    importedModuleInfos = ArrayOf<IModuleInfo const *>::Alloc(nImportedModules);
    if (importedModuleInfos == 0)
        return E_OUT_OF_MEMORY;

    ec = moduleInfo->GetAllImportModuleInfos(reinterpret_cast<ArrayOf<IModuleInfo *> *>(importedModuleInfos.Get()));
    if (FAILED(ec))
        return ec;

    for (Int32 i = 0; i < nImportedModules; ++i) {
        IModuleInfo const *_importedModuleInfo;

        String _path;

        _importedModuleInfo = (*importedModuleInfos)[i];

        ec = _importedModuleInfo->GetPath(&_path);
        if (FAILED(ec))
            return ec;

        if (_path == path) {
            *importedModuleInfo = _importedModuleInfo;

            return NOERROR;
        }
    }

    *importedModuleInfo = 0;

    return NOERROR;
}

ECode HasConstant(IModuleInfo const *moduleInfo, String const &name, Boolean *has) noexcept
{
    ECode ec;

    IConstantInfo *_;

    if (moduleInfo == 0)
        return E_INVALID_ARGUMENT;

    if (has == 0)
        return NOERROR;

    ec = moduleInfo->GetConstantInfo(name, &_);

    if (ec == E_DOES_NOT_EXIST) {
        *has = FALSE;

        return NOERROR;
    }

    if (FAILED(ec))
        return ec;

    *has = TRUE;

    _->Release();

    return ec;
}

ECode HasEnum(IModuleInfo const *moduleInfo, String const &fullName, Boolean *has) noexcept
{
    ECode ec;

    IEnumInfo *_;

    if (moduleInfo == 0)
        return E_INVALID_ARGUMENT;

    if (has == 0)
        return NOERROR;

    ec = moduleInfo->GetEnumInfo(fullName, &_);

    if (ec == E_DOES_NOT_EXIST) {
        *has = FALSE;

        return NOERROR;
    }

    if (FAILED(ec))
        return ec;

    *has = TRUE;

    _->Release();

    return ec;
}

ECode HasStruct(IModuleInfo const *moduleInfo, String const &name, Boolean *has) noexcept
{
    ECode ec;

    IStructInfo *_;

    if (moduleInfo == 0)
        return E_INVALID_ARGUMENT;

    if (has == 0)
        return NOERROR;

    ec = moduleInfo->GetStructInfo(name, &_);

    if (ec == E_DOES_NOT_EXIST) {
        *has = FALSE;

        return NOERROR;
    }

    if (FAILED(ec))
        return ec;

    *has = TRUE;

    _->Release();

    return ec;
}

ECode HasTypeAlias(IModuleInfo const *moduleInfo, String const &name, Boolean *has) noexcept
{
    ECode ec;

    ITypeAliasInfo *_;

    if (moduleInfo == 0)
        return E_INVALID_ARGUMENT;

    if (has == 0)
        return NOERROR;

    ec = moduleInfo->GetTypeAliasInfo(name, &_);

    if (ec == E_DOES_NOT_EXIST) {
        *has = FALSE;

        return NOERROR;
    }

    if (FAILED(ec))
        return ec;

    *has = TRUE;

    _->Release();

    return ec;
}

ECode HasInterface(IModuleInfo const *moduleInfo, String const &fullName, Boolean *has) noexcept
{
    ECode ec;

    IInterfaceInfo *_;

    if (moduleInfo == 0)
        return E_INVALID_ARGUMENT;

    if (has == 0)
        return NOERROR;

    ec = moduleInfo->GetInterfaceInfo(fullName, &_);

    if (ec == E_DOES_NOT_EXIST) {
        *has = FALSE;

        return NOERROR;
    }

    if (FAILED(ec))
        return ec;

    *has = TRUE;

    _->Release();

    return ec;
}

ECode HasClass(IModuleInfo const *moduleInfo, String const &fullName, Boolean *has) noexcept
{
    ECode ec;

    IClassInfo *_;

    if (moduleInfo == 0)
        return E_INVALID_ARGUMENT;

    if (has == 0)
        return NOERROR;

    ec = moduleInfo->GetClassInfo(fullName, &_);

    if (ec == E_DOES_NOT_EXIST) {
        *has = FALSE;

        return NOERROR;
    }

    if (FAILED(ec))
        return ec;

    *has = TRUE;

    _->Release();

    return ec;
}

CAR_BRIDGE_NAMESPACE_END

