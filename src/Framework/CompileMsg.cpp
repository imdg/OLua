/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "CompileMsg.h"
#include <stdio.h>
#include <stdarg.h>
#include "Logger.h"
#include "CompileCommon.h"

namespace OL
{
#define DEFAULT_LEVEL_MUTE      0
#define DEFAULT_LEVEL_MSG       1
#define DEFAULT_LEVEL_WARNING   2
#define DEFAULT_LEVEL_ERROR     3

const TCHAR* MsgLevelTitle[] = { T("Mute"), T("Info"), T("Warning"), T("Error")};

struct MsgDetailInfo
{
    ECompileMsgType Msg;
    int             Level;
    const TCHAR*    Text;
    
};

MsgDetailInfo MsgDetails[] = {
    {CMT_Dummy,             DEFAULT_LEVEL_MSG,      T("Dummy compile message for test") },
    {CMT_UnknownCh,         DEFAULT_LEVEL_ERROR,    T("Unexpected character: \'%c\'") },
    {CMT_ExponentNumErr,    DEFAULT_LEVEL_ERROR,    T("Exponent number format error: \'%s\'") },
    {CMT_DecimalNumErr,     DEFAULT_LEVEL_ERROR,    T("Decimal number format error: \'%s\'") },
    {CMT_HexExpected,       DEFAULT_LEVEL_ERROR,    T("Hex character expected") },
    {CMT_UnknowEscape,      DEFAULT_LEVEL_ERROR,    T("Unknown escape identifier: \'%c\'") },
    {CMT_UnfinishedString,  DEFAULT_LEVEL_ERROR,    T("Unfinished string") },
    {CMT_ExpectConst,       DEFAULT_LEVEL_ERROR,    T("Expect constant value") },
    {CMT_ExpectTypeIdentity,    DEFAULT_LEVEL_ERROR,    T("Expect type identity after 'as'") },
    {CMT_ExpectDotName,     DEFAULT_LEVEL_ERROR,    T("Expect member name after '.'") },
    {CMT_IncompleteBracket, DEFAULT_LEVEL_ERROR,    T("Expect ']' to complete the bracket started at (line:%d, col:%d") },

    
    {CMT_NeedMacroName,     DEFAULT_LEVEL_ERROR,   T("Expect a name for macro definition") },
    {CMT_MacroParamNeedName, DEFAULT_LEVEL_ERROR,   T("Expect a name for macro parameters") },
    {CMT_MacroSytexError,   DEFAULT_LEVEL_ERROR,   T("Sytex error in macro definition") },
    {CMT_NeedMacroOpt,      DEFAULT_LEVEL_ERROR,   T("Expect preprocessing instruction") },
    {CMT_MacroTooManyLevels,DEFAULT_LEVEL_ERROR,   T("Too many levels for recursive macro expanding") },
    {CMT_MacroExpandError,  DEFAULT_LEVEL_ERROR,   T("Parameter error when expanding macro") },
    {CM_NeedMacroNameForIf,  DEFAULT_LEVEL_ERROR,   T("Expect a macro name for this preprocessing instruction") },
    {CMT_CondiCompileNotMatch,  DEFAULT_LEVEL_ERROR,   T("Conditional compiling block instructions do not match ") },
   

    {CMT_NameAfterColon,        DEFAULT_LEVEL_ERROR,    T("Expect member name after ':'") },
    {CMT_ColonForCallOnly,      DEFAULT_LEVEL_ERROR,    T("Expect function call after derefrence member by ':'") },
    {CMT_IncompleteParenthese,  DEFAULT_LEVEL_ERROR,    T("Expect ')' to complete the bracket started at (line:%d, col:%d") },
    {CMT_NotPrimaryExpr,        DEFAULT_LEVEL_ERROR,    T("Expression error, expecting variant name or parenthese ") },
    {CMT_ConstructorMissComma,  DEFAULT_LEVEL_ERROR,    T("Constructor expects ',' to continue or '}' to finish") },
    {CMT_PartialKeys,           DEFAULT_LEVEL_ERROR,    T("Some elements do not have keys")    },
    {CMT_ExpectConstructor,     DEFAULT_LEVEL_ERROR,    T("Expect '{' to begin a constructor ") },
    {CMT_ConstructorNeedValue,  DEFAULT_LEVEL_ERROR,    T("Expect '=' to assign a value") },
    {CMT_IncompleteAngleBracket,DEFAULT_LEVEL_ERROR,    T("Expect '>' to complete the angle bracket started at (line:%d, col:%d") },

    {CMT_WhileMissingDo,        DEFAULT_LEVEL_ERROR,    T("Expect 'do' after a 'while' statement") },
    {CMT_WhileMissingEnd,       DEFAULT_LEVEL_ERROR,    T("Missing 'end' in a 'while' block") },

    {CMT_IfMissingThen,         DEFAULT_LEVEL_ERROR,    T("Expect 'then' after a 'if' statement") },
    {CMT_ElseIfMissingThen,     DEFAULT_LEVEL_ERROR,    T("Expect 'then' after a 'elseif' statement") },
    {CMT_IfMissingEnding,       DEFAULT_LEVEL_ERROR,    T("Expect 'end' or 'else' or 'elseif' after a 'if' statement") },
    {CMT_ElseIfMissingEnding,   DEFAULT_LEVEL_ERROR,    T("Expect 'end' or 'else' or 'elseif' after a 'elseif' statement") },
    {CMT_ElseMissingEnding,     DEFAULT_LEVEL_ERROR,    T("Expect 'end' after a 'else' statement") },

    {CMT_ForNeedsVarName,       DEFAULT_LEVEL_ERROR,    T("Expect a valid variant name after 'for'") },
    {CMT_NeedVarName,           DEFAULT_LEVEL_ERROR,    T("Expect a valid variant name") },
    {CMT_NeedTypeName,          DEFAULT_LEVEL_ERROR,    T("Expect a valid type name") },

    {CMT_ForNeedsComma,         DEFAULT_LEVEL_ERROR,    T("Too few parameters for the 'for' statement") },
    {CMT_ForNeedsDo,            DEFAULT_LEVEL_ERROR,    T("Expect 'do' for the 'for' statement") },
    {CMT_ForNeedsEnd,           DEFAULT_LEVEL_ERROR,    T("Expect 'end' for the 'for' statement") },
    {CMT_ForNeedsIn,            DEFAULT_LEVEL_ERROR,    T("Expect 'in' for the 'for' statement") },
    {CMT_RepeatNeedsUntil,      DEFAULT_LEVEL_ERROR,    T("Expect 'until' for the 'repeat' statement") },

    {CMT_FuncNeedName,          DEFAULT_LEVEL_ERROR,    T("Expect a valid function name") },
    {CMT_FuncNeedsParam,        DEFAULT_LEVEL_ERROR,    T("Expect funtion parameter list") },
    {CMT_FuncParamMissingRP,    DEFAULT_LEVEL_ERROR,    T("Expect ')' to complete the parameter list") },
    {CMT_FuncTypeMissingRB,     DEFAULT_LEVEL_ERROR,    T("Expect ')' to complete the return type list") },
    {CMT_FuncMissingEnd,        DEFAULT_LEVEL_ERROR,    T("Expect 'end' for a function") },

    {CMT_LValueRequire,         DEFAULT_LEVEL_ERROR,    T("Need an L-value to be assigned to") },
    {CMT_AssignNeedsEq,         DEFAULT_LEVEL_ERROR,    T("This expression cannot be a statement") },
    {CMT_AssignNeedsExpr,       DEFAULT_LEVEL_ERROR,    T("Need an expression for the assignment") },
    {CMT_LableNeedName,         DEFAULT_LEVEL_ERROR,    T("Need a name for the lable") },
    {CMT_GotoNeedName,          DEFAULT_LEVEL_ERROR,    T("Need a lable name for 'goto'") },

    {CMT_NeedLValue,            DEFAULT_LEVEL_ERROR,    T("Need an L-value for a new statement") },
    {CMT_UnknownStat,           DEFAULT_LEVEL_ERROR,    T("Unknown syntex for a new statement") },
    {CMT_ExternDefError,        DEFAULT_LEVEL_ERROR,    T("'extern' has to be used together with 'function' or 'class'") },

    {CMT_FailUniOpSubexpr,      DEFAULT_LEVEL_MSG,    T("Fail to parse an uni-operator expression") },
    {CMT_FailFuncBody,          DEFAULT_LEVEL_MSG,    T("Fail to parse a function body") },

    {CMT_FailExpr,              DEFAULT_LEVEL_MSG,    T("Fail to parse an expression") },
    {CMT_FailPrimaryExpr,       DEFAULT_LEVEL_MSG,    T("Fail to parse an primary expression") },
    {CMT_FailParamList,         DEFAULT_LEVEL_MSG,    T("Fail to parse the parameter list") },
    {CMT_FailExprInParenthese,  DEFAULT_LEVEL_MSG,    T("Fail to parse the expression in the parenthese") },
    {CMT_FailExprInBracket,     DEFAULT_LEVEL_MSG,    T("Fail to parse the expression in the bracket") },

    {CMT_FailBlock,             DEFAULT_LEVEL_MSG,    T("Fail to parse the code block") },
    {CMT_FailWhileCondition,    DEFAULT_LEVEL_MSG,    T("Fail to parse the condition expression in 'while' statement") },
    {CMT_FailIfCondition,       DEFAULT_LEVEL_MSG,    T("Fail to parse the condition expression in 'if' or 'elseif' statement") },

    {CMT_FailForDecl,           DEFAULT_LEVEL_MSG,    T("Fail to parse declearation in 'for' statement") },
    {CMT_FailForRange,          DEFAULT_LEVEL_MSG,    T("Fail to parse range in 'for' statement") },
    {CMT_FailForIter,           DEFAULT_LEVEL_MSG,    T("Fail to parse iterator in 'for' statement") },
    {CMT_FailRepeatCondition,   DEFAULT_LEVEL_MSG,    T("Fail to parse the condition expression in 'if' or 'elseif' statement") },

    {CMT_FailFuncParamDecl,     DEFAULT_LEVEL_MSG,    T("Fail to parse the parameter declearation in a function") },
    {CMT_FailFuncRetType,       DEFAULT_LEVEL_MSG,    T("Fail to parse the return type in a function") },
    {CMT_VariableParamOrder,    DEFAULT_LEVEL_ERROR,    T("Variable parameter must be decleared at last of the parameter list") },

    {CMT_ClassNeedsName,        DEFAULT_LEVEL_ERROR,    T("Need a class name") },
    {CMT_NeedBaseName,          DEFAULT_LEVEL_ERROR,    T("Need a base class or interface name") },
    {CMT_MemberDeclWrong,       DEFAULT_LEVEL_ERROR,    T("Not a valid class member") },
    {CMT_MethodNeedsName,       DEFAULT_LEVEL_ERROR,    T("Need a method name") },
    {CMT_ReservedNew,           DEFAULT_LEVEL_ERROR,    T("Cannot define a class member named 'new'. It is reserved for default constructor") },
    {CMT_CtorNotAllowOnExternal,           DEFAULT_LEVEL_WARNING,    T("Cannot define constructors for external classes. This one will be ignored") },
    {CMT_InitNotAllowOnExternal,           DEFAULT_LEVEL_WARNING,    T("Cannot initialize variable on external classes. This one will be ignored") },

    {CMT_CannotDefineGlobal,    DEFAULT_LEVEL_ERROR,    T("Cannot define a global variable here") },
    {CMT_EnumNeedsName,         DEFAULT_LEVEL_ERROR,    T("Enum needs a valid name") },
    {CMT_EnumItemSytexError,    DEFAULT_LEVEL_ERROR,    T("Syntex error in an enum definition") },
    {CMT_ErrorInEnumItem,       DEFAULT_LEVEL_ERROR,    T("Cannot parse the enum item") },
    {CMT_InterfaceNeedsName,    DEFAULT_LEVEL_ERROR,    T("Interface needs a valid name") },
    {CMT_InterfaceMemberError,  DEFAULT_LEVEL_ERROR,    T("Cannot parse interface member, where only abstract method is allowed") },

    
    {CMT_ExpectAttribName,        DEFAULT_LEVEL_ERROR,    T("Expect a valid attribute name") },
    {CMT_AttribSytexErr,        DEFAULT_LEVEL_ERROR,    T("Attribute syntex error. It should be like '[ name = value, ... ]'") },
    {CMT_AttribValueErr,        DEFAULT_LEVEL_ERROR,    T("Cannot use this token as attribute value. Only integer, float, bool and nil is allowed") },

    {CMT_NeedAliasName,        DEFAULT_LEVEL_ERROR,    T("Expect a name for 'alias'") },
    {CMT_AliasTypeNeeded,        DEFAULT_LEVEL_ERROR,    T("Expect keyword 'as' and a type identifier for the alias") },
    



    {CMT_NoType,                    DEFAULT_LEVEL_ERROR,    T("Cannot find type: '%s'. It will be compiled as 'any' if this message is processed as a warning") },
    {CMT_NoBaseType,                DEFAULT_LEVEL_ERROR,    T("Cannot find base class or interface: %s") },
    {CMT_UnresolvedVar,             DEFAULT_LEVEL_ERROR,    T("Undecleared variable found: '%s'. It will be compiled as a global variable with type 'any' if this message is processed as a warning ") },
    {CMT_UseBeforeDecl,             DEFAULT_LEVEL_ERROR,    T("Variable '%s' has been used before declearation") },
    {CMT_NameConflict,             DEFAULT_LEVEL_ERROR,    T("Name '%s' conflicted to the previous declearation at line: %d") },

    {CMT_InterfaceBaseTypeError,    DEFAULT_LEVEL_ERROR,    T("Type '%s' cannot be the base type of an interface. Only interface is allowed") },
    {CMT_TooManyBaseClasses,        DEFAULT_LEVEL_ERROR,    T("Too many base classes while only one is allowed. Try using interface instead") },
    {CMT_ClassBaseTypeError,        DEFAULT_LEVEL_ERROR,    T("Type '%s' cannot be the base type of a class. Only interfaces or a class is allowed") },
    {CMT_MixedInherit,        DEFAULT_LEVEL_ERROR,    T("External and non-external classes cannot inherit each other.") },

    {CMT_NoAliasType,            DEFAULT_LEVEL_ERROR,    T("Cannot find type: '%s'. This alias will be seen as 'any' if this message is processed as a warning") },
    
    {CMT_NoVariableParam,        DEFAULT_LEVEL_ERROR,    T("Cannot use '...' because variable param is not defined in the owner function.") },


    {CMT_MapNeedStringKey,      DEFAULT_LEVEL_ERROR,    T("Map requires keys of 'string' type to use '.' or ':' to access") },
    {CMT_NoMember,              DEFAULT_LEVEL_ERROR,    T("Type '%s' does not have member '%s'. It will be compiled as a variable with type 'any' if this message is processed as a warning") },
    {CMT_PrivateMember,         DEFAULT_LEVEL_ERROR,    T("Member '%s' in type '%s' cannot be accessed in current scope") },
    {CMT_TypeConvDataLose,      DEFAULT_LEVEL_WARNING,  T("Converting from '%s' to '%s' could possibly lose data") },
    {CMT_TypeConvUnSafe,        DEFAULT_LEVEL_WARNING,  T("Converting from '%s' to '%s' could be unsafe") },
    {CMT_TypeConvNoWay,         DEFAULT_LEVEL_ERROR,    T("Cannot convert from '%s' to '%s'") },

    {CMT_ArrayNeedIntIndex,     DEFAULT_LEVEL_ERROR,    T("Integer index is required to access arrays") },
    {CMT_MapKeyTypeMismatch,    DEFAULT_LEVEL_ERROR,    T("Map key does not match the decleared type") },
    {CMT_InvalidBracket,        DEFAULT_LEVEL_ERROR,    T("'[]' is not supported on type '%s'") },
    {CMT_RequirFunc,            DEFAULT_LEVEL_ERROR,    T("Expression need to be a function") },
    {CMT_TooManyParam,          DEFAULT_LEVEL_ERROR,    T("Too many parameters on function type '%s'") },
    {CMT_TooFewParam,           DEFAULT_LEVEL_ERROR,    T("Too few parameters on function type '%s'") },
    {CMT_ParamTypeError,        DEFAULT_LEVEL_ERROR,    T("Type mismatch of parameter %d on function call") },

    {CMT_ImplicitGlobal,        DEFAULT_LEVEL_WARNING,    T("Implicitly decleared global variant '%s'") },
    {CMT_IllegalSuper,        DEFAULT_LEVEL_ERROR,    T("Cannot use 'super' except in a method of a class with a super class") },
    {CMT_IllegalSelf,        DEFAULT_LEVEL_ERROR,    T("Cannot use 'self' except in a class method") },

    {CMT_UniopNotMatch,        DEFAULT_LEVEL_ERROR,    T("Operator '%s' does not apply on type '%s'") },
    {CMT_BinopNotMatch,        DEFAULT_LEVEL_ERROR,    T("Operator '%s' does not apply on type '%s' and type '%s'") },

    {CMT_OpMustBeFunc,        DEFAULT_LEVEL_ERROR,    T("Overrided operator '%s' must be a instance function") },
    {CMT_OpReturnCount,        DEFAULT_LEVEL_ERROR,    T("Overrided operator '%s' has to return a value") },
    {CMT_OpBoolRequired,        DEFAULT_LEVEL_ERROR,    T("Overrided operator '%s' must return 'bool' type") },
    {CMT_OpStringRequired,        DEFAULT_LEVEL_ERROR,    T("Overrided operator '%s' must return 'string' type") },
    {CMT_OpParamCount,        DEFAULT_LEVEL_ERROR,    T("Overrided operator '%s' must define %d parameter(s)") },

    
    {CMT_ArrayIterMismatch,        DEFAULT_LEVEL_ERROR,    T("Iterator dose not match the array type") },
    {CMT_MapIterMismatch,        DEFAULT_LEVEL_ERROR,    T("Iterator dose not match the map type") },
    {CMT_IterMismatch,        DEFAULT_LEVEL_ERROR,    T("Iterator dose not match the source type") },
    {CMT_NotAIterator,        DEFAULT_LEVEL_ERROR,    T("Cannot iterate this type") },
    {CMT_ReturnMismatch,        DEFAULT_LEVEL_ERROR,    T("Returned result dose not match the declear amount, decleard %d, but returned %d") },

    {CMT_NoColonForTypeName,        DEFAULT_LEVEL_ERROR,    T("Cannot use ':' to access members on a type name. Try use an instance name.") },
    {CMT_DotForStatic,        DEFAULT_LEVEL_ERROR,    T("Member '%s' is not a static member of type '%s'") },
    {CMT_ColonForNonStaticMethod,        DEFAULT_LEVEL_ERROR,    T("Member '%s' is not a non-static method of type '%s'") },
    {CMT_DotForNonStaticVar,        DEFAULT_LEVEL_ERROR,    T("Member '%s' is not a non-static variable of type '%s'") },
    {CMT_NoEnumItem,        DEFAULT_LEVEL_ERROR,    T("'%s' is not an item of enum type '%s'") },


    {CMT_NoReturn,        DEFAULT_LEVEL_ERROR,    T("Not all path returns a value") },
    {CMT_LastReturn,        DEFAULT_LEVEL_ERROR,    T("Statement is not allowed to be after 'return'") },

    {CMT_ConstRestrictAssign,        DEFAULT_LEVEL_ERROR,    T("Cannot assign value to a constant variable") },
    {CMT_ConstRestrictCall,        DEFAULT_LEVEL_ERROR,    T("Cannot pass constant variable to param %d, which is a non-constant params of a reference type") },
    {CMT_ConstRestrictOwner,        DEFAULT_LEVEL_ERROR,    T("Cannot call non-constant member function on a constant owner") },



    {CMT_IterTupleNum,        DEFAULT_LEVEL_ERROR,    T("Iterator must return 3 element for iterator function, target and first index") },
    {CMT_IterEntryType,        DEFAULT_LEVEL_ERROR,    T("Require a iterator function for the first item of iterator") },
    {CMT_IterFuncType,        DEFAULT_LEVEL_ERROR,    T("Iterator function must have 2 params for the target and index") },
    {CMT_IterTargetType,        DEFAULT_LEVEL_ERROR,    T("Iterator target is incompatible with the iterator function") },
    {CMT_IterIndexType,        DEFAULT_LEVEL_ERROR,    T("Iterator index type is incompatible with the iterator function") },
    {CMT_IterItemNum,        DEFAULT_LEVEL_ERROR,    T("Iterator function returns less results than required") },
    {CMT_IterFuncIndexType,        DEFAULT_LEVEL_ERROR,    T("Returned index is incompatible with decleared index in iterator function") },
    {CMT_IterValueType,        DEFAULT_LEVEL_ERROR,    T("Returned value %d is incompatible with that decleared in 'for'") },


    {CMT_SymbolRedefinition,        DEFAULT_LEVEL_ERROR,    T("Symbol '%s' redefinition with: %s(%d,%d)") },
};

CompileMsg::CompileMsg()
{
    ErrorCount = 0;
    WarningCount = 0;
}

void CompileMsg::SetFile(const TCHAR* File)
{
    FileName = File;

}

void  CompileMsg::Reset()
{
    ErrorCount = 0;
    WarningCount = 0;
}


#define BASE_SIZE 512
void CompileMsg::Log(ECompileMsgType msg, int Line, int Col, ...)
{
    MsgDetailInfo& CurrMsgDetail = MsgDetails[(int)msg];
    int Level = CurrMsgDetail.Level;
    if(LevelOverride.Exists(msg))
    {
        Level = LevelOverride[msg];
    }
    if(Level == DEFAULT_LEVEL_ERROR)
        ErrorCount++;
    if(Level == DEFAULT_LEVEL_WARNING)
        WarningCount++;

    TCHAR Buffer[BASE_SIZE];

    
    TCHAR* RealFormat = (TCHAR*)CurrMsgDetail.Text;
#if (defined(PLATFORM_MAC) || defined(PLATFORM_LINUX)) && USE_WCHAR
    OLString TempFormat = CurrMsgDetail.Text;
    TempFormat.Replace(T("%s"), T("%ls"));
    RealFormat = (TCHAR*)TempFormat.CStr();
#endif 


    va_list ap;
    va_start(ap, Col);
    t_vsnprintf(Buffer, BASE_SIZE, RealFormat, ap);
    va_end(ap);

    OLString FullText;
    FullText.Printf(T("%s(%d,%d): %s: (%s) %s"), FileName.CStr(), Line, Col, MsgLevelTitle[Level]
        , TRTTIEnumInfo<ECompileMsgType>::GetEnumText((int)msg, EGF_ValueText)
        , Buffer );
    Logger::LogRaw(LogCompile, FullText.CStr());
}
void CompileMsg::OverrideMsgLevel(ECompileMsgType msg, int Level)
{
    if(LevelOverride.Exists(msg))
        LevelOverride[msg] = Level;
    else
        LevelOverride.Add(msg, Level);
}
void CompileMsg::SetAsWarning(ECompileMsgType msg)
{
    OverrideMsgLevel(msg, DEFAULT_LEVEL_WARNING);
}
void CompileMsg::SetAsInfo(ECompileMsgType msg)
{
    OverrideMsgLevel(msg, DEFAULT_LEVEL_MSG);
}
void CompileMsg::SetAsError(ECompileMsgType msg)
{
    OverrideMsgLevel(msg, DEFAULT_LEVEL_ERROR);
}
void CompileMsg::SetAsMute(ECompileMsgType msg)
{
    OverrideMsgLevel(msg, DEFAULT_LEVEL_MUTE);
}

}