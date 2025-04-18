#pragma once
#include "Globals.h"
#include <QPointF>

class Scene;
QSet<SharedPoint> GetWorldPointSelection(const Selection &selection);
Selection GetDeepSelection(const Selection &selection);
Selection copySelection(
    Scene *scene,
    const Selection &selection,
    const QPointF &dp = QPointF(0.0, 0.0),
    const bool generateNewAddresses = true
);
