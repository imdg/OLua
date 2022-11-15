#pragma once
#include <vector>
#include <utility>

namespace OL
{
    class OLListBase
    {
    public:
        virtual const void* GetBuffer() = 0;
        virtual int   GetDataSize() = 0;
        virtual int   GetDataStride() = 0;
        virtual int   GetDataCount() = 0;
    };
    template<typename Ty>
    class OLList : public OLListBase
    {
    public:
        OLList(){};
        OLList(int InitSize) : Inner(InitSize) {};
        OLList(const OLList& Src) :  Inner(Src.Inner) {};
        OLList(const OLList&& Src) : Inner(std::move(Src.Inner)) {};

        Ty DequeueHead()
        {
            Ty Ret = Inner[0];
            Inner.erase(Inner.begin());
            return Ret;
        };

        Ty& Top()
        {
            return Inner[Inner.size() - 1];
        };

        void Pop()
        {
            Inner.erase(Inner.begin() + Inner.size() - 1);
        };

        
        void PopNum(int Num)
        {
            if(Num >= Inner.size())
                Inner.clear();
            else
                Inner.erase(Inner.begin() + Inner.size() - Num, Inner.end());
        };

        void PopTo(int Index)
        {
            if(Index < 0)
                Inner.clear();
            else    
                Inner.erase(Inner.begin() + Index + 1, Inner.end());
        };

        Ty PickPop()
        {
            Ty Ret = Inner[Inner.size() - 1];
            Pop();
            return Ret;
        };

        Ty& Add(const Ty& Item) 
        {
            Inner.push_back(Item);
            return Inner[Inner.size() - 1];
        };

        template <typename... ConstructArgs >
        Ty& AddConstructed(ConstructArgs... Args)
        {
            Inner.push_back(Ty(Args...));
            return Inner[Inner.size() - 1];
        }

        void RemoveAt(int Index)
        {
            Inner.erase(Inner.begin() + Index);
        };
        void Insert(Ty& Item, int Index)
        {
            Inner.insert(Inner.begin() + Index, Item);
        };
        void Insert(Ty&& Item, int Index)
        {
            Inner.insert(Inner.begin() + Index, std::move(Item));
        };
        int Count()
        {
            return (int)Inner.size();
        };

        void Clear()
        {
            Inner.clear();
        };

        void Resize(int Num)
        {
            Inner.resize(Num);
        };

        void Reserve(int Num)
        {
            Inner.reserve(Num);
        };

        Ty& operator [] (const int Index)
        {
            return Inner[Index];
        };
        Ty* Data()
        {
            return Inner.data();
        };

        virtual const void* GetBuffer()
        {
            return Inner.data();
        };
        virtual int   GetDataSize()
        {
            return (int)Inner.size() * sizeof(Ty);
        };
        virtual int   GetDataStride()
        {
            return sizeof(Ty);
        };
        virtual int   GetDataCount()
        {
            return (int)Inner.size();
        };

        const OLList<Ty>& operator = (const OLList<Ty>& Src)
        {
            Inner = Src.Inner;
            return *this;
        };
        const OLList<Ty>& operator = (const OLList<Ty>&& Src)
        {
            Inner = std::move(Src.Inner);
            return *this;
        };

    private:
        std::vector<Ty> Inner;
    };

}