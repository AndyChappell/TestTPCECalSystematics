#ifndef DrawingToolsTPCECal_h
#define DrawingToolsTPCECal_h

#include "DrawingTools.hxx"

double GetBinomialUncertainty(double numer, double denom);

class DrawingToolsTPCECal: public DrawingTools
{
public:
   DrawingToolsTPCECal(const string& file="", bool useT2Kstyle=true);
   DrawingToolsTPCECal(Experiment& exp, bool useT2Kstyle=true);
   virtual ~DrawingToolsTPCECal(){}

   /**
      Draws a 1D efficiency plot by bin. Statistical uncertainties are
      calculated using binomial statistics if errors variable is not used,
      otherwise errors are added in quadrature.
      
      \param data The data sample for which efficiencies are to be calculated.
      \param variable   The binning variable.
      \param signal  The signal.
      \param cut  The cut.
      \param numBins The number of bins.
      \param bins   The bin boundaries.
      \param options Root plotting options.
      \param legend  The legend.
      \param errors  Any errors to be applied.
   */
   void DrawEfficiency(DataSample& data, const string var, const string& signal,
      const string& cut, int nx, double *xbins, string opt = "",
      string leg = "", vector<double> *errors = 0);

   /**
      Gets the 1D matching efficiency of a data sample. Statistical
      uncertainties are calculated using binomial statistics if the lowErrs and
      highErrs variables are not used, otherwise the low and high errors are
      filled from these vectors.
      
      \param data The data sample for which efficiencies are to be calculated.
      \param variable   The binning variable.
      \param signal  The signal.
      \param cut  The cut.
      \param numBins The number of bins.
      \param bins   The bin boundaries.
      \param lowErrs The lower errors.
      \param highErrs   The higher errors.
      \return  A vector of the efficiencies.
   */
   std::vector<double> GetEfficiencyVsBin(DataSample& data, const string var,
      const string& signal, const string& cut, int nx, double *xbins,
      vector<double> *lerr = 0, vector<double> *herr = 0);

   /**
      Calculates the 1D systematic uncertainty for the data samples.
      
      \param rdp  The real data sample for which systematcis are to be calculated.
      \param mcp  The MC data sample for which systematcis are to be calculated.
      \param var  The binning variable.
      \param signal  The signal.
      \param cut  The cut.
      \param nx   The number of bins.
      \param xbins   The bin boundaries.
      \param opt  Root plotting options
      \param leg  The legend.
   */
   void CalculateSystematic(DataSample& rdp, DataSample& mcp, const string& var,
      const string& signal, const string& cut, int nx, double* xbins,
      const string& opt = "", const string& leg = "");

   /**
      Draws a histogram.

      \param histogram  The histogram to be drawn.
      \param options Root plotting options.
      \param legend  The legend.
   */
   void DrawPlot(TH1* histogram, const std::string& opt,
      const std::string& leg);

   void SetTitleZ(const std::string& titleZ){ _titleZ=titleZ; }
   void SetMin(double min){ _min = min; }
   void SetMax(double max){ _max = max; }
   void SetRange(double min, double max){ _min = min; _max = max; }
   void ApplyRange(bool range){ _range = range; }

protected:
   std::string _titleZ;
   bool _range;
   double _min;
   double _max;
};

#endif
