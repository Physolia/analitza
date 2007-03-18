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


#ifndef ANALITZA_H
#define ANALITZA_H

#include <cmath>

#include "expression.h"
#include "container.h"
#include "value.h"
#include "variables.h"

/**
*	@author Aleix Pol <aleixpol@gmail.com>
*/

class Analitza
{
public:
	Analitza();
	Analitza(const Analitza& a);
	~Analitza();
	
	QString operToString(const Operator&) const;
	
	void setVariables(Variables* v) { if(m_vars!=NULL) delete m_vars; m_vars = v; }
	void setExpression(const Expression &e);
	Expression* expression() { return &m_exp; }
	
	Cn calc(Object* e);
	Object* eval(const Object* e);
	Cn calculate();
	Expression evaluate();
	Cn operate(Container*);
	bool isCorrect() const { return m_exp.isCorrect() && m_err.isEmpty(); }
	
	Cn sum(const Container& c);
	Cn product(const Container& c);
	Cn func(const Container& c);
	bool isFunction(Ci var) const;
	QStringList bvarList() const;
	void flushErrors() { m_err = QStringList(); }
	
	void simplify(); //FIXME: Should return an Expression
	Object* simp(Object* root);
	void simpScalar(Container* c);
	void simpPolynomials(Container* c);
	
	static bool hasVars(const Object*, const QString &var=QString());
private:
	Object* derivative(const QString &var, const Object*);
	Object* derivative(const QString &var, const Container*);
	void reduce(enum Object::OperatorType op, Cn *ret, Cn oper, bool unary);
	Object* removeDependencies(Object* o) const;
	void levelOut(Container *c, Container *ob, QList<Object*>::iterator &it);
	
public: //FIXME:Shame on me
	Expression m_exp;
	Variables *m_vars;
	QStringList m_err;
};

#endif
