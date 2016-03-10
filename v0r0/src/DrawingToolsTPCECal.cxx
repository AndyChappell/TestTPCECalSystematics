#include "DrawingToolsTPCECal.hxx"
#include <iomanip>
#include <iostream>
#include <fstream>

#include "TF1.h"
#include "TMultiGraph.h"
#include "TGraphErrors.h"
#include "TSystem.h"
#include "TPaveText.h"

DrawingToolsTPCECal::DrawingToolsTPCECal(const string& file, bool useT2Kstyle):
   DrawingTools(file, useT2Kstyle)
{
   _treeForSystErrors = NULL;
   _range = false;
}

DrawingToolsTPCECal::DrawingToolsTPCECal(Experiment& exp, bool useT2Kstyle):
   DrawingTools(exp.GetFilePath(), useT2Kstyle)
{
   _treeForSystErrors = NULL;
   _range = false;
}
