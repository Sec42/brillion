#!/bin/sh

id='$Id: config.sh,v 1.7 2004/06/14 22:43:31 sec Exp $'

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
	profile) 	PROFILE=yes;;
	sound)		SOUND=yes;;
	optimize) 	OPTIMIZE=yes;;
	pedantic)	PEDANTIC=yes;;
	devel)		DEVEL=yes;;
	save)		SAVE=yes;;
	windows)	WINDOWS=yes;;
	--help|help|-h) 
		echo "Usage: ./config.sh [options]"
		echo "	profile		Compiles a binary with profiling support"
		echo "	optimize	Compiles a more optimized binary"
		echo "	pedantic	Turns on a lot of Warnings"
		echo ""
		echo "	sound		Turns on sound support"
		echo "	devel		devel support (semi-cheats)"
		echo "	windows		add windows icon/resources"
		echo ""
		exit 1;;
	*)		echo "Error: Unknown option $1";exit 1;;
	esac
	echo "Option $1 enabled"
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

for a in DEVEL SAVE; do
	eval "[ -z "\$$a" ] || echo \"CFLAGS+=-D$a\"" >>.config
done

echo ''
echo '*** Configuration sucessfully created'
echo ''
