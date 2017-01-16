#!/bin/sh

UTBIN=/opt/bin
LOG="/var/log/pfx.log"

UTOU="ou=uttoken,ou=utdata,o=fsr,dc=de"
UTSD="ou=utsession,ou=utdata,o=fsr,dc=de"

LDAP=ldap.fz-rossendorf.de
LDIF=/tmp/$$.ldif

export LD_LIBRARY_PATH=$UTBIN

RTYPE=$1
PID=$2
HOST=$3
ETYPE=0

echo "UT_SESSIONTYPE=$RTYPE"
echo "UT_TOKEN=$PID"
echo "UT_HOSTNAME=$HOST"

#--- TRAP handling for clean exit --------------------
MPID=$$
CPID=0
trap 'echo "Interrupting session for $PID ($MPID) with child $CPID" >> $LOG 2>&1; $UTBIN/tlckill.sh >> $LOG 2&>1; $UTBIN/utupdate.sh 255 $PID >> $LOG 2>&1; ETYPE=1' INT TERM
#-----------------------------------------------------

if [ "$PID" != "" ];
then
    UNAME=`ldapsearch -x -h $LDAP -b $UTOU -LLL "(cn=$PID)" uid | grep uid | awk '{ print $2; }'`
    echo "Payflex.$PID, $UNAME" >> $LOG 2>&1

    # check for existing session
    ldapsearch -x -h $LDAP -b $UTSD -LLL "(cn=$PID)" > $LDIF
    grep "cn: $PID" $LDIF >> $LOG 2>&1
    GR=$?
    
    if [ $GR = 0 ] && [ "$RTYPE" = "reconnect" ];
    then
	HOST=`grep radiusLoginIPHost $LDIF | awk '{ print $2; }'`
	STYPE=`grep radiusServiceType $LDIF | awk '{ print $2; }'`
	case $STYPE in
	    NX) 
		APP="tlc"
		;;
	    RDP) 
		APP="xfreerdp"
		;;
	    APP) 
		APP="firefox"
		;;
        esac
	echo "Session of type $STYPE for Payflex.$PID exists on host $HOST" >> $LOG 2>&1
        $UTBIN/utupdate.sh $RTYPE $PID $HOST >> $LOG 2>&1
    else
	case $RTYPE in
	    0) 
		STYPE="NX"
		APP="tlc"
		;;
	    1) 
		STYPE="RDP"
		APP="xfreerdp"
		;;
	    2) 
		STYPE="APP"
		APP="firefox"
		;;
        esac
	echo "new session of type $STYPE for $PID on host $HOST" >> $LOG 2>&1
    fi
else
#    UNAME="kiosk"
    PID="`nodename -n`"
    case $RTYPE in
	0) 
	    #		STYPE="NX"
	    # for testing ...
	    STYPE="NX"
	    APP="tlc"
	    ;;
	1) 
	    STYPE="RDP"
	    APP="xfreerdp"
	    ;;
	2) 
	    STYPE="APP"
  	    APP="firefox"
	    ;;
    esac
    echo "new session of type $STYPE for $PID on host $HOST" >> $LOG 2>&1
fi

# execute some command here
echo "EXEC: $STYPE @ $HOST : $APP" # >> $LOG 2>&1

case $STYPE in
    RDP)
	PWDARG=`./utpwd $HOST $UNAME`

        if [ "$?" != "0" ];
        then
  	  PWD_UNAME=$(echo $PWDARG | cut -f1 -d~)
	  PWD_PWD=$(echo $PWDARG | cut -f2 -d~)

	  if [ "$PWD_UNAME" != "" ];
	  then
	      echo "xfreerdp -u $PWD_UNAME -p * -d FZR -f $HOST"
	      #	    xfreerdp -u "$PWD_UNAME" -p "$PWD_PWD" -d FZR -f $HOST & >> $LOG 2>&1
	  else
	      #	    xfreerdp                           -d FZR -f $HOST & >> $LOG 2>&1
	      echo ""
	  fi
	  CPID=$!
	else
	  CPID=-1
	fi
	;;
    NX)
	if [ "$UNAME" != "" ];
	then
	    UNP="-u $UNAME"
	else
	    UNP=""
	fi
	if [ "$3" != "password" ];
	then
	    echo "TLC:pubkey"
            # enable AUTOLOGIN=1 in config file!
	    /opt/thinlinc/bin/tlclient -x -h options,controlpanel $UNP -C /etc/tlc-pubkey.cfg $HOST & >> $LOG 2>&1
	    CPID=$!
	else
	    echo "TLC:password"
	    /opt/thinlinc/bin/tlclient -x -h options,controlpanel $UNP -C /etc/tlc-password.cfg $HOST & >> $LOG 2>&1
	    CPID=$!
	fi
        # it looks like login was o.k., remove user key
	;;
    APP)
	$APP & >> $LOG 2>&1
	CPID=$!
	;;
esac

if [ "$CPID" != "-1" ];
then

    # bad idead, should be based on connection established "signal"
    sleep 5
    rm -f /tmp/user.key

    echo "Childprocess started as $CPID over $MPID" >> $LOG 2>&1
    while [ `ps -p $CPID > /dev/null; echo $?` = 0 ]; do
	sleep 1
    done

    echo "exit type=$ETYPE" >> $LOG 2>&1

    if [ "$ETYPE" = "0" ];
    then 
	SDN="cn=$PID,ou=utsession,ou=utdata,o=fsr,dc=de"
	echo "Session $CPID closed, removing $SDN" >> $LOG 2>&1
	ldapdelete -x -y $UTBIN/.pwd -h $LDAP -D "cn=manager,o=fsr,dc=de" $SDN >> $LOG 2>&1
    fi

fi

exit 0
