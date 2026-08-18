#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <raylib.h>
#include <math.h>
#include <assert.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define FONT_SIZE ((WINDOW_WIDTH / 100) * 4)

typedef enum PlayerTurn {
	TURN_BLUE = 10,
	TURN_RED,
} PlayerTurn;

typedef enum GameStates {
	STATE_MENU,
	STATE_RUNNING,
	STATE_BLUE_WON,
	STATE_RED_WON,
	STATE_DRAW,
} GameStates;

typedef struct Grid {
	unsigned int num_rows;
	unsigned int num_cols;
	unsigned char* grid;
} Grid;

typedef struct GameState {
	GameStates state;
	PlayerTurn turn;
	Grid grid;
} GameState;

GameState initGameState(void)
{
	GameState state = {0};
	state.state = STATE_MENU;

	state.grid.num_rows = 4;
	state.grid.num_cols = 4;
	assert(state.grid.num_cols == state.grid.num_rows);
	state.grid.grid = (unsigned char*) calloc(state.grid.num_rows * state.grid.num_cols, sizeof(state.grid.grid[0]));
	if (state.grid.grid == NULL) {
		perror("calloc()");
		exit(EXIT_FAILURE);
	}

	state.turn = TURN_BLUE;

	return state;
}

void updateStartMenu(GameState* state)
{
	if (IsKeyPressed(KEY_S)) {
		state->state = STATE_RUNNING;
	}
	else if (IsKeyPressed(KEY_T)) {
		switch (state->turn) {
		case TURN_BLUE: {
			state->turn = TURN_RED;
		} break;
		case TURN_RED: {
			state->turn = TURN_BLUE;
		} break;
		default: {
			fprintf(stderr, "Invalid turn (needed either blue or red)");
			exit(EXIT_FAILURE);
		} break;
		}
	}
}

void renderStartMenu(GameState* state)
{
	BeginDrawing();
	switch (state->turn) {
	case TURN_BLUE: {
		ClearBackground(BLUE);
	} break;
	case TURN_RED: {
		ClearBackground(RED);
	} break;
	default: {
		fprintf(stderr, "Invalid turn (needed either blue or red)");
		exit(EXIT_FAILURE);
	} break;
	}

	DrawText("Press 's' to start!", 10, 10, FONT_SIZE, RAYWHITE);
	DrawText("Press 't' to toggle starting player!", 10, 10 + FONT_SIZE, FONT_SIZE, RAYWHITE);
	DrawText("Background color indicates players turn!", 10, 10 + (FONT_SIZE*2), FONT_SIZE, RAYWHITE);
	EndDrawing();
}

bool _inputToGrid(GameState* state, unsigned int x, unsigned int y)
{
	if (state->grid.grid[state->grid.num_rows * y + x] != 0)
		return false;

	switch (state->turn) {
	case TURN_BLUE: {
		state->grid.grid[state->grid.num_rows * y + x] = TURN_BLUE;
	} break;
	case TURN_RED: {
		state->grid.grid[state->grid.num_rows * y + x] = TURN_RED;
	} break;
	default: {
		fprintf(stderr, "Invalid turn (needed either blue or red)");
		exit(EXIT_FAILURE);
	} break;
	}

	return true;
}

void _rotateGrid(GameState* state)
{
	// inner rotation (right) TODO: change this up?
	// 5 <- 6
	// |    ^
	// v    |
	// 9 -> 10
	unsigned char five = state->grid.grid[state->grid.num_rows * 1 + 1];
	unsigned char six  = state->grid.grid[state->grid.num_rows * 1 + 2];
	unsigned char nine = state->grid.grid[state->grid.num_rows * 2 + 1];
	unsigned char ten  = state->grid.grid[state->grid.num_rows * 2 + 2];

	state->grid.grid[state->grid.num_rows * 1 + 1] = six;
	state->grid.grid[state->grid.num_rows * 1 + 2] = ten;
	state->grid.grid[state->grid.num_rows * 2 + 1] = five;
	state->grid.grid[state->grid.num_rows * 2 + 2] = nine;

	// outer rotation (right) TODO: change this up?
	// 0 <-  1 <-  2 <-  3
	// |				 ^
	// v				 |
	// 4				 7
	// |				 ^
	// v				 |
	// 8				 11
	// |				 ^
	// v				 |
	// 12 -> 13 -> 14 -> 15
	unsigned char zero	  	 = state->grid.grid[state->grid.num_rows * 0 + 0];
	unsigned char one	   	 = state->grid.grid[state->grid.num_rows * 0 + 1];
	unsigned char two	   	 = state->grid.grid[state->grid.num_rows * 0 + 2];
	unsigned char three    	 = state->grid.grid[state->grid.num_rows * 0 + 3];
	unsigned char four	   	 = state->grid.grid[state->grid.num_rows * 1 + 0];
	unsigned char seven	   	 = state->grid.grid[state->grid.num_rows * 1 + 3];
	unsigned char eight	   	 = state->grid.grid[state->grid.num_rows * 2 + 0];
	unsigned char eleven   	 = state->grid.grid[state->grid.num_rows * 2 + 3];
	unsigned char twelve   	 = state->grid.grid[state->grid.num_rows * 3 + 0];
	unsigned char thirteen	 = state->grid.grid[state->grid.num_rows * 3 + 1];
	unsigned char fourteen	 = state->grid.grid[state->grid.num_rows * 3 + 2];
	unsigned char fifteen	 = state->grid.grid[state->grid.num_rows * 3 + 3];

	state->grid.grid[state->grid.num_rows * 0 + 0] = one;
	state->grid.grid[state->grid.num_rows * 0 + 1] = two;
	state->grid.grid[state->grid.num_rows * 0 + 2] = three;
	state->grid.grid[state->grid.num_rows * 0 + 3] = seven;
	state->grid.grid[state->grid.num_rows * 1 + 0] = zero;
	state->grid.grid[state->grid.num_rows * 1 + 3] = eleven;
	state->grid.grid[state->grid.num_rows * 2 + 0] = four;
	state->grid.grid[state->grid.num_rows * 2 + 3] = fifteen;
	state->grid.grid[state->grid.num_rows * 3 + 0] = eight;
	state->grid.grid[state->grid.num_rows * 3 + 1] = twelve;
	state->grid.grid[state->grid.num_rows * 3 + 2] = thirteen;
	state->grid.grid[state->grid.num_rows * 3 + 3] = fourteen;
}

bool _handleInput(GameState* state)
{
	bool valid_turn = false;
	if (IsKeyPressed(KEY_ZERO)) {
		valid_turn = _inputToGrid(state, 0, 0);
	}
	else if (IsKeyPressed(KEY_ONE)) {
		valid_turn = _inputToGrid(state, 1, 0);
	}
	else if (IsKeyPressed(KEY_TWO)) {
		valid_turn = _inputToGrid(state, 2, 0);
	}
	else if (IsKeyPressed(KEY_THREE)) {
		valid_turn = _inputToGrid(state, 3, 0);
	}
	else if (IsKeyPressed(KEY_FOUR)) {
		valid_turn = _inputToGrid(state, 0, 1);
	}
	else if (IsKeyPressed(KEY_FIVE)) {
		valid_turn = _inputToGrid(state, 1, 1);
	}
	else if (IsKeyPressed(KEY_SIX)) {
		valid_turn = _inputToGrid(state, 2, 1);
	}
	else if (IsKeyPressed(KEY_SEVEN)) {
		valid_turn = _inputToGrid(state, 3, 1);
	}
	else if (IsKeyPressed(KEY_EIGHT)) {
		valid_turn = _inputToGrid(state, 0, 2);
	}
	else if (IsKeyPressed(KEY_NINE)) {
		valid_turn = _inputToGrid(state, 1, 2);
	}
	else if (IsKeyPressed(KEY_A)) {
		valid_turn = _inputToGrid(state, 2, 2);
	}
	else if (IsKeyPressed(KEY_B)) {
		valid_turn = _inputToGrid(state, 3, 2);
	}
	else if (IsKeyPressed(KEY_C)) {
		valid_turn = _inputToGrid(state, 0, 3);
	}
	else if (IsKeyPressed(KEY_D)) {
		valid_turn = _inputToGrid(state, 1, 3);
	}
	else if (IsKeyPressed(KEY_E)) {
		valid_turn = _inputToGrid(state, 2, 3);
	}
	else if (IsKeyPressed(KEY_F)) {
		valid_turn = _inputToGrid(state, 3, 3);
	}

	return valid_turn;
}

void _checkGameEnd(GameState* state)
{
	// horizontally (rows)
	for (unsigned int y = 0; y < state->grid.num_rows; y++) {
		if ((state->grid.grid[state->grid.num_rows * y + 0] == state->grid.grid[state->grid.num_rows * y + 1])
		 && (state->grid.grid[state->grid.num_rows * y + 2] == state->grid.grid[state->grid.num_rows * y + 3])
		 && (state->grid.grid[state->grid.num_rows * y + 0] == state->grid.grid[state->grid.num_rows * y + 2])) {
			if (state->grid.grid[state->grid.num_rows * y + 0] == TURN_BLUE) {
				state->state = STATE_BLUE_WON;
				return;
			}
			else if (state->grid.grid[state->grid.num_rows * y + 0] == TURN_RED) {
				state->state = STATE_RED_WON;
				return;
			}
		}
	}

	// vertically (cols)
	for (unsigned int x = 0; x < state->grid.num_cols; x++) {
		if ((state->grid.grid[state->grid.num_rows * 0 + x] == state->grid.grid[state->grid.num_rows * 1 + x])
		 && (state->grid.grid[state->grid.num_rows * 2 + x] == state->grid.grid[state->grid.num_rows * 3 + x])
		 && (state->grid.grid[state->grid.num_rows * 0 + x] == state->grid.grid[state->grid.num_rows * 2 + x])) {
			if (state->grid.grid[state->grid.num_rows * 0 + x] == TURN_BLUE) {
				state->state = STATE_BLUE_WON;
				return;
			}
			else if (state->grid.grid[state->grid.num_rows * 0 + x] == TURN_RED) {
				state->state = STATE_RED_WON;
				return;
			}
		}
	}

	// diagonal (bottom left -> top right)
	if ((state->grid.grid[state->grid.num_rows * 3 + 0] == state->grid.grid[state->grid.num_rows * 2 + 1])
	 && (state->grid.grid[state->grid.num_rows * 1 + 2] == state->grid.grid[state->grid.num_rows * 0 + 3])
	 && (state->grid.grid[state->grid.num_rows * 3 + 0] == state->grid.grid[state->grid.num_rows * 1 + 2])) {
		if (state->grid.grid[state->grid.num_rows * 3 + 0] == TURN_BLUE) {
			state->state = STATE_BLUE_WON;
			return;
		}
		else if (state->grid.grid[state->grid.num_rows * 3 + 0] == TURN_RED) {
			state->state = STATE_RED_WON;
			return;
		}
	}

	// diagonal (top left -> bottom right)
	if ((state->grid.grid[state->grid.num_rows * 0 + 0] == state->grid.grid[state->grid.num_rows * 1 + 1])
	 && (state->grid.grid[state->grid.num_rows * 2 + 2] == state->grid.grid[state->grid.num_rows * 3 + 3])
	 && (state->grid.grid[state->grid.num_rows * 0 + 0] == state->grid.grid[state->grid.num_rows * 2 + 2])) {
		if (state->grid.grid[state->grid.num_rows * 0 + 0] == TURN_BLUE) {
			state->state = STATE_BLUE_WON;
			return;
		}
		else if (state->grid.grid[state->grid.num_rows * 0 + 0] == TURN_RED) {
			state->state = STATE_RED_WON;
			return;
		}
	}

	// draw (all cells are filled)
	for (unsigned int y = 0; y < state->grid.num_rows; y++) {
		for (unsigned int x = 0; x < state->grid.num_cols; x++) {
			if (state->grid.grid[state->grid.num_rows * y + x] == 0) {
				return;
			}
		}
	}
	state->state = STATE_DRAW;
}

void updateGameState(GameState* state)
{
	bool valid_turn = _handleInput(state);
	if (valid_turn) {
		_rotateGrid(state);
		_checkGameEnd(state);

		switch (state->turn) {
		case TURN_BLUE: {
			state->turn = TURN_RED;
		} break;
		case TURN_RED: {
			state->turn = TURN_BLUE;
		} break;
		default: {
			fprintf(stderr, "Invalid turn (needed either blue or red)");
			exit(EXIT_FAILURE);
		} break;
		}
	}
}

void _renderGrid(GameState* state)
{
	const int x_off = (WINDOW_WIDTH / 2) - (WINDOW_HEIGHT / 2);
	const int thickness = 8;

	// Render Grid lines
	int cell_size = (WINDOW_HEIGHT - ((state->grid.num_rows+1) * thickness)) / state->grid.num_rows;
	for (unsigned int i = 0; i <= state->grid.num_rows; i++) {
		DrawRectangle(0+x_off, i*(cell_size+thickness), WINDOW_HEIGHT, thickness, RAYWHITE);
		DrawRectangle(i*(cell_size+thickness)+x_off, 0, thickness, WINDOW_HEIGHT, RAYWHITE);
	}

	// Render Grid content
	Color color = BLACK;
	for (unsigned int y = 0; y < state->grid.num_rows; y++) {
		for (unsigned int x = 0; x < state->grid.num_cols; x++) {
			if (state->grid.grid[state->grid.num_rows * y + x] == TURN_BLUE) {
				color = BLUE;
			}
			else if (state->grid.grid[state->grid.num_rows * y + x] == TURN_RED) {
				color = RED;
			}
			else {
				color = BLACK;
			}
			DrawRectangle(x*(cell_size+thickness)+thickness+x_off, y*(cell_size+thickness)+thickness, cell_size, cell_size, color);
		}
	}
}

void renderGameState(GameState* state)
{
	BeginDrawing();
	switch (state->turn) {
	case TURN_BLUE: {
		ClearBackground(BLUE);
	} break;
	case TURN_RED: {
		ClearBackground(RED);
	} break;
	default: {
		fprintf(stderr, "Invalid turn (needed either blue or red)");
		exit(EXIT_FAILURE);
	} break;
	}

	_renderGrid(state);

	DrawText("Press 0->9", 10, 10, FONT_SIZE, RAYWHITE);
	DrawText("and A->F", 10, 10 + FONT_SIZE, FONT_SIZE, RAYWHITE);

	EndDrawing();
}

void updateGameOver(GameState* state)
{
	if (IsKeyPressed(KEY_R)) {
		state->state = STATE_MENU;
		for (unsigned int i = 0; i < (state->grid.num_rows * state->grid.num_cols); i++) {
			state->grid.grid[i] = 0;
		}
		state->turn = TURN_BLUE;
	}
}

void renderGameOver(GameState* state)
{
	BeginDrawing();

	_renderGrid(state);

	switch (state->state) {
	case STATE_BLUE_WON: {
		ClearBackground(BLUE);
		DrawText("Player Blue won!", 10, 10, FONT_SIZE, RAYWHITE);
	} break;
	case STATE_RED_WON: {
		ClearBackground(RED);
		DrawText("Player Red won!", 10, 10, FONT_SIZE, RAYWHITE);
	} break;
	case STATE_DRAW: {
		ClearBackground(BEIGE);
		DrawText("It's a Draw!", 10, 10, FONT_SIZE, RAYWHITE);
	} break;
	default: {
		fprintf(stderr, "Invalid state (needed either blue won, red won or draw)");
		exit(EXIT_FAILURE);
	} break;
	}

	DrawText("Press 'r' to return to the menu!", 10, 10 + FONT_SIZE, FONT_SIZE, RAYWHITE);
	EndDrawing();
}

int main(void)
{
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Spinning Connect 4");
	SetTargetFPS(60);

	GameState game_state = initGameState();

	while (!WindowShouldClose()) {
		switch (game_state.state) {
		case STATE_MENU: {
			updateStartMenu(&game_state);
			renderStartMenu(&game_state);
		} break;
		case STATE_RUNNING: {
			updateGameState(&game_state);
			renderGameState(&game_state);
		} break;
		case STATE_BLUE_WON:
		case STATE_RED_WON:
		case STATE_DRAW: {
			renderGameOver(&game_state);
			updateGameOver(&game_state);
		} break;
		default: {
			fprintf(stderr, "ERROR: Unreachable\n");
			exit(EXIT_FAILURE);
		} break;
		}
	}

	free(game_state.grid.grid);
	CloseWindow();
	return 0;
}
