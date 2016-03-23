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

source $ND280PATH/highland2Systematics/TPCECalSystematicsAnalysis/v*/cmt/setup.sh
cd $TN228HOME

# Set environment variables required by ROOT macros
export E_MCP_FILE=$TN228HOME/microtrees/${TESTDIR}mcp_e.root
export E_RDP_FILE=$TN228HOME/microtrees/${TESTDIR}rdp_e.root
export EBAR_MCP_FILE=$TN228HOME/microtrees/${TESTDIR}mcp_ebar.root
export EBAR_RDP_FILE=$TN228HOME/microtrees/${TESTDIR}rdp_ebar.root
export MU_MCP_FILE=$TN228HOME/microtrees/${TESTDIR}mcp_mu.root
export MU_RDP_FILE=$TN228HOME/microtrees/${TESTDIR}rdp_mu.root
export MUBAR_MCP_FILE=$TN228HOME/microtrees/${TESTDIR}mcp_mubar.root
export MUBAR_RDP_FILE=$TN228HOME/microtrees/${TESTDIR}rdp_mubar.root
export P_MCP_FILE=$TN228HOME/microtrees/${TESTDIR}mcp_p.root
export P_RDP_FILE=$TN228HOME/microtrees/${TESTDIR}rdp_p.root

