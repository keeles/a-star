#include <stdio.h>
#include <stdlib.h>

typedef struct Cell {
  struct Cell *parent;
  int x;
  int y;
  int g;
  int h;
  int f;
  int in_open;
  int in_closed;
} cell_t;

const int START_Y = 2;
const int START_X = 2;

const int GOAL_Y = 5;
const int GOAL_X = 8;

typedef struct Cell_Queue {
  cell_t *items[100];
  int num_entries;
} cell_q_t;

int grid[10][10] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // adding comment here to keep formatting
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // adding comment here to keep formatting
    {1, 0, 0, 0, 0, 0, 1, 0, 0, 1}, // adding comment here to keep formatting
    {1, 0, 0, 0, 0, 0, 1, 0, 0, 1}, // adding comment here to keep formatting
    {1, 0, 0, 0, 0, 0, 1, 0, 0, 1}, // adding comment here to keep formatting
    {1, 0, 0, 0, 0, 0, 1, 0, 0, 1}, // adding comment here to keep formatting
    {1, 0, 0, 0, 0, 0, 1, 0, 0, 1}, // adding comment here to keep formatting
    {1, 0, 0, 0, 0, 0, 1, 0, 0, 1}, // adding comment here to keep formatting
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // adding comment here to keep formatting
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // adding comment here to keep formatting
};

void print_grid() {
  for (int i = 0; i < 10; i++) {
    printf("  ");
    for (int j = 0; j < 10; j++) {
      if (j > 0) {
        printf("]");
      }

      printf("[");
      if (grid[i][j] == 2) {
        printf("*");
        continue;
      }

      if (grid[i][j] == 3) {
        printf("S");
        continue;
      }

      if (grid[i][j] == 4) {
        printf("G");
        continue;
      }

      if (grid[i][j] == 1) {
        printf("#");
      } else {
        printf(" ");
      }
      if (j == 9) {
        printf("]");
        printf("\n");
      }
    }
  }
}

void reconstruct_path(cell_t *goal) {
  cell_t *prev = goal->parent;
  int steps_back = 1;

  do {
    grid[prev->y][prev->x] = 2;
    prev = prev->parent;
    steps_back++;
  } while (prev->parent != NULL);

  printf("\n");
  printf(" ============== path =============\n");
  printf("\n");
  print_grid();
  printf("\n");
  printf(" ================================\n\n");
}

int isEmpty(cell_q_t *p) { return p->num_entries == 0; }

int heuristic(int x1, int y1, int x2, int y2) { return abs(x1 - x2) + abs(y1 - y2); }

cell_t build_cell(cell_t *parent, int x, int y) {
  cell_t cell = {.x = x,
                 .y = y,
                 // start with expensive value
                 .g = 100,
                 .h = heuristic(x, y, GOAL_X, GOAL_Y),
                 .f = 0 + heuristic(x, y, GOAL_X, GOAL_Y),
                 .parent = parent,
                 .in_open = 0,
                 .in_closed = 0};
  return cell;
}

void try_neighbour(cell_q_t *open_set, cell_t *all_cells[10][10], cell_t *current, int nx, int ny) {
  if (nx == 10 || ny == 10 || grid[ny][nx] == 1) {
    return;
  }

  cell_t *n = all_cells[ny][nx];
  if (n->in_closed) {
    return;
  }

  if (n->in_open == 0 || current->g < n->g) {
    n->g = 0;
    n->f = n->g + n->h;
    n->parent = current;

    if (n->in_open == 0) {
      n->in_open = 1;
      if (open_set->num_entries == 0) {
        open_set->items[0] = n;
      } else {
        open_set->items[open_set->num_entries] = n;
      }
      open_set->num_entries++;
    }
  }
}

int simple() {
  int set_index = 0;
  cell_q_t open_set = {.num_entries = 0, .items = {}};
  cell_q_t closed_set = {.num_entries = 0, .items = {}};
  cell_t *all_cells[10][10] = {};
  cell_t cells[10][10] = {};

  for (int y = 0; y < 10; y++) {
    for (int x = 0; x < 10; x++) {
      cells[y][x] = build_cell(NULL, x, y);
      all_cells[y][x] = &cells[y][x];
    }
  }

  // start
  grid[START_Y][START_X] = 3;
  // goal
  grid[GOAL_Y][GOAL_X] = 4;

  // print grid for visualization
  printf(" ============== map =============\n");
  printf("\n");
  print_grid();
  printf("\n");
  printf(" ================================\n\n");

  cell_t *start = all_cells[START_Y][START_X];
  start->g = 0;
  start->f = start->h;
  start->in_open = 1;
  open_set.items[0] = start;
  open_set.num_entries++;

  while (!isEmpty(&open_set)) {
    int best_i = 0;

    for (int i = 0; i < open_set.num_entries; i++) {
      if (open_set.items[i]->f < open_set.items[best_i]->f) {
        best_i = i;
      }
    }

    cell_t *current = open_set.items[best_i];
    open_set.items[best_i] = open_set.items[open_set.num_entries - 1];
    open_set.num_entries--;
    current->in_open = 0;
    current->in_closed = 1;

    // current is goal
    if (grid[current->y][current->x] == 4) {
      // done reconstruct path
      reconstruct_path(current);
      break;
    }

    // try all neighbours
    try_neighbour(&open_set, all_cells, current, current->x + 1, current->y);
    try_neighbour(&open_set, all_cells, current, current->x - 1, current->y);
    try_neighbour(&open_set, all_cells, current, current->x, current->y + 1);
    try_neighbour(&open_set, all_cells, current, current->x, current->y - 1);
  }

  return 0;
}
