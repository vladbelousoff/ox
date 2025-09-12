#include "ox_core.h"
#include "ox_log.h"
#include "ox_memory.h"
#include "ox_render.h"

#include <math.h>
#include <raylib.h>

#define GRID_SIZE          50
#define MAX_BALLS_PER_CELL 10
#define NUMBER_OF_BALLS    500
#define BALL_RADIUS        10.f

typedef struct {
  long (*init)(void);
  void (*free)(void);
  const char* name;
} ox_subsystem_t;

typedef struct {
  int* ball_indices;
  int count;
  int capacity;
} grid_cell_t;

static ox_subsystem_t subsystems[] = {
  { ox_memory_init, ox_memory_exit, "Memory" },
  { ox_render_init, ox_render_exit, "Render" },
};

static void systems_exit_starting_from(const int index)
{
  for (int i = index; i >= 0; --i) {
    OX_LOG_DBG("Exit system '%s'", subsystems[i].name);
    subsystems[i].free();
  }
}

static long systems_init(void)
{
  for (int i = 0; i < OX_ARRAY_SIZE(subsystems); ++i) {
    OX_LOG_DBG("Init system '%s'", subsystems[i].name);
    if (subsystems[i].init() != OX_SUCCESS) {
      systems_exit_starting_from(i - 1);
      return 1;
    }
  }

  return 0;
}

static void systems_exit(void)
{
  systems_exit_starting_from(OX_ARRAY_SIZE(subsystems) - 1);
}

void wrap_position(Vector2* position, const float width, const float height)
{
  position->x = fmodf(position->x, width);
  if (position->x < 0.0f)
    position->x += width;

  position->y = fmodf(position->y, height);
  if (position->y < 0.0f)
    position->y += height;
}

bool check_circle_collision(const Vector2 pos1, const Vector2 pos2,
                            const float radius1, const float radius2)
{
  const float dx = pos1.x - pos2.x;
  const float dy = pos1.y - pos2.y;
  const float distance_squared = dx * dx + dy * dy;
  const float min_distance = radius1 + radius2;
  return distance_squared < min_distance * min_distance;
}

void resolve_collision(Vector2* pos1, Vector2* pos2, Vector2* vel1,
                       Vector2* vel2, const float radius1, const float radius2)
{
  // Calculate collision vector
  Vector2 delta = { pos1->x - pos2->x, pos1->y - pos2->y };
  const float distance2 = delta.x * delta.x + delta.y * delta.y;
  const float radius_sum = radius1 + radius2;

  if (distance2 < radius_sum * radius_sum && distance2 > 0.0f) {
    // Normalize collision vector
    const float distance = sqrtf(distance2);
    delta.x /= distance;
    delta.y /= distance;

    // Separate overlapping balls
    const float overlap = radius_sum - distance;
    pos1->x += delta.x * overlap * 0.5f;
    pos1->y += delta.y * overlap * 0.5f;
    pos2->x -= delta.x * overlap * 0.5f;
    pos2->y -= delta.y * overlap * 0.5f;

    // Calculate relative velocity
    const Vector2 rel_vel = { vel1->x - vel2->x, vel1->y - vel2->y };
    const float vel_along_normal = rel_vel.x * delta.x + rel_vel.y * delta.y;

    // Don't resolve if velocities are separating
    if (vel_along_normal > 0)
      return;

    // Apply collision response (assuming equal mass)
    const float restitution =
      1.0f; // Bounciness factor (0.0 = no bounce, 1.0 = perfect bounce)
    const float impulse = -(1.f + restitution) * vel_along_normal / 2;

    vel1->x += impulse * delta.x;
    vel1->y += impulse * delta.y;
    vel2->x -= impulse * delta.x;
    vel2->y -= impulse * delta.y;
  }
}

void add_ball_to_cell(grid_cell_t* cell, const int ball_index)
{
  if (cell->count < cell->capacity) {
    cell->ball_indices[cell->count++] = ball_index;
  }
}

void check_collisions_in_cells(const grid_cell_t* cell1,
                               const grid_cell_t* cell2,
                               Vector2* ball_positions,
                               Vector2* ball_directions,
                               const float ball_radius, const bool same_cell)
{
  for (int i = 0; i < cell1->count; ++i) {
    const int ball1 = cell1->ball_indices[i];
    const int j_start = same_cell ? i + 1 : 0;
    const int j_end = same_cell ? cell1->count : cell2->count;
    const grid_cell_t* target_cell = same_cell ? cell1 : cell2;

    for (int j = j_start; j < j_end; ++j) {
      const int ball2 = target_cell->ball_indices[j];
      if (check_circle_collision(ball_positions[ball1], ball_positions[ball2],
                                 ball_radius, ball_radius)) {
        resolve_collision(&ball_positions[ball1], &ball_positions[ball2],
                          &ball_directions[ball1], &ball_directions[ball2],
                          ball_radius, ball_radius);
      }
    }
  }
}

int main(void)
{
  const long ret_code = systems_init();
  if (ret_code != OX_SUCCESS) {
    return (int)ret_code;
  }

  static const int number_of_balls = NUMBER_OF_BALLS;
  static const float ball_radius = BALL_RADIUS;

  Vector2* ball_positions =
    ox_mem_acquire(sizeof(Vector2) * number_of_balls, OX_SOURCE_LOCATION);
  Vector2* ball_directions =
    ox_mem_acquire(sizeof(Vector2) * number_of_balls, OX_SOURCE_LOCATION);
  Color* ball_colors =
    ox_mem_acquire(sizeof(Color) * number_of_balls, OX_SOURCE_LOCATION);

  // Initialize grid for spatial partitioning
  const int grid_width = (GetScreenWidth() + GRID_SIZE - 1) / GRID_SIZE;
  const int grid_height = (GetScreenHeight() + GRID_SIZE - 1) / GRID_SIZE;
  const int total_cells = grid_width * grid_height;

  grid_cell_t* grid =
    ox_mem_acquire(sizeof(grid_cell_t) * total_cells, OX_SOURCE_LOCATION);

  // Initialize grid cells
  for (int i = 0; i < total_cells; ++i) {
    grid[i].ball_indices =
      ox_mem_acquire(sizeof(int) * MAX_BALLS_PER_CELL, OX_SOURCE_LOCATION);
    grid[i].count = 0;
    grid[i].capacity = MAX_BALLS_PER_CELL;
  }

  // Initialize balls
  for (int i = 0; i < number_of_balls; ++i) {
    ball_positions[i].x = (float)GetRandomValue(
      (int)ball_radius, GetScreenWidth() - (int)ball_radius);
    ball_positions[i].y = (float)GetRandomValue(
      (int)ball_radius, GetScreenHeight() - (int)ball_radius);
    ball_directions[i].x = (float)GetRandomValue(-150, 150);
    ball_directions[i].y = (float)GetRandomValue(-150, 150);
    ball_colors[i].a = 255;
    ball_colors[i].r = GetRandomValue(100, 255);
    ball_colors[i].g = GetRandomValue(100, 255);
    ball_colors[i].b = GetRandomValue(100, 255);
  }

  while (!WindowShouldClose()) {
    const float delta_time = GetFrameTime();

    // Update ball positions
    for (int i = 0; i < number_of_balls; ++i) {
      ball_positions[i].x += ball_directions[i].x * delta_time;
      ball_positions[i].y += ball_directions[i].y * delta_time;
      wrap_position(&ball_positions[i], (float)GetScreenWidth(),
                    (float)GetScreenHeight());
    }

    // Clear grid
    for (int i = 0; i < total_cells; ++i) {
      grid[i].count = 0;
    }

    // Populate grid with balls
    for (int i = 0; i < number_of_balls; ++i) {
      int grid_x = (int)(ball_positions[i].x / GRID_SIZE);
      int grid_y = (int)(ball_positions[i].y / GRID_SIZE);

      // Clamp to grid bounds
      grid_x =
        grid_x < 0 ? 0 : (grid_x >= grid_width ? grid_width - 1 : grid_x);
      grid_y =
        grid_y < 0 ? 0 : (grid_y >= grid_height ? grid_height - 1 : grid_y);

      const int cell_index = grid_y * grid_width + grid_x;
      add_ball_to_cell(&grid[cell_index], i);
    }

    // Check collisions using spatial partitioning
    for (int cell_y = 0; cell_y < grid_height; ++cell_y) {
      for (int cell_x = 0; cell_x < grid_width; ++cell_x) {
        const int current_cell = cell_y * grid_width + cell_x;

        // Check collisions within current cell
        if (grid[current_cell].count > 1) {
          check_collisions_in_cells(&grid[current_cell], &grid[current_cell],
                                    ball_positions, ball_directions,
                                    ball_radius, true);
        }

        // Check collisions with adjacent cells (right and down to avoid duplicates)
        // Check right cell
        if (cell_x + 1 < grid_width) {
          const int right_cell = cell_y * grid_width + (cell_x + 1);
          if (grid[current_cell].count > 0 && grid[right_cell].count > 0) {
            check_collisions_in_cells(&grid[current_cell], &grid[right_cell],
                                      ball_positions, ball_directions,
                                      ball_radius, false);
          }
        }

        // Check down cell
        if (cell_y + 1 < grid_height) {
          const int down_cell = (cell_y + 1) * grid_width + cell_x;
          if (grid[current_cell].count > 0 && grid[down_cell].count > 0) {
            check_collisions_in_cells(&grid[current_cell], &grid[down_cell],
                                      ball_positions, ball_directions,
                                      ball_radius, false);
          }
        }

        // Check down-right cell (diagonal)
        if (cell_x + 1 < grid_width && cell_y + 1 < grid_height) {
          const int down_right_cell = (cell_y + 1) * grid_width + (cell_x + 1);
          if (grid[current_cell].count > 0 && grid[down_right_cell].count > 0) {
            check_collisions_in_cells(&grid[current_cell],
                                      &grid[down_right_cell], ball_positions,
                                      ball_directions, ball_radius, false);
          }
        }
      }
    }

    // Render
    BeginDrawing();
    ClearBackground(BLACK);

    for (int i = 0; i < number_of_balls; ++i) {
      DrawCircleV(ball_positions[i], ball_radius, ball_colors[i]);
    }

    // Optional: Draw grid for debugging
    for (int x = 0; x <= GetScreenWidth(); x += GRID_SIZE) {
      DrawLine(x, 0, x, GetScreenHeight(), DARKGRAY);
    }

    for (int y = 0; y <= GetScreenHeight(); y += GRID_SIZE) {
      DrawLine(0, y, GetScreenWidth(), y, DARKGRAY);
    }

    ox_render_draw_text(TextFormat("FPS: %d", GetFPS()), 10, 10, 20, WHITE);
    EndDrawing();
  }

  // Cleanup
  for (int i = 0; i < total_cells; ++i) {
    ox_mem_release(grid[i].ball_indices);
  }

  ox_mem_release(grid);
  ox_mem_release(ball_positions);
  ox_mem_release(ball_directions);
  ox_mem_release(ball_colors);

  systems_exit();
  return 0;
}