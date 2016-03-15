#include "TPCECalSystematicsAnalysis.hxx"
#include "AnalysisLoop.hxx"

int main(int argc, char *argv[]){
   TPCECalSystematicsAnalysis* ana = new TPCECalSystematicsAnalysis();
   AnalysisLoop loop(ana, argc, argv); 
   loop.Execute();
}
