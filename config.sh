#!/bin/sh

id='$Id: config.sh,v 1.2 2003/03/15 01:14:38 sec Exp $'

echo ''
echo '*** Welcome to the configuration checker for brillion (V0.1)'
echo ''


### Check for old options

if [ -r .config ] ; then
	echo -n 
fi

### Check for sdl-config

echo -n "looking for sdl-config ... "

[ -z "$SDL_CONFIG" ] && {
sdl11-config --version && SDL_CONFIG=sdl11-config
sdl-config   --version && SDL_CONFIG=sdl-config
} >/dev/null 2>&1

if [ -z "$SDL_CONFIG" ] ; then
	echo Not found
	exit 1
else
	echo $SDL_CONFIG
fi

### End of checks

cat <<EOM >.config
SDL_CONFIG=$SDL_CONFIG
EOM

echo ''
echo '*** Configuration sucessfully created'
echo ''
