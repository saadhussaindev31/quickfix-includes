#ifndef FIX43_LISTSTATUS_H
#define FIX43_LISTSTATUS_H

#include "Message.h"

namespace FIX43
{

  class ListStatus : public Message
  {
    static FIX::MsgType::Pack PackedType() { return FIX::MsgType::Pack("N"); }
  public:
    ListStatus() : Message(PackedType()) {}
    ListStatus(const FIX::Message& m) : Message(m) {}
    ListStatus(const Message& m) : Message(m) {}
    ListStatus(const ListStatus& m) : Message(m) {}
    static FIX::MsgType MsgType() { return FIX::MsgType("N"); }

    ListStatus(
      const FIX::ListID& aListID,
      const FIX::ListStatusType& aListStatusType,
      const FIX::NoRpts& aNoRpts,
      const FIX::ListOrderStatus& aListOrderStatus,
      const FIX::RptSeq& aRptSeq,
      const FIX::TotNoOrders& aTotNoOrders )
    : Message(PackedType())
    {
      // must be in this order
      Sequence::push_back_to(*this, aListID);
      Sequence::push_back_to(*this, aTotNoOrders);
      Sequence::push_back_to(*this, aNoRpts);
      Sequence::push_back_to(*this, aRptSeq);
      Sequence::push_back_to(*this, aListStatusType);
      Sequence::push_back_to(*this, aListOrderStatus);
    }

    ListStatus(
      const FIX::ListID::Pack& aListID,
      const FIX::ListStatusType::Pack& aListStatusType,
      const FIX::NoRpts::Pack& aNoRpts,
      const FIX::ListOrderStatus::Pack& aListOrderStatus,
      const FIX::RptSeq::Pack& aRptSeq,
      const FIX::TotNoOrders::Pack& aTotNoOrders )
    : Message(PackedType())
    {
      // must be in this order
      Sequence::push_back_to(*this, aListID);
      Sequence::push_back_to(*this, aTotNoOrders);
      Sequence::push_back_to(*this, aNoRpts);
      Sequence::push_back_to(*this, aRptSeq);
      Sequence::push_back_to(*this, aListStatusType);
      Sequence::push_back_to(*this, aListOrderStatus);
    }

    FIELD_SET(*this, FIX::ListID);
    FIELD_SET(*this, FIX::ListStatusType);
    FIELD_SET(*this, FIX::NoRpts);
    FIELD_SET(*this, FIX::ListOrderStatus);
    FIELD_SET(*this, FIX::RptSeq);
    FIELD_SET(*this, FIX::ListStatusText);
    FIELD_SET(*this, FIX::EncodedListStatusTextLen);
    FIELD_SET(*this, FIX::EncodedListStatusText);
    FIELD_SET(*this, FIX::TransactTime);
    FIELD_SET(*this, FIX::TotNoOrders);
    FIELD_SET(*this, FIX::NoOrders);
    class NoOrders: public FIX::Group
    {
    public:
    NoOrders() : FIX::Group(73,11,FIX::message_order(11,526,14,39,636,151,84,6,103,58,354,355,0)) {}
      FIELD_SET(*this, FIX::ClOrdID);
      FIELD_SET(*this, FIX::SecondaryClOrdID);
      FIELD_SET(*this, FIX::CumQty);
      FIELD_SET(*this, FIX::OrdStatus);
      FIELD_SET(*this, FIX::WorkingIndicator);
      FIELD_SET(*this, FIX::LeavesQty);
      FIELD_SET(*this, FIX::CxlQty);
      FIELD_SET(*this, FIX::AvgPx);
      FIELD_SET(*this, FIX::OrdRejReason);
      FIELD_SET(*this, FIX::Text);
      FIELD_SET(*this, FIX::EncodedTextLen);
      FIELD_SET(*this, FIX::EncodedText);
    };
  };

}

#endif
