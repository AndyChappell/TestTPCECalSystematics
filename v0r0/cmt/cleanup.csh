if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /storage/epp2/t2k/software/CMT/v1r20p20081118
endif
source ${CMTROOT}/mgr/setup.csh
set tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set tempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt cleanup -csh -pack=TPCECalSystematicsAnalysis -version=v0r0 -path=/home/epp/phrdqd/ND280/v11r31p5/highland2Systematics $* >${tempfile}; source ${tempfile}
/bin/rm -f ${tempfile}

