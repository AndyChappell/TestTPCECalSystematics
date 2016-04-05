#ifndef Detector_h
#define Detector_h

#include <string>

namespace TPCECalSystematics
{
class Detector
{
public:
   /**
      Constructs a Detector object.

      \param name
         The name of the detector. This is used in producing plot filenames.
      \param description
         The description for the detector. This is used in producing summary
         print outs.
      \param signal
         The microtree condition indicating that the selection for the detector
         was passed (e.g. "entersBarrel==1").
      \param cut
         The microtree condition indicating that a track was reconstructed with
         the relevant detector segment (e.g. "ecalDetector==6").
   */
   Detector(const std::string& name, const std::string& description,
      const std::string& signal, const std::string& cut);

   /**
      Copies the given Detector object.

      \param detector The object to be copied.
   */
   Detector(const Detector& detector);

   /**
      Assigns the state of the given Detector object to this Detector.

      \param detector   The object whose state is to be copied.
   */
   Detector& operator=(const Detector& detector);

   /**
      Destroys this Detector object.
   */
   virtual ~Detector();

   /**
      Returns the name of the detector.

      \return The name of the detector.
   */   
   const std::string& GetName() const;

   /**
      Returns the description of the detector.

      \return The description of the detector.
   */   
   const std::string& GetDescription() const;

   /**
      Returns the signal associated with the detector.

      \return The signal associated with the detector.
   */   
   const std::string& GetSignal() const;

   /**
      Returns the cut associated with the detector.

      \return The cut associated with the detector.
   */   
   const std::string& GetCut() const;

private:
   std::string _name;
   std::string _description;
   std::string _signal;
   std::string _cut;
};
}

#endif
