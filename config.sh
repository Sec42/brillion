#!/bin/sh

id='$Id: config.sh,v 1.9 2004/06/16 23:18:29 sec Exp $'

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
	nosound)	SOUND=no;;
	optimize) 	OPTIMIZE=yes;;
	pedantic)	PEDANTIC=yes;;
	devel)		DEVEL=yes;;
	save)		SAVE=yes;;
	windows)	WINDOWS=yes;;
	nowindows)	WINDOWS=no;;
	--help|help|-h) 
		echo "Usage: ./config.sh [options]"
		echo "	profile		Compiles a binary with profiling support"
		echo "	optimize	Compiles a more optimized binary"
		echo "	pedantic	Turns on a lot of Warnings"
		echo ""
		echo "	[no]sound	Forces sound support on or off"
		echo "	devel		devel support (semi-cheats)"
		echo "	[no]windows		add windows icon/resources"
		echo ""
		exit 1;;
	*)		echo "Error: Unknown option $1";exit 1;;
	esac
	echo "Option $1 enabled"
	shift
done

### Check for sdl-config

echo -n "looking for sdl-config ...      "

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

### Check for windows

if [ -z "$WINDOWS" ] ; then
echo -n "checking for windows extras ... "
WINDOWS=NO
case `uname` in 
CYGWIN*) WINDOWS=YES;;
esac
	echo $WINDOWS
fi

### Check for sound

if [ -z "$SOUND" ] ; then
echo -n "checking for sound lib ...      "
set -- `sdl-config --libs`
for a in $* ; do
case $a in
-L*) libpath=${a#-L};;
esac
done
if [ -z "$libpath" ] ; then 
SOUND=NO
else
if [ ! -f "$libpath/libSDL_mixer.a" ] ; then 
SOUND=NO
else
SOUND=YES
fi
fi

echo $SOUND
fi

### End of checks, write .config

[ "$SOUND" = "NO" ] && unset SOUND
[ "$WINDOWS" = "NO" ] && unset WINDOWS

:>.config
for a in SDL_CONFIG PROFILE SOUND OPTIMIZE PEDANTIC WINDOWS; do
	eval "[ -z "\$$a" ] || echo \"$a=\$$a\"" >>.config
done

for a in DEVEL SAVE; do
	eval "[ -z "\$$a" ] || echo \"CFLAGS+=-D$a\"" >>.config
done

echo ''
echo '*** Configuration sucessfully created'
echo ''
