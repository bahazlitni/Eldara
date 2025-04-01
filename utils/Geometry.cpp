#include "Geometry.h"
#include <QList>
#include <QDebug>
#include <algorithm>
#include <cmath>

bool PinL(const QPointF &p, const QLineF &l, float lw) {
    const QPointF seg = l.p2() - l.p1();
    const float segLenSq = seg.x()*seg.x() + seg.y()*seg.y();
    if(segLenSq <= 0.0f) return false;
    const float t = qBound(0.0f, QPointF::dotProduct(p - l.p1(), seg) / segLenSq, 1.0f);
    const QPointF closest = l.p1() + t * seg;
    const float dx = p.x() - closest.x();
    const float dy = p.y() - closest.y();
    return (dx*dx + dy*dy) <= lw*lw;
}

bool PinC(const QPointF &p, const QPointF &c, float r){
    float dx = p.x() - c.x(), dy = p.y() - c.y();
    return (dx*dx + dy*dy) <= r*r;
}

bool PinBC(const QPointF &p, const QPointF &P0, const QPointF &P1, const QPointF &P2, const QPointF &P3, float lw){
    const float x0 = P0.x();
    const float x1 = P1.x();
    const float x2 = P2.x();
    const float x3 = P3.x();
    const float y0 = P0.y();
    const float y1 = P1.y();
    const float y2 = P2.y();
    const float y3 = P3.y();
    const float xm = p.x();
    const float ym = p.y();

    const float ax = -x0 + 3*(x1 - x2) + x3;
    const float bx = x0 - 2*x1 + x2;
    const float cx = -x0 + x1;
    const float dx = x0 - xm;
    const float ay = -y0 + 3*(y1 - y2) + y3;
    const float by = y0 - 2*y1 + y2;
    const float cy = -y0 + y1;
    const float dy = y0 - ym;

    const float a = ax*ax + ay*ay;
    const float b = 5*(ax*bx + ay*by);
    const float c = 4*(ax*cx + ay*cy) + 6*(bx*bx + by*by);
    const float d = 9*(bx*cx + by*cy) + ax*dx + ay*dy;
    const float e = 2*(bx*dx + by*dy) + 3*(cx*cx + cy*cy);
    const float f = cx*dx + cy*dy;

    float roots[5], t;
    int n = 0;

    for (float guess = 0.0; guess < 1.1; guess += 0.1) {
        t = guess;
        for (int i = 0; i < 10; i++) {
            const float fpt = (((5*a*t + 4*b)*t + 3*c)*t + 2*d)*t + e;
            if(std::abs(fpt) < EPSILON) break;
            const float nextT = t - ( ((((a*t + b)*t + c)*t + d)*t + e)*t + f ) / fpt;

            if (std::abs(nextT - t) < EPSILON) break;
            t = nextT;
        }
        if(t < 0.0 || t > 1.0) continue;

        bool isUnique = true;
        for (int j = 0; j<n; j++) {
            if (std::abs(roots[j] - t) > EPSILON) continue;
            isUnique = false;
            break;
        }
        if (isUnique) {
            roots[n++] = t;
            if (n == 5) break;
        }
    }

    for(int i = 0; i<n; i++){
        t = roots[i];
        const float X = (ax*t*t + 3*(bx*t + cx))*t + dx;
        const float Y = (ay*t*t + 3*(by*t + cy))*t + dy;
        if( (X*X + Y*Y) < lw*lw ) return true;
    }
    return false;
}

bool PinRotatedR(const QPointF &p, const QPointF &center, const float halfW, const float halfH, float angle){
    const float dx = p.x() - center.x();
    const float dy = p.y() - center.y();
    const float cosA = std::cos(angle);
    const float sinA = -std::sin(angle);

    return (
        std::abs(dx*cosA - dy*sinA) <= halfW &&
        std::abs(dx*sinA + dy*cosA) <= halfH
    );
}



// Line
QRectF LtoR(const QLineF &l){
    float minX, minY, maxX, maxY;
    if(l.x1() < l.x2()){
        minX = l.x1(); maxX = l.x2();
    } else {
        minX = l.x2(); maxX = l.x1();
    }
    if(l.y1() < l.y2()){
        minY = l.y1(); maxY = l.y2();
    } else {
        minY = l.y2(); maxY = l.y1();
    }
    return QRectF(minX, minY, maxX - minX, maxY - minY);
}
QRectF LtoR(const QLine &l){
    float minX, minY, maxX, maxY;
    if(l.x1() < l.x2()){
        minX = l.x1(); maxX = l.x2();
    } else {
        minX = l.x2(); maxX = l.x1();
    }
    if(l.y1() < l.y2()){
        minY = l.y1(); maxY = l.y2();
    } else {
        minY = l.y2(); maxY = l.y1();
    }
    return QRectF(minX, minY, maxX - minX, maxY - minY);
}

bool LinL(const QLineF &l1, const QLineF &l2) {
    QPointF intersectionPoint;
    QLineF::IntersectionType intersectionType = l1.intersects(l2, &intersectionPoint);

    if (intersectionType == QLineF::BoundedIntersection) {
        return true;
    }

    float dx1 = l1.x2() - l1.x1();
    float dy1 = l1.y2() - l1.y1();
    float dx2 = l2.x2() - l2.x1();
    float dy2 = l2.y2() - l2.y1();

    if (qFuzzyCompare(dy1 * dx2, dy2 * dx1)) {
        QVector<QPointF> points = {l1.p1(), l1.p2(), l2.p1(), l2.p2()};
        std::sort(points.begin(), points.end(), PLessThan);

        QPointF start = points[1];
        QPointF end = points[2];

        if (segmentContains(l1, start) && segmentContains(l1, end) &&
            segmentContains(l2, start) && segmentContains(l2, end)) {
            return true;
        }
    }
    return false;
}

bool LinR(const QLineF &l, const QRectF &rect) {
    // Fast reject based on bounding box check
    if (std::max(l.x1(), l.x2()) < rect.x() ||
        std::min(l.x1(), l.x2()) > rect.right() ||
        std::max(l.y1(), l.y2()) < rect.y() ||
        std::min(l.y1(), l.y2()) > rect.bottom()) {
        return false;
    }

    // Constants for rectangle corners
    const float rx1 = rect.x();
    const float ry1 = rect.y();
    const float rx2 = rect.right();
    const float ry2 = rect.bottom();

    // Line segment parameters
    const float x1 = l.x1();
    const float y1 = l.y1();
    const float dx = l.dx();
    const float dy = l.dy();

    // Special case: vertical line
    if (dx == 0.0f) {
        return (x1 >= rx1 && x1 <= rx2 &&
                ((y1 <= ry1 && y1 + dy >= ry1) || (y1 >= ry2 && y1 + dy <= ry2) ||
                 (y1 >= ry1 && y1 <= ry2)));
    }

    // Special case: horizontal line
    if (dy == 0.0f) {
        return (y1 >= ry1 && y1 <= ry2 &&
                ((x1 <= rx1 && x1 + dx >= rx1) || (x1 >= rx2 && x1 + dx <= rx2) ||
                 (x1 >= rx1 && x1 <= rx2)));
    }

    // Check intersection with each edge of the rectangle
    // Parameters for parametric line equation: p = p1 + t * (p2 - p1)
    float t;

    // Left edge: x = rx1
    t = (rx1 - x1) / dx;
    if (t >= 0.0f && t <= 1.0f) {
        float y = y1 + t * dy;
        if (y >= ry1 && y <= ry2) {
            return true;
        }
    }

    // Right edge: x = rx2
    t = (rx2 - x1) / dx;
    if (t >= 0.0f && t <= 1.0f) {
        float y = y1 + t * dy;
        if (y >= ry1 && y <= ry2) {
            return true;
        }
    }

    // Top edge: y = ry1
    t = (ry1 - y1) / dy;
    if (t >= 0.0f && t <= 1.0f) {
        float x = x1 + t * dx;
        if (x >= rx1 && x <= rx2) {
            return true;
        }
    }

    // Bottom edge: y = ry2
    t = (ry2 - y1) / dy;
    if (t >= 0.0f && t <= 1.0f) {
        float x = x1 + t * dx;
        if (x >= rx1 && x <= rx2) {
            return true;
        }
    }

    // Check if line start point is inside rectangle
    return (x1 >= rx1 && x1 <= rx2 && y1 >= ry1 && y1 <= ry2);
}


// Losange
bool PinDia(const QPointF &p, const QPointF &center, const float d1, const float d2) {
    return d2*std::fabs(p.x() - center.x()) + d1*std::fabs(p.y() - center.y()) <= d1*d2;
}

// Circle
bool CinR(const QPointF &center, const float radius, const QRectF &rect) {
    const float cx = center.x();
    const float cy = center.y();
    const float left = rect.left();
    const float top = rect.top();
    const float right = rect.right();
    const float bottom = rect.bottom();

    if((cx >= left && cx <= right && cy >= top && cy <= bottom)
        ||
        ((cx + radius > left) &&
        (cx - radius < right) &&
        (cy + radius > top) &&
        (cy - radius < bottom))
    )
        return true;

    const float dx1 = left - cx, dy1 = top - cy;
    const float dx2 = right - cx, dy2 = bottom - cy;
    const float rSquared = radius * radius;

    const float dx1_sq = dx1*dx1, dx2_sq = dx2*dx2;
    const float dy1_sq = dy1*dy1, dy2_sq = dy2*dy2;

    return (dx1_sq + dy1_sq <= rSquared)
        && (dx2_sq + dy1_sq <= rSquared)
        && (dx1_sq + dy2_sq <= rSquared)
        && (dx2_sq + dy2_sq <= rSquared);
}




// BCPath
bool BCinR(const QPointF &P0, const QPointF &P1, const QPointF &P2, const QPointF &P3, const QRectF &rect){
    if(rect.contains(P0) || rect.contains(P3))
        return true;

    const float x0 = P0.x();
    const float x1 = P1.x();
    const float x2 = P2.x();
    const float x3 = P3.x();
    const float y0 = P0.y();
    const float y1 = P1.y();
    const float y2 = P2.y();
    const float y3 = P3.y();

    const float ax = -x0 + 3*(x1 - x2) + x3;
    const float bx = 3*(x0 - 2*x1 + x2);
    const float cx = 3*(-x0 + x1);
    const float ay = -y0 + 3*(y1 - y2) + y3;
    const float by = 3*(y0 - 2*y1 + y2);
    const float cy = 3*(-y0 + y1);
    float t[4];

    solveThird(t, ax,bx,cx,x0 - rect.x());
    for(int i = 0; i < t[3]; i++){
        if(!between(0, t[i], 1) ||
            !between(0, (t[i]*( cy + t[i]*(by + t[i]*ay) ) + y0 - rect.y()) / rect.height(), 1)
        ) continue;
        return true;
    }

    solveThird(t, ax,bx,cx,x0 - rect.right());
    for(int i = 0; i < t[3]; i++){
        if(!between(0, t[i], 1) ||
            !between(0, (t[i]*( cy + t[i]*(by + t[i]*ay) ) + y0 - rect.y()) / rect.height(), 1)
            ) continue;
        return true;
    }

    solveThird(t,ay,by,cy,y0 - rect.y());
    for(int i = 0; i < t[3]; i++){
        if(!between(0, t[i], 1) ||
            !between(0, (t[i]*( cx + t[i]*(bx + t[i]*ax) ) + x0 - rect.x()) / rect.width(), 1)
            ) continue;
        return true;
    }

    solveThird(t,ay,by,cy,y0 - rect.bottom());
    for(int i = 0; i < t[3]; i++){
        if(!between(0, t[i], 1) ||
            !between(0, (t[i]*( cx + t[i]*(bx + t[i]*ax) ) + x0 - rect.x()) / rect.width(), 1)
            ) continue;
        return true;
    }

    return false;
}
QRectF BCtoR(const QPointF &P0, const QPointF &P1, const QPointF &P2, const QPointF &P3){
    const float x0 = P0.x();
    const float x1 = P1.x();
    const float x2 = P2.x();
    const float x3 = P3.x();
    const float y0 = P0.y();
    const float y1 = P1.y();
    const float y2 = P2.y();
    const float y3 = P3.y();

    const float ax = -x0 + 3*(x1 - x2) + x3;
    const float bx = x0 - 2*x1 + x2;
    const float cx = -x0 + x1;
    const float ay = -y0 + 3*(y1 - y2) + y3;
    const float by = y0 - 2*y1 + y2;
    const float cy = -y0 + y1;

    float xmin = std::min(x0, x3), xmax = std::max(x0, x3);
    float ymin = std::min(y0, y3), ymax = std::max(y0, y3);
    float roots[3], t;

    // solving for x
    solveSecond(roots, ax, 2*bx, cx);
    if(roots[2]){
        t = roots[0];
        if(between(0,t,1)){
            float x = (ax*t*t + 3*(bx*t + cx))*t + x0;
            if(x < xmin) xmin = x;
            else if(x > xmax) xmax = x;
        }
        if(roots[2] == 2.0){
            t = roots[1];
            if(between(0,t,1)){
                float x = (ax*t*t + 3*(bx*t + cx))*t + x0;
                if(x < xmin) xmin = x;
                else if(x > xmax) xmax = x;
            }
        }
    }

    // solving for y
    solveSecond(roots, ay, 2*by, cy);
    if(roots[2]){
        t = roots[0];
        if(between(0,t,1)){
            float y = (ay*t*t + 3*(by*t + cy))*t + y0;
            if(y < ymin) ymin = y;
            else if(y > ymax) ymax = y;
        }
        if(roots[2] == 2.0){
            t = roots[1];
            if(between(0,t,1)){
                float y = (ay*t*t + 3*(by*t + cy))*t + y0;
                if(y < ymin) ymin = y;
                else if(y > ymax) ymax = y;
            }
        }
    }
    return QRectF(xmin, ymin, xmax-xmin, ymax-ymin);
}

void solveSecond(float roots[3], float a, float b, float c){
    if(a == 0){
        if(b){
            roots[0] = -c/b;
            roots[2] = 1.0;
        } else {
            roots[2] = 0.0;
        }
    } else {
        b /= a;
        c /= a;
        const float delta = b*b - 4*c;
        if (delta > 0.0) {
            roots[0] = -(std::sqrt(delta) + b)/2.0;
            roots[1] = -roots[0] - b;
            roots[2] = 2.0;
        } else if (delta == 0.0) {
            roots[0] = -b/2.0;
            roots[2] = 1.0;
        } else {
            roots[2] = 0.0;
        }
    }
}

void solveThird(float roots[4], float a, float b, float c, float d){
    if(a == 0){
        if(b == 0){
            if(c){
                roots[0] = -d/c;
                roots[3] = 1.0;
            } else {
                roots[3] = 0.0;
            }
        } else {
            c /= b;
            d /= b;
            const float delta = c*c - 4*d;
            if (delta > 0.0) {
                roots[0] = -(std::sqrt(delta) + c)/2.0;
                roots[1] = roots[0] - c;
                roots[3] = 2.0;
            } else if (delta == 0.0) {
                roots[0] = -c/2.0;
                roots[3] = 1.0;
            } else {
                roots[3] = 0.0;
            }
        }
    }
    else {
        b /= a;
        c /= a;
        d /= a;

        const float b_div_3 = b/3.0;
        const float q = c/3.0 - b_div_3*b_div_3;
        const float r = (c*b_div_3 - d)/2 - b_div_3*b_div_3*b_div_3;
        const float delta = r*r + q*q*q;

        if(delta < 0){
            const float A = 2.0*std::sqrt(-q);
            const float phi1 = std::acos(-2.0*r/(q*A))/3.0;
            roots[0] = A*std::cos(phi1) - b_div_3;
            roots[1] = A*std::cos(phi1-DEG120) - b_div_3;
            roots[2] = A*std::cos(phi1+DEG120) - b_div_3;
            roots[3] = 3.0;
        } else if(delta > 0){
            if(r < 0){
                const float A = std::cbrt(-r + std::sqrt(delta));
                roots[0] = q/A - A - b_div_3;
            } else {
                const float A = std::cbrt(r + std::sqrt(delta));
                roots[0] = A - q/A - b_div_3;
            }
            roots[3] = 1.0;
        }
        else {
            const float A = std::cbrt(r);
            roots[0] = A + A - b_div_3;
            roots[1] = - A - b_div_3;
            roots[3] = 2.0;
        }
    }
}

bool segmentContains(const QLineF &line, const QPointF &point) {
    const QPointF vec = line.p2() - line.p1();
    const float lenSq = vec.x() * vec.x() + vec.y() * vec.y();

    if (lenSq < EPSILON) // Line is a point
        return PPfuzzyCmp(line.p1(), point);

    const QPointF pointVec = point - line.p1();
    const float t = QPointF::dotProduct(pointVec, vec) / lenSq;

    if (t < -EPSILON || t > 1.0 + EPSILON)
        return false;

    const QPointF projection = line.p1() + t * vec;
    return PPfuzzyCmp(projection, point);
}

QPair<QLineF::IntersectionType, QLineF> LlapL(const QLineF &l1, const QLineF &l2) {
    // Extract endpoints for readability.
    const QPointF p1 = l1.p1();
    const QPointF p2 = l1.p2();
    const QPointF p3 = l2.p1();
    const QPointF p4 = l2.p2();

    // Compute the direction vector of l1.
    const QPointF d1 = p2 - p1;

    // Check for degenerate l1 (nearly a point)
    const qreal d1Sq = d1.x()*d1.x() + d1.y()*d1.y();
    if (d1Sq < EPSILON) {
        // l1 is essentially a point; if l2 contains p1, we treat it as an intersection.
        if (PinL(p1, l2))
            return qMakePair(QLineF::BoundedIntersection, QLineF(p1, p1));
        else
            return qMakePair(QLineF::NoIntersection, QLineF());
    }

    // Check collinearity: both endpoints of l2 should lie on the line defined by l1.
    const qreal cross1 = d1.x() * (p3.y() - p1.y()) - d1.y() * (p3.x() - p1.x());
    const qreal cross2 = d1.x() * (p4.y() - p1.y()) - d1.y() * (p4.x() - p1.x());
    if (qAbs(cross1) < EPSILON && qAbs(cross2) < EPSILON) {
        // The segments are collinear.
        // Project l2's endpoints onto l1.
        const qreal t3 = ((p3.x() - p1.x()) * d1.x() + (p3.y() - p1.y()) * d1.y()) / d1Sq;
        const qreal t4 = ((p4.x() - p1.x()) * d1.x() + (p4.y() - p1.y()) * d1.y()) / d1Sq;

        // Determine the min and max parameters for l2.
        const qreal tMin = std::min(t3, t4);
        const qreal tMax = std::max(t3, t4);

        // The overlapping portion (if any) within l1's [0,1] parameter range:
        const qreal tOverlapStart = std::max(0.0, tMin);
        const qreal tOverlapEnd   = std::min(1.0, tMax);

        // If the overlap interval is empty, there is no intersection.
        if (tOverlapEnd < tOverlapStart + EPSILON)
        {
            // If the overlap is tiny (nearly a point) we treat it as a point intersection.
            if (tOverlapEnd + EPSILON >= tOverlapStart) {
                const QPointF ip = p1 + d1 * tOverlapStart;
                return qMakePair(QLineF::BoundedIntersection, QLineF(ip, ip));
            }
            return qMakePair(QLineF::NoIntersection, QLineF());
        }

        // Compute the actual overlapping points.
        const QPointF ip1 = p1 + d1 * tOverlapStart;
        const QPointF ip2 = p1 + d1 * tOverlapEnd;
        QLineF overlap(ip1, ip2);
        if (overlap.length() < EPSILON)
            return qMakePair(QLineF::BoundedIntersection, QLineF(ip1, ip1));
        else
            return qMakePair(QLineF::UnboundedIntersection, overlap);
    }
    else {
        // Not collinear. Use QLineF's intersection.
        QPointF ip;
        const QLineF::IntersectionType type = l1.intersects(l2, &ip);
        if (type == QLineF::BoundedIntersection)
            return qMakePair(QLineF::BoundedIntersection, QLineF(ip, ip));
        return qMakePair(QLineF::NoIntersection, QLineF());
    }
}


QLineF::IntersectionType T_LlapL(const QLineF& l1, const QLineF& l2, bool closed) {
    // Extract points and vectors
    const QPointF& a = l1.p1();
    const QPointF& b = l1.p2();
    const QPointF& c = l2.p1();
    const QPointF& d = l2.p2();

    const qreal dx1 = b.x() - a.x();
    const qreal dy1 = b.y() - a.y();
    const qreal dx2 = d.x() - c.x();
    const qreal dy2 = d.y() - c.y();

    const qreal lenSq1 = dx1*dx1 + dy1*dy1;
    const qreal lenSq2 = dx2*dx2 + dy2*dy2;

    // Handle point-line cases
    const bool l1_is_point = lenSq1 <= EPSILON_SQ;
    const bool l2_is_point = lenSq2 <= EPSILON_SQ;

    if (l1_is_point || l2_is_point) {
        if (l1_is_point && l2_is_point) {
            return (QPointF::dotProduct(a - c, a - c) <= EPSILON_SQ)
            ? (closed ? QLineF::BoundedIntersection : QLineF::NoIntersection)
            : QLineF::NoIntersection;
        }

        const QPointF& point = l1_is_point ? a : c;
        const QLineF& line = l1_is_point ? l2 : l1;
        const qreal ldx = line.dx();
        const qreal ldy = line.dy();
        const qreal lLenSq = ldx*ldx + ldy*ldy;

        if (lLenSq <= EPSILON_SQ) {
            const QPointF linePt = line.p1();
            return (QPointF::dotProduct(point - linePt, point - linePt) <= EPSILON_SQ)
                       ? (closed ? QLineF::BoundedIntersection : QLineF::NoIntersection)
                       : QLineF::NoIntersection;
        }

        const QPointF ap = point - line.p1();
        const qreal t = (ap.x()*ldx + ap.y()*ldy) / lLenSq;

        const bool onLine = (t >= -EPSILON && t <= 1.0 + EPSILON) &&
                            ((ap.x()*ldy - ap.y()*ldx)*(ap.x()*ldy - ap.y()*ldx) <= EPSILON_SQ * lLenSq);

        if (!onLine) return QLineF::NoIntersection;

        if (closed) {
            return (t >= -EPSILON && t <= 1.0 + EPSILON)
            ? QLineF::BoundedIntersection
            : QLineF::NoIntersection;
        }
        return (t > EPSILON && t < 1.0 - EPSILON)
                   ? QLineF::BoundedIntersection
                   : QLineF::NoIntersection;
    }

    // Check collinearity
    const qreal crossDir = dx1 * dy2 - dy1 * dx2;
    if (std::abs(crossDir) <= EPSILON) {
        // Verify true collinearity by checking if c lies on l1
        const QPointF ac = c - a;
        const qreal crossAC = ac.x() * dy1 - ac.y() * dx1;
        if (crossAC * crossAC > EPSILON_SQ * lenSq1) return QLineF::NoIntersection;

        // Project l2 onto l1's parametric space
        const qreal t1 = (ac.x()*dx1 + ac.y()*dy1) / lenSq1;
        const qreal t2 = (d.x() - a.x())*dx1 + (d.y() - a.y())*dy1;
        const qreal t2_norm = t2 / lenSq1;

        const qreal tMin = std::min(t1, t2_norm);
        const qreal tMax = std::max(t1, t2_norm);

        const qreal overlapStart = std::max(0.0, tMin);
        const qreal overlapEnd = std::min(1.0, tMax);

        if (overlapEnd - overlapStart < -EPSILON) return QLineF::NoIntersection;

        // Check overlap length
        const qreal overlapLength = (overlapEnd - overlapStart) * std::sqrt(lenSq1);
        if (overlapLength > EPSILON) return QLineF::UnboundedIntersection;

        // Check point containment
        const qreal tPoint = (overlapStart + overlapEnd) * 0.5;  // Midpoint to avoid fp errors
        const bool containedL1 = closed
                                     ? (tPoint >= -EPSILON && tPoint <= 1.0 + EPSILON)
                                     : (tPoint > EPSILON && tPoint < 1.0 - EPSILON);

        const QPointF midPoint = a + QPointF(dx1 * tPoint, dy1 * tPoint);
        const qreal s = (midPoint.x() - c.x())*dx2 + (midPoint.y() - c.y())*dy2;
        const qreal s_norm = s / lenSq2;
        const bool containedL2 = closed
                                     ? (s_norm >= -EPSILON && s_norm <= 1.0 + EPSILON)
                                     : (s_norm > EPSILON && s_norm < 1.0 - EPSILON);

        return (containedL1 && containedL2) ? QLineF::BoundedIntersection : QLineF::NoIntersection;
    }

    // Non-collinear case - compute intersection parameters
    const qreal denominator = (a.x() - b.x())*(c.y() - d.y()) - (a.y() - b.y())*(c.x() - d.x());
    if (std::abs(denominator) < EPSILON) return QLineF::NoIntersection;

    const qreal t_num = (a.x() - c.x())*(c.y() - d.y()) - (a.y() - c.y())*(c.x() - d.x());
    const qreal t = t_num / denominator;

    const qreal s_num = (a.x() - c.x())*(a.y() - b.y()) - (a.y() - c.y())*(a.x() - b.x());
    const qreal s = s_num / denominator;

    // Check containment with closed flag
    const bool containedL1 = closed
                                 ? (t >= -EPSILON && t <= 1.0 + EPSILON)
                                 : (t > EPSILON && t < 1.0 - EPSILON);

    const bool containedL2 = closed
                                 ? (s >= -EPSILON && s <= 1.0 + EPSILON)
                                 : (s > EPSILON && s < 1.0 - EPSILON);

    return (containedL1 && containedL2) ? QLineF::BoundedIntersection : QLineF::NoIntersection;
}



QPointF pA(const QPointF& A, const QPointF& B) {
    float dx = A.x() - B.x();
    float dy = A.y() - B.y();
    float angle = std::atan2(dy, dx);
    float absAngle = std::abs(angle);
    if(absAngle > DEG45 && absAngle < DEG135)
        return QPointF(B.x(), B.y() + dy);
    else
        return QPointF(B.x() + dx, B.y());
}

QPointF pLA(const QLineF& L, const QPointF& A) {
    const QPointF p1 = L.p1();
    const QPointF vec = L.p2() - p1;
    const float lenSq = vec.x() * vec.x() + vec.y() * vec.y();

    if (lenSq < EPSILON) return p1;

    const float t = QPointF::dotProduct(A - p1, vec) / lenSq;
    return p1 + t * vec;
}
