#pragma once
#include "UnitDipole.h"
#include "utils/Types.h"

class Node;
class DC_CurrentGenerator: public UnitDipole {
public:
    DC_CurrentGenerator(
        const SharedAlias &A,
        const SharedAlias &B,
        const QColor &color,
        const SharedDouble &value,
        const bool showLabel
    ): UnitDipole(A, B, color, value, showLabel) {}

    int boxWidth() const override { return DEFAULT_CIRCLE_DIAMETER; }
    int boxHeight() const override { return boxWidth(); }
    int halfBoxWidth() const override { return DEFAULT_CIRCLE_RADIUS; }
    int halfBoxHeight() const override { return halfBoxWidth(); }

    ObjectType type() const override { return DC_CURRENT_GENERATOR; }

    bool hover(const QPointF &p, const float zoom) override {
        return (
            PinL(p, line(), STROKE_HOVER_MAGNET/zoom) ||
            PinC(p, center(), halfBoxWidth())
        );
    }

    SharedDipole clone(const SharedAlias &A, const SharedAlias &B) override {
        return std::make_shared<DC_CurrentGenerator>(A, B, color(), share(), showLabel());
    }
};
