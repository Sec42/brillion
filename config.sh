#!/bin/sh

id='$Id: config.sh,v 1.3 2003/03/15 02:02:20 sec Exp $'

echo ''
echo '*** Welcome to the configuration checker for brillion (V0.1)'
echo ''


if [ "$1" = "rerun" ] ; then
	shift
	if [ -r .config ] ; then
		echo -n 
	fi
fi

### Read commandline options

while [ $# -gt 0 ] ; do
	case $1 in
	--help|help|-h) echo "No help.";exit 1;;
	profile) 	PROFILE=yes;;
	sound)		SOUND=yes;;
	optimize) 	OPTIMIZE=yes;;
	pedantic)	PEDANTIC=yes;;
	*)		echo "Unknown option $1";exit 1;;
	esac
	shift
done

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

### End of checks, write .config

:>.config
for a in SDL_CONFIG PROFILE SOUND OPTIMIZE PEDANTIC; do
eval "[ -z "\$$a" ] || echo \"$a=\$$a\"" >>.config
done

echo ''
echo '*** Configuration sucessfully created'
echo ''
