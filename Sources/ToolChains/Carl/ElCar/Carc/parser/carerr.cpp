//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include <stdarg.h>
#include <stdio.h>
#include <assert.h>

#include "carc.h"

int g_nErrorNumber = 0;
int g_nWarningNumber = 0;

typedef struct ErrorMessage {
    CARErrorNo      err;
    const char *    pszMessage;
} ErrorMessage;

static ErrorMessage s_errorMessages[] = {
    { CAR_W_LocalResult, "Return type of method \"%s\" is not ECode." },
    { CAR_W_LocalArg, "Parameter \"%s\" is local type." },
    { CAR_W_NoMethods, "No methods defined in interface." },
    { CAR_W_LocalParent, "Inherited from local interface \"%s\"." },
    { CAR_W_NoClassInterfaces, "Class has no interface included." },
    { CAR_W_LoadLibrary, "Library \"%s\" could not be loaded." },
    { CAR_W_TooManyLibraries, "Too many libraries imported." },
    { CAR_W_LocalClass, "All interfaces are local in class, set as local." },
    { CAR_W_DupMethodName, "Interface method name \"%s\" is duplicated." },
    { CAR_W_UnexpectFileType, "File \"%s\" has an unexpected type, ignored." },
    { CAR_W_NoAutoParamAttrib, "Attributes of parameter \"%s\" can "
            "not be resolved automatically." },
    { CAR_W_IllegalMemberName, "Illegal member Name \"%s\"." },
    { CAR_W_IllegalCharacterInURL, "Illegal character \'%c\' in URL \"%s\"." },
    { CAR_W_NoArrayBufSize, "Lack of n in xxxArray_<n> or xxxBuf_<n>." },
    { CAR_W_IgnoreThreadMode, "Keyword \"%s\" will be ignored, because the "
        "compiler specified '-A' option." },
    { CAR_W_DecrepitTypeUsage, "%s"},
    { CAR_W_NoProperties, "No properties are declared."},
    { CAR_W_CARUunmUndef, "No UUNM specified for component." },

    { CAR_E_UnexpectEOF, "Unexpected end of file." },
    { CAR_E_UnexpectSymbol, "Unexpected symbol \"%s\"." },
    { CAR_E_UnexpectChar, "Unexpected character '%c'." },
    { CAR_E_SymbolTooLong, "Symbol is too long." },
    { CAR_E_IllegalChar, "Illegal character '%c'." },
    { CAR_E_LibraryProject, "The attribute \"project\" " \
        "can't use with \"library\"" },
    { CAR_E_UunmDifferFromCarName, "UUNM should be in accord with the name of "
        ".car file." },
    { CAR_E_LoadLibrary, "Library \"%s\" could not be loaded." },
    { CAR_E_ExpectSymbol, "Symbol \"%s\" may be missing." },
    { CAR_E_UuidFormat, "Illegal uuid format." },
    { CAR_E_OutOfMemory, "Compilation out of memory." },
    { CAR_E_AttribConflict, "Attributes conflict: %s with %s." },
    { CAR_E_MscomNoUuid, "The uuid is needed when \"mscom\" specified." },
    { CAR_E_IllegalValue, "Illegal number value." },
    { CAR_E_DupEntry, "%s \"%s\" redefined." },
    { CAR_E_FullEntry, "Too many %s defined." },
    { CAR_E_NameConflict, "Symbol(%s \"%s\") has been defined." },
    { CAR_E_NotFound, "Undefined %s \"%s\"." },
    { CAR_E_UndefinedSymbol, "Undefined symbol \"%s\"." },
    { CAR_E_UuidNoMscom, "The uuid specified but \"mscom\" is not declared." },
    { CAR_E_RedefUuid, "The uuid redefined." },
    { CAR_E_IllegalMethodProperties, \
        "The method has illegal properties." },
    { CAR_E_IllegalMethodAnnotation, \
        "The method has illegal annotation." },
    { CAR_E_IllegalInterfaceProperties, \
        "The interface has illegal properties." },
    { CAR_E_IllegalClassProperties, \
        "The class has illegal properties." },
    { CAR_E_IllegalClassOrInterfaceAnnotation, \
        "The class or interface has illegal annotation." },
    { CAR_E_ExpectInterfaceName, "Interface name expected." },
    { CAR_E_ExpectClassName, "Class name expected." },
    { CAR_E_ExpectStructName, "Struct name expected." },
    { CAR_E_ExpectEnumName, "Enum name expected." },
    { CAR_E_UndefType, "Undefined type \"%s\"." },
    { CAR_E_ExpectMethodName, "Method name expected." },
    { CAR_E_ExpectParamName, "Parameter name expected." },
    { CAR_E_TypeConflict, "Type conflicted." },
    { CAR_E_AspectUse, "aspect can only used with regime." },
    { CAR_E_RegimeNoAspect, "No aspects declared for regime." },
    { CAR_E_NotAspect, "\"%s\" is not an aspect." },
    { CAR_E_NotClass, "\"%s\" is not a class." },
    { CAR_E_TooManyParents, "Too many parents defined." },
    { CAR_E_NestedType, "Type nested with BufferOf or Enum." },
    { CAR_E_AsyncOut, "Attribute \"out\" not "
        "permitted in async interface." },
    { CAR_E_NoMethods, "No methods defined in interface." },
    { CAR_E_DupUuid, "The uuid is a duplicate of previous definition." },
    { CAR_E_NoMainClass, "No main class specified for component." },
    { CAR_E_NoClassInterfaces, "Class has no interface included." },
    { CAR_E_LoadSystemLib, "Can't load system types library." },
    { CAR_E_NestedStruct, "Struct has a nested member \"%s\"." },
    { CAR_E_MergeCLS, "Error when merge library \"%s\"." },
    { CAR_E_GenDisp, "Error on generate dispatch interface of class." },
    { CAR_E_DupMethodName, "Method name %s is duplicated in class." },
    { CAR_E_InheritNoVirtual, "Inherit from none-virtual interface class." },
    { CAR_E_NoClasses, "No class defined in component." },
    { CAR_E_OpenFile, "Can't open file \"%s\"." },
    { CAR_E_NestedInherit, "%s \"%s\" is nested inheriting." },
    { CAR_E_OutParameterInCtor, "Parameter can't be out in constructor!" },
    { CAR_E_ParameterInSingletonCtor, "Constructor of singleton class can't "
        "have any parameter!" },
    { CAR_E_InvalidMemberName, "Invalid member name \"%s\"." },
    { CAR_E_IllegalClassName, "Illegal class name. First character of \"%s\" "
        "should be 'C'."},
    { CAR_E_IllegalAspectName, "Illegal aspect name. First character of \"%s\" "
        "should be 'A'."},
    { CAR_E_IllegalRegimeName, "Illegal regime name. First character of "
        "\"%s\" should be 'R'."},
    { CAR_E_IllegalDomainName, "Illegal domain name. First character of \"%s\" "
        "should be 'D'."},
    { CAR_E_IllegalGenericName, "Illegal generic name. First character of "
        "\"%s\" should be 'G'."},
    { CAR_E_IllegalInterfaceName, "Illegal interface name. First character of "
        "\"%s\" should be 'I'."},
    { CAR_E_IllegalCallbacksOrDelegatesName, "Illegal callbacks or delegates name. First "
        "character of \"%s\" should be 'J'."},
    { CAR_E_IlleagalSizeType, "Illegal n type of xxxArray_<n> or xxxBuf_<n>."},
    { CAR_E_ExpectConstName, "Const name expected." },
    { CAR_E_ExpectGenericName, "Generic class name expected." },
    { CAR_E_AggregateUse, "\"aggregate\" can not be used as the attribute of "
        "aspect or generic."},
    { CAR_E_DupNameWithModule, "Name is duplicated with module." },
    { CAR_E_OutParameterWithCallback, "Callback or Asynchronous interface can't "
        "support method \"%s\" with [out] parameter." },
    { CAR_E_ContructorWithAspect, "\"aspect\" can't support \"contructor\" with"
        " parameter." },
    { CAR_E_LocalUse, "\"local\" can not be used as the attribute of "
        "\"aspect\"." },
    { CAR_E_IllegalVersion, "Both of the major and miner version should be "
        "between 0 and 255." },
    { CAR_E_NoConstructor, "Because parent class \"%s\" has no default "
        "constructor, child class \"%s\" must define a constructor"},
    { CAR_E_NoVirtualInterface, "class \"%s\" has no virtual interface, can't "
        "be inherited."},
    { CAR_E_InheritanceConflict, "The parent or children of interface \"%s\" "
        "has been declared in class \"%s\"."},
    { CAR_E_CouldntHasCtor, "%s couldn't has constructor."},
    { CAR_E_CallbackInterface, "Interface \"%s\" can only be implemented as "
        "callback interface."},
    { CAR_E_LocalConflict, "Callback or Asynchronous interface don't support "
        "local attribute."},
    { CAR_E_NoParameters, "Coalesced methods \"%s\" should have parameters."},
    { CAR_E_CouldNotInheritGeneric, "\"%s\" is Generic class and couldn't be inherited."},
    { CAR_E_NotGeneric, "\"%s\" is not a Generic." },
    { CAR_E_NoImplIntfOfGeneric, "The \"%s\" must be implemented in class %s, "
        "because it was included by generic." },
    { CAR_E_InterfaceShouldBeLocal, "The interface \"%s\" should be local "
        "because the type of some parameter of its method \"%s\" is local."},
    { CAR_E_ParamNoAttrib, "The parameter \"%s\" of method \"%s\" does not has attributes."},
    { CAR_E_IllegalParamAttrib, "The attributs of the parameter \"%s\" of method \"%s\" are illegal."},
    { CAR_E_IllegalParamType, "The type of parameter \"%s\" is illegal." },
    { CAR_E_CalleeParam, "The parameter \"%s\" of method \"%s\" has \"callee\" attribute,"
        " so the interface should be local."},
    { CAR_E_IllegalTypeUsage, "%s"},
    { CAR_E_GenericCouldntHaveVirtualInterface, "Generic class \"%s\" couldn't "
        "have virtual interface \"%s\"."},
    { CAR_E_NestedCARQuient, "CARQuient type cann't be nested."},
    { CAR_E_IllegalPrivilegedInterface, "Class \"%s\" is not aspect, "
        "it can't support privileged interface \"%s\"."},
    { CAR_E_CouldNotInheritFinalClass, "Class \"%s\" is final, and could not be inherited."},
    { CAR_E_DeclaredInterfaceProp, "Forward interface declarations cann't have properties."},
    { CAR_E_DeclaredClassProp, "Forward class declarations cann't have properties."},
    { CAR_E_IllegalFinalUsage, "\"final\" can only modify normal classes."},
	{ CAR_E_UnmatchedInterface, "The interface \"%s\" declared in class doesn't accord with "
        "the definition in module."},
    { CAR_E_CalleeDecl, "The keyword \"callee\" must be used behind of the keyword \"out\"."},
    { CAR_E_ExpectParamAttrib, "The parameter should have attributes." },
    { CAR_E_InvalidUUNM, "The Universal Unique Name of this CAR module is not valid."},
    { CAR_E_NotIntegerConst, "The const \"%s\" is not integer."},
    { CAR_E_TypeAmbiguous, "The type \"%s\" is ambiguous."},
    { CAR_E_ExpectAnnotationName, "Annotation name expected." },
    { CAR_E_TooManyAnnotation, "Too many annotation defined." },
    { CAR_E_DupAnnotationName, "Annotation name %s is duplicated." },
};

const int c_nErrorNumber = \
    sizeof(s_errorMessages) / sizeof(ErrorMessage);

static BOOL s_bSuppressWarning = FALSE;
static BOOL s_bWarn2Error = FALSE;
static BOOL s_bErrorCounting = TRUE;
static int s_cSuppressed = 0;
static int s_suppressedWarnings[32];

static char s_szSourceFile[c_nMaxTokenSize] = "";
static const char *s_pszPrefix = NULL;
static const char *s_pszSuffix = NULL;

void SuppressWarning()
{
    s_bSuppressWarning = TRUE;
}

void TreatWarningAsError()
{
    s_bWarn2Error = TRUE;
}

void ErrorReporterInit(const char *pszFile)
{
    strcpy(s_szSourceFile, pszFile);
}

void DisableErrorCounting()
{
    s_bErrorCounting = FALSE;
}

void EnableErrorCounting()
{
    s_bErrorCounting = TRUE;
}

void ErrorStringFix(const char *pszPrefix, const char *pszSuffix)
{
    s_pszPrefix = pszPrefix;
    s_pszSuffix = pszSuffix;
}

void *SaveErrorContext()
{
    char *pszSource;

    pszSource = new char[strlen(s_szSourceFile) + 1];
    if (!pszSource) return NULL;
    strcpy(pszSource, s_szSourceFile);

    return (void *)pszSource;
}

void RestoreErrorContext(void *pv)
{
    if (pv) {
        strcpy(s_szSourceFile, (char *)pv);
        delete [] (char *)pv;
    }
}

void DisableWarning(int wrn)
{
    int n;

    if (wrn <= 0 || wrn >= CAR_ErrorStart) return;

    for (n = 0; n < s_cSuppressed; n++) {
        if (0 == s_suppressedWarnings[n]) {
            s_suppressedWarnings[n] = wrn;
            return;
        }
    }
    s_suppressedWarnings[s_cSuppressed++] = wrn;
}

void EnableWarning(int wrn)
{
    int n;

    if (wrn <= 0 || wrn >= CAR_ErrorStart) return;

    for (n = 0; n < s_cSuppressed; n++) {
        if (wrn == s_suppressedWarnings[n]) {
            s_suppressedWarnings[n] = 0;
            return;
        }
    }
}

void __cdecl ErrorReport(CARErrorNo err, ...)
{
    int n;
    va_list vl;

    if (err < CAR_ErrorStart) {
        if (s_bSuppressWarning) return;

        for (n = 0; n < s_cSuppressed; n++)
            if (err == s_suppressedWarnings[n]) return;
    }

    for (n = 0; n < c_nErrorNumber; n++) {
        if (err == s_errorMessages[n].err) {
            if (s_pszPrefix) fputs(s_pszPrefix, stderr);

            if (err > CAR_ErrorStart || s_bWarn2Error) {
                fprintf(stderr, "[ERROR] carc (0x%04x) %s %d : ",
                        err, s_szSourceFile, g_nLineNumber);
                if (s_bErrorCounting) g_nErrorNumber++;
            }
            else {
                fprintf(stderr, "[WARN] carc (0x%04x) %s %d : ",
                        err, s_szSourceFile, g_nLineNumber);
                if (s_bErrorCounting) g_nWarningNumber++;
            }
            va_start(vl, err);
            vfprintf(stderr, s_errorMessages[n].pszMessage, vl);
            va_end(vl);

            if (s_pszSuffix) fputs(s_pszSuffix, stderr);
            fputs("\n", stderr);
            break;
        }
    }

    if (CLSLastError() != CLS_NoError) {
        fprintf(stderr, "[ERROR] carc (0x7FFF) %s %d : Scrap - %s\n",
                s_szSourceFile, g_nLineNumber, CLSLastErrorMessage());
    }
}

void CreateError(int r, const char *pszType, const char *pszName)
{
    switch (r) {
        case CLSError_DupEntry:
            ErrorReport(CAR_E_DupEntry, pszType, pszName);
            break;

        case CLSError_FullEntry:
            ErrorReport(CAR_E_FullEntry, pszType);
            break;

        case CLSError_OutOfMemory:
            ErrorReport(CAR_E_OutOfMemory);
            break;

        case CLSError_NameConflict:
            ErrorReport(CAR_E_NameConflict, pszType, pszName);
            break;

        default:
            assert(TRUE == FALSE);
            break;
    }
}
