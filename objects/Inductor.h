#pragma once
#include "UnitDipole.h"
#include "utils/Types.h"

class Node;
class Inductor: public UnitDipole {
public:
    Inductor(
        const SharedAlias &A,
        const SharedAlias &B,
        const QColor &color,
        const SharedDouble &value,
        const bool showLabel
    ): UnitDipole(A, B, color, value, showLabel) {}

    int boxWidth() const override { return INDUCTOR_WIDTH; }
    int boxHeight() const override { return INDUCTOR_HEIGHT; }
    int halfBoxWidth() const override { return HALF_INDUCTOR_WIDTH; }
    int halfBoxHeight() const override { return HALF_INDUCTOR_HEIGHT; }

    ObjectType type() const override { return INDUCTOR; }

    SharedDipole clone(const SharedAlias &A, const SharedAlias &B) override {
        return std::make_shared<Inductor>(A, B, color(), share(), showLabel());
    }
};
