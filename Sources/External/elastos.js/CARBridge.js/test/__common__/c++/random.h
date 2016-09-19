#ifndef __CAR_BRIDGE_TEST_RANDOM_H
# define __CAR_BRIDGE_TEST_RANDOM_H

# include <stddef.h>

# include <elastos.h>

# include "macros.h"



CAR_BRIDGE_TEST_NAMESPACE_BEGIN

extern _ELASTOS Int16 RandomInt16(void);

extern _ELASTOS Int32 RandomInt32(void);

extern _ELASTOS Int64 RandomInt64(void);

extern _ELASTOS Byte RandomByte(void);

extern _ELASTOS Float RandomFloat(void);

extern _ELASTOS Double RandomDouble(void);

extern _ELASTOS Char32 RandomChar32(void);

extern _ELASTOS String RandomString(void);

extern _ELASTOS Boolean RandomBoolean(void);

extern void RandomEMuid(_ELASTOS EMuid *id);

extern void RandomEGuid(_ELASTOS EGuid *id);

extern _ELASTOS ECode RandomECode(void);

extern _ELASTOS PVoid RandomLocalPtr(void);

extern _ELASTOS Int32 RandomEnum(void);

template<class T>
_ELASTOS AutoPtr<_ELASTOS ArrayOf<T> > RandomArrayOf(void);

template<class T, size_t L>
T (*RandomCArrayOf(void))[L];

CAR_BRIDGE_TEST_NAMESPACE_END

# include "random-cpp.h"

#endif
