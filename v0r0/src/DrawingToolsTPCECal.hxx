#ifndef DrawingToolsTPCECal_h
#define DrawingToolsTPCECal_h

#include "DrawingTools.hxx"
#include "TMultiGraph.h"
#include "TGraphAsymmErrors.h"

double GetBinomialUncertainty(double numer, double denom);
double GetSystematic(double rdpEfficiency, double mcpEfficiency,
   double rdpError, double mcpError);
double GetSystematicUncertainty(double rdpEfficiency, double mcpEfficiency);
double GetSystematicError(double rdpEffErr, double mcpEffErr);

class DrawingToolsTPCECal: public DrawingTools
{
public:
   DrawingToolsTPCECal(const std::string& file = "", bool useT2Kstyle = true);
   DrawingToolsTPCECal(Experiment& exp, bool useT2Kstyle = true);
   virtual ~DrawingToolsTPCECal();

   /**
      Draws a 1D efficiency plot by bin.
      
      \param rdp  The real data sample for which efficiencies are to be calculated.
      \param mcp  The MC data sample for which efficiencies are to be calculated.
      \param variable   The binning variable.
      \param signal  The signal.
      \param cut  The cut.
      \param numBins The number of bins.
      \param bins   The bin boundaries.
      \param isAnti  Indicates whether the data is from anti-neutrino-mode running.
   */
   void PlotEfficiency(DataSample& rdp, DataSample& mcp,
      const std::string& variable, const std::string& signal,
      const std::string& cut, const int numBins, double* bins, bool isAnti);

   /**
      Draws a 1D efficiency plot combining nu mode and nubar mode samples, by
      bin.
      
      \param rdp
         The nu mode real data sample for which efficiencies are to be
         calculated.
      \param rdpbar
         The nubar mode real data sample for which efficiencies are to be
         calculated.
      \param mcp
         The nu mode MC data sample for which efficiencies are to be calculated.
      \param mcpbar
         The nubar mode MC data sample for which efficiencies are to be
         calculated.
      \param variable   The binning variable.
      \param signal  The signal.
      \param cut  The cut.
      \param numBins The number of bins.
      \param bins   The bin boundaries.
   */
   void PlotEfficiency(DataSample& rdp, DataSample& rdpbar, 
      DataSample& mcp, DataSample& mcpbar, const std::string& variable,
      const std::string& signal, const std::string& cut, const int numBins,
      double* bins);
   
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
      Gets the combined 1D matching efficiency of data samples. Statistical
      uncertainties are calculated using binomial statistics if the lowErrs and
      highErrs variables are not used, otherwise the low and high errors are
      filled from these vectors.
      
      \param data1   The first data sample.
      \param data2   The second data sample.
      \param variable   The binning variable.
      \param signal  The signal.
      \param cut  The cut.
      \param numBins The number of bins.
      \param bins   The bin boundaries.
      \param lerr The lower errors.
      \param herr The higher errors.
      \return  A vector of the efficiencies.
   */
   std::vector<double> GetEfficiency(DataSample& data1, DataSample& data2,
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
      \param options Root plotting options
      \param isAnti  Indicates whether the data is from anti-neutrino-mode running.
   */
   void PlotSystematic(DataSample& rdp, DataSample& mcp,
      const std::string& variables, const std::string& signal,
      const std::string& cut, int numBins, double* bins,
      const std::string& options = "", bool isAnti=false);

   /**
      Calculates the 1D systematic uncertainty for combined nu and nubar samples.
      
      \param nuRdp   The nu mode real data sample for which systematcis are to be calculated.
      \param nubarRdp   The nubar mode real data sample for which systematcis are to be calculated.
      \param nuMcp   The nu mode MC data sample for which systematcis are to be calculated.
      \param nubarMcp   The nubar mode MC data sample for which systematcis are to be calculated.
      \param variable   The binning variable.
      \param signal  The signal.
      \param cut  The cut.
      \param numBins The number of bins.
      \param bins The bin boundaries.
      \param options Root plotting options
      \param legend  The legend.
   */
   void PlotSystematic(DataSample& nuRdp, DataSample& nubarRdp,
      DataSample& nuMcp, DataSample& nubarMcp,
      const std::string& variables, const std::string& signal,
      const std::string& cut, int numBins, double* bins,
      const std::string& options = "");

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
   /**
      Creates a graph with asymmetric errors from the data sample provided.
      
      \param data The data sample for which the graph is to be created.
      \param variable   The binning variable.
      \param signal  The signal.
      \param cut  The cut.
      \param n The number of bins.
      \param bins   The bin boundaries.
   */
   TGraphAsymmErrors* CreateEfficiencyGraph(DataSample& data,
      const std::string& variable, const std::string& signal,
      const std::string& cut, int n, double* bins);

   /**
      Creates a graph with asymmetric errors from the combined nu mode and nubar
      mode data samples provided.
      
      \param data
         The nu mode data sample for which the graph is to be created.
      \param antidata
         The nubar mode data sample for which the graph is to be created.
      \param variable
         The binning variable.
      \param signal
         The signal.
      \param cut
         The cut.
      \param n
         The number of bins.
      \param bins
         The bin boundaries.
   */
   TGraphAsymmErrors* CreateEfficiencyGraph(DataSample& data,
      DataSample& antidata, const std::string& variable,
      const std::string& signal, const std::string& cut, int n, double* bins);

   std::string _titleZ;
   bool _range;
   double _min;
   double _max;
   TMultiGraph* _multigraph;
   TH1F* _histogram1;
};

#endif
