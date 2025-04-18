#pragma once
#include "utils/Globals.h"

#include "DCV.h"

class Battery: public DCV {
public:
    Battery(const uint64_t id,
        const SharedAlias &A,
        const SharedAlias &B,
        const QPen &pen,
        const bool showLabel,
        const double V
    ): DCV(id, A, B, pen, showLabel, V){}

    int boxWidth() const override { return BATTERY_WIDTH; }
    int boxHeight() const override { return BATTERY_POSITIVE_HEIGHT; }
    int halfBoxWidth() const override { return HALF_BATTERY_WIDTH; }
    int halfBoxHeight() const override { return HALF_BATTERY_POSITIVE_HEIGHT; }

    ObjectType type() const override { return ObjectType::Battery; }

    SharedDipole clone(const uint64_t id, const SharedAlias &A, const SharedAlias &B) override {
        return std::make_shared<Battery>(id, A, B, _pen, _showLabel, _V);
    }
};


