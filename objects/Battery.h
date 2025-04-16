#pragma once
#include "DC_VoltageGenerator.h"
#include "utils/Globals.h"

class Node;
class Battery: public DC_VoltageGenerator {
public:
    Battery(
        const SharedAlias &A,
        const SharedAlias &B,
        const QPen &pen,
        const bool showLabel,
        const double V
    ): DC_VoltageGenerator(A, B, pen, showLabel, V){}

    int boxWidth() const override { return BATTERY_WIDTH; }
    int boxHeight() const override { return BATTERY_POSITIVE_HEIGHT; }
    int halfBoxWidth() const override { return HALF_BATTERY_WIDTH; }
    int halfBoxHeight() const override { return HALF_BATTERY_POSITIVE_HEIGHT; }

    ObjectType type() const override { return BATTERY; }

    SharedDipole clone(const SharedAlias &A, const SharedAlias &B) override {
        return std::make_shared<Battery>(A, B, _pen, _showLabel, _V);
    }
};


