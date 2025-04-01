#pragma once
#include "UnitDipole.h"
#include "utils/Types.h"

class Node;
class Battery: public UnitDipole {
public:
    Battery(
        const SharedAlias &A,
        const SharedAlias &B,
        const QColor &color,
        const SharedDouble &value,
        const bool showLabel
    ): UnitDipole(A, B, color, value, showLabel) {}

    int boxWidth() const override { return BATTERY_WIDTH; }
    int boxHeight() const override { return BATTERY_POSITIVE_HEIGHT; }
    int halfBoxWidth() const override { return HALF_BATTERY_WIDTH; }
    int halfBoxHeight() const override { return HALF_BATTERY_POSITIVE_HEIGHT; }

    ObjectType type() const override { return BATTERY; }

    SharedDipole clone(const SharedAlias &A, const SharedAlias &B) override {
        return std::make_shared<Battery>(A, B, color(), share(), showLabel());
    }
};
