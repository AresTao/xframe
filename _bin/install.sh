# set .bash_profile
PROFNAME=$HOME/.bash_profile
UNIDIR=$PWD
export UNIDIR
UNIDIRLINE="UNIDIR=$UNIDIR # xframe home dir"
LDLINE="LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:\$UNIDIR/app:\$UNIDIR/lib"

if [ `grep "$UNIDIRLINE" $PROFNAME | wc -l` -lt 1 ]
then
   echo >> $PROFNAME
   echo $UNIDIRLINE >> $PROFNAME
   echo export UNIDIR >> $PROFNAME
   echo PATH=\$PATH:\$UNIDIR/bin:\$UNIDIR >> $PROFNAME
   echo export PATH >> $PROFNAME
   echo $LDLINE >> $PROFNAME
   echo export LD_LIBRARY_PATH >> $PROFNAME
else
   if [ `grep "$LDLINE" $PROFNAME | wc -l` -lt 1 ]
   then
      echo $LDLINE >> $PROFNAME
      echo export LD_LIBRARY_PATH >> $PROFNAME
   fi
fi
