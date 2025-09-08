#pragma once

#include "ox_core.h"
#include "ox_log.h"

#include <microui.h>
#include <raylib.h>

static void r_draw_rect(const mu_Rect rect, const mu_Color color)
{
  DrawRectangle(rect.x, rect.y, rect.w, rect.h,
                (Color){ color.r, color.g, color.b, color.a });
}

static void r_draw_text(const char* text, const mu_Vec2 pos,
                        const mu_Color color)
{
  DrawText(text, pos.x, pos.y, 20,
           (Color){ color.r, color.g, color.b, color.a });
}

static void r_draw_icon(int id, mu_Rect rect, mu_Color color)
{
}

static int r_get_text_width(mu_Font font, const char* text, int len)
{
  return MeasureText(text, 16);
}

static int r_get_text_height(mu_Font font)
{
  return 20;
}

static void ox_ui_update_input(mu_Context* ctx)
{
  // Mouse input
  const Vector2 mouse_pos = GetMousePosition();
  mu_input_mousemove(ctx, mouse_pos.x, mouse_pos.y);

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    mu_input_mousedown(ctx, mouse_pos.x, mouse_pos.y, MU_MOUSE_LEFT);
  if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
    mu_input_mouseup(ctx, mouse_pos.x, mouse_pos.y, MU_MOUSE_LEFT);

  // Keyboard input
  int key = GetCharPressed();
  if (key > 0) {
    mu_input_text(ctx, (char*)&key);
  }

  // Handle special keys (backspace, enter, etc.)
  if (IsKeyPressed(KEY_BACKSPACE))
    mu_input_keydown(ctx, MU_KEY_BACKSPACE);
  if (IsKeyPressed(KEY_ENTER))
    mu_input_keydown(ctx, MU_KEY_RETURN);
}

static mu_Context ui_ctx;

static long ox_ui_init(void)
{
  mu_init(&ui_ctx);
  ui_ctx.text_width = r_get_text_width;
  ui_ctx.text_height = r_get_text_height;

  return OX_SUCCESS;
}

static void ox_ui_exit(void)
{
}

static void ox_ui_update(void)
{
  ox_ui_update_input(&ui_ctx);

  mu_begin(&ui_ctx);

  if (mu_begin_window(&ui_ctx, "Demo Window", mu_rect(40, 40, 300, 450))) {
    if (mu_button(&ui_ctx, "Button 1")) {
      OX_LOG_DBG("Button 1 pressed");
    }
    if (mu_button(&ui_ctx, "Button 2")) {
      OX_LOG_DBG("Button 2 pressed");
    }
    mu_end_window(&ui_ctx);
  }

  mu_end(&ui_ctx);
}

static void ox_ui_render(void)
{
  mu_Command* cmd = NULL;
  while (mu_next_command(&ui_ctx, &cmd)) {
    switch (cmd->type) {
    case MU_COMMAND_RECT:
      r_draw_rect(cmd->rect.rect, cmd->rect.color);
      break;
    case MU_COMMAND_TEXT:
      r_draw_text(cmd->text.str, cmd->text.pos, cmd->text.color);
      break;
    case MU_COMMAND_ICON:
      r_draw_icon(cmd->icon.id, cmd->icon.rect, cmd->icon.color);
      break;
    case MU_COMMAND_CLIP:
      if (cmd->clip.rect.w == 0) {
        EndScissorMode();
      } else {
        BeginScissorMode(cmd->clip.rect.x, cmd->clip.rect.y, cmd->clip.rect.w,
                         cmd->clip.rect.h);
      }
      break;
    default:
      break;
    }
  }
}
