#pragma once
#include "UnitDipole.h"
#include "utils/Types.h"


class Alias;
class Resistor: public UnitDipole {
public:
    Resistor(
        const SharedAlias &A,
        const SharedAlias &B,
        const QColor &color,
        const SharedDouble &value,
        const bool showLabel
    ): UnitDipole(A, B, color, value, showLabel) {}


    int boxWidth() const override { return RESISTOR_WIDTH; }
    int boxHeight() const override { return RESISTOR_HEIGHT; }
    int halfBoxWidth() const override { return HALF_RESISTOR_WIDTH; }
    int halfBoxHeight() const override { return HALF_RESISTOR_HEIGHT; }

    ObjectType type() const override { return RESISTOR; }

    SharedDipole clone(const SharedAlias &A, const SharedAlias &B) override {
        return std::make_shared<Resistor>(A, B, color(), share(), showLabel());
    }
};


