#ifndef DrawingToolsTPCECal_h
#define DrawingToolsTPCECal_h

#include "DrawingTools.hxx"

class DrawingToolsTPCECal: public DrawingTools
{
public:
   DrawingToolsTPCECal(const string& file="", bool useT2Kstyle=true);
   DrawingToolsTPCECal(Experiment& exp, bool useT2Kstyle=true);
   virtual ~DrawingToolsTPCECal(){}

   /// Set the title in Z
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
