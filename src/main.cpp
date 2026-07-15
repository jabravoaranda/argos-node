#include <Arduino.h>

#include "ArgosNode.h"

void setup() {
    argos::node().begin();
}

void loop() {
    argos::node().update();
}
