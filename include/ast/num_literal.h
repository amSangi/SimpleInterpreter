#pragma once

#include "expression.h"

class NumLiteral : public Expression
{
public:
	explicit NumLiteral(double value) : value_(value) {}
	~NumLiteral() override = default;

	VisitorValue Accept(IVisitor* v) override               {   return v->Visit(this); }
	std::string ToString() override                         {   return std::to_string(value_); }

	double GetValue() const                                 {   return value_; }
private:
	double value_;

}; // class NumLiteral

