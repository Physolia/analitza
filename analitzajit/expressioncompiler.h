/*************************************************************************************
 *  Copyright (C) 2014 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      *
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

#ifndef ANALITZAJIT_VALUECOMPILER_H
#define ANALITZAJIT_VALUECOMPILER_H

#include "abstractexpressionvisitor.h"

#include "expressiontype.h"

namespace llvm {
class Module;
class Type;
class Value;
}

namespace Analitza
{

class Variables;
class Expression;

/**
 * \class ExpressionCompiler
 * 
 * \ingroup AnalitzaJITModule
 * 
 * \brief Compiles Analitza::Expression into LLVM IR (i.e. into llvm::Value)
 */

class ExpressionCompiler : public Analitza::AbstractExpressionVisitor
{
	public:
		ExpressionCompiler(llvm::Module *module, Variables* v = 0);
		virtual ~ExpressionCompiler();
		
		/** 
		 * In case the input is invalid or could not perform the compilation, then returns a null pointer.
		 * If the expression is a lambda and/or contains variables, then 
		 * for each variable you must specify its type using @p bvartypes.
		 * @returns Returns the IR generated by the a valid @p expression
		 */
		 ///Point out that the expression must be valid
		llvm::Value *compileExpression(const Analitza::Expression& expression, const QMap< QString, Analitza::ExpressionType >& bvartypes=QMap< QString, Analitza::ExpressionType >());
		
		/** Convenience method that applies compileExpression over the @p expression object. */
		llvm::Value *compileExpression(Analitza::Object *expression, const QMap< QString, Analitza::ExpressionType >& bvartypes = QMap< QString, Analitza::ExpressionType >());
		
		/**
		 * This method can be used only after compileExpression has been called.
		 * @returns Returns the last expression type that was successfully compiled.
		 */
		Analitza::ExpressionType compiledType() const;
		
		llvm::Module *module() const;
		Analitza::Variables *variables() const;
		
	private:
		virtual QVariant visit(const Analitza::None* var);
		virtual QVariant visit(const Analitza::Ci* var);
		virtual QVariant visit(const Analitza::Cn* val);
		virtual QVariant visit(const Analitza::Container* c);
		virtual QVariant visit(const Analitza::Operator* var);
		virtual QVariant visit(const Analitza::Vector* var);
		virtual QVariant visit(const Analitza::List* list);
		virtual QVariant visit(const Analitza::Matrix* mat);
		virtual QVariant visit(const Analitza::MatrixRow* mr);
		virtual QVariant visit(const Analitza::Apply* a);
		virtual QVariant visit(const Analitza::CustomObject* c);
		virtual QVariant result() const { return QVariant(); }
		
	private:
		class ExpressionCompilerPrivate;
		ExpressionCompilerPrivate* const d;
};

}

#endif // ANALITZAJIT_VALUECOMPILER_H
