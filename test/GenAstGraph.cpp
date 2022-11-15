#include "Common.h"
#include "Action.h"
#include "Logger.h"
#include "ABase.h"
#include <stdio.h>
#include "Env.h"

namespace OL
{





class GenAstGraph : public Action
{
public:

    struct TypeNodeInfo
    {
        TypeInfo* Type;
        int Level;
    };

    void TranversSubClass(TypeInfo* BaseType, int Level, OLList<TypeNodeInfo>& OutList)
    {
        TypeInfo* Node = TypeInfo::GlobalTypeList;
        TypeNodeInfo Item;
        Item.Type = BaseType;
        Item.Level = Level;
        OutList.Add(Item);
        while(Node != nullptr)
        {
            if(Node->BaseType == BaseType)
            {
                TranversSubClass(Node, Level + 1, OutList);
            }
            Node = Node->GlobalNext;
        }

        //CallBack(BaseType, IsLeaf);

    }

    
    void WriteGraph(OLString& Content)
    {

        OLList<TypeNodeInfo> AllTypes;
        TranversSubClass(&(ABase::RTTI), 0,  AllTypes);
        for(int i = 0; i < AllTypes.Count(); i++)
        {
            TypeInfo* Curr = AllTypes[i].Type;
            for(int k = 0; k < AllTypes[i].Level; k++)
            {
                Content.Append(T("    "));
            }
            Content.AppendF(T("%s\n"), Curr->Name);

        }
    }

    virtual int Run()
    {

        OLString Output = Env::PrepareSavedFilePath(T("Doc"),T("AstGraph.txt"));
        OLString OutputContent;

        WriteGraph(OutputContent);

        FILE* OutputFile = fopen(T2A(Output.CStr()), "wb");
        fwrite(OutputContent.CStr(), OutputContent.Len(), 1, OutputFile);
        fclose(OutputFile);

        VERBOSE(LogMisc, T("Gen ast graph success!\n"));
        return 0;
    };
};

REGISTER_ACTION(GenAstGraph)

}