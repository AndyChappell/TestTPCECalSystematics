#ifndef Bins_h
#define Bins_h

namespace TPCECalSystematics
{

typedef unsigned int uint;
class Bins
{
public:
   /**
      Constructs a Bin object from the specified boundaries with the given
      number of bins.

      \param boundaries The boundaries to be defined for the bins.
      \param n The number of bins.
   */
   Bins(const double* boundaries, const uint n);

   /**
      Copies the given Bin object.

      \param bins The object to be copied.
   */
   Bins(const Bins& bins);

   /**
      Assigns the state of the given Bins object to this bin.

      \param bins The object whose state is to be copied.
   */
   Bins& operator=(const Bins& bins);

   /**
      Accesses the nth index of the bin boundaries.

      \param n The index of the boundaries to access.
      \return The boundary at the nth index.
   */
   double& operator[](const uint n);

   /**
      Destroys this Bins object.
   */
   virtual ~Bins();

   /**
      Retrieves the number of bins.

      \return The number of bins.
   */
   uint GetNumBins() const;

   /**
      Retrieves the bin boundaries.

      \return The bin boundaries.
   */
   double* GetBoundaries();

private:
   double* _boundaries;
   uint _n;
};
}

#endif
