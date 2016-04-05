#include "Particle.hxx"

namespace TPCECalSystematics
{

Particle::Particle(const std::string& name, const int pdg):
   _name(name), _antiname(name + "bar"), _pdg(pdg)
{
}

Particle::Particle(const Particle& particle):
   _name(particle._name), _antiname(particle._antiname), _pdg(particle._pdg)
{
}

Particle& Particle::operator=(const Particle& particle)
{
   _name = particle._name;
   _antiname = particle._antiname;
   _pdg = particle._pdg;

   return *this;
}

Particle::~Particle()
{
}

const std::string& Particle::GetName(bool isAntiparticle) const
{
   return (isAntiparticle ? _antiname : _name);
}

const int Particle::GetPDG(bool isAntiparticle) const
{
   return (isAntiparticle ? -_pdg : _pdg);
}

}
