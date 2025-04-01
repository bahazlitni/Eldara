#include "Functions.h"
#include "Geometry.h"
#include "App.h"
#include "Grid.h"
#include "objects/Alias.h"
#include "objects/Dipole.h"

#include <chrono>
#include <QPainterPath>


QString formatUnit(double value, int precision) {
    // SI Prefixes
    const QStringList prefixes = {"p", "n", "µ", "m", "", "k", "M", "G", "T"};
    const int middleIndex = 4;

    if (value == 0.0) {
        return QString("0");
    }

    int exponent = static_cast<int>(std::floor(std::log10(std::abs(value))));
    int index = middleIndex + exponent / 3;
    index = std::clamp(index, 0, static_cast<int>(prefixes.size()) - 1);

    float scaledValue = value / std::pow(10, (index - middleIndex) * 3);

    // Format the result
    return QString::number(scaledValue, 'f', precision) + prefixes[index];
}

long long now() {
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return static_cast<long long>(ms.count());
}


QPointF indicateTarget(App *app, QPainterPath &indicators, const QPointF &target, const QVector<QPointF> &points){
    float dmin[2][2];
    QPointF refp[2][2];
    bool found[2][2] = { { false, false }, { false, false } };

    const float margin = 5.0 / app->grid.zoom();
    QPointF suggestedTarget = target;

    // Process each candidate point.
    for (const QPointF &p : points) {
        const float x = p.x();
        const float y = p.y();
        const float dx = std::abs(target.x() - x);
        const float dy = std::abs(target.y() - y);

        int i = (x > target.x()) ? 1 : 0;
        int j = (y > target.y()) ? 1 : 0;

        if (dx > 0 && dy < margin && (!found[0][i] || dx < dmin[0][i])) {
            dmin[0][i] = dx;
            refp[0][i] = p;
            found[0][i] = true;
            suggestedTarget.setY(y);
        }
        if (dy > 0 && dx < margin && (!found[1][j] || dy < dmin[1][j])) {
            dmin[1][j] = dy;
            refp[1][j] = p;
            found[1][j] = true;
            suggestedTarget.setX(x);
        }
    }

    if (!found[0][0] && !found[0][1] && !found[1][0] && !found[1][1]) return target;

    QPointF client_refp[2][2] = {
        {app->grid.toScreen(refp[0][0]),
         app->grid.toScreen(refp[0][1])},
        {app->grid.toScreen(refp[1][0]),
         app->grid.toScreen(refp[1][1])}
    };

    QPointF client_target = app->grid.toScreen(target);

    // Create horizontal segments in the painter path.
    if (found[0][0] && found[0][1]) {
        indicators.moveTo(client_refp[0][0]);
        indicators.lineTo(client_refp[0][1]);
        suggestedTarget.setY(refp[0][0].y());
    } else if (found[0][0]) {
        indicators.moveTo(client_refp[0][0]);
        indicators.lineTo(client_target.x(), client_refp[0][0].y());
        suggestedTarget.setY(refp[0][0].y());
    } else if (found[0][1]) {
        indicators.moveTo(client_refp[0][1]);
        indicators.lineTo(client_target.x(), client_refp[0][1].y());
        suggestedTarget.setY(refp[0][1].y());
    }

    // Create vertical segments in the painter path.
    if (found[1][0] && found[1][1]) {
        indicators.moveTo(client_refp[1][0]);
        indicators.lineTo(client_refp[1][1]);
        suggestedTarget.setX(refp[1][0].x());
    } else if (found[1][0]) {
        indicators.moveTo(client_refp[1][0]);
        indicators.lineTo(client_refp[1][0].x(), client_target.y());
        suggestedTarget.setX(refp[1][0].x());
    } else if (found[1][1]) {
        indicators.moveTo(client_refp[1][1]);
        indicators.lineTo(client_refp[1][1].x(), client_target.y());
        suggestedTarget.setX(refp[1][1].x());
    }

    return suggestedTarget;
}
