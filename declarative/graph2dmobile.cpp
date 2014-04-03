/*************************************************************************************
 *  Copyright (C) 2011 by Aleix Pol <aleixpol@kde.org>                               *
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

#include "graph2dmobile.h"
#include <QPainter>
#include <QEvent>
#include <analitza/variables.h>
#include <analitzaplot/planecurve.h>
#include <analitzaplot/plotsmodel.h>
#include <analitzaplot/plotsfactory.h>

using namespace Analitza;

Graph2DMobile::Graph2DMobile(QQuickItem* parent)
	: QQuickPaintedItem(parent), Plotter2D(boundingRect().size())
	, m_dirty(true), m_currentFunction(-1)
{
	setSize(QSizeF(100,100));
	
	defViewport = QRectF(QPointF(-12., 10.), QSizeF(24., -20.));
	resetViewport();
}

void Graph2DMobile::paint(QPainter* p)
{
// 	qDebug() << "hellooo" << boundingRect();
	if(boundingRect().size().isEmpty())
		return;
	
	if(m_buffer.size()!=boundingRect().size()) {
		m_buffer = QImage(boundingRect().size().toSize(), QImage::Format_ARGB32);
		setPaintedSize(boundingRect().size().toSize());
	}
	
	Q_ASSERT(!m_buffer.isNull());
	
	if(m_dirty) {
		m_buffer.fill(Qt::transparent);
		drawFunctions(&m_buffer);
		m_dirty=false;
	}
	
	p->drawImage(QPoint(0,0), m_buffer);
}

void Graph2DMobile::forceRepaint()
{
	m_dirty=true;
	update();
}

void Graph2DMobile::resetViewport()
{
	setViewport(defViewport);
}

void Graph2DMobile::modelChanged()
{
	connect(model(), SIGNAL(dataChanged( const QModelIndex&, const QModelIndex& )),
		this, SLOT(updateFuncs(const QModelIndex&, const QModelIndex)));
	connect(model(), SIGNAL( rowsInserted ( const QModelIndex &, int, int ) ),
		this, SLOT(addFuncs(const QModelIndex&, int, int)));
	connect(model(), SIGNAL( rowsRemoved ( const QModelIndex &, int, int ) ),
		this, SLOT(removeFuncs(const QModelIndex&, int, int)));
}

void Graph2DMobile::addFuncs(const QModelIndex& parent, int start, int end) { updateFunctions(parent, start, end); }

void Graph2DMobile::removeFuncs(const QModelIndex&, int, int) { forceRepaint(); }
void Graph2DMobile::updateFuncs(const QModelIndex& start, const QModelIndex& end) { updateFunctions(QModelIndex(), start.row(), end.row()); }

void Graph2DMobile::scale(qreal s, int x, int y)
{
	QRectF userViewport = lastUserViewport();
	if(s>1 || (userViewport.height() < -3. && userViewport.width() > 3.)) {
		scaleViewport(s, QPoint(x,y));
	}
}

void Graph2DMobile::translate(qreal x, qreal y)
{
	moveViewport(QPoint(x,y));
}

QColor randomFunctionColor() { return QColor::fromHsv(qrand()%255, 255, 225); }

QStringList Graph2DMobile::addFunction(const QString& expression, Analitza::Variables* vars)
{
	Analitza::Expression e(expression, Analitza::Expression::isMathML(expression));
	PlotsModel* plotsmodel = qobject_cast<PlotsModel*>(model());
	if(!plotsmodel)
		qWarning() << "only can add plots to a PlotsModel instance";
	QString fname;
	do {
		fname = plotsmodel->freeId();
	} while(vars && vars->contains(fname));
	QColor fcolor = randomFunctionColor();
	
	QStringList err;
	PlotBuilder req = PlotsFactory::self()->requestPlot(e, Dim2D, vars);
	if(req.canDraw()) {
		PlaneCurve* it = static_cast<PlaneCurve*>(req.create(fcolor, fname));
		
		if(it->isCorrect())
			plotsmodel->addPlot(it);
		else {
			err = it->errors();
			delete it;
		}
	}
	
	return err;
}
void Graph2DMobile::setTicksShownAtAll(bool shown)
{
	Qt::Orientations show = shown ? Qt::Vertical|Qt::Horizontal : Qt::Orientations(0);
	setShowTicks(show);
	setShowTickLabels(show);
}
