#!/bin/sh

id='$Id: config.sh,v 1.1 2003/03/14 19:12:07 sec Exp $'

echo ''
echo '*** Welcome to the configuration checker for brillion (V0.1)'
echo ''

echo -n "looking for sdl-config ... "

SDL_CONFIG=`whereis -bq sdl-config sdl11-config|head -1`

if [ -z "$SDL_CONFIG" ] ; then
	echo Not found
	exit 1
else
	echo $SDL_CONFIG
fi

cat <<EOM >.config
SDL_CONFIG=$SDL_CONFIG
EOM

echo ''
echo '*** Configuration sucessfully created'
echo ''
