/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#pragma once
#include "Common.h"
#include "CompileCommon.h"
namespace OL
{


RTTI_ENUM(ECompileMsgType,
    CMT_Dummy,
    CMT_UnknownCh, // %c: ch
    CMT_ExponentNumErr, // %s: token
    CMT_DecimalNumErr, // %s: token
    CMT_HexExpected, 
    CMT_UnknowEscape, // %c: ch
    CMT_UnfinishedString, 
    CMT_ExpectConst,
    CMT_ExpectTypeIdentity,
    CMT_ExpectDotName,
    CMT_IncompleteBracket, // %d,%d   begin bracket (line, col)
    CMT_DecimalEscTooLarge,

    CMT_NeedMacroName,
    CMT_MacroParamNeedName, 
    CMT_MacroSytexError,
    CMT_NeedMacroOpt,
    CMT_MacroTooManyLevels,
    CMT_MacroExpandError,
    CM_NeedMacroNameForIf,
    CMT_CondiCompileNotMatch,
    
    CMT_NameAfterColon, 
    CMT_ColonForCallOnly, 
    CMT_IncompleteParenthese, // %d,%d   begin parenthese (line, col)
    CMT_NotPrimaryExpr,
    CMT_ConstructorMissComma,
    CMT_PartialKeys,
    CMT_ExpectConstructor,
    CMT_ConstructorNeedValue,
    CMT_IncompleteAngleBracket, // %d, %d begin angle bracket(line, col)

    CMT_WhileMissingDo,
    CMT_WhileMissingEnd,

    CMT_IfMissingThen,
    CMT_ElseIfMissingThen,
    CMT_IfMissingEnding,
    CMT_ElseIfMissingEnding,
    CMT_ElseMissingEnding,

    CMT_ForNeedsVarName,
    CMT_NeedVarName,
    CMT_NeedTypeName,

    CMT_ForNeedsComma,
    CMT_ForNeedsDo,
    CMT_ForNeedsEnd,
    CMT_ForNeedsIn,
    CMT_RepeatNeedsUntil,

    CMT_FuncNeedName,
    CMT_FuncNeedsParam,
    CMT_FuncParamMissingRP,
    CMT_FuncTypeMissingRB,
    CMT_FuncMissingEnd,

    CMT_LValueRequire,
    CMT_AssignNeedsEq,
    CMT_AssignNeedsExpr,
    CMT_LableNeedName,
    CMT_GotoNeedName,

    CMT_NeedLValue,
    CMT_UnknownStat,
    CMT_ExternDefError,

    CMT_FailUniOpSubexpr,
    CMT_FailFuncBody,
    CMT_FailExpr,
    CMT_FailPrimaryExpr,
    CMT_FailParamList,
    CMT_FailExprInParenthese,
    CMT_FailExprInBracket,

    CMT_FailBlock,
    CMT_FailWhileCondition,
    CMT_FailIfCondition,

    CMT_FailForDecl,
    CMT_FailForRange,
    CMT_FailForIter,
    CMT_FailRepeatCondition,

    CMT_FailFuncParamDecl,
    CMT_FailFuncRetType,
    CMT_VariableParamOrder,

    CMT_ClassNeedsName,
    CMT_NeedBaseName,
    CMT_MemberDeclWrong,
    CMT_MethodNeedsName,
    CMT_ReservedNew,
    CMT_CtorNotAllowOnExternal,
    CMT_InitNotAllowOnExternal,


    CMT_CannotDefineGlobal,
    CMT_EnumNeedsName,
    CMT_EnumItemSytexError,
    CMT_ErrorInEnumItem,
    CMT_InterfaceNeedsName,
    CMT_InterfaceMemberError,

    CMT_ExpectAttribName,
    CMT_AttribSytexErr,
    CMT_AttribValueErr,

    CMT_NeedAliasName,
    CMT_AliasTypeNeeded,
    
    // Semantics
    CMT_NoType, // %s
    CMT_NoBaseType, // %s
    CMT_UnresolvedVar, // %s
    CMT_UseBeforeDecl, // %s
    CMT_NameConflict,  // %s Name,%d line
    CMT_InterfaceBaseTypeError, // %s
    CMT_TooManyBaseClasses, 
    CMT_ClassBaseTypeError, //%s
    CMT_MixedInherit,

    CMT_NoAliasType, //%s

    CMT_NoVariableParam,

    CMT_MapNeedStringKey,
    CMT_NoMember, //%s ClassName, %s FieldName
    CMT_PrivateMember, //%s FieldName, %s ClassName

    CMT_TypeConvDataLose,   // %s from, %s to
    CMT_TypeConvUnSafe,   // %s from, %s to
    CMT_TypeConvNoWay,   // %s from, %s to
    
    CMT_ArrayNeedIntIndex,
    CMT_MapKeyTypeMismatch,
    CMT_InvalidBracket, // %s type
    CMT_RequirFunc,
    CMT_TooManyParam,
    CMT_TooFewParam,
    CMT_ParamTypeError,

    CMT_ImplicitGlobal, // %s var name
    CMT_IllegalSuper,
    CMT_IllegalSelf,

    CMT_UniopNotMatch, //%s op, %s typename
    CMT_BinopNotMatch, //%s op, %s typename1, %s typename2

    CMT_OpMustBeFunc,   // %s opfunc
    CMT_OpReturnCount,  // %s opfunc
    CMT_OpBoolRequired, // %s opfunc
    CMT_OpStringRequired, // %s opfunc
    CMT_OpParamCount,  // %s opfunc, %d param required

    CMT_ArrayIterMismatch,
    CMT_MapIterMismatch,
    CMT_IterMismatch,
    CMT_NotAIterator,
    CMT_ReturnMismatch, //%d decl num, %d ret num

    CMT_NoColonForTypeName, 
    CMT_DotForStatic,       // %s var/method name, %s class
    CMT_ColonForNonStaticMethod,    // %s var/method name, %s class
    CMT_DotForNonStaticVar,     // %s var/method name, %s class
    CMT_NoEnumItem,     // %s enum item name, %s enum name


    CMT_NoReturn,
    CMT_LastReturn,

    CMT_ConstRestrictAssign,
    CMT_ConstRestrictCall,  // %d param index
    CMT_ConstRestrictOwner,

    CMT_IterTupleNum,
    CMT_IterEntryType,
    CMT_IterFuncType,
    CMT_IterTargetType,
    CMT_IterIndexType,
    CMT_IterItemNum,
    CMT_IterFuncIndexType,
    CMT_IterValueType,   // %d value index

    CMT_SymbolRedefinition, //%s name, %s file, %d %d Line Col


    //Nilable
    CMT_NilableAsIndex,
    CMT_NonnilableNotInit,
    CMT_NilableIter,
    CMT_NilableConvert,
    CMT_AssignNilToNonnilable,
    CMT_UnwrapNonnilable,
    CMT_NonnilableCoalescing,
    CMT_NilableCoalescingNilable,
    CMT_NonnilableMember,
    CMT_NotAcceptedNilable,
    CMT_DerefNilable,
    CMT_CallNilable,
    CMT_MaxMsg
)
struct CodeLineInfo;
class CompileMsg
{
public:

    CompileMsg();
    void SetFile(const TCHAR* File);
    void Log(ECompileMsgType msg, int Line, int Col,  ...);

    template <typename ...T>
    void Log(ECompileMsgType msg, CodeLineInfo& LineInfo , T... args)
    {
        Log(msg, LineInfo.Line, LineInfo.Col, args...);
    };

    void Reset();

    void SetAsWarning(ECompileMsgType msg);
    void SetAsInfo(ECompileMsgType msg);
    void SetAsError(ECompileMsgType msg);
    void SetAsMute(ECompileMsgType msg);

    int ErrorCount;
    int WarningCount;


    static CompileMsg Default;

private:
    OLString FileName;
    OLMap<ECompileMsgType, int> LevelOverride;
    void OverrideMsgLevel(ECompileMsgType msg, int Level);
};

} // namespace OL
