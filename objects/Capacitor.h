#pragma once
#include "UnitDipole.h"
#include "utils/Types.h"

class Node;
class Capacitor: public UnitDipole {
public:
    Capacitor(
        const SharedAlias &A,
        const SharedAlias &B,
        const QColor &color,
        const SharedDouble &value,
        const bool showLabel
    ): UnitDipole(A, B, color, value, showLabel) {}

    int boxWidth() const override { return CAPACITOR_WIDTH; }
    int boxHeight() const override { return CAPACITOR_HEIGHT; }
    int halfBoxWidth() const override { return HALF_CAPACITOR_WIDTH; }
    int halfBoxHeight() const override { return HALF_CAPACITOR_HEIGHT; }

    ObjectType type() const override { return CAPACITOR; }

    SharedDipole clone(const SharedAlias &A, const SharedAlias &B) override {
        return std::make_shared<Capacitor>(A, B, color(), share(), showLabel());
    }
};
