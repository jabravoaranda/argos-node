#include "HttpApi.h"

#include "JsonSerializer.h"

#include <Arduino.h>
#include <uri/UriBraces.h>

namespace argos {

void HttpApi::begin(const Logger& logger, const NodeState& nodeState, Valves& valves, NodeCommandPort& commandPort, const NodeQueryPort& queryPort) {
    logger_ = &logger;
    nodeState_ = &nodeState;
    valves_ = &valves;
    commandPort_ = &commandPort;
    queryPort_ = &queryPort;

    registerRoutes();
    server_.begin();
    started_ = true;

    logger_->info(F("HTTP API server started on port 80"));
    if (!nodeState_->status().network.connected) {
        logger_->info(F("HTTP API: network unavailable"));
    }
}

void HttpApi::update() {
    if (!started_) {
        return;
    }

    server_.handleClient();
}

void HttpApi::registerRoutes() {
    registerReadRoutes();
    registerWriteRoutes();
    server_.onNotFound([this]() { handleNotFound(); });
}

void HttpApi::registerReadRoutes() {
    server_.on("/health", HTTP_GET, [this]() { handleHealth(); });
    logger_->info(F("HTTP API endpoint registered: GET /health"));

    server_.on("/info", HTTP_GET, [this]() { handleInfo(); });
    logger_->info(F("HTTP API endpoint registered: GET /info"));

    server_.on("/status", HTTP_GET, [this]() { handleStatus(); });
    logger_->info(F("HTTP API endpoint registered: GET /status"));

    server_.on("/metrics", HTTP_GET, [this]() { handleMetrics(); });
    logger_->info(F("HTTP API endpoint registered: GET /metrics"));
    logger_->info(F("Metrics endpoint started"));

    server_.on("/outputs", HTTP_GET, [this]() { handleOutputs(); });
    logger_->info(F("HTTP API endpoint registered: GET /outputs"));

    server_.on("/valves", HTTP_GET, [this]() { handleValves(); });
    logger_->info(F("HTTP API endpoint registered: GET /valves"));

    server_.on(UriBraces("/valves/{}"), HTTP_GET, [this]() { handleValve(); });
    logger_->info(F("HTTP API endpoint registered: GET /valves/{id}"));

    server_.on(UriBraces("/valves/{}/open"), HTTP_GET, [this]() { handleOpenValve(); });
    logger_->info(F("HTTP API endpoint registered: GET /valves/{id}/open"));

    server_.on(UriBraces("/valves/{}/close"), HTTP_GET, [this]() { handleCloseValve(); });
    logger_->info(F("HTTP API endpoint registered: GET /valves/{id}/close"));
}

void HttpApi::registerWriteRoutes() {
    server_.on(UriBraces("/outputs/relays/{}"), HTTP_PUT, [this]() { handleSetRelay(); });
    logger_->info(F("HTTP API endpoint registered: PUT /outputs/relays/{id}"));

    server_.on(UriBraces("/valves/{}"), HTTP_PUT, [this]() { handleSetValve(); });
    logger_->info(F("HTTP API endpoint registered: PUT /valves/{id}"));

    server_.on(UriBraces("/valves/{}/open"), HTTP_POST, [this]() { handleOpenValve(); });
    logger_->info(F("HTTP API endpoint registered: POST /valves/{id}/open"));

    server_.on(UriBraces("/valves/{}/close"), HTTP_POST, [this]() { handleCloseValve(); });
    logger_->info(F("HTTP API endpoint registered: POST /valves/{id}/close"));
}

void HttpApi::handleHealth() {
    sendJson(200, JsonSerializer::health(nodeState_->health()));
}

void HttpApi::handleInfo() {
    sendJson(200, JsonSerializer::info(nodeState_->info()));
}

void HttpApi::handleStatus() {
    sendJson(200, JsonSerializer::status(nodeState_->status()));
}

void HttpApi::handleMetrics() {
    sendJson(200, JsonSerializer::metrics(nodeState_->metrics()));
}

void HttpApi::handleOutputs() {
    sendJson(200, JsonSerializer::outputs(nodeState_->status()));
}

void HttpApi::handleValves() {
    sendJson(200, JsonSerializer::valves(nodeState_->status()));
}

void HttpApi::handleValve() {
    uint8_t valveId = 0;
    if (!parseValveId(server_.pathArg(0), valveId)) {
        sendError(404, F("invalid_valve"));
        return;
    }

    ValveStatus status;
    if (!valves_->status(valveId, status)) {
        sendError(404, F("invalid_valve"));
        return;
    }

    sendJson(200, JsonSerializer::valve(status));
}

void HttpApi::handleSetValve() {
    uint8_t valveId = 0;
    if (!parseValveId(server_.pathArg(0), valveId)) {
        sendError(404, F("invalid_valve"));
        return;
    }

    ValveState requestedState = ValveState::Closed;
    String error;
    if (!parseValveStateBody(server_.arg("plain"), requestedState, error)) {
        sendJson(400, String(F("{\"error\":\"")) + error + F("\"}"));
        return;
    }

    logValveWriteClient(valveId, requestedState);
    if (!valves_->setState(valveId, requestedState)) {
        sendError(500, F("valve_command_failed"));
        return;
    }

    ValveStatus status;
    if (!valves_->status(valveId, status)) {
        sendError(500, F("valve_status_unavailable"));
        return;
    }
    sendJson(200, JsonSerializer::valve(status));
}

void HttpApi::handleOpenValve() {
    uint8_t valveId = 0;
    if (!parseValveId(server_.pathArg(0), valveId)) {
        sendError(404, F("invalid_valve"));
        return;
    }

    logValveWriteClient(valveId, ValveState::Open);
    if (!valves_->open(valveId)) {
        sendError(500, F("valve_command_failed"));
        return;
    }

    ValveStatus status;
    if (!valves_->status(valveId, status)) {
        sendError(500, F("valve_status_unavailable"));
        return;
    }
    sendJson(200, JsonSerializer::valve(status));
}

void HttpApi::handleCloseValve() {
    uint8_t valveId = 0;
    if (!parseValveId(server_.pathArg(0), valveId)) {
        sendError(404, F("invalid_valve"));
        return;
    }

    logValveWriteClient(valveId, ValveState::Closed);
    if (!valves_->close(valveId)) {
        sendError(500, F("valve_command_failed"));
        return;
    }

    ValveStatus status;
    if (!valves_->status(valveId, status)) {
        sendError(500, F("valve_status_unavailable"));
        return;
    }
    sendJson(200, JsonSerializer::valve(status));
}

void HttpApi::handleSetRelay() {
    uint8_t relay = 0;
    if (!parseRelayId(server_.pathArg(0), relay)) {
        sendError(404, F("invalid_relay"));
        return;
    }

    bool requestedState = false;
    String error;
    if (!parseRelayStateBody(server_.arg("plain"), requestedState, error)) {
        sendJson(400, String(F("{\"error\":\"")) + error + F("\"}"));
        return;
    }

    logWriteClient(relay, requestedState);
    if (!commandPort_->setRelay(relay, requestedState)) {
        sendError(500, F("relay_command_failed"));
        return;
    }

    sendJson(200, JsonSerializer::relayCommand(relay, queryPort_->relayState(relay)));
}

void HttpApi::handleNotFound() {
    if (logger_ != nullptr) {
        logger_->info(F("HTTP API request failed: not found"));
    }
    sendJson(404, F("{\"error\":\"not_found\"}"));
}

void HttpApi::sendJson(int statusCode, const String& body) {
    server_.send(statusCode, F("application/json"), body);
}

void HttpApi::sendError(int statusCode, const __FlashStringHelper* error) {
    String body = F("{\"error\":\"");
    body += error;
    body += F("\"}");
    sendJson(statusCode, body);
}

bool HttpApi::parseRelayId(const String& text, uint8_t& relay) const {
    if (text.length() != 1 || text[0] < '1' || text[0] > '8') {
        return false;
    }

    relay = static_cast<uint8_t>(text[0] - '0');
    return true;
}

bool HttpApi::parseRelayStateBody(const String& body, bool& state, String& error) const {
    const String compact = compactJson(body);

    if (!compact.startsWith("{") || !compact.endsWith("}")) {
        error = F("invalid_json");
        return false;
    }

    if (compact.indexOf(F("\"state\"")) < 0) {
        error = F("missing_state");
        return false;
    }

    if (compact == F("{\"state\":true}")) {
        state = true;
        return true;
    }
    if (compact == F("{\"state\":false}")) {
        state = false;
        return true;
    }

    if (compact.indexOf(F("\"state\":true")) >= 0 || compact.indexOf(F("\"state\":false")) >= 0) {
        error = F("invalid_json");
    } else {
        error = F("non_boolean_state");
    }
    return false;
}

bool HttpApi::parseValveId(const String& text, uint8_t& valveId) const {
    if (text.length() != 1 || text[0] < '1' || text[0] > '9') {
        return false;
    }

    valveId = static_cast<uint8_t>(text[0] - '0');
    return valveId >= 1 && valveId <= Valves::kValveCount;
}

bool HttpApi::parseValveStateBody(const String& body, ValveState& state, String& error) const {
    const String compact = compactJson(body);

    if (!compact.startsWith("{") || !compact.endsWith("}")) {
        error = F("invalid_json");
        return false;
    }

    if (compact.indexOf(F("\"state\"")) < 0) {
        error = F("missing_state");
        return false;
    }

    if (compact == F("{\"state\":\"open\"}")) {
        state = ValveState::Open;
        return true;
    }
    if (compact == F("{\"state\":\"closed\"}")) {
        state = ValveState::Closed;
        return true;
    }

    if (compact.indexOf(F("\"state\":\"")) >= 0) {
        error = F("invalid_state");
    } else {
        error = F("non_string_state");
    }
    return false;
}

String HttpApi::compactJson(const String& body) const {
    String compact;
    compact.reserve(body.length());
    for (size_t i = 0; i < body.length(); ++i) {
        const char c = body[i];
        if (c != ' ' && c != '\r' && c != '\n' && c != '\t') {
            compact += c;
        }
    }
    return compact;
}

void HttpApi::logWriteClient(uint8_t relay, bool state) {
    if (logger_ == nullptr) {
        return;
    }

    String message = F("HTTP write ");
    message += server_.client().remoteIP().toString();
    message += F(" PUT relay ");
    message += relay;
    message += state ? F(" ON") : F(" OFF");
    logger_->info(message.c_str());
}

void HttpApi::logValveWriteClient(uint8_t valveId, ValveState state) {
    if (logger_ == nullptr) {
        return;
    }

    String message = F("HTTP write ");
    message += server_.client().remoteIP().toString();
    message += F(" valve ");
    message += valveId;
    message += state == ValveState::Open ? F(" open") : F(" closed");
    logger_->info(message.c_str());
}

}  // namespace argos
