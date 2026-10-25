/*
 * OSMesa on the Galaxy S7's Mali-T880: osmesa_create_screen() (patched to
 * try this first) gets a Panfrost screen over the HeroGPU libdrm stand-in
 * (gpu/mesa/herodrm.c) instead of softpipe. HERO_GPU=0 keeps softpipe;
 * so does a kernel without HeroGPU.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "pipe/p_screen.h"
#include "util/driconf.h"
#include "util/macros.h"
#include "util/xmlconfig.h"
#include "panfrost/drm/panfrost_drm_public.h"
#include "xf86drm.h"

struct pipe_screen *hero_hw_create_screen(void);

static const driOptionDescription hero_driconf[] = {
#include "panfrost/driinfo_panfrost.h"
};

/* Panfrost reads its options from these for the screen's lifetime. */
static driOptionCache options, options_info;

struct pipe_screen *
hero_hw_create_screen(void)
{
   const char *env = getenv("HERO_GPU");
   if (env && env[0] == '0')
      return NULL;

   int fd = drmOpenWithType("panfrost", NULL, 0);
   if (fd < 0) {
      fprintf(stderr, "hero_hw: no HeroGPU, using softpipe\n");
      return NULL;
   }

   driParseOptionInfo(&options_info, hero_driconf, ARRAY_SIZE(hero_driconf));
   driParseConfigFiles(&options, &options_info, 0, "panfrost", NULL, NULL, NULL, 0,
                       NULL, 0);
   struct pipe_screen_config config = {
      .options = &options,
      .options_info = &options_info,
   };
   struct pipe_screen *screen = panfrost_drm_screen_create(fd, &config);
   /* The screen holds its own dup of the descriptor. */
   close(fd);
   if (!screen)
      fprintf(stderr, "hero_hw: Panfrost screen creation failed, using softpipe\n");
   return screen;
}
