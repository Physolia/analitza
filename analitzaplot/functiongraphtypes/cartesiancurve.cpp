/*************************************************************************************
 *  Copyright (C) 2007-2009 by Aleix Pol <aleixpol@kde.org>                          *
 *  Copyright (C) 2010 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      *
 *                                                                                   *
 *  This program is free software; you can redistribute it and/or                    *
 *  modify it under the terms of the GNU General Public License                      *
 *  as published by the Free Software Foundation; either version 2                   *
 *  of the License, or (at your option) any later version.                           *
 *                                                                                   *
 *  This program is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 *  GNU General Public License for more details.                                     *
 *                                                                                   *
 *  You should have received a copy of the GNU General Public License                *
 *  along with this program; if not, write to the Free Software                      *
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA   *
 *************************************************************************************/


#include "functiongraph.h"
#include "functiongraphfactory.h"

#include <QRectF>
#include "analitza/value.h"
#include "analitza/variable.h"


class ANALITZAPLOT_EXPORT CartesianCurveY : public AbstractPlaneCurve
{
public:
    CartesianCurveY(const Analitza::Expression &functionExpression, Analitza::Variables *variables);
	~CartesianCurveY()
	{
	}


    //MappingGraph
    const QString typeName() const { return TypeName(); }
    QString iconName() const { return IconName(); }
    QStringList examples() const { return Examples(); }
    int spaceDimension() const { return SpaceDimension(); }
    CoordinateSystem coordinateSystem() const { return CoordSystem(); }
    QStringList errors() const { return m_errors; }
    bool isCorrect() const { return false; }
    AbstractMappingGraph * copy();

    //FunctionGraph
    QStringList arguments() const { return Arguments(); }
    void update(const QRect& viewport);

    //Curve
    double arcLength() const;
    bool isClosed() const { return false; }
    double area() const;
    QPair<bool, double> isParallelTo(const Curve &othercurve);

    //Own
    QPair<QVector2D, QString> calc(const QPointF &mousepos);
    QLineF derivative(const QPointF &mousepos) const;

    bool isImplicit() const { return false; }
    bool isParametric() const { return false; }

    //factory registration
    static QString TypeName() { return QString("CartesianCurveY"); }
    static Analitza::ExpressionType ExpressionType()
    {
        return Analitza::ExpressionType(Analitza::ExpressionType::Lambda).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                   Analitza::ExpressionType(Analitza::ExpressionType::Value));
    }
    static int SpaceDimension() { return 2; }
    static CoordinateSystem CoordSystem() { return Cartesian; }
    static QStringList Arguments() { return QStringList() << "x"; }
    static QString IconName() { return QString(); }
    static QStringList Examples() { return QStringList() << "x*x+x" << "x-2*x*x"; }

private:
    void optimizeJump();
    
    
QStringList m_errors;
};

CartesianCurveY::CartesianCurveY(const Analitza::Expression &functionExpression, Analitza::Variables *variables)
:AbstractPlaneCurve(functionExpression, variables)
{
}


AbstractMappingGraph * CartesianCurveY::copy()
{
	return 0;
}


//FunctionGraph

void CartesianCurveY::update(const QRect& viewport)
{
    double l_lim=viewport.left()-.1, r_lim=viewport.right()+.1;

    if(!points().isEmpty()
            && isSimilar(points().first().x(), l_lim)
            && isSimilar(points().last().x(), r_lim)) {
        return;
    }
    
    clearPoints();
    clearPoints();;
//     points.reserve(resolution());
 
    //TODO GSOC(pres)
    double step= 0.1;
    
    bool jumping=true;
    
    for(double x=l_lim; x<r_lim-step; x+=step) 
    {

        arg("x")->setValue(x);
        Analitza::Cn y = analyzer.calculateLambda().toReal();
        QVector2D p(x, y.value());
        bool ch=addPoint(p);
        
        
        bool jj=jumping;
        jumping=false;
        if(ch && !jj) {
//          if(!m_jumps.isEmpty()) qDebug() << "popopo" << m_jumps.last() << points.count();
            double prevY=points()[points().count()-2].y();
            if(y.format()!=Analitza::Cn::Real && prevY!=y.value()) {
                setJump(points().count()-1);
                jumping=true;
            } else if(points().count()>3 && traverse(points()[points().count()-3].y(), prevY, y.value())) {
                optimizeJump();
                setJump(points().count()-1);
                jumping=true;
            }
        }
    }
    

//  qDebug() << "juuuumps" << m_jumps << resolution();
}

//Curve


double CartesianCurveY::arcLength() const
{
    return 0;
}

double CartesianCurveY::area() const
{
    return 0;
}

QPair<bool, double> CartesianCurveY::isParallelTo(const Curve &othercurve)
{
    return qMakePair(false, 0.0);
}


//Own
QPair<QVector2D, QString> CartesianCurveY::calc(const QPointF &mousepos)
{
    return qMakePair(QVector2D(), QString());
}

QLineF CartesianCurveY::derivative(const QPointF &mousepos) const
{
    return QLineF();
}

void CartesianCurveY::optimizeJump()
{
//     QPointF before = points.at(points.count()-2), after=points.last();
//     qreal x1=before.x(), x2=after.x();
//     qreal y1=before.y(), y2=after.y();
//     int iterations=5;
//     
// //  qDebug() << "+++++++++" << before << after;
//     for(; iterations>0; --iterations) {
//         qreal dist = x2-x1;
//         qreal x=x1+dist/2;
//         
//         vx->setValue(x);
//         qreal y = func.calculateLambda().toReal().value();
//         
//         if(fabs(y1-y)<fabs(y2-y)) {
//             before.setX(x);
//             before.setY(y);
//             x1=x;
//             y1=y;
//         } else {
//             after.setX(x);
//             after.setY(y);
//             x2=x;
//             y2=y;
//         }
//     }
// //  qDebug() << "---------" << before << after;
//     points[points.count()-2]=before;
//     points.last()=after;
}





REGISTER_PLANECURVE(CartesianCurveY)

