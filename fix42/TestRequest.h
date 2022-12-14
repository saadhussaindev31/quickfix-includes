#ifndef FIX42_TESTREQUEST_H
#define FIX42_TESTREQUEST_H

#include "Message.h"

namespace FIX42
{

  class TestRequest : public Message
  {
    static FIX::MsgType::Pack PackedType() { return FIX::MsgType::Pack("1"); }
  public:
    TestRequest() : Message(PackedType()) {}
    TestRequest(const FIX::Message& m) : Message(m) {}
    TestRequest(const Message& m) : Message(m) {}
    TestRequest(const TestRequest& m) : Message(m) {}
    static FIX::MsgType MsgType() { return FIX::MsgType("1"); }

    TestRequest(
      const FIX::TestReqID& aTestReqID )
    : Message(PackedType())
    {
      // must be in this order
      Sequence::push_back_to(*this, aTestReqID);
    }

    TestRequest(
      const FIX::TestReqID::Pack& aTestReqID )
    : Message(PackedType())
    {
      // must be in this order
      Sequence::push_back_to(*this, aTestReqID);
    }

    FIELD_SET(*this, FIX::TestReqID);
  };

}

#endif
