#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <sys/time.h>

typedef unsigned char u_char;
typedef unsigned long u_long;

u_long digest;

u_long ghistory;
u_long gdrought[] = {
    4, 4, 4, 4, 4, 4, 4
    };
    u_long gdriest = 0;
u_long gselection[] = {
    0, 0, 0, 0, 0,
    1, 1, 1, 1, 1,
    2, 2, 2, 2, 2,
    3, 3, 3, 3, 3,
    4, 4, 4, 4, 4,
    5, 5, 5, 5, 5,
    6, 6, 6, 6, 6,
    };

char unconvert(const unsigned char input) {
	switch(input & 0xff) {
		case 1: return 'I';
		case 2: return 'Z';
		case 4: return 'S';
		case 8: return 'J';
		case 16: return 'L';
		case 32: return 'O';
		case 64: return 'T';
	}
	return 'X';
}

int hash_forward (void)
{
	u_long magic = 0x41C64E6D;
	digest = (u_long) (digest * magic);
	digest += 12345;

	return (short) ((digest >> 10) & 0x7FFF);
}

int hash_backwards (void)
{
	digest -= 12345;
	u_long magic = 0xEEB9EB65;
	digest = (u_long) (digest * magic);

    return (short) ((digest >> 10) & 0x7FFF);
}

int get_next_piece (void)
{
	return (hash_forward () % 7);
}

int get_next_piece_Ti (void)
{
	return (hash_forward () % 35);
}

void tgm2_rand (void)
{
	int i;
	int piece;								// 0 - 6



	for (i = 0; i < 5; ++i)
	{

        piece = get_next_piece();
        if (!((0x01010101 << piece) & ghistory))
			break;
		piece = get_next_piece ();
	}
    ghistory = (ghistory << 8) | (1 << piece);
}

void tgm3_rand (void)
{
	int i, p;
	int piece;								// 0 - 6
	int piece_index;
	int glitch = 0;



	for (i = 0; i < 6; ++i)
	{

        piece_index = get_next_piece_Ti();
        piece = gselection[piece_index];
        if (!((0x01010101 << piece) & ghistory))
			break;
        gselection[piece_index] = gdriest;
		piece_index = get_next_piece_Ti();
		piece = gselection[piece_index];
	}
    ghistory = (ghistory << 8) | (1 << piece);
    int old_driest_size = gdrought[piece];
    gdrought[0]++;
    gdrought[1]++;
    gdrought[2]++;
    gdrought[3]++;
    gdrought[4]++;
    gdrought[5]++;
    gdrought[6]++;
    gdrought[piece]= 0;
    if (piece == gdriest) {
        if  (i > 0) glitch = 1;
        for (p = 0; p < 7; p++) {
            if (gdrought[p] > old_driest_size) glitch = 0;
            if (gdrought[p] > gdrought[gdriest]) gdriest = p;
        }
    }
    if (glitch == 0) {
        gselection[piece_index] = gdriest;
    }
}

void tgm2_rand_seed (const unsigned long initialseed)
{

	int piece;								// 0 - 6

	digest = (u_long) initialseed;		// replace with prng_rand function
	piece = get_next_piece ();
	while ((piece == 1) || (piece == 2) || (piece == 5))	// Z, S, or O pieces
	{
		piece = get_next_piece ();
	}

    ghistory = 0x04040200 | (1 << piece);
}

unsigned char convert(const char input) {
	switch (input) {
	case 'i':
	case 'I':
		return 0;
	case 'z':
	case 'Z':
		return 1;
	case 's':
	case 'S':
		return 2;
	case 'j':
	case 'J':
		return 3;
	case 'l':
	case 'L':
		return 4;
	case 'o':
	case 'O':
		return 5;
	case 't':
	case 'T':
		return 6;
	default:
    fprintf(stderr, "Sequence must only contain: I Z S J L O T\n");
    exit(2);
	return 0;
	}
}

int main(int argc, char **argv)
{
	int i = 0;
    u_long seed;
    u_long inithistory ;
    u_long history;
    unsigned short* sequence;


	if (argc != 2) {
		fprintf(stderr, "I need a search sequence as a parameter.\n");
		return 1;
	}

    time_t now = time(NULL);

	const short ln = strlen(argv[1]);

    seed = atoi(argv[1]);
    if(seed != 0)
        goto render;

	sequence = (unsigned short*)alloca(ln*sizeof(unsigned short));
	for (i = 0; i < ln; i++) sequence[i] = convert(argv[1][i]);
    if (sequence[0] == 1 || sequence[0] == 2 || sequence[0] == 5 ) {
        fprintf(stderr, "Sequence cannot start with: Z S O\n");
        return 1;
    }

	seed = sequence[0] << 10;
	inithistory = 0x04040200 | (1 << sequence[0]);
    history = inithistory;

	while (1) {
        digest = seed;
        u_long drought[7] = {
            4, 4, 4, 4, 4, 4, 4
        };
        u_long driest = 0;
        u_long selection[] = {
            0, 0, 0, 0, 0,
            1, 1, 1, 1, 1,
            2, 2, 2, 2, 2,
            3, 3, 3, 3, 3,
            4, 4, 4, 4, 4,
            5, 5, 5, 5, 5,
            6, 6, 6, 6, 6,
        };

        unsigned short p;
        unsigned short old_driest_size;
        unsigned short rerolled;
        unsigned short piece_index;
        unsigned short glitch;

		i = 1; {
            rerolled = 0;
            glitch = 0;
            piece_index = get_next_piece_Ti();
            if (!((0x01010101 << selection[piece_index]) & history)) goto chose_1;
            selection[piece_index] = driest;
            hash_forward();
            rerolled = 1;

            piece_index = get_next_piece_Ti();
            if (!((0x01010101 << selection[piece_index]) & history)) goto chose_1;
            selection[piece_index] = driest;
            hash_forward();

            piece_index = get_next_piece_Ti();
            if (!((0x01010101 << selection[piece_index]) & history)) goto chose_1;
            selection[piece_index] = driest;
            hash_forward();

            piece_index = get_next_piece_Ti();
            if (!((0x01010101 << selection[piece_index]) & history)) goto chose_1;
            selection[piece_index] = driest;
            hash_forward();

            piece_index = get_next_piece_Ti();
            if (!((0x01010101 << selection[piece_index]) & history)) goto chose_1;
            selection[piece_index] = driest;
            hash_forward();

            piece_index = get_next_piece_Ti();
            if (!((0x01010101 << selection[piece_index]) & history)) goto chose_1;
            selection[piece_index] = driest;
            piece_index = get_next_piece_Ti();

            chose_1:
			if (sequence[1] != selection[piece_index]) {
			    history = inithistory;
			    goto nextseed;
			}

            history = (history << 8) | (1 << selection[piece_index]);
            old_driest_size = drought[driest];
            drought[0]++;
            drought[1]++;
            drought[2]++;
            drought[3]++;
            drought[4]++;
            drought[5]++;
            drought[6]++;
            drought[selection[piece_index]]= 0;
            if (selection[piece_index] == driest) {
                if  (rerolled > 0) glitch = 1;
                for (p = 0; p < 7; p++) {
                    if (drought[p] > old_driest_size) glitch = 0;
                    if (drought[p] > drought[driest]) driest = p;
                }
            }
            if (glitch == 0) {
                selection[piece_index] = driest;
            }
		}
		for (i = 2; i < ln; i++) {

            rerolled = 0;
            glitch = 0;
            piece_index = get_next_piece_Ti();
            if (!((0x01010101 << selection[piece_index]) & history)) goto chose_i;
            selection[piece_index] = driest;
            hash_forward();
            rerolled = 1;

            piece_index = get_next_piece_Ti();
            if (!((0x01010101 << selection[piece_index]) & history)) goto chose_i;
            selection[piece_index] = driest;
            hash_forward();

            piece_index = get_next_piece_Ti();
            if (!((0x01010101 << selection[piece_index]) & history)) goto chose_i;
            selection[piece_index] = driest;
            hash_forward();

            piece_index = get_next_piece_Ti();
            if (!((0x01010101 << selection[piece_index]) & history)) goto chose_i;
            selection[piece_index] = driest;
            hash_forward();

            piece_index = get_next_piece_Ti();
            if (!((0x01010101 << selection[piece_index]) & history)) goto chose_i;
            selection[piece_index] = driest;
            hash_forward();

            piece_index = get_next_piece_Ti();
            if (!((0x01010101 << selection[piece_index]) & history)) goto chose_i;
            selection[piece_index] = driest;
            piece_index = get_next_piece_Ti();

            chose_i:
			if (sequence[i] != selection[piece_index]) {
			    history = inithistory;
			    goto nextseed;
			}

            history = (history << 8) | (1 << selection[piece_index]);
            old_driest_size = drought[driest];
            drought[0]++;
            drought[1]++;
            drought[2]++;
            drought[3]++;
            drought[4]++;
            drought[5]++;
            drought[6]++;
            drought[selection[piece_index]]= 0;
            if (selection[piece_index] == driest) {
                if  (rerolled > 0) glitch = 1;
                for (p = 0; p < 7; p++) {
                    if (drought[p] > old_driest_size) glitch = 0;
                    if (drought[p] > drought[driest]) driest = p;
                }
            }
            if (glitch == 0) {
                selection[piece_index] = driest;
            }
		}

        if (__builtin_expect(i == ln,0)) {
        	break;
        }

		nextseed:
		if ((seed & 0x3FF) == 0x3FF) seed += 0x1801;
        else seed++;

        if (__builtin_expect(seed & 0xFE000000,0)) {
            fprintf(stderr, "Running time: %d seconds (approx)\n", (int)(time(NULL) - now));
            fprintf(stderr, "No such sequence exists.\n");
            fprintf(stderr, "Last seed was %lu\n", seed);
            return 1;
        }
	}

    fprintf(stderr, "Running time: %d seconds (approx)\n", (int)(time(NULL) - now));

    digest = seed;
    hash_backwards();
    seed = digest;
render:
    fprintf(stderr, "Using seed %lu\n", seed);

    tgm2_rand_seed(seed);
	while (1) {
		char junk[10];
		for (i=0; i < 10; i++) {
			 fprintf(stdout, "%c", unconvert(ghistory));
             tgm3_rand();
		}
		fprintf(stdout, "\n");
		fgets(junk, sizeof(junk), stdin);
	}
	return 1;
}
