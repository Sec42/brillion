/* Do all the audible stuff
 * vim:set cin sm ts=8 sw=4 sts=4: - Sec <sec@42.org>
 * $Id: music.c,v 1.9 2004/06/14 22:18:00 sec Exp $
 */
#include "brillion.h"

music* init_music(){
  music* m;

  m=calloc(1,sizeof(music));
  if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY,MIX_DEFAULT_FORMAT,1,1024)){
    fprintf(stderr,"Could not initialize SDL/Audio: %s.\n", SDL_GetError());
    exit(-1);
  };
  Mix_AllocateChannels(4);

  /*
  bg=Mix_LoadMUS("Cymbals.mp3");
  Mix_PlayMusic(title, 1);
  */

  m->wall=Mix_LoadWAV("wall.wav");
  m->death=Mix_LoadWAV("death.wav");
  m->disk=Mix_LoadWAV("disk.wav");
  m->block=Mix_LoadWAV("block.wav");
  m->star=Mix_LoadWAV("star.wav");
  m->level=Mix_LoadWAV("level.wav");

  return m;
}

void play_touch(int piece){
    music *m = play->m;

/*  printf("Playing snd %d\n",piece); */
  switch(piece){
    case OUTER_WALL:
    case WALL:
      if(m->wall)
	Mix_PlayChannel(1,m->wall, 0);
      break;
    case DEATH:
      if(m->death)
	Mix_PlayChannel(1,m->death, 0);
      break;
    case DISK:
      if(m->disk)
	Mix_PlayChannel(1,m->disk, 0);
      break;
    case STAR:
      if(m->star)
	Mix_PlayChannel(1,m->star, 0);
      break;
    case BLOCK:
      if(m->block)
	Mix_PlayChannel(0,m->block, 0);
      break;
    case LEVEL:
      if(m->level)
	Mix_PlayChannel(0,m->level, 0);
      break;
    default:
      /*printf("Whoops, unknown sound %d?\n",piece); */
      ;
  };
}
