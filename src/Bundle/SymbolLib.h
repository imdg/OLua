#pragma once
#include "Common.h"
#include "BundleCommon.h"
#include "CompileCommon.h"
#include "SourceFile.h"
namespace OL
{

class SourceFile;
class CompileMsg;
struct PendingImport
{
    OLString Name;
    WPtr<SourceFile> RefFile;
    SPtr<PendingImport> Next;
};
DECLEAR_STRUCT_RTTI(PendingImport)

struct ExportDecl
{
    OLString Name;
    CodeLineInfo Line;
    WPtr<SourceFile> DeclFile;
    
};
DECLEAR_STRUCT_RTTI(ExportDecl)

class SymbolLib
{
    DECLEAR_RTTI()
public:
    OLMap<OLString, ExportDecl>            ExportVarLib;
    OLMap<OLString, SPtr<PendingImport> > ImportVarLib;

    OLMap<OLString, ExportDecl>            ExportTypeLib;
    OLMap<OLString, SPtr<PendingImport> > ImportTypeLib;


    void MatchFileTypes(SPtr<SourceFile> InputFile, OLList<SPtr<SourceFile> >& MatchedFile);
    void MatchFileVars(SPtr<SourceFile> InputFile, OLList<SPtr<SourceFile> >& MatchedFile);

    void GetTypeUnmatched( OLList<SPtr<SourceFile> >& Files);
    void GetVarUnmatched( OLList<SPtr<SourceFile> >& Files);
    OLMap<SPtr<SourceFile>, bool> TypeMatchStatus;
    OLMap<SPtr<SourceFile>, bool> VarMatchStatus;


};


}