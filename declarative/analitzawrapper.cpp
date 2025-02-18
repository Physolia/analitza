/*************************************************************************************
 *  Copyright (C) 2010 by Aleix Pol <aleixpol@kde.org>                               *
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

#include "analitzawrapper.h"
#include <analitza/analyzer.h>
#include <analitza/value.h>
#include <analitza/analitzautils.h>
#include <analitzagui/variablesmodel.h>
#include <QCoreApplication>


Q_DECLARE_METATYPE(ExpressionWrapper*)

ExpressionWrapper::ExpressionWrapper(QObject* parent)
    : QObject(parent)
{}


ExpressionWrapper::ExpressionWrapper(const Analitza::Expression& e, QObject* parent)
    : QObject(parent)
    , m_exp(e)
{}

bool ExpressionWrapper::isCorrect() const { return m_exp.isCorrect(); }
QString ExpressionWrapper::toString() const { return m_exp.toString(); }
QVariant ExpressionWrapper::value() const { return AnalitzaUtils::expressionToVariant(m_exp); }
QStringList ExpressionWrapper::errors() const { return m_exp.error(); }

//////////////////////////

AnalitzaWrapper::AnalitzaWrapper(QObject* parent)
    : QObject(parent)
    , m_wrapped(nullptr), m_vars(new Analitza::Variables), m_calc(false)
{
    initWrapped();
}

AnalitzaWrapper::~AnalitzaWrapper()
{}

void AnalitzaWrapper::initWrapped()
{
    if(!m_wrapped) {
        m_wrapped.reset(new Analitza::Analyzer(m_vars));
    }
}

void AnalitzaWrapper::setVariables(const QSharedPointer<Analitza::Variables> &v)
{
    m_wrapped->setVariables(v);
    m_vars = v;
    initWrapped();
}

QVariant AnalitzaWrapper::simplify(const QString& expression)
{
    initWrapped();
    Analitza::Expression e(expression, false);
    if(!e.isCorrect()) {
        return e.error();
    }
    m_wrapped->setExpression(e);
    m_wrapped->simplify();

    return QVariant::fromValue(new ExpressionWrapper(m_wrapped->expression()));
}

QVariant AnalitzaWrapper::execute(const QString& expression)
{
    initWrapped();
    Analitza::Expression e(expression, false);
    if(!e.isCorrect()) {
        return e.error();
    }
    m_wrapped->setExpression(e);
    
    Analitza::Expression res;
    if(m_calc)
        res = m_wrapped->calculate();
    else
        res = m_wrapped->evaluate();
    
    if(!m_wrapped->isCorrect())
        return QVariant();
    
    return QVariant::fromValue(new ExpressionWrapper(res));
}

QVariant AnalitzaWrapper::executeFunc(const QString& name, const QVariantList& args)
{
    if(m_vars && !m_vars->contains(name))
        return QVariant();
    
    QStack<Analitza::Object*> stack;
    QList<Analitza::Expression> exps;
    foreach(const QVariant& v, args) {
        exps += AnalitzaUtils::variantToExpression(v);
        stack << exps.last().tree();
    }
    
    m_wrapped->setExpression(Analitza::Expression(name, false));
    m_wrapped->setExpression(m_wrapped->calculate());

    if (const auto bvars = m_wrapped->expression().bvarList(); bvars.size() != args.size()) {
        return QCoreApplication::tr("Have %1 bounded variables needed, got %2").arg(bvars.join(QLatin1String(", "))).arg(args.size());
    }

    m_wrapped->setStack(stack);
    Analitza::Expression expr = m_wrapped->calculateLambda();
    
    if(!m_wrapped->isCorrect())
        return QVariant();
    else
        return QVariant::fromValue(new ExpressionWrapper(expr));
}

QString AnalitzaWrapper::dependenciesToLambda(const QString& expression) const
{
    Analitza::Expression exp(expression, false);
    if (!exp.isCorrect()) {
        qWarning() << "Error on expression" << expression << exp.error();
        return {};
    }
    m_wrapped->setExpression(exp);
    return m_wrapped->dependenciesToLambda().toString();
}

void AnalitzaWrapper::insertVariable(const QString& name, const QString& expression) const
{
    m_wrapped->insertVariable(name, Analitza::Expression(expression, false));
}

QString AnalitzaWrapper::unusedVariableName() const
{
    QString candidate;
    char curr='a';
    
    for(candidate=curr; m_vars->contains(candidate); ) {
        curr+=1;
        if(curr>'z')
            curr='a';
        else
            candidate.chop(1);
        
        candidate += curr;
    }
    
    return candidate;
}

void AnalitzaWrapper::removeVariable(const QString& name)
{
    m_vars->remove(name);
}

bool AnalitzaWrapper::isCorrect() const { return m_wrapped->isCorrect(); }

QStringList AnalitzaWrapper::errors() const { return m_wrapped->errors(); }

void AnalitzaWrapper::setCalculate(bool calc)
{
    if (m_calc != calc) {
        m_calc = calc;
        Q_EMIT isCalculateChanged(calc);
    }
}

#include "moc_analitzawrapper.cpp"
