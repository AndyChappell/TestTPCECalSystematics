#include <cassert>
#include "Bins.hxx"

namespace TPCECalSystematics
{

Bins::Bins(const double* boundaries, const uint n): _boundaries(0), _n(n)
{
   if(_n > 0)
   {
      _boundaries = new double[_n + 1];
      for(uint i = 0; i <= _n; ++i)
      {
         _boundaries[i] = boundaries[i];
      }
   }
}

Bins::Bins(const Bins& bins): _boundaries(0), _n(bins._n)
{
   if(_n > 0)
   {
      _boundaries = new double[_n + 1];
      for(uint i = 0; i <= _n; ++i)
      {
         _boundaries[i] = bins._boundaries[i];
      }
   }
}

Bins& Bins::operator=(const Bins& bins)
{
   delete[] _boundaries;
   _n = bins._n;
   if(_n > 0)
   {
      _boundaries = new double[_n + 1];
      for(uint i = 0; i <= _n; ++i)
      {
         _boundaries[i] = bins._boundaries[i];
      }
   }

   return *this;
}

double& Bins::operator[](const uint n)
{
   assert(n <= _n);
   return _boundaries[n];
}

Bins::~Bins()
{
   delete[] _boundaries;
}

uint Bins::GetNumBins() const
{
   return _n;
}

double* Bins::GetBoundaries()
{
   return _boundaries;
}

}
