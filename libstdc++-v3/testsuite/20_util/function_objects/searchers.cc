// Copyright (C) 2014-2017 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING3.  If not see
// <http://www.gnu.org/licenses/>.

// { dg-options "-std=gnu++17" }

#include <functional>
#include <cstring>
#ifdef _GLIBCXX_USE_WCHAR_T
# include <cwchar>
#endif
#include <algorithm>
#include <testsuite_hooks.h>

#ifndef __cpp_lib_boyer_moore_searcher
# error "Feature-test macro for searchers missing"
#elif __cpp_lib_boyer_moore_searcher < 201603
# error "Feature-test macro for searchers has wrong value"
#endif

using std::default_searcher;
using std::boyer_moore_searcher;
using std::boyer_moore_horspool_searcher;

void
test01()
{
  const char s[] = { 'a', (char)-97, 'a', '\0' };
  const char* needles[] = {
    s, "", "a", "aa", "aaa", "ab", "cd", "abcd", "abcdabcd", "abcabcd"
  };
  const char* haystacks[] = {
    s, "", "a", "aa", "aaa", "ab", "cd", "abcd", "abcdabcd", "abcabcd",
    "aaaaaaa", "aabaa", "aaacab", "cdabcdab", "abcdabcd", "xyzabcdxyz"
  };

  for (auto n : needles)
  {
    auto ne = n + std::strlen(n);
    default_searcher d(n, ne);
    boyer_moore_searcher bm(n, ne);
    boyer_moore_horspool_searcher bmh(n, ne);
    for (auto h : haystacks)
    {
      auto he = h + std::strlen(h);
      auto res = std::search(h, he, n, ne);
      auto d_res = d(h, he);
      VERIFY( d_res.first == res );
      auto bm_res = bm(h, he);
      VERIFY( bm_res.first == res );
      auto bmh_res = bmh(h, he);
      VERIFY( bmh_res.first == res );
    }
  }
}

void
test02()
{
#ifdef _GLIBCXX_USE_WCHAR_T
  const wchar_t s[] = { L'a', (wchar_t)-97, L'a', L'\0' };
  const wchar_t* needles[] = {
    s, L"", L"a", L"aa", L"aaa", L"ab", L"cd", L"abcd", L"abcdabcd", L"abcabcd"
  };
  const wchar_t* haystacks[] = {
    s, L"", L"a", L"aa", L"aaa", L"ab", L"cd", L"abcd", L"abcdabcd", L"abcabcd",
    L"aaaaaaa", L"aabaa", L"aaacab", L"cdabcdab", L"abcdabcd", L"xyzabcdxyz"
  };

  for (auto n : needles)
  {
    auto ne = n + std::wcslen(n);
    default_searcher d(n, ne);
    boyer_moore_searcher bm(n, ne);
    boyer_moore_horspool_searcher bmh(n, ne);
    for (auto h : haystacks)
    {
      auto he = h + std::wcslen(h);
      auto res = std::search(h, he, n, ne);
      auto d_res = d(h, he);
      VERIFY( d_res.first == res );
      auto bm_res = bm(h, he);
      VERIFY( bm_res.first == res );
      auto bmh_res = bmh(h, he);
      VERIFY( bmh_res.first == res );
    }
  }
#endif
}

void
test03()
{
  // custom predicate
  struct
  {
    static unsigned char
    norm(unsigned char c) { return std::isalnum(c) ? c : '#'; }

    // equality
    bool operator()(char l, char r) const { return norm(l) == norm(r); }

    // hash
    std::size_t operator()(char c) const { return std::hash<char>{}(norm(c)); }
  } eq;

  const char* needle = " foo 123 ";
  const char* haystack = "*****foo*123******";
  const char* ne = needle + std::strlen(needle);
  const char* he = haystack + std::strlen(haystack);

  default_searcher d(needle, ne, eq);
  boyer_moore_searcher bm(needle, ne, eq, eq);
  boyer_moore_horspool_searcher bmh(needle, ne, eq, eq);

  auto res = std::search(haystack, he, needle, ne, eq);
  auto d_res = d(haystack, he);
  VERIFY( d_res.first == res );
  auto bm_res = bm(haystack, he);
  VERIFY( bm_res.first == res );
  auto bmh_res = bmh(haystack, he);
  VERIFY( bmh_res.first == res );
}

int
main()
{
  test01();
  test02();
  test03();
}
