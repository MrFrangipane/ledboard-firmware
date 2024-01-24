#include <Arduino.h>
#include "SerialCommunicator.h"
#include "SerialProtocol.h"
#include "SerialCallbacks.h"

using namespace ledboard;

SerialCommunicator serialCommunicator;


void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    Serial.begin();
    serialCommunicator.init();
    serialCommunicator.registerCallback(SerialProtocol::MessageType::Illuminate, illuminate);
    serialCommunicator.registerCallback(SerialProtocol::MessageType::Configure, configure);
}

void loop() {
    serialCommunicator.poll();
}
