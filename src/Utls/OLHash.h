/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#pragma once
#include "Defs.h"
#include <type_traits>

namespace OL
{

template<typename Ty> struct OLHash {};


template < typename Ty >
struct TEnumHasher
{
    typename std::enable_if<std::is_enum<Ty>::value, size_t>::type operator()(const Ty _Keyval) const  {
        return std::hash<int>{}((int)_Keyval);
    };
};

template < typename Ty >
struct THasher
{
    typedef typename std::conditional<std::is_enum<Ty>::value, TEnumHasher< Ty >, OLHash<Ty> >::type Type;
};



// template < typename Ty >
// struct THasher< std::enable_if<!std::is_enum<Ty>::value, Ty > >
// {
//     typedef std::enable_if_t<!std::is_enum<Ty>::value, OLHash<Ty> > Type; 
// };

template < typename Ty >
struct THasher<Ty*>
{
    typedef std::hash< Ty* > Type;
};

// template <typename Ty> 
// struct THasher< std::enable_if<std::is_enum<Ty>::value, Ty > >
// { 
//     typedef std::enable_if_t<std::is_enum<Ty>::value, std::hash< int > > Type; 
// };


template <> struct THasher < int > { typedef std::hash< int > Type; };
template <> struct THasher < uint > { typedef std::hash< uint > Type; };
template <> struct THasher < int64 > { typedef std::hash< int64 > Type; };
template <> struct THasher < uint64 > { typedef std::hash< uint64 > Type; };
template <> struct THasher < float > { typedef std::hash< float > Type; };
template <> struct THasher < double > { typedef std::hash< double > Type; };
template <> struct THasher < byte > { typedef std::hash< byte > Type; };



}