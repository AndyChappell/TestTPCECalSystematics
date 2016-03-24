#ifndef DrawingToolsTPCECal_h
#define DrawingToolsTPCECal_h

#include "DrawingTools.hxx"
#include "TMultiGraph.h"

double GetBinomialUncertainty(double numer, double denom);
double GetSystematic(double rdpEfficiency, double mcpEfficiency,
   double rdpError, double mcpError);

class DrawingToolsTPCECal: public DrawingTools
{
public:
   DrawingToolsTPCECal(const std::string& file = "", bool useT2Kstyle = true);
   DrawingToolsTPCECal(Experiment& exp, bool useT2Kstyle = true);
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
      \param histogram  The histogram to be filled.
      \param options Root plotting options.
      \param legend  The legend.
      \param errors  Any errors to be applied.
   */
   void PlotEfficiency(DataSample& data, const std::string& variable,
      const std::string& signal, const std::string& cut, const int numBins,
      double* bins, TH1F& histogram, const std::string& options = "",
      const std::string& legend = "", std::vector<double>* errors = 0);

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
      \param lerr The lower errors.
      \param herr The higher errors.
      \return  A vector of the efficiencies.
   */
   std::vector<double> GetEfficiency(DataSample& data,
      const std::string& variable, const std::string& signal,
      const std::string& cut, int numBins, double* bins,
      std::vector<double>* lerr = 0, std::vector<double>* herr = 0);

   /**
      Calculates the 1D systematic uncertainty for the data samples.
      
      \param rdp  The real data sample for which systematcis are to be calculated.
      \param mcp  The MC data sample for which systematcis are to be calculated.
      \param variable   The binning variable.
      \param signal  The signal.
      \param cut  The cut.
      \param numBins The number of bins.
      \param bins The bin boundaries.
      \param histogram  The histogram to be filled.
      \param options Root plotting options
      \param legend  The legend.
   */
   void PlotSystematic(DataSample& rdp, DataSample& mcp,
      const std::string& variables, const std::string& signal,
      const std::string& cut, int numBins, double* bins, TH1F& histogram,
      const std::string& options = "", const std::string& legend = "");

   /**
      Draws a histogram.

      \param histogram  The histogram to be drawn.
      \param options Root plotting options.
      \param legend  The legend.
   */
   void Plot(TH1& histogram, const std::string& options,
      const std::string& legend);

   /**
      Plots a graph with asymmetric errors.

      \param graph   The graph to be drawn.
      \param options Root plotting options.
      \param legend  The legend.
   */
   void Plot(TMultiGraph& graph, TGraphAsymmErrors& rdpGraph,
      TGraphAsymmErrors& mcpGraph, const std::string& options,
      const std::vector<std::string>& legend);

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
