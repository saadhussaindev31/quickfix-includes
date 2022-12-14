#ifndef FIX50SP2_USERREQUEST_H
#define FIX50SP2_USERREQUEST_H

#include "Message.h"

namespace FIX50SP2
{

  class UserRequest : public Message
  {
    static FIX::MsgType::Pack PackedType() { return FIX::MsgType::Pack("BE"); }
  public:
    UserRequest() : Message(PackedType()) {}
    UserRequest(const FIX::Message& m) : Message(m) {}
    UserRequest(const Message& m) : Message(m) {}
    UserRequest(const UserRequest& m) : Message(m) {}
    static FIX::MsgType MsgType() { return FIX::MsgType("BE"); }

    UserRequest(
      const FIX::UserRequestID& aUserRequestID,
      const FIX::UserRequestType& aUserRequestType,
      const FIX::Username& aUsername )
    : Message(PackedType())
    {
      // must be in this order
      Sequence::push_back_to(*this, aUsername);
      Sequence::push_back_to(*this, aUserRequestID);
      Sequence::push_back_to(*this, aUserRequestType);
    }

    UserRequest(
      const FIX::UserRequestID::Pack& aUserRequestID,
      const FIX::UserRequestType::Pack& aUserRequestType,
      const FIX::Username::Pack& aUsername )
    : Message(PackedType())
    {
      // must be in this order
      Sequence::push_back_to(*this, aUsername);
      Sequence::push_back_to(*this, aUserRequestID);
      Sequence::push_back_to(*this, aUserRequestType);
    }

    FIELD_SET(*this, FIX::UserRequestID);
    FIELD_SET(*this, FIX::UserRequestType);
    FIELD_SET(*this, FIX::Username);
    FIELD_SET(*this, FIX::Password);
    FIELD_SET(*this, FIX::NewPassword);
    FIELD_SET(*this, FIX::EncryptedPasswordMethod);
    FIELD_SET(*this, FIX::EncryptedPasswordLen);
    FIELD_SET(*this, FIX::EncryptedPassword);
    FIELD_SET(*this, FIX::EncryptedNewPasswordLen);
    FIELD_SET(*this, FIX::EncryptedNewPassword);
    FIELD_SET(*this, FIX::RawDataLength);
    FIELD_SET(*this, FIX::RawData);
  };

}

#endif
