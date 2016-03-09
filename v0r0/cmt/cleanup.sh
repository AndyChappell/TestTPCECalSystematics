if test "${CMTROOT}" = ""; then
  CMTROOT=/storage/epp2/t2k/software/CMT/v1r20p20081118; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then tempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt cleanup -sh -pack=TPCECalSystematicsAnalysis -version=v1r15 -path=/home/epp/phrdqd/ND280/v11r31p5/highland2Systematics $* >${tempfile}; . ${tempfile}
/bin/rm -f ${tempfile}

