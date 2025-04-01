#pragma once
#include "Resistor.h"
#include "utils/Types.h"
#include "objects/BCPath.h"

class Wire: public Dipole {
private:
    BCPath _path;

public:
    Wire(const SharedAlias &A, const SharedAlias &B,
        const QColor &color,
        const BCPath &path
    ): Dipole(A, B, color), _path(path) {
        setA(A);
        setB(B);
    }

    ObjectType type() const override { return WIRE; }

    void setA(const SharedAlias &A);
    void setB(const SharedAlias &B);
    
    bool inside(const QRectF &box, [[maybe_unused]] const float zoom) override;
    bool hover(const QPointF &p, [[maybe_unused]] const float zoom) override;
    bool visible(const QRectF &viewport, [[maybe_unused]] const float zoom) override {
        return inside(viewport, zoom);
    }
    BCPath path() const { return _path; }

    SharedDipole clone(const SharedAlias &A, const SharedAlias &B) override;
};
