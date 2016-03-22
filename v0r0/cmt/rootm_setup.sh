# This script sets up a .rootmap file in the directory that includes the highlandTools DrawingToolsTPCECal library.
# The .rootmap file means that ROOT will automatically load the required libraries, so calls like
# gSystem->Load("libhighlandTools.so") are not required in analysis macros.
#
# There is a slight caveat that OSX generates a .dylib file, rather that a .so file. The rlibmap
# utility assumes a .so file, so we must make a symlink between the two.
base=$TPCECALSYSTEMATICSANALYSISROOT/$TPCECALSYSTEMATICSANALYSISCONFIG 
base1=$HIGHLANDTOOLSROOT/$HIGHLANDTOOLSCONFIG 
base2=$PSYCHECOREROOT/$PSYCHECORECONFIG 
base3=$PSYCHEUTILSROOT/$PSYCHEUTILSCONFIG 

echo "rootm_setup"

echo $base
echo $base1
echo $base2
echo $base3

if [[ -d $base ]]; then
  output=$base/libDrawingToolsTPCECal.rootmap
  hl_lib="$base/libDrawingToolsTPCECal.so"
  root_libs="$base1/libhighlandTools.so $base2/libpsycheCore.so $base3/libpsycheUtils.so libRIO.so libHistPainter.so libMatrix.so libPhysics.so libTree.so libGeom.so" 
  includes="$TPCECALSYSTEMATICSANALYSISROOT/src/*_LinkDef.h"

  rlibmap -f -o $output -l $hl_lib -d $root_libs -c $includes

  if [[ $HIGHLANDTOOLSCONFIG == "Darwin" ]]; then
    ln -fs $base/libDrawingToolsTPCECal.dylib $base/libDrawingToolsTPCECal.so
    ln -fs $base1/libhighlandTools.dylib $base1/libhighlandTools.so
    ln -fs $base2/libpsycheCore.dylib $base2/libpsycheCore.so
    ln -fs $base3/libpsycheUtils.dylib $base3/libpsycheUtils.so
  fi
fi
