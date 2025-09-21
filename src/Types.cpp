#include "Types.h"
#include <iostream>
#include <iomanip>

std::string message_type_to_string(MessageType type) {
    switch (type) {
        case MessageType::ADD_ORDER: return "ADD_ORDER";
        case MessageType::CANCEL_ORDER: return "CANCEL_ORDER";
        case MessageType::MODIFY_ORDER: return "MODIFY_ORDER";
        case MessageType::EXECUTE_ORDER: return "EXECUTE_ORDER";
        case MessageType::TRADE: return "TRADE";
        case MessageType::HEARTBEAT: return "HEARTBEAT";
        default: return "UNKNOWN";
    }
}

std::string side_to_string(Side side) {
    return (side == Side::BUY) ? "BUY" : "SELL";
}
