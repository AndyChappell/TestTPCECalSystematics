#include "Detector.hxx"

namespace TPCECalSystematics
{

Detector::Detector(const std::string& name, const std::string& description,
   const std::string& signal, const std::string& cut):
   _name(name), _description(description), _signal(signal), _cut(cut)
{
}

Detector::Detector(const Detector& detector):
   _name(detector._name), _description(detector._description),
   _signal(detector._signal), _cut(detector._cut)
{
}

Detector& Detector::operator=(const Detector& detector)
{
   _name = detector._name;
   _description = detector._description;
   _signal = detector._signal;
   _cut = detector._cut;

   return *this;
}

Detector::~Detector()
{
}

const std::string& Detector::GetName() const
{
   return _name;
}

const std::string& Detector::GetDescription() const
{
   return _description;
}

const std::string& Detector::GetSignal() const
{
   return _signal;
}

const std::string& Detector::GetCut() const
{
   return _cut;
}

}
