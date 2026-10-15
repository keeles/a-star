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
  int is_goal;
  int is_start;
} cell_t;

const int START_Y = 2;
const int START_X = 2;

const int GOAL_Y = 5;
const int GOAL_X = 8;

typedef struct Cell_Queue {
  cell_t **items;
  int num_entries;
  int capacity;
} cell_q_t;

int grid[10][10] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // adding comment here to keep formatting
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // adding comment here to keep formatting
    {1, 0, 0, 1, 1, 1, 1, 1, 1, 1}, // adding comment here to keep formatting
    {1, 0, 0, 1, 0, 0, 1, 0, 0, 1}, // adding comment here to keep formatting
    {1, 0, 0, 1, 0, 0, 1, 0, 0, 1}, // adding comment here to keep formatting
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
    // this is just to draw map
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

int euclidean_d(int x1, int y1, int x2, int y2) {
  int dx = abs(x1 - x2);
  int dy = abs(y1 - y2);
  return (dx * dx) + (dy * dy);
}

int chebyshev_d(int x1, int y1, int x2, int y2) {
  int dx = abs(x1 - x2);
  int dy = abs(y1 - y2);
  if (dx > dy) {
    return dx;
  }

  return dy;
}

int get_index(int x, int y, int width) { return y * width + x; }

cell_t build_cell(cell_t *parent, int x, int y, int is_goal, int is_start) {
  cell_t cell = {.x = x,
                 .y = y,
                 // start with expensive value
                 .g = 100,
                 // manhattan
                 // .h = heuristic(x, y, GOAL_X, GOAL_Y),
                 // .f = 0 + heuristic(x, y, GOAL_X, GOAL_Y),
                 // euclidean
                 // .h = euclidean_d(x, y, GOAL_X, GOAL_Y),
                 // .f = 0 + euclidean_d(x, y, GOAL_X, GOAL_Y),
                 // chebyshev
                 .h = chebyshev_d(x, y, GOAL_X, GOAL_Y),
                 .f = 0 + chebyshev_d(x, y, GOAL_X, GOAL_Y),
                 .parent = parent,
                 .in_open = 0,
                 .in_closed = 0,
                 .is_goal = is_goal,
                 .is_start = is_start};
  return cell;
}

void try_neighbour(cell_q_t *open_set, cell_t *cells, cell_t *current, int width, int height,
                   int nx, int ny) {
  if (nx < 0 || nx == width || ny < 0 || ny == height || grid[ny][nx] == 1) {
    return;
  }

  cell_t *n = &cells[ny * width + nx];
  if (n->in_closed) {
    return;
  }

  // tentative_g = current->g + cost_of_moving(current, neighbor)
  // in my current 4 directional movement, cost_of_moving always is 1
  int tentative_g = current->g + 1;

  if (n->in_open == 0 || tentative_g < n->g) {
    n->g = tentative_g;
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

int main() {
  int width = 10;
  int height = 10;

  cell_t *cells = malloc(width * height * sizeof(cell_t));
  cell_q_t open_set = {.num_entries = 0,
                       .capacity = width * height,
                       .items = malloc(sizeof(cell_t *) * width * height)};

  cell_q_t closed_set = {.num_entries = 0,
                         .capacity = width * height,
                         .items = malloc(sizeof(cell_t *) * width * height)};

  for (int y = 0; y < 10; y++) {
    for (int x = 0; x < 10; x++) {
      if (x == GOAL_X && y == GOAL_Y) {
        cells[y * width + x] = build_cell(NULL, x, y, 1, 0);
      } else if (x == START_X && y == START_Y) {
        cells[y * width + x] = build_cell(NULL, x, y, 0, 1);
      } else {
        cells[y * width + x] = build_cell(NULL, x, y, 0, 0);
      }
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

  cell_t *start = &cells[START_Y * width + START_X];
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
    if (cells[current->y * width + current->x].is_goal) {
      // done reconstruct path
      reconstruct_path(current);
      break;
    }

    // try all neighbours
    try_neighbour(&open_set, cells, current, width, height, current->x + 1, current->y);
    try_neighbour(&open_set, cells, current, width, height, current->x + 1, current->y + 1);
    try_neighbour(&open_set, cells, current, width, height, current->x + 1, current->y + -1);
    try_neighbour(&open_set, cells, current, width, height, current->x - 1, current->y);
    try_neighbour(&open_set, cells, current, width, height, current->x - 1, current->y + 1);
    try_neighbour(&open_set, cells, current, width, height, current->x - 1, current->y - 1);
    try_neighbour(&open_set, cells, current, width, height, current->x, current->y + 1);
    try_neighbour(&open_set, cells, current, width, height, current->x, current->y - 1);
  }

  free(cells);
  free(open_set.items);
  free(closed_set.items);
  return 0;
}
