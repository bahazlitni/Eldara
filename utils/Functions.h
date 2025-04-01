#pragma once
#include <QString>
#include <QPointF>
#include <QLineF>
#include <QSet>
#include <QMap>
#include "Types.h"
#include <QPainterPath>
#include <QColor>

class App;
QString formatUnit(double value, int precision = 2);
long long now();

QString singleNameOf(ObjectType type);
QString pluralNameOf(ObjectType type);
QPointF indicateTarget(App *app, QPainterPath &indicators, const QPointF &target, const QVector<QPointF> &points);

inline bool isDarkColor(const QColor &color) {
    return (0.299 * color.red() + 0.587 * color.green() + 0.114 * color.blue()) < 128;
}
