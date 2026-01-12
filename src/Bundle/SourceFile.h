/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#pragma once
#include "Common.h"
#include "SymbolTable.h"
#include "AstDecl.h"
#include "SymbolScope.h"
#include "OLFunc.h"
#include "BundleCommon.h"
#include "CompileCommon.h"
#include "TextBuilder.h"


namespace OL
{

class CompileMsg;
class DebugVisitor;
class RecursiveVisitor;
class SourceFile;

struct SymbolImport
{
    OLString         Name;
    WPtr<SourceFile> DeclFile;
    bool            IsType;
};
DECLEAR_STRUCT_RTTI(SymbolImport)

struct SymbolExport
{
    OLString        Name;
    SourceRange    SrcRange;
    bool            IsType;

};
DECLEAR_STRUCT_RTTI(SymbolExport)

class ImportedScopeGroup;
class LPClassLib;
class BuildSetting;
class SourceFile
{
    DECLEAR_RTTI();
public:
    SourceFile(OLString InFileName);
    
    OLString            FileIdenty;    // name use in project, to be used in 'require'
    OLString            FileName;
    OLString            OutputName;

    SPtr<ARoot>         AstRoot;
    SPtr<SymbolTable>   Symbols;
    CompileMsg          CM;
    

    OLMap<OLString, SymbolImport> ImportVars;
    OLMap<OLString, SymbolImport> ImportTypes;
    OLMap<OLString, SymbolExport> Exports;

    SPtr<ImportedScopeGroup> ImportedScope;

    TextBuilder             OutText;
    SPtr<TextParagraph>     MainBlock;
    SPtr<TextParagraph>     StaticBlock;

    BuildSetting*           Settings;

    int UnbindVarCount;    //Includes UnbindTypeCount
    int UnbindTypeCount;

    bool DoLocalCompile();
    bool DoTypeResolve();
    bool DoCodeGen(LPClassLib& ClassLib);

    void DoResolveAsAPI();

    void RunVisitor(SPtr<ARoot> Ast, RecursiveVisitor& Vis, bool EnableBreak, OLFunc<void(DebugVisitor&)> LoggerSetter);
    void RunVisitor(SPtr<ARoot> Ast, RecursiveVisitor& Vis, bool EnableBreak);

    int GetUnbindTypeCount();
    int GetUnbindVarCount();
    bool BindType(OLString Name, SPtr<SourceFile> DeclFile);
    bool BindVar(OLString Name, SPtr<SourceFile> DeclFile);
    void AddAPISource(SPtr<SourceFile> APIFile);
    void PrintUnbindError();

    void ApplyBuildSetting(BuildSetting& Setting);

};

}