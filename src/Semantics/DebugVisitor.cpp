/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "Common.h"
#include "DebugVisitor.h"
#include "ABase.h"
#include "Logger.h"

namespace OL
{
DebugVisitor::DebugVisitor(RecursiveVisitor* InWrappedVisitor, bool InEnableBreak)
{
    WrappedVisitor = InWrappedVisitor;
    EnableBreak = InEnableBreak;
}

void DebugVisitor::OnVisit(SPtr<ABase> Node)
{
    if(Node->SrcRange.Flag & LF_Break)
    {
        if(EnableBreak)
            VERBOSE(LogMisc, T("Breakpoint hitsat (%d, %d)\n"), Node->SrcRange.Start.Line, Node->SrcRange.Start.Col);
    }
    for(int i = 0; i < OnVisitCallbacks.Count(); i++)
    {
        OnVisitCallbacks[i](Node);
    }
}

void DebugVisitor::OnBeginVisit(SPtr<ABase> Node)
{
    if(Node->SrcRange.Flag & LF_Break)
    {
        if(EnableBreak)
            VERBOSE(LogMisc, T("Breakpoint hits at (%d, %d)\n"), Node->SrcRange.Start.Line, Node->SrcRange.Start.Col);
    }

    for(int i = 0; i < OnBeginVisitCallbacks.Count(); i++)
    {
        OnBeginVisitCallbacks[i](Node);
    }
}

void DebugVisitor::OnEndVisit(SPtr<ABase> Node)
{
    if(Node->SrcRange.Flag & LF_Break)
    {
        if(EnableBreak)
            VERBOSE(LogMisc, T("Breakpoint hitsat (%d, %d)\n"), Node->SrcRange.Start.Line, Node->SrcRange.Start.Col);
    }

    for(int i = 0; i < OnEndVisitCallbacks.Count(); i++)
    {
        OnEndVisitCallbacks[i](Node);
    }
}

void DebugVisitor::OnVisitFinish(SPtr<ABase> Node, EVisitStatus Status)
{
    for(int i = 0; i < OnVisitFinishCallbacks.Count(); i++)
    {
        OnVisitFinishCallbacks[i](Node, Status);
    }
}

void DebugVisitor::OnBeginVisitFinish(SPtr<ABase> Node, EVisitStatus Status)
{

    for(int i = 0; i < OnBeginVisitFinishCallbacks.Count(); i++)
    {
        OnBeginVisitFinishCallbacks[i](Node, Status);
    }
}

void DebugVisitor::OnEndVisitFinish(SPtr<ABase> Node, EVisitStatus Status)
{
    for(int i = 0; i < OnEndVisitFinishCallbacks.Count(); i++)
    {
        OnEndVisitFinishCallbacks[i](Node, Status);
    }
}

}