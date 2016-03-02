#ifndef TestTPCECalSystematics_h
#define TestTPCECalSystematics_h

#include "baseTrackerAnalysis.hxx"
#include "AnalysisUtils.hxx"

class TestTPCECalSystematics: public baseTrackerAnalysis
{
   public:
   TestTPCECalSystematics(AnalysisAlgorithm* ana=NULL);
   virtual ~TestTPCECalSystematics(){}

   //---- These are mandatory functions
   /**
      Performs all initialization that cannot be done in the constructor.
      
      In particular it handles initializations requiring access to a parameter
      in the parameters file, because use of the parameter override file option
      (-p param.dat) precludes the possibility of accessing parameters in the
      constructors.
      \return  True if the initialization was successful, false otherwise.
    */
   bool Initialize();

   /**
      Defines the selections to be added to the SelectionManager.
     
      In general an analysis has a single selection, but may have multiple
      branches. Each branch acts as a different selection (different cuts and
      actions), but provides a gain in speed and simplicity since common steps
      are applied only once.
    */
   void DefineSelections();

   /**
      Define the corrections to be added to the CorrectionManager, which will
      modify the input data (real data or MC).

      The entire analysis will proceed on the modified data
    */
   void DefineCorrections();

   /**
      Define the systematics to be added to the SystematicManager, which will
      modify the effective number of events passing the selection and the
      distribution of the observables. 

      There are two kind of systematics:
      - Variations: modify the input data (momentum, PID variables, etc) and
        thus can affect the selection.
      - Weights: do not modify the input data and thus do not affect the
        selection, rather a weight is added to the event. Since events with
        different values of a given observable (i.e. momentum ) can have
        different weights, distributions of that observable may change.
    */
   void DefineSystematics();

   /**
      Defines the configurations to be added to the ConfigurationManager, based
      on the systematics that are enabled, the number of toys being run and the
      random seed used to generate the toys.
      
      Each configuration has a micro-tree associated in the output file (with
      the same name).
    */
   void DefineConfigurations();

   /**
      Define the micro-trees appearing in the output file.

      The "default" micro-tree should contain the basic information to
      understand the selection. Additional micro-trees can be added by including
      additional configurations to the analysis.

      \param addBase Adds the variables from the base class to the micro-tree if
                     true.
    */
   void DefineMicroTrees(bool addBase=true);

   /**
      Define the truth trees appearing in the output file.
      
      The truth tree contains information on all interactions in which we are
      interested regardless of whether or not the selection passed. As such,
      truth trees should be used to compute signal efficiencies.
    */
   void DefineTruthTree();

   /**
      Fills all toy-independent variables defined in the micro-trees. 

      This method is called once all toys have been run, meaning that the value
      of all variables will correspond to the last toy run. This is not a
      problem for variables that do not change from one toy to another.

      \param addBase Fills the variables from the base class if true.
    */
   void FillMicroTrees(bool addBase=true);

   /**
      Fills all toy-dependent variables defined in the micro-trees. 

      This method is called at the end of each toy. Toy variables consume a lot
      of space and therefore only those variables that we want to plot for
      different toys should be saved as toy variables, even if other variables
      may be toy dependent.

      \param addBase Fills the variables from the base class if true.
    */
   void FillToyVarsInMicroTrees(bool addBase=true);

   /**
      Checks whether or not to include a given true vertex in the truth tree.

      \param vtx The true vertex to be checked.
    */
   bool CheckFillTruthTree(const AnaTrueVertex& vtx);

   /**
      Fill the truth tree variables.

      \param vtx The true vertex from which the variables should be filled.
    */
   void FillTruthTree(const AnaTrueVertex& vtx);
   //--------------------

   /**
     Fill the micro-tree variables used for colour code drawing.
     
     The colour coding variables are added automatically (i.e. they do not need
     to be added in DefineMicroTrees) to the  micro-trees, but must be filled by
     the analyser, providing the event and the relevant track. 

     If this method is not implemented, the one from the base class
     (baseTrackerAnalysis::FillCategories()) will be called.
   */
   void FillCategories();
  
};

#endif
