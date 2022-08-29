/* -*- C++ -*- */

/****************************************************************************
** Copyright (c) quickfixengine.org  All rights reserved.
**
** This file is part of the QuickFIX FIX Engine
**
** This file may be distributed under the terms of the quickfixengine.org
** license as defined by quickfixengine.org and appearing in the file
** LICENSE included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.quickfixengine.org/LICENSE for licensing information.
**
** Contact ask@quickfixengine.org if any conditions of this licensing are
** not clear to you.
**
****************************************************************************/

#ifndef FIX_STRING_H
#define FIX_STRING_H

#include <ostream>
#include "Utility.h"

namespace FIX
{
  struct String
  {
#if defined(__GNUC__) && defined(__GLIBCXX__)
     struct _Rep_base
     {
       std::size_t               _M_length;
       std::size_t               _M_capacity;
       std::size_t               _M_refcount;
     };
#endif
     static inline void NOTHROW
     swap(std::string& a, std::string& b)
     {
#if defined(__GNUC__) && defined(__GLIBCXX__)
       if( sizeof(std::string) == sizeof(uintptr_t) )
       {
         register uintptr_t* MAY_ALIAS pa = reinterpret_cast<uintptr_t*>(&a);
         register uintptr_t* MAY_ALIAS pb = reinterpret_cast<uintptr_t*>(&b);
         register uintptr_t  a_dataplus = *pa;
         *pa = *pb;
         *pb = a_dataplus;
       }
       else
#endif
       a.swap(b);
     }

     template <typename Traits, typename Allocator>
     static inline std::size_t HEAVYUSE NOTHROW
     size(const std::basic_string<char, Traits, Allocator>& r)
     {
#if defined(__GNUC__) && defined(__GLIBCXX__)
       if( sizeof(std::string) == sizeof(uintptr_t) )
       {
         register uintptr_t v = reinterpret_cast<const uintptr_t&>(r);
         register const _Rep_base* MAY_ALIAS p = (const _Rep_base*)v - 1;
         return p->_M_length;
       }
#endif
       return r.size();
     }

     template <typename Traits, typename Allocator>
     static inline std::size_t HEAVYUSE NOTHROW
     length(const std::basic_string<char, Traits, Allocator>& r)
     {
#if defined(__GNUC__) && defined(__GLIBCXX__)
       if( sizeof(std::string) == sizeof(uintptr_t) )
       {
         register uintptr_t v = reinterpret_cast<const uintptr_t&>(r);
         register const _Rep_base* MAY_ALIAS p = (const _Rep_base*)v - 1;
         return p->_M_length;
       }
#endif
       return r.length();
     }

     template <typename Traits, typename Allocator>
     static inline const char NOTHROW_PRE * NOTHROW_POST HEAVYUSE
     data(const std::basic_string<char, Traits, Allocator>& r)
     {
#if defined(__GNUC__) && defined(__GLIBCXX__)
       if( sizeof(std::string) == sizeof(uintptr_t) )
       {
         register uintptr_t v = reinterpret_cast<const uintptr_t&>(r);
         return (const char*)v;
       }
#endif
       return r.data();
     }

     template <typename Traits, typename Allocator>
     static inline const char NOTHROW_PRE * NOTHROW_POST HEAVYUSE
     c_str(const std::basic_string<char, Traits, Allocator>& r)
     {
#if defined(__GNUC__) && defined(__GLIBCXX__)
       if( sizeof(std::string) == sizeof(uintptr_t) )
       {
         register uintptr_t v = reinterpret_cast<const uintptr_t&>(r);
         return (const char*)v;
       }
#endif
       return r.c_str();
     }

     static inline std::string NOTHROW_PRE & NOTHROW_POST
     append( std::string& sink, const std::string& r )
     {
       return sink.append(r);
     }

     template <typename S, typename B>
     static S NOTHROW_PRE & NOTHROW_POST
     append( S& s, B buf )
     {
       return s.append( (const char*)IOV_BUF(buf), IOV_LEN(buf) );
     }

     template <typename S>
     static Sg::sg_buf_t NOTHROW toBuffer( const S& s )
     {
       Sg::sg_buf_t buf = IOV_BUF_INITIALIZER( data(s), size(s) );
       return buf;
     }

#ifdef ENABLE_SSO

     /// Class optimized for short strings with a subset of std::string methods
     class short_string_type
     {
       public:

         typedef char value_type;
         typedef char const_reference;
         typedef std::string::size_type size_type;

       private:

	 static const std::size_t MaxQWord = ENABLE_SSO;

         typedef uint64_t qword_storage_type[MaxQWord + 1];
         typedef uint32_t dword_storage_type[2 * (MaxQWord + 1)];
         typedef uint16_t  word_storage_type[4 * (MaxQWord + 1)];
         typedef std::string string_type;

         static const std::size_t MaxLocalCapacity = sizeof(qword_storage_type) - 1;
         static const unsigned char HaveString = 255;

         template <typename T> struct TypeHolder { typedef T type; };
         template <typename T> struct RefHolder { typedef T type; };

         union Data
         {
           qword_storage_type m_q;
           dword_storage_type m_d;
            word_storage_type m_w;
           struct
           {
             Util::CharBuffer::Fixed<MaxLocalCapacity> m_fixed; // including '\0'!
             unsigned char m_length;
           };

           template <typename T> T* storage_type();

           Data()
           { m_length = 0; m_q[0] = 0; }

           Data(std::size_t n, char c)
           {
             if( LIKELY(n < MaxLocalCapacity) )
             {
               uint64_t v = 0x0101010101010101ULL * (unsigned char)c;
               for(std::size_t i = 0; i <= MaxQWord; i++) m_q[i] = v;
               m_fixed.data[n] = '\0';
               m_length = n;
             }
             else
             {
               new (reinterpret_cast<string_type*>(m_fixed.data)) string_type(n, c);
               m_length = HaveString;
             }
           }

           Data(const std::string& s)
           {
             new (reinterpret_cast<string_type*>(m_fixed.data)) string_type(s);
             m_length = HaveString;
           }

           Data(const char* p, std::size_t sz)
           {
             set(p, sz);
           }

           template <class InputIterator>
           Data( InputIterator first, InputIterator last )
           {
             std::size_t sz = last - first;
             if ( LIKELY(sz < MaxLocalCapacity) )
             {
               std::copy(first, last, m_fixed.data);
               m_fixed.data[sz] = '\0';
               m_length = (unsigned char)sz;
             }
             else
               toString(first, last);
           }
         
           Data( const Data& d ) { set(d); }

           Data( const Data& src, size_type pos, size_type len )
           {
             if( src.isLocal() )
             {
               if( LIKELY( pos <= src.m_length ) )
               {
                 m_length = ( (pos + len) <= src.m_length )
                            ? (unsigned char)len
                            : (src.m_length - (unsigned char)pos);
                 m_fixed.set(src.m_fixed.data + pos, m_length);
                 m_fixed.data[m_length] = '\0';
               }
               else
                 throw std::out_of_range("FIX::short_string_type::substr");
             }
             else
             {
               const std::string& s = src.asString();
               size_type size = String::size(s);
               if ( size >= pos )
               {
                 if( (pos + len) > size ) len = size - pos;
                 set( String::data(s) + pos, len );
               }
               else
                 throw std::out_of_range("FIX::short_string_type::substr");
             }
           }

           template <typename T, typename F> Data(TypeHolder<T>, F& f)
           {
               std::size_t l = f(storage_type<T>());
               m_length = l;
           }

           ~Data() { destroy(); }

           inline bool PURE_DECL HEAVYUSE isLocal() const
           { return LIKELY(m_length < MaxLocalCapacity); }

           inline void clear()
           {
             destroy();
             m_length = 0;
           }

           inline void resize(std::size_t sz)
           {
             if ( LIKELY(sz < MaxLocalCapacity) )
             {
               if ( isLocal() )
               {
fin:
                 m_fixed.data[sz] = '\0';
                 m_length = sz;
                 return;
               }
               string_type& s = asString();
               std::size_t l = String::size(s);
               m_fixed.set(String::data(s), l >= MaxLocalCapacity ? MaxLocalCapacity - 1 : l);
               s.~string_type();
               goto fin;
             }
             Data u = *this;
             ( HaveString == m_length ? asString() : toString(u.m_fixed.data, u.m_length) ).resize(sz);
           }

           inline bool empty() const
           { return isLocal() ? (m_length == 0) : asString().empty(); }

           inline void HEAVYUSE assign( const char* p, std::size_t sz)
           {
             destroy();
             set(p, sz);
           }

           template <typename T, typename F> std::size_t HEAVYUSE short_assign(F& f)
           {
             if ( isLocal() )
             {
fin:
               std::size_t l = f(storage_type<T>());
               m_length = l;
               return l;
             }
             asString().~string_type();
             goto fin;
           }

           template <std::size_t N> void HEAVYUSE assign( const Util::CharBuffer::Fixed<N>& buf, std::size_t l )
           {
             if ( isLocal() )
             {
fin:
               *storage_type<typename Util::CharBuffer::Fixed<N>::value_type>() = buf.value;
               m_length = l;
               return;
             }
             asString().~string_type();
             goto fin;
           }

           template <class InputIterator>
           void HEAVYUSE assign( InputIterator first, InputIterator last )
           {
             if ( isLocal() )
             {
               new (this) Data(first, last);
               return;
             }
             if ( (std::size_t)(last - first) < MaxLocalCapacity )
             {
               asString().~string_type();
               new (this) Data(first, last);
             }
             else
               asString().assign(first, last);
           }

           inline void HEAVYUSE push_back( char c )
           {
             if( LIKELY(m_length < MaxLocalCapacity - 1) )
             {
               m_fixed.data[m_length++] = c;
               m_fixed.data[m_length] = '\0';
               return;
             }
             Data u = *this;
             ( HaveString == m_length ? asString() : toString(u.m_fixed.data, u.m_length) ).push_back(c);
           }

           inline void HEAVYUSE append( const char* p, std::size_t sz )
           {
             if( LIKELY(m_fixed.set(p, sz, m_length)) )
             {
               m_length += sz;
               m_fixed.data[m_length] = '\0';
               return;
             }
             if ( 0 == m_length )
               toString(p, sz);
             else
             {
               Data u = *this;
               ( HaveString == m_length ? asString() : toString(u.m_fixed.data, u.m_length) ).append(p, sz);
             }
           }

           template <class InputIterator>
           void append( InputIterator first, InputIterator last )
           {
             typename InputIterator::difference_type sz = last - first;
             if( LIKELY(m_length + (std::size_t)sz < MaxLocalCapacity) )
             {
               std::copy(first, last, m_fixed.data + m_length);
               m_length += (unsigned char)sz;
               m_fixed.data[m_length] = '\0';
               return;
             }
             if ( 0 == m_length )
               toString(first, last);
             else
             {
               Data u = *this;
               ( HaveString == m_length ? asString() : toString(u.m_fixed.data, u.m_length) ).append(first, last);
             }
           }

           inline Data& operator = ( const Data& d )
           {
             destroy();
             set(d);
             return *this;
           }

           inline operator string_type& ()
           {
             if( isLocal() )
             {
               Data u = *this;
               return toString(u.m_fixed.data, u.m_length);
             }
             return asString();
           }

           inline void swap( Data& d )
           {
#if defined(__GNUC__) && defined(__x86_64__) && ENABLE_SSO == 3
              __m128i a1, a2, b1, b2;
              a1 = _mm_loadu_si128((__m128i*)m_q);
              b1 = _mm_loadu_si128((__m128i*)d.m_q);
              a2 = _mm_loadu_si128((__m128i*)m_q + 1);
              b2 = _mm_loadu_si128((__m128i*)d.m_q + 1);
              _mm_storeu_si128((__m128i*)m_q, b1);
              _mm_storeu_si128((__m128i*)d.m_q, a1);
              _mm_storeu_si128((__m128i*)m_q + 1, b2);
              _mm_storeu_si128((__m128i*)d.m_q + 1, a2);
#else
             for(std::size_t i = 0; i <= MaxQWord; i++)
               std::swap(m_q[i], d.m_q[i]);
#endif
           }

           inline std::size_t HEAVYUSE size() const
           {
             return ( isLocal() ) ? (std::size_t)m_length
                                  : String::size(asString());
           }

           inline const char* HEAVYUSE data() const
           {
             return ( isLocal() ) ? m_fixed.data
                                  : String::data(asString());
           }

           inline bool PURE_DECL HEAVYUSE equal(const Data& d) const
           {
	     std::size_t s;
             return LIKELY((m_length | d.m_length) != HaveString)
                   ? (m_length == d.m_length && Util::CharBuffer::equal(m_fixed, d.m_fixed, m_length)) 
                   : (s = size()) == d.size() && !::memcmp(data(), d.data(), s);
           }

           inline bool PURE_DECL HEAVYUSE equal(const char* p, std::size_t sz ) const
           {
             return LIKELY(m_length != HaveString)
                    ? (m_length == sz && Util::CharBuffer::equal(m_fixed, p, sz))
                    : size() == sz && !::memcmp(data(), p, sz);
           }

           inline int PURE_DECL HEAVYUSE compare( const char* p, std::size_t sz ) const
           {
             if( isLocal() )
             {
               int r = ::memcmp( m_fixed.data, p, std::min((std::size_t)m_length, sz) );
               return r ? r : ((int)m_length - sz);
             }
             return asString().compare(0, std::string::npos, p, sz);
           }

           inline int PURE_DECL HEAVYUSE compare( const char* p ) const
           {
             if( isLocal() )
             {
               int r = ::strncmp( m_fixed.data, p, m_length );
               return r ? r : (unsigned char)p[m_length];
             }
             return asString().compare(p);
           }

           size_type find_first_of( char c, size_type pos ) const
           {
             if( isLocal() )
             {
               if( LIKELY(pos < m_length) )
               {
                 const char* p = Util::CharBuffer::find( Util::CharBuffer::Fixed<1>(c),
                                                         m_fixed.data + pos, MaxLocalCapacity + 1 - pos );
                 if( p && (pos = (p - m_fixed.data)) < m_length)
                   return pos;
               }
               return std::string::npos;
             }
             return asString().find_first_of( c, pos );
           }

         private:

           void destroy() {
             if( isLocal() )
               return;
             asString().~string_type();
           }

           void HEAVYUSE set( const char* p, std::size_t sz)
           {
             if( LIKELY(m_fixed.set(p, sz)) )
             {
               m_fixed.data[sz] = '\0';
               m_length = (unsigned char)sz;
             }
             else
               toString(p, sz);
           }

           void HEAVYUSE set(const Data& d)
           {
             if (d.isLocal()) {
#if defined(__GNUC__) && defined(__x86_64__) && ENABLE_SSO == 3
               _mm_storeu_si128((__m128i*)m_q, _mm_loadu_si128((__m128i*)d.m_q));
               _mm_storeu_si128((__m128i*)m_q + 1, _mm_loadu_si128((__m128i*)d.m_q + 1));
#else
               for(std::size_t i = 0; i <= MaxQWord; i++)
                 m_q[i] = d.m_q[i];
#endif
             } else {
               m_length = HaveString;
               new (reinterpret_cast<string_type*>(m_fixed.data))
                                     string_type(d.asString());
             }
           }

           inline string_type& asString() 
           {
             string_type* MAY_ALIAS p =
               reinterpret_cast<string_type*>(m_fixed.data);
             return *p;
           }
           inline const string_type& asString() const
           {
             const string_type* MAY_ALIAS p =
               reinterpret_cast<const string_type*>(m_fixed.data);
             return *p;
           }

           string_type& toString( const char* p, std::size_t sz )
           {
             m_length = HaveString;
             return *new (reinterpret_cast<string_type*>(m_fixed.data)) string_type(p, sz);
           }

           template <class InputIterator>
           string_type& toString( InputIterator first, InputIterator last )
           {
             m_length = HaveString;
             return *new (reinterpret_cast<string_type*>(m_fixed.data)) string_type(first, last);
           }
         };

         short_string_type( const Data& d, size_type pos, size_type len )
         : s_(d, pos, len) {}

       public:

         short_string_type() : s_()
         {}

         short_string_type( std::size_t n, char c ) : s_(n, c)
         {}

         short_string_type( const char* p ) : s_(p, ::strlen(p))
         {}

         short_string_type( const char* p, std::size_t sz ) : s_(p, sz)
         {}

         short_string_type( const short_string_type& ss ) : s_(ss.s_)
         {}

         short_string_type( const std::string& s ) : s_(s)
         {}

         template <class InputIterator>
         short_string_type( InputIterator first, InputIterator last )
         : s_(first, last) {}

         short_string_type& operator = ( const short_string_type& ss )
         {
           s_ = ss.s_;
           return *this;
         }

         inline void clear()
         {
           s_.clear();
         }

         inline void resize(std::size_t sz)
         {
           s_.resize(sz);
         }

         inline bool empty() const
         {
           return s_.empty();
         }

         inline short_string_type& assign(const char* p, std::size_t sz)
         {
           s_.assign(p, sz);
           return *this;
         }

         template <class InputIterator>
         short_string_type& assign(InputIterator first, InputIterator last)
         {
           s_.assign(first, last);
           return *this;
         }

         inline void push_back(char c)
         {
           s_.push_back(c);
         }

         inline short_string_type& append( const char* p, std::size_t sz)
         {
           s_.append(p, sz);
           return *this;
         }

         template <class InputIterator>
         short_string_type& append( InputIterator first, InputIterator last )
         {
           s_.append(first, last);
           return *this;
         }

         inline char operator[]( std::size_t n ) const
         {
           return data()[n];
         }

         inline operator std::string& ()
         {
           return s_;
         }

         inline operator const std::string& () const
         {
           return s_;
         }

         inline std::size_t length () const
         {
           return s_.size();
         }

         inline std::size_t size() const
         {
           return s_.size();
         }

         inline const char* data() const
         {
           return s_.data();
         }

         inline void swap( short_string_type& ss )
         {
           s_.swap(ss.s_);
         }

         inline bool operator == ( const short_string_type& ss ) const
         {
           return s_.equal( ss.s_ );
         }

         inline bool operator != ( const short_string_type& ss ) const
         {
           return !s_.equal( ss.s_ );
         }

         inline bool operator == ( const char* p ) const
         {
           return s_.equal( p, ::strlen(p) );
         }

         inline bool operator != ( const char* p ) const
         {
           return !s_.equal( p, ::strlen(p) );
         }

         inline bool operator == ( const std::string& ss ) const
         {
           return s_.equal( String::data(ss), String::size(ss) );
         }

         inline bool operator != ( const std::string& ss ) const
         {
           return !s_.equal( String::data(ss), String::size(ss) );
         }

         inline int compare( const short_string_type& ss ) const
         {
           return s_.compare( ss.data(), ss.size() );
         }

         inline int compare( const std::string& ss ) const
         {
           return s_.compare( ss.data(), ss.size() );
         }

         inline int compare( const char* p, std::size_t sz ) const
         {
           return s_.compare( p, sz );
         }

         inline int compare( const char* p ) const
         {
           return s_.compare( p );
         }

         inline size_type find_first_of( char c, size_type pos ) const
         {
           return s_.find_first_of( c, pos );
         }

         inline short_string_type substr( size_type pos, size_type len ) const
         {
           return short_string_type( s_, pos, len );
         }

       private:

         mutable Data s_;

         // F writes a null terminated string that fits into the local storage and returns written size without the last '\0'
         template <typename T, typename F> explicit short_string_type(TypeHolder<T>, F f)
         : s_( TypeHolder<T>(), f )
         {}
         template <typename T, typename F> explicit short_string_type(RefHolder<T>, F& f)
         : s_( TypeHolder<T>(), f )
         {}

         template <typename T, typename F> std::size_t HEAVYUSE short_assign(F f)
         {
           return s_.short_assign<T>(f);
         }
         template <typename T, typename F> std::size_t HEAVYUSE short_assign_r(F& f)
         {
           return s_.short_assign<T>(f);
         }

         // buffer must be null terminated at buf[l]
         template <std::size_t N> void HEAVYUSE assign( const Util::CharBuffer::Fixed<N>& buf, std::size_t l )
         {
           s_.assign(buf, l);
         }

         friend class IntConvertor;
         friend class PositiveIntConvertor;
         friend class CheckSumConvertor;
         friend class CharConvertor;
         friend class BoolConvertor;
         friend class DoubleConvertor;
         friend class UtcTimeStampConvertor;
         friend class UtcTimeOnlyConvertor;
         friend class UtcDateConvertor;
     };

     typedef short_string_type value_type;

     static inline void NOTHROW
     swap(value_type& a, value_type& b)
     {
       a.swap(b);
     }

     static inline std::size_t NOTHROW
     size(const value_type& r)
     {
       return r.size();
     }

     static inline std::size_t NOTHROW
     length(const value_type& r)
     {
       return r.length();
     }

     static inline const char NOTHROW_PRE * NOTHROW_POST
     data(const value_type& r)
     {
       return r.data();
     }

     static inline std::string NOTHROW_PRE & NOTHROW_POST
     append( std::string& sink, const value_type& r)
     {
       return sink.append(r.data(), r.size());
     }

#else // !ENABLE_SSO

     typedef std::string value_type;

#endif

     struct equal_to
     {
       inline bool NOTHROW
       operator()(const std::string& a, const std::string& b) const
       {
         return a == b;
       }
#ifdef ENABLE_SSO
       inline bool NOTHROW
       operator()(const value_type& a, const value_type& b) const
       {
         return a == b;
       }
#endif
     };

     struct Rval
     {
       typedef const value_type& result_type;
       result_type operator()( const value_type& v ) const
       { return v; }
     };

     struct Size
     {
       typedef std::size_t result_type;
       result_type operator()( const value_type& v ) const
       { return String::size(v); }
     };

     struct Copy
     {
       typedef std::string result_type;
       result_type operator()( const value_type& v ) const
#ifndef ENABLE_SSO
       { return std::string( v ); }
#else
       { return std::string( data(v), size(v) ); }
       value_type operator()( const result_type& r ) const
       { return value_type( data(r), size(r) ); }
#endif
     };

  }; // String

  struct ItemHash
  {
    std::size_t HEAVYUSE NOTHROW generate(const char* p, std::size_t sz, std::size_t hash) const
    {
      static const unsigned int PRIME = 709607;

      for(; sz >= sizeof(uint64_t); sz -= sizeof(uint64_t), p += sizeof(uint64_t))
      {
        uint32_t v = *(uint32_t*)p;
#if defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
        __asm__ __volatile__ ( "bswap %0" : "+r" (v) : : );
        hash = (hash ^ (v^*(uint32_t *)(p+sizeof(uint32_t)))) * PRIME;
#else
        hash = (hash ^ (((v << 5) | (v >> 27))^*(uint32_t *)(p+sizeof(v)))) * PRIME;
#endif
      }
  
      if (sz & sizeof(uint32_t))
      {
              hash = (hash ^ *(uint32_t*)p) * PRIME;
              p += sizeof(uint32_t);
      }
      if (sz & sizeof(uint16_t))
      {
              hash = (hash ^ *(uint16_t*)p) * PRIME;
              p += sizeof(uint16_t);
      }
      if (sz & 1)
              hash = (hash ^ *p) * PRIME;

      return hash;
    }

    std::size_t HEAVYUSE NOTHROW operator()(const std::string& key) const
    {
      std::size_t l = String::size(key);
      return generate(String::data(key), l, l);
    }
#ifdef ENABLE_SSO
    std::size_t HEAVYUSE NOTHROW operator()(const String::value_type& key) const
    {
      std::size_t l = String::size(key);
      return generate(String::data(key), l, l);
    }

    std::size_t HEAVYUSE NOTHROW operator()(const std::pair<int, String::value_type>& key ) const
    {
      const String::value_type& s = key.second;
      return generate(String::data(s), String::size(s), key.first);
    }
#endif

    std::size_t HEAVYUSE NOTHROW operator()(const std::pair<int, std::string>& key ) const
    {
      const std::string& s = key.second;
      return generate(String::data(s), String::size(s), key.first);
    }

    std::size_t HEAVYUSE NOTHROW operator()(const std::pair< std::string, int>& key ) const
    {
      const std::string& s = key.first;
      return generate(String::data(s), String::size(s), key.second);
    }

  }; // class ItemHash

#ifdef ENABLE_SSO // provide SSO string overloads for common operations

  template<> inline uint64_t* String::short_string_type::Data::storage_type<uint64_t>() { return m_q; }
  template<> inline uint32_t* String::short_string_type::Data::storage_type<uint32_t>() { return m_d; }
  template<> inline uint16_t* String::short_string_type::Data::storage_type<uint16_t>() { return m_w; }
  template<> inline char*     String::short_string_type::Data::storage_type<char>() { return m_fixed.data; }

  static inline std::ostream& operator << ( std::ostream& out, const String::value_type& bs )
  {
    return out.write( bs.data(), bs.size() );
  }

  static inline bool PURE_DECL HEAVYUSE
  operator < ( const String::value_type& lhs, const String::value_type& rhs )
  { return lhs.compare( rhs ) < 0; }
  static inline bool PURE_DECL HEAVYUSE
  operator <=( const String::value_type& lhs, const String::value_type& rhs )
  { return lhs.compare( rhs ) <= 0; }

  static inline bool PURE_DECL HEAVYUSE
  operator > ( const String::value_type& lhs, const String::value_type& rhs )
  { return lhs.compare( rhs ) > 0; }
  static inline bool PURE_DECL HEAVYUSE
  operator >=( const String::value_type& lhs, const String::value_type& rhs )
  { return lhs.compare( rhs ) >= 0; }

  static inline bool PURE_DECL HEAVYUSE
  operator==( const char* p, const String::value_type& rhs ) { return rhs == p; }
  static inline bool PURE_DECL HEAVYUSE
  operator!=( const char* p, const String::value_type& rhs ) { return rhs != p; }

  static inline bool PURE_DECL HEAVYUSE
  operator < ( const char* p, const String::value_type& rhs )
  { return rhs.compare( p ) > 0; }
  static inline bool PURE_DECL HEAVYUSE
  operator <=( const char* p, const String::value_type& rhs )
  { return rhs.compare( p ) >= 0; }
  static inline bool PURE_DECL HEAVYUSE
  operator > ( const char* p, const String::value_type& rhs )
  { return rhs.compare( p ) < 0; }
  static inline bool PURE_DECL HEAVYUSE
  operator >=( const char* p, const String::value_type& rhs )
  { return rhs.compare( p ) <= 0; }
  static inline bool PURE_DECL HEAVYUSE
  operator < ( const String::value_type& lhs, const char* p )
  { return lhs.compare( p ) < 0; }
  static inline bool PURE_DECL HEAVYUSE
  operator <=( const String::value_type& lhs, const char* p )
  { return lhs.compare( p ) <= 0; }
  static inline bool PURE_DECL HEAVYUSE
  operator > ( const String::value_type& lhs, const char* p )
  { return lhs.compare( p ) > 0; }
  static inline bool PURE_DECL HEAVYUSE
  operator >=( const String::value_type& lhs, const char* p )
  { return lhs.compare( p ) >= 0; }

  static inline bool PURE_DECL HEAVYUSE
  operator==( const std::string& s, const String::value_type& rhs ) { return rhs == s; }
  static inline bool PURE_DECL HEAVYUSE
  operator!=( const std::string& s, const String::value_type& rhs ) { return rhs != s; }

  static inline bool PURE_DECL HEAVYUSE
  operator < ( const std::string& s, const String::value_type& rhs )
  { return rhs.compare( s ) > 0; }
  static inline bool PURE_DECL HEAVYUSE
  operator <=( const std::string& s, const String::value_type& rhs )
  { return rhs.compare( s ) >= 0; }
  static inline bool PURE_DECL HEAVYUSE
  operator > ( const std::string& s, const String::value_type& rhs )
  { return rhs.compare( s ) < 0; }
  static inline bool PURE_DECL HEAVYUSE
  operator >=( const std::string& s, const String::value_type& rhs )
  { return rhs.compare( s ) <= 0; }
  static inline bool PURE_DECL HEAVYUSE
  operator < ( const String::value_type& lhs, const std::string& s )
  { return lhs.compare( s ) < 0; }
  static inline bool PURE_DECL HEAVYUSE
  operator <=( const String::value_type& lhs, const std::string& s )
  { return lhs.compare( s ) <= 0; }
  static inline bool PURE_DECL HEAVYUSE
  operator > ( const String::value_type& lhs, const std::string& s )
  { return lhs.compare( s ) > 0; }
  static inline bool PURE_DECL HEAVYUSE
  operator >=( const String::value_type& lhs, const std::string& s )
  { return lhs.compare( s ) >= 0; }
#endif

} // namespace FIX

#endif
