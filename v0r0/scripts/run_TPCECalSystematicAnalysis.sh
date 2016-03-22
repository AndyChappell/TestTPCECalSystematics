#!/bin/bash
export INSTALLPATH=$HOME/ND280
export ND280VERSION=v11r31p5
export ND280PATH=$INSTALLPATH/$ND280VERSION
export CMTPATH=$ND280PATH
export TN228HOME=/storage/epp2/phrdqd/TN228

isTesting() {
   # For quick testing the TESTING environment variable should be set
   if [ "${TESTING}" ]; then
      retval=0
   else
      retval=1
   fi
   
   return $retval
}

# For quick testing the TESTING environment variable should be set
if isTesting; then
   export TESTDIR=testing/
else
   export TESTDIR=
fi

check() {
   if isTesting; then
      retval=0
   else
      echo -n "Testing mode is not set. This will delete your microtrees. Are you sure you want to proceed (Y/N)? "
      while read -r -n 1 -s answer; do
         if [[ $answer = [YyNn] ]]; then
            [[ $answer = [Yy] ]] && retval=0
            [[ $answer = [Nn] ]] && retval=1
            break
         fi
      done
   fi

   return $retval
}

if check; then
   echo; echo "Starting..."
else
   echo; echo "Aborting"
   kill -INT $$
fi

cd $TN228HOME
source $ND280PATH/highland2Systematics/TPCECalSystematicsAnalysis/v*/cmt/setup.sh

pids=""
param=${ND280PATH}/highland2Systematics/TPCECalSystematicsAnalysis/v0r0/parameters/TPCECalSystematicsAnalysis.e.parameters.dat
output=$TN228HOME/microtrees/${TESTDIR}rdp_e.root
input=$TN228HOME/input_files/${TESTDIR}neutrino_flattrees.list
log=$TN228HOME/logs/${TESTDIR}TestTPCECal_rdp_e.log
rm ${output}
RunTPCECalSystematicsAnalysis.exe -p ${param} -o ${output} ${input} > ${log} &

pids="$pids $!"

param=${ND280PATH}/highland2Systematics/TPCECalSystematicsAnalysis/v0r0/parameters/TPCECalSystematicsAnalysis.mu.parameters.dat
output=$TN228HOME/microtrees/${TESTDIR}rdp_mu.root
input=$TN228HOME/input_files/${TESTDIR}neutrino_flattrees.list
log=$TN228HOME/logs/${TESTDIR}TestTPCECal_rdp_mu.log
rm ${output}
RunTPCECalSystematicsAnalysis.exe -p ${param} -o ${output} ${input} > ${log} &

pids="$pids $!"

param=${ND280PATH}/highland2Systematics/TPCECalSystematicsAnalysis/v0r0/parameters/TPCECalSystematicsAnalysis.p.parameters.dat
output=$TN228HOME/microtrees/${TESTDIR}rdp_p.root
input=$TN228HOME/input_files/${TESTDIR}neutrino_flattrees.list
log=$TN228HOME/logs/${TESTDIR}TestTPCECal_rdp_p.log
rm ${output}
RunTPCECalSystematicsAnalysis.exe -p ${param} -o ${output} ${input} > ${log} &

pids="$pids $!"
wait $pids
pids=""
echo "nu mode rdp complete"

param=${ND280PATH}/highland2Systematics/TPCECalSystematicsAnalysis/v0r0/parameters/TPCECalSystematicsAnalysis.e.parameters.dat
output=$TN228HOME/microtrees/${TESTDIR}mcp_e.root
input=$TN228HOME/input_files/${TESTDIR}mcneutrino_flattrees.list
log=$TN228HOME/logs/${TESTDIR}TestTPCECal_mcp_e.log
rm ${output}
RunTPCECalSystematicsAnalysis.exe -p ${param} -o ${output} ${input} > ${log} &

pids="$pids $!"

param=${ND280PATH}/highland2Systematics/TPCECalSystematicsAnalysis/v0r0/parameters/TPCECalSystematicsAnalysis.mu.parameters.dat
output=$TN228HOME/microtrees/${TESTDIR}mcp_mu.root
input=$TN228HOME/input_files/${TESTDIR}mcneutrino_flattrees.list
log=$TN228HOME/logs/${TESTDIR}TestTPCECal_mcp_mu.log
rm ${output}
RunTPCECalSystematicsAnalysis.exe -p ${param} -o ${output} ${input} > ${log} &

pids="$pids $!"

param=${ND280PATH}/highland2Systematics/TPCECalSystematicsAnalysis/v0r0/parameters/TPCECalSystematicsAnalysis.p.parameters.dat
output=$TN228HOME/microtrees/${TESTDIR}mcp_p.root
input=$TN228HOME/input_files/${TESTDIR}mcneutrino_flattrees.list
log=$TN228HOME/logs/${TESTDIR}TestTPCECal_mcp_p.log
rm ${output}
RunTPCECalSystematicsAnalysis.exe -p ${param} -o ${output} ${input} > ${log} &

pids="$pids $!"
wait $pids
pids=""
echo "nu mode mcp complete"

param=${ND280PATH}/highland2Systematics/TPCECalSystematicsAnalysis/v0r0/parameters/TPCECalSystematicsAnalysis.ebar.parameters.dat
output=$TN228HOME/microtrees/${TESTDIR}rdp_ebar.root
input=$TN228HOME/input_files/${TESTDIR}antineutrino_flattrees.list
log=$TN228HOME/logs/${TESTDIR}TestTPCECal_rdp_ebar.log
rm ${output}
RunTPCECalSystematicsAnalysis.exe -p ${param} -o ${output} ${input} > ${log} &

pids="$pids $!"

param=${ND280PATH}/highland2Systematics/TPCECalSystematicsAnalysis/v0r0/parameters/TPCECalSystematicsAnalysis.mubar.parameters.dat
output=$TN228HOME/microtrees/${TESTDIR}rdp_mubar.root
input=$TN228HOME/input_files/${TESTDIR}antineutrino_flattrees.list
log=$TN228HOME/logs/${TESTDIR}TestTPCECal_rdp_mubar.log
rm ${output}
RunTPCECalSystematicsAnalysis.exe -p ${param} -o ${output} ${input} > ${log} &

pids="$pids $!"
wait $pids
pids=""
echo "nubar mode rdp complete"

param=${ND280PATH}/highland2Systematics/TPCECalSystematicsAnalysis/v0r0/parameters/TPCECalSystematicsAnalysis.ebar.parameters.dat
output=$TN228HOME/microtrees/${TESTDIR}mcp_ebar.root
input=$TN228HOME/input_files/${TESTDIR}mcantineutrino_flattrees.list
log=$TN228HOME/logs/${TESTDIR}TestTPCECal_mcp_ebar.log
rm ${output}
RunTPCECalSystematicsAnalysis.exe -p ${param} -o ${output} ${input} > ${log} &

pids="$pids $!"

param=${ND280PATH}/highland2Systematics/TPCECalSystematicsAnalysis/v0r0/parameters/TPCECalSystematicsAnalysis.mubar.parameters.dat
output=$TN228HOME/microtrees/${TESTDIR}mcp_mubar.root
input=$TN228HOME/input_files/${TESTDIR}mcantineutrino_flattrees.list
log=$TN228HOME/logs/${TESTDIR}TestTPCECal_mcp_mubar.log
rm ${output}
RunTPCECalSystematicsAnalysis.exe -p ${param} -o ${output} ${input} > ${log} &

pids="$pids $!"
wait $pids
echo "nubar mode mcp complete"

echo "Finished"


