#include "GameOver.h"
#include "../dataCode/historicalGame.h"
#include "../dataCode/user.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

// Helper for button drawing (similar to menu.c but simplified or reused if we
// move it to a util)
static bool DrawStyledButton(Rectangle bounds, const char *text) {
  bool pressed = false;
  Vector2 mousePoint = GetMousePosition();
  bool isHover = CheckCollisionPointRec(mousePoint, bounds);

  Color baseColor = isHover ? LIGHTGRAY : GRAY;
  DrawRectangleRec(bounds, baseColor);
  DrawRectangleLinesEx(bounds, 2, WHITE);

  int fontSize = 20;
  int textWidth = MeasureText(text, fontSize);
  DrawText(text, bounds.x + (bounds.width - textWidth) / 2,
           bounds.y + (bounds.height - fontSize) / 2, fontSize, WHITE);

  if (isHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    pressed = true;
  }
  return pressed;
}

void RunGameOver(int winnerId, User *users[]) {
  // Determine winner text
  const char *winnerText;
  Color winnerColor;

  if (winnerId == -1) {
    winnerText = "MATCH NUL !"; // Draw
    winnerColor = WHITE;
  } else {
    winnerText = TextFormat("VICTOIRE DU JOUEUR %d : %s !", winnerId + 1,
                            users[winnerId]->username);
    winnerColor = GOLD;

    // Save Game Result to History
    if (users[0] && users[1]) { // Assume at least 2 players for score tracking
      HistoricalGame *game =
          createHistoricalGame(users[0]->username, users[1]->username,
                               users[2] ? users[2]->username : "NULL",
                               users[3] ? users[3]->username : "NULL");

      // Set scores from current game stats
      game->score1 = users[0]->currentScore;
      game->score2 = users[1]->currentScore;
      if (users[2])
        game->score3 = users[2]->currentScore;
      if (users[3])
        game->score4 = users[3]->currentScore;

      whoWin(game, winnerId + 1); // +1 because ID 0 is Player 1
      saveGame(game);

      // Update User Stats
      for (int i = 0; i < 4; i++) {
        if (users[i]) {
          incrementGame(users[i]);
          incrementKill(users[i], users[i]->currentKills);
          incrementDeath(users[i], users[i]->currentDeaths);
          updateUserStats(users[i], users[i]->currentScore);
          saveUser(users[i]); // Persist changes to users.txt
        }
      }
      free(game);
    }
  }

  Texture2D background = LoadTexture("src/Menu/images/background_start.jpg");

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);

    // Draw Background
    DrawTexturePro(background,
                   (Rectangle){0, 0, background.width, background.height},
                   (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()},
                   (Vector2){0, 0}, 0.0f, WHITE);

    // Dark Overlay
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));

    // Title
    const char *title = "GAME OVER";
    int titleSize = 60;
    int titleW = MeasureText(title, titleSize);
    DrawText(title, (GetScreenWidth() - titleW) / 2, 100, titleSize, RED);

    // Winner Text
    int winnerSize = 40;
    int winnerW = MeasureText(winnerText, winnerSize);
    DrawText(winnerText, (GetScreenWidth() - winnerW) / 2, 250, winnerSize,
             winnerColor);

    // Menu Button
    Rectangle btnRect = {(GetScreenWidth() - 200) / 2, 400, 200, 50};
    if (DrawStyledButton(btnRect, "RETOUR MENU")) {
      break;
    }

    EndDrawing();
  }

  UnloadTexture(background);
}
