// SPDX-License-Identifier: LGPL-2.1-or-later

#include <libbluray/bluray.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <inttypes.h>
#include <unistd.h>

#define OPTS "p:t:f:o:h"
#define STDOUT_STR "-"

#define FORMAT_TEXT   "text"
#define FORMAT_FFMPEG "ffmpeg"

enum FORMAT {
  TEXT    = 1,
  FFMPEG,
};

static void _usage(char *cmd)
{
  fprintf(stderr,
    "Usage: %s <bd path> -p <playlist number> [-f format] [-o outfile]\n"
    "       %s <bd path> -t <title number>    [-f format] [-o outfile]\n"
    "   Lists the chaprs of a playlist\n"
    "   -p N              - Playlist N. (overides if title is spesified as well)\n"
    "   -t N              - Title N. First title is 1.\n"
    "   -f <format>       - Format (text, ffmpeg)\n"
    "   -o <outfile>      - Destination (defualt -)\n"
    "   -h                - This message\n",
    cmd , cmd
  );
  exit(1);
}

static void _dump_chapters_ffmpeg(const BLURAY_TITLE_INFO *ti)
{
  int cc, ii;
  cc = ti->chapter_count;
  if (ti->chapters) {
    printf(
      ";FFMETADATA1\n"
      "bd_playlist=%05d.mpls",
      ti->playlist
    );
    printf("\n\n");

    for (ii = 0; ii < cc; ii++) {
      BLURAY_TITLE_CHAPTER tc = ti->chapters[ii];
      printf(
        "[CHAPTER]\n"
        "TIMEBASE=1/90000\n"
        "START=%lu\n"
        "END=%lu\n"
        "title=Chapter %d",
        tc.start,
        (tc.start + tc.duration),
        (tc.idx + 1)
      );
      printf("\n\n");
    }
  }
}

static void _dump_chapters_text(const BLURAY_TITLE_INFO *ti)
{
  int cc, ii;
  cc = ti->chapter_count;
  printf(
  "playlist: %05d.mpls\n",
    ti->playlist
  );
  if (ti->chapters) {
    for (ii = 0; ii < cc; ii++) {
      BLURAY_TITLE_CHAPTER tc = ti->chapters[ii];
      printf(
      "number: %3d\tstart: %lums\tduration: %lums\toffset: %luB",
        tc.idx + 1,  // human readable (lol please don't overflow)
        (tc.start    / 90),
        (tc.duration / 90),
        tc.offset
      );
      printf("\n");
    }
  }
}

int main(int argc, char *argv[])
{
  // VARS
  BLURAY *bd;
  FILE * appout = stdout;
  int opt;
  unsigned int title_count;
  unsigned int title_idx     = 0;
  unsigned int title_num     = 0;
  unsigned int playlist_num  = 0;
  char *bd_dir      = NULL;
  char *outfilepath = STDOUT_STR;

  enum FORMAT fmt = TEXT;

  // ARGS
  do
  {
    opt = getopt(argc, argv, OPTS);
    switch (opt)
    {
      case -1:
        if (optind < argc && bd_dir == NULL)
        {
          bd_dir = argv[optind];
          optind++;
          opt = 1;
        }
        break;
      case 'p':
        playlist_num  = strtol(optarg, NULL, 0);
        break;
      case 't':
        title_num     = strtol(optarg, NULL, 0);
        title_idx     = title_num - 1;
        break;
      case 'o':
        outfilepath = optarg;
        break;
      case 'f':
        if (strcmp(optarg, FORMAT_TEXT) == 0) {
          fmt = TEXT;
        } else if (strcmp(optarg, FORMAT_FFMPEG) == 0) {
          fmt = FFMPEG;
        } else {
          fprintf(stderr, "Unkown format: %s\n", optarg);
          _usage(argv[0]);
          return 1;
        }
        break;
      case 'h':
      default:
        _usage(argv[0]);
        break;
      }
  } while (opt != -1);

  // INIT
  // // outfile
  if (strcmp(outfilepath, STDOUT_STR) != 0) {
    appout = freopen(outfilepath,"w",stdout);
  }
  if ( appout == NULL ) {
    perror("Could not open file!");
    return 1;
  }

  // // bd_dir
  if (bd_dir == NULL)
  {
    _usage(argv[0]);
    return 1;
  }

  // // bd_dir open
  bd = bd_open(bd_dir, NULL);
  if (!bd) {
    fprintf(stderr, "bd_open(%s) failed\n", bd_dir);
    return 1;
  }

  // TITLE INFO
  BLURAY_TITLE_INFO* ti;
  title_count = bd_get_titles(bd, TITLES_ALL, 0);
  if (title_count <= 0) {
    fprintf(stderr, "No titles found in: %s\n", bd_dir);
    return 1;
  }

  // PLAYLIST / TITLE
  if (playlist_num > 0) {
    ti = bd_get_playlist_info(bd, playlist_num, 0);
  } else {
    ti = bd_get_title_info(bd, title_idx, 0);
  };

  // SWITCH FORMAT
  switch (fmt)
  {
    case TEXT:
      _dump_chapters_text(ti);
      break;
    case FFMPEG:
      _dump_chapters_ffmpeg(ti);
      break;
    default:
      fprintf(stderr, "FMT not set? You get a cookie.\n");
      break;
  };

  // CLOSE / FREE
  bd_free_title_info(ti);
  bd_close(bd);
  fclose(appout);

  // OK
  return 0;
}
