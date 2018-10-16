#pragma once

#include "ast_node.h"
#include "static_type.h"

class FunctionParam : public ASTNode
{
    typedef std::string string;
    typedef std::shared_ptr<StaticType> TypePtr;
public:
    FunctionParam() = default;
    ~FunctionParam() = default;

    virtual void Accept(std::shared_ptr<IVisitor> v)        {   v->Visit(this); }
    virtual std::string ToString()                          {   return type_->ToString() + " " + identifier_name_; }

    void SetType(TypePtr type)								{	type_ = std::move(type); }
    void SetName(const string name)							{	identifier_name_ = name; }
    StaticType* GetType() const								{	return type_.get(); }
    string GetParamName() const								{	return identifier_name_; }
private:
    TypePtr type_;
    string identifier_name_;

}; // class FunctionParam