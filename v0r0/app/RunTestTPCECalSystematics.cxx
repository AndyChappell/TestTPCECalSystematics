#include "TestTPCECalSystematics.hxx"
#include "AnalysisLoop.hxx"

int main(int argc, char *argv[])
{
   TestTPCECalSystematics* ana = new TestTPCECalSystematics();
   AnalysisLoop loop(ana, argc, argv); 
   loop.Execute();
}
