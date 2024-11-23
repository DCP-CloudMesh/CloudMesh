#ifndef _MESSAGE_
#define _MESSAGE_

#include "../utility.h"
#include "payload.h"
#include <memory>
#include <string>

class Message {
    std::string uuid;
    std::string senderUuid;
    IpAddress senderIpAddr;
    std::shared_ptr<Payload> payload;

  public:
    Message();
    Message(const std::string& senderUuid, const IpAddress& senderIpAddr,
            std::shared_ptr<Payload> payload);
    virtual ~Message();

    std::string getUuid() const;
    std::string getSenderUuid() const;
    IpAddress getSenderIpAddr() const;
    std::shared_ptr<Payload> getPayload() const;
    void setPayload(std::shared_ptr<Payload> payload);
    template <typename T> std::shared_ptr<T> getPayloadAs() const;
    void initializePayload(const std::string& payloadTypeStr);
    std::string serialize() const;
    void deserialize(const std::string& serializedData);
};

#endif // _MESSAGE_
