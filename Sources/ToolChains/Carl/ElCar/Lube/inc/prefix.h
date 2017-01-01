#ifndef __PREFIX_H__
#define __PREFIX_H__

char* GenerateTypePrefix(const char* type);

char* GenerateCalleePrefix(const char* type);

char* GeneratePrefixalVarName(const char* prefix, const char* variable);

char* TrimOutTypeNameBar(const char* type);

char* GetTypeName(const char* type);

#endif /*__PREFIX_H__*/
