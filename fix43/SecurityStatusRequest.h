#ifndef FIX43_SECURITYSTATUSREQUEST_H
#define FIX43_SECURITYSTATUSREQUEST_H

#include "Message.h"

namespace FIX43
{

  class SecurityStatusRequest : public Message
  {
    static FIX::MsgType::Pack PackedType() { return FIX::MsgType::Pack("e"); }
  public:
    SecurityStatusRequest() : Message(PackedType()) {}
    SecurityStatusRequest(const FIX::Message& m) : Message(m) {}
    SecurityStatusRequest(const Message& m) : Message(m) {}
    SecurityStatusRequest(const SecurityStatusRequest& m) : Message(m) {}
    static FIX::MsgType MsgType() { return FIX::MsgType("e"); }

    SecurityStatusRequest(
      const FIX::SecurityStatusReqID& aSecurityStatusReqID,
      const FIX::SubscriptionRequestType& aSubscriptionRequestType )
    : Message(PackedType())
    {
      // must be in this order
      Sequence::push_back_to(*this, aSubscriptionRequestType);
      Sequence::push_back_to(*this, aSecurityStatusReqID);
    }

    SecurityStatusRequest(
      const FIX::SecurityStatusReqID::Pack& aSecurityStatusReqID,
      const FIX::SubscriptionRequestType::Pack& aSubscriptionRequestType )
    : Message(PackedType())
    {
      // must be in this order
      Sequence::push_back_to(*this, aSubscriptionRequestType);
      Sequence::push_back_to(*this, aSecurityStatusReqID);
    }

    FIELD_SET(*this, FIX::SecurityStatusReqID);
    FIELD_SET(*this, FIX::Symbol);
    FIELD_SET(*this, FIX::SymbolSfx);
    FIELD_SET(*this, FIX::SecurityID);
    FIELD_SET(*this, FIX::SecurityIDSource);
    FIELD_SET(*this, FIX::Product);
    FIELD_SET(*this, FIX::CFICode);
    FIELD_SET(*this, FIX::SecurityType);
    FIELD_SET(*this, FIX::MaturityMonthYear);
    FIELD_SET(*this, FIX::MaturityDate);
    FIELD_SET(*this, FIX::CouponPaymentDate);
    FIELD_SET(*this, FIX::IssueDate);
    FIELD_SET(*this, FIX::RepoCollateralSecurityType);
    FIELD_SET(*this, FIX::RepurchaseTerm);
    FIELD_SET(*this, FIX::RepurchaseRate);
    FIELD_SET(*this, FIX::Factor);
    FIELD_SET(*this, FIX::CreditRating);
    FIELD_SET(*this, FIX::InstrRegistry);
    FIELD_SET(*this, FIX::CountryOfIssue);
    FIELD_SET(*this, FIX::StateOrProvinceOfIssue);
    FIELD_SET(*this, FIX::LocaleOfIssue);
    FIELD_SET(*this, FIX::RedemptionDate);
    FIELD_SET(*this, FIX::StrikePrice);
    FIELD_SET(*this, FIX::OptAttribute);
    FIELD_SET(*this, FIX::ContractMultiplier);
    FIELD_SET(*this, FIX::CouponRate);
    FIELD_SET(*this, FIX::SecurityExchange);
    FIELD_SET(*this, FIX::Issuer);
    FIELD_SET(*this, FIX::EncodedIssuerLen);
    FIELD_SET(*this, FIX::EncodedIssuer);
    FIELD_SET(*this, FIX::SecurityDesc);
    FIELD_SET(*this, FIX::EncodedSecurityDescLen);
    FIELD_SET(*this, FIX::EncodedSecurityDesc);
    FIELD_SET(*this, FIX::NoSecurityAltID);
    class NoSecurityAltID: public FIX::Group
    {
    public:
    NoSecurityAltID() : FIX::Group(454,455,FIX::message_order(455,456,0)) {}
      FIELD_SET(*this, FIX::SecurityAltID);
      FIELD_SET(*this, FIX::SecurityAltIDSource);
    };
    FIELD_SET(*this, FIX::Currency);
    FIELD_SET(*this, FIX::SubscriptionRequestType);
    FIELD_SET(*this, FIX::TradingSessionID);
    FIELD_SET(*this, FIX::TradingSessionSubID);
  };

}

#endif
