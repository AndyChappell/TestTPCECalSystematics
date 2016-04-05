#include "AnalysisVariable.hxx"

namespace TPCECalSystematics
{

AnalysisVariable::AnalysisVariable(const std::string& microtreeVariable,
   const std::string& name, const std::string& description):
   _microtreeVariable(microtreeVariable), _name(name), _description(description)
{
}

AnalysisVariable::AnalysisVariable(const AnalysisVariable& variable):
   _microtreeVariable(variable._microtreeVariable), _name(variable._name),
   _description(variable._description)
{
}

AnalysisVariable& AnalysisVariable::operator=(const AnalysisVariable& variable)
{
   _microtreeVariable = variable._microtreeVariable;
   _name = variable._name;
   _description = variable._description;

   return *this;
}

AnalysisVariable::~AnalysisVariable()
{
}

const std::string& AnalysisVariable::GetMicrotreeVariable() const
{
   return _microtreeVariable;
}

const std::string& AnalysisVariable::GetName() const
{
   return _name;
}

const std::string& AnalysisVariable::GetDescription() const
{
   return _description;
}

}
