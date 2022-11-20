/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#pragma once

#include <unordered_map>

#include "Defs.h"
#include "OLString.h"
#include "OLHash.h"


namespace OL
{

class OLMapBase
{
public:
    class BaseIterator
    {
    public:
        virtual void* GetKeyPtr() = 0;
        virtual void* GetValuePtr() = 0;
        virtual void  BaseNext() = 0;
        virtual bool BaseIsEnd() = 0;
        virtual ~BaseIterator() {};
    };

    virtual BaseIterator* BeginBaseIter() = 0;
    virtual void FinishBaseIter(BaseIterator* Iter) = 0;
    
};


template < typename TK, typename TV >
class OLMap : public OLMapBase
{
private:
    std::unordered_map<TK, TV, typename THasher<TK>::Type > InnerMap;
public:
    typedef TK KeyType;
    typedef TV ValueType;
    typedef typename std::unordered_map<TK, TV, typename THasher<TK>::Type >::iterator IterType;


    OLMap() {} ;
    OLMap(const OLMap<TK, TV>& Src) : InnerMap(Src.InnerMap) {} ;
    OLMap(const OLMap<TK, TV>&& Src) : InnerMap(std::move(Src.InnerMap)) {} ;

    void Add(const TK& Key, const TV& Value)
    {
        InnerMap[Key] = Value;
    };

    TV& Find(const TK& Key)
    {
        return InnerMap[Key];
    };

    TV* TryFind(const TK& Key)
    {
        IterType It = InnerMap.find(Key);
        if(It == InnerMap.end())
            return nullptr;
        return &(It->second);
    };

    bool Exists(const TK& Key)
    {
        if(InnerMap.find(Key) == InnerMap.end())
            return false;
        return true;
    }

    TV& Find(const TK& Key, const TV& Default)
    {
        auto It = InnerMap.find(Key);
        if(It == InnerMap.end())
            return Default;
        else
            return It->second;
    }

    TV& Find(const TK& Key, TV&& Default)
    {
        auto It = InnerMap.find(Key);
        if(It == InnerMap.end())
        {
            static TV StaticDefVal;
            StaticDefVal = std::move(Default);
            return StaticDefVal;
        }
        else
            return It->second;
    }

    TV& operator [] (const TK& Key)
    {
        return InnerMap.try_emplace(Key).first->second;
    };

    TV& operator [] (TK&& Key)
    {
        return InnerMap.try_emplace(std::move(Key)).first->second;
    };

    const OLMap<TK, TV>& operator = (const OLMap<TK, TV>& Src)
    {
        InnerMap = Src.InnerMap;
        return *this;
    }
    const OLMap<TK, TV>& operator = (const OLMap<TK, TV>&& Src)
    {
        InnerMap = std::move(Src.InnerMap);
        return *this;
    }

    int Count()
    {
        return (int)InnerMap.size();
    };

    void Clear()
    {
        InnerMap.clear();
    };

    void Reserve(int InSize)
    {
        InnerMap.reserve(InSize);
    };

    void Remove(const TK& Key)
    {
        InnerMap.erase(Key);
    };

    class Iterator : public OLMapBase::BaseIterator
    {
    public:
        
        IterType it;
        OLMap<TK, TV>* owner;
        Iterator() : owner(nullptr)  {};
        Iterator(IterType InIt, OLMap<TK, TV>& InOwner) : owner(&InOwner) 
        {
            it = InIt;
        };
        Iterator(const Iterator& src): it(src.it), owner(src.owner) {};
        inline const TK& GetKey()
        {
            //const TK& Ret = it->first;
            return it->first;
        };
        inline TV& GetValue()
        {
            return it->second;
        };
        inline void Next()
        {
            it++;
        };
        inline bool IsEnd()
        {
            return it == owner->InnerMap.end();
        }

        virtual void* GetKeyPtr()
        {
            return  (void*)&(it->first);
        };
        virtual void* GetValuePtr()
        {
            return  (void*)&(it->second);
        };
        virtual void  BaseNext()
        {
            it++;
        };

        virtual bool BaseIsEnd()
        {
            return it == owner->InnerMap.end();
        };

        virtual ~Iterator() {};

    };

    Iterator BeginIter()
    {
        return Iterator(InnerMap.begin(), *this);
    }


    virtual BaseIterator* BeginBaseIter() override
    {
        Iterator* NewItPtr = new Iterator(InnerMap.begin(), *this);
        return NewItPtr;
    };
    virtual void FinishBaseIter(BaseIterator* Iter) override
    {
        delete Iter;
    };


};






}
