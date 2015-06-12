#!/bin/sh

UTBIN=/opt/bin
LOG="/var/log/pfx.log"

UTOU="ou=uttoken,ou=utdata,o=fsr,dc=de"
UTSD="ou=utsession,ou=utdata,o=fsr,dc=de"

LDAP=ldap.fz-rossendorf.de
LDIF=/tmp/$$.ldif

echo "utupdate $*" >> $LOG 2>&1
echo "LDIF = $LDIF" >> $LOG 2>&1

export LD_LIBRARY_PATH=$UTBIN

RTYPE=$1
PID=$2
RIP=$3

# update or create session entry
SDN="cn=$PID,ou=utsession,ou=utdata,o=fsr,dc=de"
echo "dn: $SDN" > $LDIF

ldapsearch -x -h $LDAP -b $UTSD -LLL "(cn=$PID)" > $LDIF
grep "cn: $PID" $LDIF >> $LOG # > /dev/null
GR=$?

SDN="cn=$PID,ou=utsession,ou=utdata,o=fsr,dc=de"
echo "dn: $SDN" > $LDIF

sleep 2

if [ $GR = 0 ];
then
    HOST=`grep radiusLoginIPHost $LDIF | awk '{ print $2; }'`
    STYPE=`grep radiusServiceType $LDIF | awk '{ print $2; }'`
    echo "Session of type $STYPE exists on host $HOST" >> $LOG 2>&1
    if [ "$STYPE" = "RDP" ];
    then
	RIP=`netstat -anp | grep "$RPID/rdesktop" | grep tcp | grep ESTABLISHED | awk '{ print $5; }' | awk -F: '{ print $1; }'`
	echo "Opened RDP session to $HOST" >> $LOG
    fi
    echo "update session entry"
    echo "changetype: Modify" >> $LDIF

    echo "replace: radiusLoginTime radiusClientIPAddress" >> $LDIF

    echo -n "radiusLoginTime: " >> $LDIF
    date +"%Y-%m-%d %H:%M:%S" >> $LDIF

    echo -n "radiusClientIPAddress: " >> $LDIF
    hostname -i >> $LDIF

    ldapmodify -x -y $UTBIN/.pwd -h $LDAP -D "cn=manager,o=fsr,dc=de" -f $LDIF >> $LOG 2>&1
    echo "Updated session data for $SDN" >> $LOG
else
    if [ "$RTYPE" = "1" ];
    then
	RIP=`netstat -anp | grep "$RPID/rdesktop" | grep tcp | grep ESTABLISHED | awk '{ print $5; }' | awk -F: '{ print $1; }'`
	echo "Opened RDP session to $HOST" >> $LOG
    fi
    echo "create new session entry" >> $LOG 2>&1
    echo "objectClass: radiusObjectProfile" >> $LDIF
    echo "objectClass: radiusprofile" >> $LDIF
    echo "cn: $PID" >> $LDIF
    echo "radiusRealm: $PID@Payflex" >> $LDIF
    echo "radiusLoginIPHost: $RIP" >> $LDIF

    echo -n "radiusLoginTime: " >> $LDIF
    date +"%Y-%m-%d %H:%M:%S" >> $LDIF

    echo -n "radiusClientIPAddress: " >> $LDIF
    hostname -i >> $LDIF

    case $RTYPE in
	0)
	    echo "radiusServiceType: NX" >> $LDIF
	    ;;
	1)
	    echo "radiusServiceType: RDP" >> $LDIF
	    ;;
	2)
	    echo "radiusServiceType: APP" >> $LDIF
	    ;;
    esac
    ldapmodify -x -y $UTBIN/.pwd  -h $LDAP -D "cn=manager,o=fsr,dc=de" -a -f $LDIF >> $LOG 2>&1
    echo "Added session data for $SDN" >> $LOG
fi
    
#rm $LDIF
