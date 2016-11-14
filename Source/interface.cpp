/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * This is Brown, a simple go program.                           *
 *                                                               *
 * Copyright 2003 and 2004 by Gunnar Farneb‰ck.                  *
 *                                                               *
 * Permission is hereby granted, free of charge, to any person   *
 * obtaining a copy of this file gtp.c, to deal in the Software  *
 * without restriction, including without limitation the rights  *
 * to use, copy, modify, merge, publish, distribute, and/or      *
 * sell copies of the Software, and to permit persons to whom    *
 * the Software is furnished to do so, provided that the above   *
 * copyright notice(s) and this permission notice appear in all  *
 * copies of the Software and that both the above copyright      *
 * notice(s) and this permission notice appear in supporting     *
 * documentation.                                                *
 *                                                               *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY     *
 * KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE    *
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR       *
 * PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS. IN NO      *
 * EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS INCLUDED IN THIS  *
 * NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT OR    *
 * CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING    *
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF    *
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT    *
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS       *
 * SOFTWARE.                                                     *
 *                                                               *
 * Except as contained in this notice, the name of a copyright   *
 * holder shall not be used in advertising or otherwise to       *
 * promote the sale, use or other dealings in this Software      *
 * without prior written authorization of the copyright holder.  *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

#include "NoBB.h"
#include "gtp.h"
#include "STDatasetGenerator.h"
#include "STPredictor.h"

/* generate ST dataset*/
static void generate_STDataset();

/* train crf model */
static void train_crf();

/* Forward declarations. */
static int gtp_protocol_version(char *s);
static int gtp_name(char *s);
static int gtp_version(char *s);
static int gtp_known_command(char *s);
static int gtp_list_commands(char *s);
static int gtp_quit(char *s);
static int gtp_boardsize(char *s);
static int gtp_clear_board(char *s);
static int gtp_komi(char *s);
static int gtp_fixed_handicap(char *s);
static int gtp_place_free_handicap(char *s);
static int gtp_set_free_handicap(char *s);
static int gtp_play(char *s);
static int gtp_genmove(char *s);
static int gtp_final_score(char *s);
static int gtp_final_status_list(char *s);
static int gtp_showboard(char *s);
static int gtp_prob(char *s);


/* List of known commands. */
static struct gtp_command commands[] = {
  {"protocol_version",    gtp_protocol_version},
  {"name",                gtp_name},
  {"version",             gtp_version},
  {"known_command",    	  gtp_known_command},
  {"list_commands",    	  gtp_list_commands},
  {"quit",             	  gtp_quit},
  {"boardsize",        	  gtp_boardsize},
  {"clear_board",      	  gtp_clear_board},
  {"komi",        	  gtp_komi},
  {"fixed_handicap",   	  gtp_fixed_handicap},
  {"place_free_handicap", gtp_place_free_handicap},
  {"set_free_handicap",   gtp_set_free_handicap},
  {"play",            	  gtp_play},
  {"genmove",             gtp_genmove},
  {"final_score",         gtp_final_score},
  {"final_status_list",   gtp_final_status_list},
  {"showboard",        	  gtp_showboard},
  {"move_probabilities",  gtp_prob},
  {NULL,                  NULL}
};

NoBB *my_nobb;

int main(int argc, char **argv)
{
	/* Train CRF model*/
	if (true) {
		train_crf();
		return 0;
	}

	/* ST DATASET GENERATOR*/
	if (GENERATE_STDATASET)
	{
		generate_STDataset();
		return 0;
	}

    unsigned int random_seed = 1;

    /* Optionally a random seed can be passed as an argument to the program. */
    if (argc > 1)
        sscanf(argv[1], "%u", &random_seed);
    srand(random_seed);

    /* Make sure that stdout is not block buffered. */
    setbuf(stdout, NULL);

    /* create our nobb */
    my_nobb = new NoBB();
    /* Inform the GTP utility functions about the initial board size. */
    gtp_internal_set_boardsize(my_nobb->board_size);

    /* Initialize the board. */
    my_nobb->init_brown();

    /* Process GTP commands. */
    gtp_main_loop(commands, stdin, NULL);

    return 0;
}

/* generate ST dataset*/
static void generate_STDataset() {
	int startSteps[] = {5,10,15,20,25,30,35,40};
	for (int startStep : startSteps) {
		int color = BLACK;
		int monte_carlo_num = 32;
		STDatasetGenerator(startStep, color, monte_carlo_num);
		printf("finished\n");
	}
	getchar();
}

/* train crf model */
static void train_crf() {
	STPredictor stp = STPredictor();
	getchar();
	return;


}


/* We are talking version 2 of the protocol. */
static int gtp_protocol_version(char *s)
{
  return gtp_success("2");
}

static int gtp_name(char *s)
{
  return gtp_success(NoBB_NAME);
}

static int gtp_version(char *s)
{
  return gtp_success(NoBB_VERSION);
}

static int gtp_known_command(char *s)
{
  int i;
  char command_name[GTP_BUFSIZE];

  /* If no command name supplied, return false (this command never
   * fails according to specification).
   */
  if (sscanf(s, "%s", command_name) < 1)
    return gtp_success("false");

  for (i = 0; commands[i].name; i++)
    if (!strcmp(command_name, commands[i].name))
      return gtp_success("true");

  return gtp_success("false");
}

static int gtp_list_commands(char *s)
{
  int i;

  gtp_start_response(GTP_SUCCESS);

  for (i = 0; commands[i].name; i++)
    gtp_printf("%s\n", commands[i].name);

  gtp_printf("\n");
  return GTP_OK;
}

static int gtp_quit(char *s)
{
  gtp_success("");
  return GTP_QUIT;
}

static int gtp_boardsize(char *s)
{
  int boardsize;

  if (sscanf(s, "%d", &boardsize) < 1)
    return gtp_failure("boardsize not an integer");

  if (boardsize < MIN_BOARD || boardsize > MAX_BOARD)
    return gtp_failure("unacceptable size");

  my_nobb->board_size = boardsize;
  gtp_internal_set_boardsize(boardsize);
  my_nobb->init_brown();

  return gtp_success("");
}

static int gtp_clear_board(char *s)
{
  my_nobb->clear_board();
  return gtp_success("");
}

static int gtp_komi(char *s)
{
  if (sscanf(s, "%f", &my_nobb->komi) < 1)
    return gtp_failure("komi not a float");

  return gtp_success("");
}

/* Common code for fixed_handicap and place_free_handicap. */
static int place_handicap(char *s, int fixed)
{
  int handicap;
  int m, n;
  int first_stone = 1;

  if (!my_nobb->board_empty())
    return gtp_failure("board not empty");

  if (sscanf(s, "%d", &handicap) < 1)
    return gtp_failure("handicap not an integer");

  if (handicap < 2)
    return gtp_failure("invalid handicap");

  if (fixed && !my_nobb->valid_fixed_handicap(handicap))
    return gtp_failure("invalid handicap");

  if (fixed)
    my_nobb->place_fixed_handicap(handicap);
  else
    my_nobb->place_free_handicap(handicap);

  gtp_start_response(GTP_SUCCESS);
  for (m = 0; m < my_nobb->board_size; m++)
    for (n = 0; n < my_nobb->board_size; n++)
      if (my_nobb->get_board(m, n) != EMPTY) {
	if (first_stone)
	  first_stone = 0;
	else
	  gtp_printf(" ");
	gtp_mprintf("%m", m, n);
      }
  return gtp_finish_response();
}

static int gtp_fixed_handicap(char *s)
{
  return place_handicap(s, 1);
}

static int gtp_place_free_handicap(char *s)
{
  return place_handicap(s, 0);
}

static int gtp_set_free_handicap(char *s)
{
  int i, j;
  int n;
  int handicap = 0;

  if (!my_nobb->board_empty())
    return gtp_failure("board not empty");

  while ((n = gtp_decode_coord(s, &i, &j)) > 0) {
    s += n;

    if (my_nobb->get_board(i, j) != EMPTY) {
      my_nobb->clear_board();
      return gtp_failure("repeated vertex");
    }

    my_nobb->play_move(i, j, BLACK);
    handicap++;
  }

  if (sscanf(s, "%*s") != EOF) {
      my_nobb->clear_board();
      return gtp_failure("invalid coordinate");
  }

  if (handicap < 2 || handicap >= my_nobb->board_size * my_nobb->board_size) {
      my_nobb->clear_board();
      return gtp_failure("invalid handicap");
  }

  return gtp_success("");
}

static int gtp_play(char *s)
{
  int i, j;
  int color = EMPTY;

  if (!gtp_decode_move(s, &color, &i, &j))
    return gtp_failure("invalid color or coordinate");

  if (!my_nobb->legal_move(i, j, color))
    return gtp_failure("illegal move");

  my_nobb->play_move(i, j, color);
  return gtp_success("");
}

static int gtp_genmove(char *s)
{
    int move;
	int board_size = my_nobb->board_size;
    int color = EMPTY;

    if (!gtp_decode_color(s, &color))
    return gtp_failure("invalid color");

    move = my_nobb->generate_move(color);
    my_nobb->play_move(I(move), J(move), color);

    gtp_start_response(GTP_SUCCESS);
    gtp_mprintf("%m", I(move), J(move));

    return gtp_finish_response();
}

/* Compute final score. We use area scoring since that is the only
 * option that makes sense for this move generation algorithm.
 */
static int gtp_final_score(char *s)
{
  float score = my_nobb->komi;
  int i, j;

  my_nobb->compute_final_status();
  for (i = 0; i < my_nobb->board_size; i++)
    for (j = 0; j < my_nobb->board_size; j++) {
      int status = my_nobb->get_final_status(i, j);
      if (status == BLACK_TERRITORY)
	score--;
      else if (status == WHITE_TERRITORY)
	score++;
      else if ((status == ALIVE) ^ (my_nobb->get_board(i, j) == WHITE))
	score--;
      else
	score++;
    }

  if (score > 0.0)
    return gtp_success("W+%3.1f", score);
  if (score < 0.0)
    return gtp_success("B+%3.1f", -score);
  return gtp_success("0");
}

static int gtp_final_status_list(char *s)
{
  int n;
  int i, j;
  int status = UNKNOWN;
  char status_string[GTP_BUFSIZE];
  int first_string;

  if (sscanf(s, "%s %n", status_string, &n) != 1)
    return gtp_failure("missing status");

  if (!strcmp(status_string, "alive"))
    status = ALIVE;
  else if (!strcmp(status_string, "dead"))
    status = DEAD;
  else if (!strcmp(status_string, "seki"))
    status = SEKI;
  else
    return gtp_failure("invalid status");

  my_nobb->compute_final_status();

  gtp_start_response(GTP_SUCCESS);

  first_string = 1;
  for (i = 0; i < my_nobb->board_size; i++)
    for (j = 0; j < my_nobb->board_size; j++)
      if (my_nobb->get_final_status(i, j) == status) {
	int k;
	int stonei[MAX_BOARD * MAX_BOARD];
	int stonej[MAX_BOARD * MAX_BOARD];
	int num_stones = my_nobb->get_string(i, j, stonei, stonej);
	/* Clear the status so we don't find the string again. */
	for (k = 0; k < num_stones; k++)
	  my_nobb->set_final_status(stonei[k], stonej[k], UNKNOWN);

	if (first_string)
	  first_string = 0;
	else
	  gtp_printf("\n");

	gtp_print_vertices(num_stones, stonei, stonej);
      }

  return gtp_finish_response();
}

/* Write a row of letters, skipping 'I'. */
static void letters(void)
{
  int i;

  printf("  ");
  for (i = 0; i < my_nobb->board_size; i++)
    printf(" %c", 'A' + i + (i >= 8));
}

static int gtp_showboard(char *s)
{
    int i, j;
    int symbols[3] = {'.', 'O', 'L'};

    gtp_start_response(GTP_SUCCESS);
    gtp_printf("\n  ");

    letters();

    for (i = 0; i < my_nobb->board_size; i++) {
        printf("\n%2d", my_nobb->board_size - i);
        for (j = 0; j < my_nobb->board_size; j++)
          printf(" %c", symbols[my_nobb->get_board(i, j)]);
        printf(" %d", my_nobb->board_size - i);
    }

    printf("\n");
    letters();

    return gtp_finish_response();
}

static int gtp_prob(char *s){
	int i, j;
	gtp_start_response(GTP_SUCCESS);

	gtp_printf("\n  ");
	for (i = 0; i < my_nobb->board_size; ++i){
		for (j = 0; j < my_nobb->board_size; ++j){
			gtp_printf("%c%d  %.f\n", 'A' + j + (j >= 8), my_nobb->board_size - i, my_nobb->final_board[my_nobb->board_size*i+j]);
			// gtp_printf("%c%d  %d\n", 'A' + j + (j >= 8), my_nobb->board_size - i, 
			//		my_nobb->rational_move(i, j, WHITE));
		}
	}

	return gtp_finish_response();
}

