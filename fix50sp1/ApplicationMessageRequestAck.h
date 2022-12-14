#ifndef FIX50SP1_APPLICATIONMESSAGEREQUESTACK_H
#define FIX50SP1_APPLICATIONMESSAGEREQUESTACK_H

#include "Message.h"

namespace FIX50SP1
{

  class ApplicationMessageRequestAck : public Message
  {
    static FIX::MsgType::Pack PackedType() { return FIX::MsgType::Pack("BX"); }
  public:
    ApplicationMessageRequestAck() : Message(PackedType()) {}
    ApplicationMessageRequestAck(const FIX::Message& m) : Message(m) {}
    ApplicationMessageRequestAck(const Message& m) : Message(m) {}
    ApplicationMessageRequestAck(const ApplicationMessageRequestAck& m) : Message(m) {}
    static FIX::MsgType MsgType() { return FIX::MsgType("BX"); }

    ApplicationMessageRequestAck(
      const FIX::ApplResponseID& aApplResponseID )
    : Message(PackedType())
    {
      // must be in this order
      Sequence::push_back_to(*this, aApplResponseID);
    }

    ApplicationMessageRequestAck(
      const FIX::ApplResponseID::Pack& aApplResponseID )
    : Message(PackedType())
    {
      // must be in this order
      Sequence::push_back_to(*this, aApplResponseID);
    }

    FIELD_SET(*this, FIX::ApplResponseID);
    FIELD_SET(*this, FIX::ApplReqID);
    FIELD_SET(*this, FIX::ApplReqType);
    FIELD_SET(*this, FIX::ApplResponseType);
    FIELD_SET(*this, FIX::ApplTotalMessageCount);
    FIELD_SET(*this, FIX::NoApplIDs);
    class NoApplIDs: public FIX::Group
    {
    public:
    NoApplIDs() : FIX::Group(1351,1355,FIX::message_order(1355,1182,1183,1357,1354,0)) {}
      FIELD_SET(*this, FIX::RefApplID);
      FIELD_SET(*this, FIX::ApplBegSeqNum);
      FIELD_SET(*this, FIX::ApplEndSeqNum);
      FIELD_SET(*this, FIX::RefApplLastSeqNum);
      FIELD_SET(*this, FIX::ApplResponseError);
    };
    FIELD_SET(*this, FIX::Text);
    FIELD_SET(*this, FIX::EncodedTextLen);
    FIELD_SET(*this, FIX::EncodedText);
  };

}

#endif
