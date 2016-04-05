#ifndef AnalysisVariable_h
#define AnalysisVariable_h

#include <string>

namespace TPCECalSystematics
{
class AnalysisVariable
{
public:
   /**
      Constructs an AnalysisVariable based on the given microtree variable.

      \param microtreeVariable   The microtree variable to use for the analysis.
      \param name The name of the analysis variable. This is used in producing
                  plot filenames.
      \param description   A descriptive phrase for the analysis variable. This
                           is used as the title for the analysis variable's plot
                           axis.
   */
   AnalysisVariable(const std::string& microtreeVariable,
      const std::string& name="", const std::string& description="");

   /**
      Copies the given AnalysisVariable object.

      \param variable The object to be copied.
   */
   AnalysisVariable(const AnalysisVariable& variable);

   /**
      Assigns the state of the given AnalsysisVariable object to this
      AnalysisVariable.

      \param variable   The object whose state is to be copied.
   */
   AnalysisVariable& operator=(const AnalysisVariable& variable);

   /**
      Destroys this AnalysisVariable object.
   */
   virtual ~AnalysisVariable();

   /**
      Returns the string that provies access to the microtree variable.

      \return The microtree variable describing this analysis variable.
   */   
   const std::string& GetMicrotreeVariable() const;

   /**
      Returns the name of the analysis variable.

      \return The name of this analysis variable.
   */   
   const std::string& GetName() const;

   /**
      Returns the description of the analysis variable.

      \return The description of this analysis variable.
   */   
   const std::string& GetDescription() const;

private:
   std::string _microtreeVariable;
   std::string _name;
   std::string _description;
};
}

#endif
