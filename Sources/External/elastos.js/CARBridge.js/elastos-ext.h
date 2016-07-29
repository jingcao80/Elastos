#ifndef __CAR_BRIDGE_ELASTOS_EXT_H
# define __CAR_BRIDGE_ELASTOS_EXT_H

# include <elastos.h>

# include "macros.h"



# define NO_ERROR NOERROR

# define E_FAILED E_FAIL

CAR_BRIDGE_NAMESPACE_BEGIN

extern char const *GetEName(_ELASTOS ECode ecode) noexcept;

extern _ELASTOS ECode GetFullName(IDataTypeInfo const *dataTypeInfo, _ELASTOS String *fullName) noexcept;

enum ClassCategory {
    CLASS_CATEGORY_CLASS,
    CLASS_CATEGORY_GENERIC,
    CLASS_CATEGORY_REGIME,
    CLASS_CATEGORY_ASPECT
};

extern _ELASTOS ECode GetCategory(IClassInfo const *classInfo, enum ClassCategory *category) noexcept;

extern _ELASTOS ECode GetImportedModuleCount(IModuleInfo const *moduleInfo, _ELASTOS Int32 *count) noexcept;

extern _ELASTOS ECode GetAllImportedModuleInfos(IModuleInfo const *moduleInfo,
        _ELASTOS ArrayOf<IModuleInfo const *> *importedModuleInfos) noexcept;

extern _ELASTOS ECode HasImportedModule(IModuleInfo const *moduleInfo,
        _ELASTOS String const &path,
        _ELASTOS Boolean *has) noexcept;

extern _ELASTOS ECode GetImportedModuleInfo(IModuleInfo const *moduleInfo,
        _ELASTOS String const &path,
        IModuleInfo const **importedModuleInfo) noexcept;

extern _ELASTOS ECode HasConstant(IModuleInfo const *moduleInfo,
        _ELASTOS String const &name,
        _ELASTOS Boolean *has) noexcept;

extern _ELASTOS ECode HasEnum(IModuleInfo const *moduleInfo,
        _ELASTOS String const &fullName,
        _ELASTOS Boolean *has) noexcept;

extern _ELASTOS ECode HasStruct(IModuleInfo const *moduleInfo,
        _ELASTOS String const &name,
        _ELASTOS Boolean *has) noexcept;

extern _ELASTOS ECode HasTypeAlias(IModuleInfo const *moduleInfo,
        _ELASTOS String const &name,
        _ELASTOS Boolean *has) noexcept;

extern _ELASTOS ECode HasInterface(IModuleInfo const *moduleInfo,
        _ELASTOS String const &fullName,
        _ELASTOS Boolean *has) noexcept;

extern _ELASTOS ECode HasClass(IModuleInfo const *moduleInfo,
        _ELASTOS String const &fullName,
        _ELASTOS Boolean *has) noexcept;

CAR_BRIDGE_NAMESPACE_END

#endif
