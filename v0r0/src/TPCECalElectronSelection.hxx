#ifndef TPCECalElectronSelection_h
#define TPCECalElectronSelection_h

#include "TPCECalSelection.hxx"
#include "SelectionBase.hxx"
#include "Parameters.hxx"

//---- Define the class for the new selection, which should inherit from SelectionBase or from another existing selection -------
class TPCECalElectronSelection: public SelectionBase
{
public:
  TPCECalElectronSelection(bool forceBreak=true);
  virtual ~TPCECalElectronSelection(){}

  ///========= These are mandatory functions ==================

  /// In this method all steps are added to the selection
  void DefineSteps();
  
  /// Set detector FV
  void DefineDetectorFV();

  /// Create a proper instance of the box (ToyBoxB) to store all relevant 
  /// information to be passed from one step to the next
  ToyBoxB* MakeToyBox() {return new ToyBoxTPCECal();}

  /// Fill the EventBox with the objects needed by this selection
  void InitializeEvent(AnaEventB& event);

  //---- These are optional functions, needed by FITTERS but not by highland2 analyses --------------

  bool FillEventSummary(AnaEventB& event, Int_t allCutsPassed[]);
  nd280Samples::SampleEnum GetSampleEnum(){return nd280Samples::kFGD1NuMuCC;}
};

///---- Define all steps -------
class FindElectronPIDAction: public StepBase
{
public:
   using StepBase::Apply;
   bool Apply(AnaEventB& event, ToyBoxB& box) const;
   StepBase* MakeClone(){ return new FindElectronPIDAction(); }
};

class ElectronPIDCut: public StepBase
{
public:
   ElectronPIDCut()
   {
   }
   using StepBase::Apply;
   bool Apply(AnaEventB& event, ToyBoxB& box) const;
   StepBase* MakeClone(){ return new ElectronPIDCut(); }
};

#endif
