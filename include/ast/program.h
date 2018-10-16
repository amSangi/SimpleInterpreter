#pragma once

#include "ast_node.h"
#include "function_decl.h"

class Program : public ASTNode
{
	typedef std::shared_ptr<FunctionDecl> FunDeclPtr;
public:
	Program() = default; 
	~Program() override = default;

	void Accept(std::shared_ptr<IVisitor> v) override			{	v->Visit(this);	}
	std::string ToString() override								{
		std::string res;
		for (auto function : functions_) {
			res += function->ToString();
		}
		res += main_->ToString();
		return res;
	}

	void SetMain(FunDeclPtr main)								{	main_ = std::move(main); }
	void AddFuncDecl(FunDeclPtr fun)							{	functions_.emplace_back(std::move(fun)); }
	const std::vector<FunDeclPtr>& GetFunctions() const			{	return functions_; }
	FunctionDecl* GetMain() const								{	return main_.get(); }
private:
	std::vector<FunDeclPtr> functions_;
	FunDeclPtr main_;

}; // class Program

