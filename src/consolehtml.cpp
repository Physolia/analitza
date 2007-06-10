/*************************************************************************************
 *  Copyright (C) 2007 by Aleix Pol <aleixpol@gmail.com>                             *
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

#include "consolehtml.h"
#include <QFile>
#include <QHeaderView>
#include <QScrollBar>

#include <KLocale>
#include <KUrl>
#include <KHTMLView>
#include <KApplication>

#include "variables.h"
#include "expression.h"

ConsoleHtml::ConsoleHtml(QWidget *parent) : KHTMLPart(parent), m_mode(Evaluation)
{
	setJScriptEnabled(false);
	setJavaEnabled(false);
	setMetaRefreshEnabled(false);
	setPluginsEnabled(false);
	
	//FIXME: Should check if it is connected
	//if it is connect to a kalgebra page that doesn't exist, if not I don't know
	begin();
	write("<html>Make me useful, please!!!</html>");
	end();
}

ConsoleHtml::~ConsoleHtml() {}

bool ConsoleHtml::addOperation(const QString& op, bool mathml)
{
	
	QString result, newEntry;
	a.setExpression(Expression(op, mathml));
	if(a.isCorrect()) {
		if(m_mode==Evaluation) {
			Expression res=a.evaluate();
			result = res.toHtml();
			a.m_vars->modify("ans", res.tree());
		} else {
			Cn val=a.calculate();
			result = val.toHtml();
			a.m_vars->modify("ans", &val);
		}
	}
	
	if(a.isCorrect()) {
		m_script += op; //Script won't have the errors
		newEntry = QString("%1 = %2").arg(a.expression()->toHtml()).arg(result);
	} else
		m_htmlLog += QString("<span class='error'>Error: %1 <br />%2</span>").arg(op).arg(a.m_err.join("<br />\n"));
	
	updateView(newEntry);
	
	return a.isCorrect();
}

bool ConsoleHtml::loadScript(const QString& path)
{
	bool correct=false;
	if(!path.isEmpty()) {
		QStringList lines;
		QFile file(path);
		if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			QTextStream stream(&file);
			QString line;
			correct=true;
			while (!stream.atEnd()) {
				line = stream.readLine(); // line of text excluding '\n'
				line = line.trimmed();
				if(!line.isEmpty())
					correct &= addOperation(line);
			}
			file.close();
		}
	}
	return correct;
}

bool ConsoleHtml::saveScript(const QString & path) const
{
	bool correct=false;
	if(!path.isEmpty()) {
		QFile file(path);
		correct=file.open(QIODevice::WriteOnly | QIODevice::Text);
		
		if(correct) {
			QTextStream out(&file);
			QStringList::const_iterator it = m_script.begin();
			for(; it!=m_script.end(); it++)
				out << *it << endl;
		}
		file.close();
	}
	return correct;
}

bool ConsoleHtml::saveLog(const QString& path) const
{
	//FIXME: We have to choose between txt and html
	bool correct=false;
	if(!path.isEmpty()) {
		QFile file(path);
		correct=file.open(QIODevice::WriteOnly | QIODevice::Text);
		if(correct) {
			QTextStream out(&file);
			foreach(QString entry, m_htmlLog)
				out << "<p>" << entry << "</p>" << endl;
		}
		file.close();
	}
	return correct;
}

void ConsoleHtml::updateView(const QString& newEntry)
{
	QString log("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
	log +="<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\">\n<head>\n\t<title> :) </title>\n";
	//log +="\t<link rel=\"stylesheet\" type=\"text/css\" href=\"http://math.nist.gov/~BMiller/mathml-css/style/mathml.css\"/>\n";
	log +="<style type=\"text/css\">\n";
	log +="\t.error { border-style: solid; border-width: 1px; border-color: #ff3b21; background-color: #ffe9c4; padding:7px;}\n";
	log +="\t.last  { border-style: solid; border-width: 1px; border-color: #2020ff; background-color: #e0e0ff; padding:7px;}\n"	;
	log +="\t.before { text-align:right; }\n";
	log +="\t.op  { font-weight: bold; }\n";
	log +="\t.sep { font-weight: bold; }\n";
	log +="\t.num { color: #0000C4; }\n";
	log +="\t.var { color: #640000; }\n";
	log +="\t.func{ color: #003600; }\n";
	log +="</style>\n</head>\n<body>\n";
	
	begin();
	write(log);
	foreach(QString entry, m_htmlLog) {
		write("<p>"+entry+"</p>");
	}
	if(!newEntry.isEmpty()) {
		m_htmlLog += newEntry;
		write("<p class='last'>"+newEntry+"</p>");
	}
	write("</body></html>");
	end();
	
// 	qDebug() << "puto!" << m_htmlLog.join("<p />\n");
	
	view()->verticalScrollBar()->setValue(view()->verticalScrollBar()->maximum());
	emit changed();
	qApp->processEvents();
}

void ConsoleHtml::clear()
{
	m_script.clear();
	m_htmlLog.clear();
	updateView();
}

///////////////////////////////////////////////////
///////////////////////////////////////////////////
///////////////////////////////////////////////////

VariableView::VariableView(QWidget *parent) : QTreeWidget(parent), a(NULL)
{
	setColumnCount(2);
	headerItem()->setText(0, i18n("Name"));
	headerItem()->setText(1, i18n("Value"));
	clear();
	header()->setResizeMode(0, QHeaderView::ResizeToContents);
	setRootIsDecorated(false);
	setSortingEnabled(false);
	setSelectionMode(QAbstractItemView::SingleSelection);
}

VariableView::~VariableView() {}

void VariableView::updateVariables()
{
	clear();
	if(a!=NULL) {
		QList<QTreeWidgetItem *> items;
		QHash<QString, Object*>::const_iterator it = a->m_vars->begin();
		for(; it != a->m_vars->end(); ++it) {
			QTreeWidgetItem* item = new QTreeWidgetItem(this);
			item->setText(0, it.key());
			item->setText(1, it.value()->toString());
			
			items.append(item);
		}
		sortItems(0, Qt::AscendingOrder);
	}
}

#include "consolehtml.moc"
