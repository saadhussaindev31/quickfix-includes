/* -*- C++ -*- */

/****************************************************************************
** Copyright (c) 2001-2014
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

#ifndef FIX_SESSIONSTATE_H
#define FIX_SESSIONSTATE_H

#ifdef _MSC_VER
#pragma warning( disable : 4503 4355 4786 4290 )
#endif

#include "FieldTypes.h"
#include "MessageStore.h"
#include "Log.h"
#include "Mutex.h"

namespace FIX
{
/// Maintains all of state for the Session class.
class SessionState : public MessageStore, public Log
{
#ifdef HAVE_BOOST
  typedef boost::unordered_map < int, Message > Messages;
#else
  typedef std::map < int, Message > Messages;
#endif

public:
  SessionState()
: m_enabled( true ), m_receivedLogon( false ),
  m_sentLogout( false ), m_sentLogon( false ),
  m_sentReset( false ), m_receivedReset( false ),
  m_initiate( false ), m_logonTimeout( 10 ), 
  m_logoutTimeout( 2 ), m_testRequest( 0 ),
  m_heartBtIntValue( -1 ), m_pStore( 0 ),
  m_pLog( 0 ), m_LogCaps( 0 ) {}

  bool enabled() const { return m_enabled; }
  void enabled( bool value ) { m_enabled = value; }

  bool receivedLogon() const { return m_receivedLogon; }
  void receivedLogon( bool value ) { m_receivedLogon = value; }

  bool sentLogout() const { return m_sentLogout; }
  void sentLogout( bool value ) { m_sentLogout = value; }

  bool sentLogon() const { return m_sentLogon; }
  void sentLogon( bool value ) { m_sentLogon = value; }

  bool receivedReset() const { return m_receivedReset; }
  void receivedReset( bool value ) { m_receivedReset = value; }

  bool sentReset() const { return m_sentReset; }
  void sentReset( bool value ) { m_sentReset = value; }

  bool initiate() const { return m_initiate; }
  void initiate( bool value ) { m_initiate = value; }

  int logonTimeout() const { return m_logonTimeout; }
  void logonTimeout( int value ) { m_logonTimeout = value; }

  int logoutTimeout() const { return m_logoutTimeout; }
  void logoutTimeout( int value ) { m_logoutTimeout = value; }

  int testRequest() const { return m_testRequest; }
  void testRequest( int value ) { m_testRequest = value; }

  bool resendRequested() const
  { return !(m_resendRange.first == 0 && m_resendRange.second == 0); }

  typedef std::pair<int, int> ResendRange;

  ResendRange resendRange () const { return m_resendRange; }
  void resendRange (int begin, int end)
  { m_resendRange = std::make_pair( begin, end ); }

  MessageStore* store() { return m_pStore; }
  void store( MessageStore* pValue )
  {
    m_pStore = pValue;
    setCreationTime( m_pStore->getCreationTime() );
  }
  Log* log() { return m_pLog; }
  void log( Log* pValue )
  {
    m_pLog = pValue;
    m_LogCaps = (pValue) ? pValue->queryLogCapabilities() : 0;
  }

  unsigned queryLogCapabilities() const { return m_LogCaps; }

  void heartBtInt( const HeartBtInt& value )
  {
    m_heartBtIntValue = value.getValue();
    m_heartBtInt = value;
  }
  HeartBtInt& heartBtInt()
  {
    m_heartBtIntValue = -1;
    return m_heartBtInt;
  }
  const HeartBtInt& heartBtInt() const
  { return m_heartBtInt; }
  int heartBtIntValue() const
  {
    return ( m_heartBtIntValue >= 0)
           ? m_heartBtIntValue :
           ( m_heartBtIntValue = m_heartBtInt.getValue() );
  }

  void lastSentTime( const UtcTimeStamp& value )
  { m_lastSentTime = value; }
  UtcTimeStamp& lastSentTime()
  { return m_lastSentTime; }
  const UtcTimeStamp& lastSentTime() const
  { return m_lastSentTime; }

  void lastReceivedTime( const UtcTimeStamp& value )
  { m_lastReceivedTime = value; }
  UtcTimeStamp& lastReceivedTime()
  { return m_lastReceivedTime; }
  const UtcTimeStamp& lastReceivedTime() const
  { return m_lastReceivedTime; }

  bool shouldSendLogon() const { return initiate() && !sentLogon(); }
  bool alreadySentLogon() const { return initiate() && sentLogon(); }
  bool logonTimedOut() const
  {
    UtcTimeStamp now;
    return now - lastReceivedTime() >= logonTimeout();
  }
  bool logoutTimedOut( const UtcTimeStamp& now ) const
  {
    return sentLogout() && ( ( now - lastSentTime() ) >= logoutTimeout() );
  }
  bool logoutTimedOut() const { return logoutTimedOut( UtcTimeStamp() ); }
  bool withinHeartBeat( const UtcTimeStamp& now = UtcTimeStamp() ) const
  {
    return ( ( now - lastSentTime() ) < heartBtIntValue() ) &&
           ( ( now - lastReceivedTime() ) < heartBtIntValue() );
  }
  bool timedOut( const UtcTimeStamp& now = UtcTimeStamp() ) const
  {
    return ( now - lastReceivedTime() ) >= ( 2.4 * ( double ) heartBtIntValue() );
  }
  bool needHeartbeat() const
  {
    UtcTimeStamp now;
    return ( ( now - lastSentTime() ) >= heartBtInt() ) && !testRequest();
  }
  bool needTestRequest() const
  {
    UtcTimeStamp now;
    return ( now - lastReceivedTime() ) >=
           ( ( 1.2 * ( ( double ) testRequest() + 1 ) ) * ( double ) heartBtIntValue() );
  }

  std::string logoutReason() const 
  { Locker l( m_mutex ); return m_logoutReason; }
  void logoutReason( const std::string& value ) 
  { Locker l( m_mutex ); m_logoutReason = value; }

  void queue( int msgSeqNum, const Message& message )
  { Locker l( m_mutex ); m_queue[ msgSeqNum ] = message; }
  template <typename F> bool dequeue( F& f )
  {
    FlexLocker l( m_mutex );
    if ( !m_queue.empty() )
    {
      int msgSeqNum = m_pStore->getNextTargetMsgSeqNum();
      Messages::iterator i = m_queue.find( msgSeqNum );
      if ( i != m_queue.end() )
      {
        Message msg = i->second;
        m_queue.erase( i );
        l.unlock();
  
        return f( msgSeqNum, msg );
      }
    }
    return false;
  }

  bool retrieve( int msgSeqNum, Message& message )
  {
    Locker l( m_mutex );
    Messages::iterator i = m_queue.find( msgSeqNum );
    if ( i != m_queue.end() )
    {
      message = i->second;
      m_queue.erase( i );
      return true;
    }
    return false;
  }
  void clearQueue()
  { Locker l( m_mutex ); m_queue.clear(); }

  bool set( int s, const std::string& m ) THROW_DECL( IOException )
  { Locker l( m_mutex ); return m_pStore->set( s, m ); }
  bool set( int s, Sg::sg_buf_ptr m, int n )
  { Locker l( m_mutex ); return m_pStore->set( s, m, n ); }
  void get( int b, int e, std::vector < std::string > &m ) const THROW_DECL( IOException )
  { Locker l( m_mutex ); m_pStore->get( b, e, m ); }
  int getNextSenderMsgSeqNum() const THROW_DECL( IOException )
  { Locker l( m_mutex ); return m_pStore->getNextSenderMsgSeqNum(); }
  int getNextTargetMsgSeqNum() const THROW_DECL( IOException )
  { Locker l( m_mutex ); return m_pStore->getNextTargetMsgSeqNum(); }
  void setNextSenderMsgSeqNum( int n ) THROW_DECL( IOException )
  { Locker l( m_mutex ); m_pStore->setNextSenderMsgSeqNum( n ); }
  void setNextTargetMsgSeqNum( int n ) THROW_DECL( IOException )
  { Locker l( m_mutex ); m_pStore->setNextTargetMsgSeqNum( n ); }
  void incrNextSenderMsgSeqNum() THROW_DECL( IOException )
  { Locker l( m_mutex ); m_pStore->incrNextSenderMsgSeqNum(); }
  void incrNextTargetMsgSeqNum() THROW_DECL( IOException )
  { Locker l( m_mutex ); m_pStore->incrNextTargetMsgSeqNum(); }
  void reset() THROW_DECL( IOException )
  { Locker l( m_mutex ); m_pStore->reset(); setCreationTime( m_pStore->getCreationTime() ); }
  void refresh() THROW_DECL( IOException )
  { Locker l( m_mutex ); m_pStore->refresh(); setCreationTime( m_pStore->getCreationTime() ); }

  void clear() {
    if ( LIKELY(!(m_LogCaps & Log::LC_CLEAR)) ) return ;
    if ( m_LogCaps & Log::LC_THREADSAFE ) m_pLog->clear();
    else {
      Locker l( m_mutex ); m_pLog->clear();
    }
  }
  void backup() {
    if ( LIKELY(!(m_LogCaps & Log::LC_BACKUP)) ) return ;
    if ( m_LogCaps & Log::LC_THREADSAFE ) m_pLog->backup();
    else {
      Locker l( m_mutex ); m_pLog->backup();
    }
  }
  void onIncoming( const std::string& string ) {
    if ( LIKELY(!(m_LogCaps & Log::LC_INCOMING)) ) return ;
    if ( m_LogCaps & Log::LC_THREADSAFE ) m_pLog->onIncoming( string );
    else {
      Locker l( m_mutex ); m_pLog->onIncoming( string );
    }
  }
  void onIncoming( Sg::sg_buf_ptr b, int n ) {
    if ( LIKELY(!(m_LogCaps & Log::LC_INCOMING)) ) return ;
    if ( m_LogCaps & Log::LC_THREADSAFE ) m_pLog->onIncoming( b, n );
    else {
      Locker l( m_mutex ); m_pLog->onIncoming( b, n );
    }
  }
  void onOutgoing( const std::string& string ) {
    if ( LIKELY(!(m_LogCaps & Log::LC_OUTGOING)) ) return ;
    if ( m_LogCaps & Log::LC_THREADSAFE ) m_pLog->onOutgoing( string );
    else {
      Locker l( m_mutex ); m_pLog->onOutgoing( string );
    }
  }
  void onOutgoing( Sg::sg_buf_ptr b, int n ) {
    if ( LIKELY(!(m_LogCaps & Log::LC_OUTGOING)) ) return ;
    if ( m_LogCaps & Log::LC_THREADSAFE ) m_pLog->onOutgoing( b, n );
    else {
      Locker l( m_mutex ); m_pLog->onOutgoing( b, n );
    }
  }
  void onEvent( const std::string& string ) {
    if ( LIKELY(!(m_LogCaps & Log::LC_EVENT)) ) return ;
    if ( m_LogCaps & Log::LC_THREADSAFE ) m_pLog->onEvent( string );
    else {
      Locker l( m_mutex ); m_pLog->onEvent( string );
    }
  }

private:

  bool m_enabled;
  bool m_receivedLogon;
  bool m_sentLogout;
  bool m_sentLogon;
  bool m_sentReset;
  bool m_receivedReset;
  bool m_initiate;
  int m_logonTimeout;
  int m_logoutTimeout;
  int m_testRequest;
  mutable int m_heartBtIntValue;
  HeartBtInt m_heartBtInt;
  ResendRange m_resendRange;
  UtcTimeStamp m_lastSentTime;
  UtcTimeStamp m_lastReceivedTime;
  std::string m_logoutReason;
  Messages m_queue;
  MessageStore* m_pStore;
  Log* m_pLog;
  unsigned m_LogCaps;
  mutable Mutex m_mutex;
};
}

#endif //FIX_SESSIONSTATE_H
