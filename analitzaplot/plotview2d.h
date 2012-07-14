/*************************************************************************************
 *  Copyright (C) 2007-2008 by Aleix Pol <aleixpol@kde.org>                          *
 *  Copyright (C) 2012 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      *
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

#ifndef GRAPH2D_H
#define GRAPH2D_H

#include <QResizeEvent>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QKeyEvent>
#include <QWidget>
#include <QPainter>
#include <QLabel>
#include <QPixmap>
#include <QModelIndex>

#include "planecurve.h"

#include "analitzaplotexport.h"
#include "plotter2d.h"

/**
 *  This class lets you create a widget that can draw multiple 2D graphs.
 *  @author Aleix Pol Gonzàlez
 */

class QItemSelectionModel;
class VisualItemsModel;

class ANALITZAPLOT_EXPORT Graph2D : public QWidget, public FunctionsPainter
{
Q_OBJECT
Q_PROPERTY(bool squares READ squares WRITE setSquares)
public:
    /** The graph mode will especify the selection mode we are using at the moment */
    enum GraphMode {
        None=0,     /**< Normal behaviour */
        Pan,        /**< Panning, translates the viewport. */
        Selection   /**< There is a rectangle delimiting a region, for zooming. */
    };
    
    enum Format { PNG, SVG };
    
    /** Constructor. Constructs a new Graph2D. */
    Graph2D(QWidget *parent = 0); // this ctor es para que pueda ser usado en el designer
    
    //prererenciteme 1 constructor
    
    Graph2D(VisualItemsModel* fm, QWidget *parent = 0);
    
    /** Destructor. */
    ~Graph2D();
    
//     setmodel
    
    QSize sizeHint() const { return QSize(100, 100); }
    
    /** Saves the graphs to a file located at @p path. */
    bool toImage(const QString& path, Format f);
    
    /** Returns the viewing port */
    QRectF definedViewport() const;
    
    void setSelectionModel(QItemSelectionModel* selection);
    
public slots:
    /** Marks the image as dirty and repaints everything. */
    void forceRepaint() { valid=false; repaint(); }

    /** Sets the viewport to a default viewport. */
    void resetViewport() { setViewport(defViewport); }
    
    /** Zooms in to the Viewport center */
    void zoomIn();
    
    /** Zooms out */
    void zoomOut();
        
    /** Returns whether it has a little border frame. */
    bool isFramed() const { return m_framed; }
    
    /** Sets whether it has a little border frame. */
    void setFramed(bool fr) { m_framed=fr; }
    
    /** Returns whether it is a read-only widget. */
    bool isReadOnly() const { return m_readonly; }
    
    /** Sets whether it is a read-only widget. */
    void setReadOnly(bool ro);
    
private slots:
    void updateFuncs(const QModelIndex& start, const QModelIndex& end);
    void addFuncs(const QModelIndex & parent, int start, int end);
    void removeFuncs(const QModelIndex & parent, int start, int end);
    void changeViewport(const QRectF& vp) { setViewport(vp); }
    
signals:
    /** Emits a status when it changes. */
    void status(const QString &msg);
    
    void viewportChanged(const QRectF&);
    
private:
    virtual void viewportChanged();
    virtual int currentFunction() const;
    virtual void modelChanged();
    
    //painting
    QPixmap buffer;
    bool valid;
    QLabel *micepos;
    QPointF mark;
    
    //events
    void paintEvent( QPaintEvent * );
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void keyPressEvent(QKeyEvent * e );
    void wheelEvent(QWheelEvent *e);
    void resizeEvent(QResizeEvent *);
    GraphMode mode;
    QPoint press; QPoint last;
    
    //presentation
    QPointF ant;
    QRectF defViewport;
    void drawFunctions(QPaintDevice*);
        
    void sendStatus(const QString& msg) { emit status(msg); }
    bool m_framed;
    bool m_readonly;
    QString m_posText;
    QItemSelectionModel* m_selection;
};

#endif