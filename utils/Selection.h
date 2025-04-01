#pragma once
#include "Types.h"

class App;
QSet<SharedPoint> GetWorldPointSelection(const LockedSelection &selection);
LockedSelection GetDeepSelection(const LockedSelection &selection);
LockedSelection copySelection(
    App *app,
    const LockedSelection &selection,
    const QPointF &dp = QPointF(0.0, 0.0),
    const bool generateNewAddresses = true
);
