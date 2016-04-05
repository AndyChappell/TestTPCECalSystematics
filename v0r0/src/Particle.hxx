#ifndef Particle_h
#define Particle_h

#include <string>

namespace TPCECalSystematics
{
class Particle
{
public:
   /**
      Constructs a Particle object.

      \param name The name of the particle. This is used in producing plot filenames.
      \param pdg The Particle Data Group ID for the particle.
   */
   Particle(const std::string& name, const int pdg);

   /**
      Copies the given Particle object.

      \param particle The object to be copied.
   */
   Particle(const Particle& particle);

   /**
      Assigns the state of the given Particle object to this Particle.

      \param particle   The object whose state is to be copied.
   */
   Particle& operator=(const Particle& particle);

   /**
      Destroys this Particle object.
   */
   virtual ~Particle();

   /**
      Returns the name of the particle.
      
      \param isAntiparticle   Indicates if the antiparticle name should be returned.

      \return The name of the particle or antiparticle.
   */   
   const std::string& GetName(bool isAntiparticle=false) const;

   /**
      Returns the PDG id associated with the particle.

      \param isAntiparticle   Indicates if the antiparticle PDG ID should be returned.

      \return The PDG ID of the particle or antiparticle.
   */   
   const int GetPDG(bool isAntiparticle=false) const;

private:
   std::string _name;
   std::string _antiname;
   int _pdg;
};
}

#endif
