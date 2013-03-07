/*************************************************************************************
 *  Copyright (C) 2013 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      *
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

#ifndef ANALITZAPLOT_PLOTITEM_H
#define ANALITZAPLOT_PLOTITEM_H

#include <QStringList>
#include <QSet>
#include <QColor>

#include "analitzaplotexport.h"
#include "plottingenums.h"
#include <analitza/analyzer.h>

namespace Analitza {
class Variables;
class Expression;

/**
 * \interface ShapeInterface
 * 
 * \ingroup AnalitzaPlotModule
 *
 * \brief Commmon contract/interface for all shape types.
 *
 * Concrete classes have to implement implicit memory sharing by using 
 * QSharedDataPointer.
 */

template <typename ShapeType>
class ANALITZAPLOT_EXPORT ShapeInterface
{
public:
    virtual QColor color() const = 0;
    virtual CoordinateSystem coordinateSystem() const = 0;
    virtual QStringList errors() const = 0;
    virtual Expression expression() const = 0;
    virtual QString iconName() const = 0;
    virtual bool isValid() const = 0; // see if expression match its definition
    virtual bool isVisible() const = 0;
    virtual QString name() const = 0;
    virtual void setColor(const QColor &color) = 0;
    virtual void setName(const QString &name) = 0;
    virtual void setVisible(bool visible) = 0;
    virtual Dimension dimension() const = 0; // dim of the space where the item can be drawn ... IS NOT the variety/top dimension
    virtual QString typeName() const = 0;
    virtual Variables *variables() const = 0;
    
    virtual bool operator==(const ShapeType &other) const = 0;
    virtual bool operator!=(const ShapeType &other) const = 0;
    virtual ShapeType & operator=(const ShapeType &other) = 0;
};

}

#endif // ANALITZAPLOT_PLOTITEM_H
