#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t *state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t *state, unsigned int snum);
static char next_square(game_state_t *state, unsigned int snum);
static void update_tail(game_state_t *state, unsigned int snum);
static void update_head(game_state_t *state, unsigned int snum);

/* Task 1 */
game_state_t* create_default_state() {
  // Allocate memory for the game state structure
  game_state_t *state;
  
  //initialize state
  state = malloc(sizeof(game_state_t));
  state->num_rows = 18;
  state->num_snakes = 1;

  // initialize state->snakes
  state->snakes = malloc(sizeof(snake_t));
  state->snakes->tail_row = 2;
  state->snakes->tail_col = 2;
  state->snakes->head_row = 2;
  state->snakes->head_col = 4;
  state->snakes->live = true;

  //initialize state->board
  state->board = (char**) malloc(sizeof(char*) * (state->num_rows));
  for (int i=0 ; i<(state->num_rows); i++) {
    (state->board)[i] = malloc(sizeof(char)*21);
    if (i==0 || i==(state->num_rows-1)) {
      strcpy(state->board[i], "####################");
    } else if (i==2) {
      strcpy(state->board[i], "# d>D    *         #");
    } else {
      strcpy(state->board[i], "#                  #");
    }
  }

  return state;
}



/* Task 2 */
void free_state(game_state_t *state) {
  for (unsigned int row = 0; row < state->num_rows - 1; ++row) {
    free(state->board[row]);
  }
  // Free the array of row pointers
  free(state->board);

  // Free the snakes array
  free(state->snakes);

  // Free the state itself
  free(state);
}

/* Task 3 */
void print_board(game_state_t *state, FILE *fp) {
  // TODO: Implement this function.
  for (int rows = 0; rows < state->num_rows; rows++) {
        size_t len = strlen(state->board[rows]);
        if (len > 0 && state->board[rows][len - 1] == '\n') {
            fprintf(fp, "%s", state->board[rows]);
        } else {
            fprintf(fp, "%s\n", state->board[rows]);
        }
  }
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t *state, char *filename) {
  FILE *f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t *state, unsigned int row, unsigned int col) { return state->board[row][col]; }

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t *state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  // TODO: Implement this function.
  return c == 'w' || c == 'a' || c == 's' || c == 'd';
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  // TODO: Implement this function.
  return c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x';
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implement this function.
  return is_head(c) || is_tail(c) || c == '>' || c == '<' || c == '^' || c == 'v';
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implement this function.
  switch (c) {
    case '^': return 'w';
    case '>': return 'd';
    case 'v': return 's';
    case '<': return 'a';
    default:  return '\0'; 
  }
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implement this function.
  switch (c) {
    case 'W': return '^';
    case 'D': return '>';
    case 'S': return 'v';
    case 'A': return '<';
    default:  return '\0'; // Undefined for characters that are not snake head
  }
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.
  if (c == 'v' || c == 's' || c == 'S') {
    return cur_row + 1;
  } else if (c == '^' || c == 'w' || c == 'W') {
    return cur_row - 1;
  } else {
    return cur_row;
  }
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
  if (c == '>' || c == 'd' || c == 'D') {
    return cur_col + 1;
  } else if (c == '<' || c == 'a' || c == 'A') {
    return cur_col - 1;
  } else {
    return cur_col;
  }
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.

  struct snake_t snake = state->snakes[snum];
  
  // Get the current position of the snake's head
  unsigned int head_row = snake.head_row;
  unsigned int head_col = snake.head_col;

  // Get the character representing the direction the snake's head is facing
  char head_char = get_board_at(state, head_row, head_col);

  // Calculate the next position of the snake's head
  unsigned int next_row = get_next_row(head_row, head_char);
  unsigned int next_col = get_next_col(head_col, head_char);

  // Return the character at the next position
  return get_board_at(state, next_row, next_col);
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.

  struct snake_t snake = state->snakes[snum];

  // Get current head position from the snake structure
  unsigned int head_row = snake.head_row;
  unsigned int head_col = snake.head_col;

  // Get the current head character from the board
  char head_char = get_board_at(state, head_row, head_col);

  // Determine the new head position based on the current direction
  unsigned int new_head_row = get_next_row(head_row, head_char);
  unsigned int new_head_col = get_next_col(head_col, head_char);

  // Update the character on the board at the new head position
  // Assuming the new head character is the same as the old one
  set_board_at(state, new_head_row, new_head_col, head_char);

  // Convert the old head character to a body character on the board
  set_board_at(state, head_row, head_col, head_to_body(head_char));

  // Update the snake structure with the new head position
  state->snakes[snum].head_row = new_head_row;
  state->snakes[snum].head_col = new_head_col;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t *state, unsigned int snum) {
  // Pointer to the snake for which we're updating the tail.
  snake_t* current_snake = &state->snakes[snum];

  // Get the current tail's direction character from the board.
  char tail_direction = get_board_at(state, current_snake->tail_row, current_snake->tail_col);

  // Update the board: clear the snake's current tail position.
  set_board_at(state, current_snake->tail_row, current_snake->tail_col, ' ');

  // Calculate the new tail position based on the direction.
  unsigned int updated_tail_row = get_next_row(current_snake->tail_row, tail_direction);
  unsigned int updated_tail_col = get_next_col(current_snake->tail_col, tail_direction);

  // Update the snake structure with the new tail position.
  current_snake->tail_row = updated_tail_row;
  current_snake->tail_col = updated_tail_col;

  // Retrieve the new tail character from the body part and update the board.
  char new_tail_char = body_to_tail(get_board_at(state, updated_tail_row, updated_tail_col));
  set_board_at(state, updated_tail_row, updated_tail_col, new_tail_char);

  // End of tail update process.
  return;
}


/* Task 4.5 */
void update_state(game_state_t* state, int (*spawn_fruit)(game_state_t*)) {
  // Obtain the total number of snakes in the current state.
  unsigned int snake_count = state->num_snakes;

  for (unsigned int snake_index = 0; snake_index < snake_count; snake_index++) {
      // Extract current snake's head position.
      unsigned int head_row = state->snakes[snake_index].head_row;
      unsigned int head_col = state->snakes[snake_index].head_col;

      // Determine what the next cell contains based on the current direction of the snake.
      char next_cell = next_square(state, snake_index);

      // Check for collision with a wall or another snake.
      if (is_snake(next_cell) || next_cell == '#') {
          state->board[head_row][head_col] = 'x'; // Mark the snake as dead.
          state->snakes[snake_index].live = false;
      }
      // Check for fruit consumption.
      else if(next_cell == '*') {
          update_head(state, snake_index); // Move snake head to the fruit.
          spawn_fruit(state); // Add new fruit to the board.
      } else {
          // No collision and no fruit, move the snake forward.
          update_head(state, snake_index); // Move snake head forward.
          update_tail(state, snake_index); // Move snake tail forward.
      }
  }
}

/* Task 5.1 */
char *read_line(FILE *fp) {
  // TODO: Implement this function.
  if (fp == NULL) {
    return NULL;
  }

  const size_t initialSize = 128;
  size_t currentSize = initialSize;
  char *buffer = malloc(currentSize);
  if (buffer == NULL) {
    return NULL;
  }

  // Prepare to read the line
  buffer[0] = '\0'; // Set the first character to null terminator, so that strncat knows where to start.
  char tempBuffer[128]; // Temporary buffer to hold chunks read by fgets.

  while (fgets(tempBuffer, sizeof(tempBuffer), fp) != NULL) {
    // Resize buffer if necessary
    size_t tempLen = strlen(tempBuffer);
    size_t newLen = strlen(buffer) + tempLen;
    if (newLen >= currentSize - 1) {
      currentSize *= 2;
      char *newBuffer = realloc(buffer, currentSize);
      if (newBuffer == NULL) {
        free(buffer);
        return NULL;
      }
      buffer = newBuffer;
    }

    // Concatenate temporary buffer to main buffer
    strcat(buffer, tempBuffer);

    // Check if the last character read was a newline
    if (buffer[newLen - 1] == '\n') {
      break;
    }
  }

  // If no characters were read, free the buffer and return NULL
  if (buffer[0] == '\0') {
    free(buffer);
    return NULL;
  }

  // Resize to the actual size used
  char *minimalBuffer = realloc(buffer, strlen(buffer) + 1);
  if (minimalBuffer != NULL) {
    buffer = minimalBuffer;
  }

  return buffer;
}

/* Task 5.2 */
game_state_t *load_board(FILE *fp) {
  // Check if the file pointer is valid
  if (!fp) return NULL;

  game_state_t *state = malloc(sizeof(game_state_t));
  if (!state) return NULL;

  state->num_rows = 0;
  state->num_snakes = 0;
  state->snakes = NULL; 
  state->board = NULL; 

  char *line;

  while ((line = read_line(fp)) != NULL) {
    if (line[0] == '\n' || line[0] == '\0') {
      free(line);
      continue;
    }

    char **newBoard = realloc(state->board, (state->num_rows + 1) * sizeof(char *));
    if (!newBoard) {
      for (size_t i = 0; i < state->num_rows; ++i) {
        free(state->board[i]);
      }
      free(state->board);
      free(state);
      free(line);
      return NULL;
    }
    state->board = newBoard;
    state->board[state->num_rows] = line; 
    state->num_rows++;
  }

  if (state->num_rows == 0) {
    free(state);
    return NULL;
  }

  return state;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t* state, unsigned int snum) {
  // Access the snake structure from the array of snakes.
  snake_t* snake = state->snakes + snum;
  
  // Initialize the current position to the tail's location.
  unsigned int current_row = snake->tail_row;
  unsigned int current_col = snake->tail_col;
  
  // Get the character at the tail's position, which indicates the direction.
  char current_char = get_board_at(state, current_row, current_col);
  
  // Traverse the snake from tail to head.
  while (!is_head(current_char)) {
      // Move to the next position in the direction indicated by the current character.
      current_row = get_next_row(current_row, current_char);
      current_col = get_next_col(current_col, current_char);
      
      // Get the character at the new position.
      current_char = get_board_at(state, current_row, current_col);
  }
  
  // Once the head is found, update the snake's head position in the structure.
  snake->head_row = current_row;
  snake->head_col = current_col;
}


/* Task 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  // Keep track of the number of snakes found on the board.
  unsigned int total_snakes = 0;

  // Allocate initial memory for one snake.
  state->snakes = malloc(sizeof(snake_t));

  // Iterate over each row of the board.
  for(unsigned int row = 0; row < state->num_rows; row++) {
    // Iterate over each column in the current row.
    for(unsigned int col = 0; col < strlen(state->board[row]); col++){
        // Check if the current character is a tail.
        if (is_tail(get_board_at(state, row, col))){
            // Increase the count of snakes.
            total_snakes++;
            // Reallocate memory for the additional snake.
            state->snakes = realloc(state->snakes, total_snakes * sizeof(snake_t));
            // Initialize the newly added snake.
            state->snakes[total_snakes - 1].live = true;
            state->snakes[total_snakes - 1].tail_row = row;
            state->snakes[total_snakes - 1].tail_col = col;
            // Find the head of the new snake.
            find_head(state, total_snakes - 1);
        }
    }
  }

  // Update the total number of snakes in the game state.
  state->num_snakes = total_snakes;

  // Return the updated game state.
  return state;
}
