/*
 * snake_game.c
 * Snake game for ESP32 + ILI9341 240x320 display
 *
 * Grid  : 19 cols x 25 rows, cell size 11px, gap 1px
 * Fits  : within 240x320 with space for score bar at top
 * Timer : 200ms game tick via FreeRTOS timer
 * Input : 5 buttons via your existing event manager
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/semphr.h"

#include "spi_hal.h"
#include "event_manager.h"
#include "gpio_hal.h"
#include "shapes.h"
#include "display_manager.h"

/* ─── colours (RGB565) ───────────────────────────────────────────── */
#define COLOR_BG         0x0000   /* black          */
#define COLOR_GRID       0x1082   /* dark grey      */
#define COLOR_SNAKE_HEAD 0x07E0   /* bright green   */
#define COLOR_SNAKE_BODY 0x03E0   /* darker green   */
#define COLOR_FOOD       0xF800   /* red            */
#define COLOR_TEXT       0xFFFF   /* white          */
#define COLOR_DEAD       0xF800   /* red flash      */

/* ─── grid config ────────────────────────────────────────────────── */
#define GRID_COLS        19
#define GRID_ROWS        25
#define CELL_SIZE        11
#define CELL_GAP         1
#define GRID_OFFSET_X    5        /* left margin    */
#define GRID_OFFSET_Y    20       /* top margin (score bar) */

/* max snake length = total cells */
#define MAX_SNAKE_LEN    (GRID_COLS * GRID_ROWS)

/* ─── directions ─────────────────────────────────────────────────── */
#define DIR_UP    0
#define DIR_DOWN  1
#define DIR_LEFT  2
#define DIR_RIGHT 3

/* ─── game states ────────────────────────────────────────────────── */
#define STATE_RUNNING   0
#define STATE_DEAD      1
#define STATE_PAUSED    2

/* ─── key numbers (match your hardware) ─────────────────────────── */
#define KEY_LEFT    0
#define KEY_UP      1
#define KEY_SELECT  2
#define KEY_DOWN    3
#define KEY_RIGHT   4

/* ─── snake state ────────────────────────────────────────────────── */
typedef struct {
    uint8_t  x[MAX_SNAKE_LEN];
    uint8_t  y[MAX_SNAKE_LEN];
    uint16_t length;
    uint8_t  dir;           /* current direction  */
    uint8_t  next_dir;      /* queued direction   */
    uint8_t  food_x;
    uint8_t  food_y;
    uint16_t score;
    uint8_t  state;         /* running/dead/paused */
    uint16_t tick_ms;       /* speed in ms         */
} SnakeGame_t;

extern int8_t hardware_init();


/* ─── globals ────────────────────────────────────────────────────── */
extern displayHandle_t *get_display_handle();
static displayHandle_t  *display_handle;
static SnakeGame_t       game;
static TimerHandle_t     game_timer;
static SemaphoreHandle_t game_mutex;
static uint8_t           keypad_cid = 0xFF;

/* ─── forward declarations ───────────────────────────────────────── */
static void game_init(void);
static void game_tick(void);
static void game_timer_cb(TimerHandle_t xTimer);
static void spawn_food(void);
static bool check_self_collision(uint8_t hx, uint8_t hy);
static void draw_cell(uint8_t col, uint8_t row, uint16_t color);
static void draw_score(void);
static void draw_game_over(void);
static void draw_start_screen(void);
static void draw_pixel_wrapper(uint16_t x, uint16_t y, uint16_t color);

/* ═══════════════════════════════════════════════════════════════════
 * draw_pixel_wrapper — bridge to your display driver
 * ═══════════════════════════════════════════════════════════════════ */
static void draw_pixel_wrapper(uint16_t x, uint16_t y, uint16_t color)
{
    display_drawPixel(display_handle, x, y, color);
}

/* ═══════════════════════════════════════════════════════════════════
 * draw_cell — draw one grid cell at (col, row) with given colour
 * ═══════════════════════════════════════════════════════════════════ */
static void draw_cell(uint8_t col, uint8_t row, uint16_t color)
{
    int px = GRID_OFFSET_X + col * (CELL_SIZE + CELL_GAP);
    int py = GRID_OFFSET_Y + row * (CELL_SIZE + CELL_GAP);
    draw_filled_rectangle(px, py, CELL_SIZE, CELL_SIZE, color);
}

/* ═══════════════════════════════════════════════════════════════════
 * draw_score — top bar showing score and speed level
 * ═══════════════════════════════════════════════════════════════════ */
static void draw_score(void)
{
    /* clear score bar area */
    draw_filled_rectangle(0, 0, 240, GRID_OFFSET_Y - 2, COLOR_BG);

    /* draw horizontal separator */
    draw_horizontal_line(0, GRID_OFFSET_Y - 2, 240, COLOR_GRID);

    /* NOTE: replace draw_text calls with your actual
     * text rendering function once you have it.
     * For now using printf for debugging.         */
    printf("\rScore: %d  Len: %d", game.score, game.length);
}

/* ═══════════════════════════════════════════════════════════════════
 * spawn_food — place food at a random empty cell
 * ═══════════════════════════════════════════════════════════════════ */
static void spawn_food(void)
{
    uint8_t fx, fy;
    bool    on_snake;

    /* keep trying until we land on an empty cell */
    do {
        fx       = rand() % GRID_COLS;
        fy       = rand() % GRID_ROWS;
        on_snake = false;

        for (uint16_t i = 0; i < game.length; i++) {
            if (game.x[i] == fx && game.y[i] == fy) {
                on_snake = true;
                break;
            }
        }
    } while (on_snake);

    game.food_x = fx;
    game.food_y = fy;
    draw_cell(fx, fy, COLOR_FOOD);
}

/* ═══════════════════════════════════════════════════════════════════
 * check_self_collision — did head hit any body segment?
 * ═══════════════════════════════════════════════════════════════════ */
static bool check_self_collision(uint8_t hx, uint8_t hy)
{
    /* start from index 1 — skip the head itself */
    for (uint16_t i = 1; i < game.length; i++) {
        if (game.x[i] == hx && game.y[i] == hy)
            return true;
    }
    return false;
}

/* ═══════════════════════════════════════════════════════════════════
 * game_init — reset all state and draw initial frame
 * ═══════════════════════════════════════════════════════════════════ */
static void game_init(void)
{
    /* create mutex on first init, reuse on restart */
    if (game_mutex == NULL)
        game_mutex = xSemaphoreCreateMutex();

    xSemaphoreTake(game_mutex, portMAX_DELAY);

    memset(&game, 0, sizeof(game));

    /* start snake in the middle of the grid, length 3 */
    game.length   = 3;
    game.dir      = DIR_RIGHT;
    game.next_dir = DIR_RIGHT;
    game.score    = 0;
    game.state    = STATE_RUNNING;
    game.tick_ms  = 200;

    uint8_t start_x = GRID_COLS / 2;
    uint8_t start_y = GRID_ROWS / 2;

    /* head at start, body extends left */
    for (uint8_t i = 0; i < game.length; i++) {
        game.x[i] = start_x - i;
        game.y[i] = start_y;
    }

    /* clear screen */
    display_clearBuffer(display_handle);

    /* draw grid outline (optional, subtle) */
    for (uint8_t r = 0; r < GRID_ROWS; r++) {
        for (uint8_t c = 0; c < GRID_COLS; c++) {
            draw_cell(c, r, COLOR_GRID);
        }
    }

    /* draw initial snake */
    for (uint8_t i = 0; i < game.length; i++) {
        uint16_t color = (i == 0) ? COLOR_SNAKE_HEAD : COLOR_SNAKE_BODY;
        draw_cell(game.x[i], game.y[i], color);
    }

    /* spawn first food */
    spawn_food();
    draw_score();

    display_writeBuffer(display_handle);

    xSemaphoreGive(game_mutex);
}

/* ═══════════════════════════════════════════════════════════════════
 * game_tick — advance the game by one step
 * called every tick_ms milliseconds by the FreeRTOS timer
 * ═══════════════════════════════════════════════════════════════════ */
static void game_tick(void)
{
    xSemaphoreTake(game_mutex, portMAX_DELAY);

    if (game.state != STATE_RUNNING) {
        xSemaphoreGive(game_mutex);
        return;
    }

    /* apply queued direction */
    game.dir = game.next_dir;

    /* calculate new head position */
    int8_t new_hx = (int8_t)game.x[0];
    int8_t new_hy = (int8_t)game.y[0];

    switch (game.dir) {
        case DIR_UP:    new_hy -= 1; break;
        case DIR_DOWN:  new_hy += 1; break;
        case DIR_LEFT:  new_hx -= 1; break;
        case DIR_RIGHT: new_hx += 1; break;
    }

    /* ── wall collision ─────────────────────────────────────────── */
    if (new_hx < 0 || new_hx >= GRID_COLS ||
        new_hy < 0 || new_hy >= GRID_ROWS) {
        game.state = STATE_DEAD;
        xSemaphoreGive(game_mutex);
        draw_game_over();
        return;
    }

    /* ── self collision ─────────────────────────────────────────── */
    if (check_self_collision((uint8_t)new_hx, (uint8_t)new_hy)) {
        game.state = STATE_DEAD;
        xSemaphoreGive(game_mutex);
        draw_game_over();
        return;
    }

    bool ate_food = (new_hx == game.food_x && new_hy == game.food_y);

    /* ── erase tail (before shifting) ───────────────────────────── */
    if (!ate_food) {
        /* erase last segment only if not growing */
        draw_cell(game.x[game.length - 1],
                  game.y[game.length - 1],
                  COLOR_GRID);
    }

    /* ── shift body segments back ────────────────────────────────── */
    uint16_t new_length = ate_food ? game.length + 1 : game.length;

    /* shift from tail toward head */
    for (uint16_t i = new_length - 1; i > 0; i--) {
        game.x[i] = game.x[i - 1];
        game.y[i] = game.y[i - 1];
    }

    /* ── place new head ─────────────────────────────────────────── */
    game.x[0] = (uint8_t)new_hx;
    game.y[0] = (uint8_t)new_hy;
    game.length = new_length;

    /* ── redraw old head as body (segment 1) ─────────────────────── */
    if (game.length > 1) {
        draw_cell(game.x[1], game.y[1], COLOR_SNAKE_BODY);
    }

    /* ── draw new head ──────────────────────────────────────────── */
    draw_cell(game.x[0], game.y[0], COLOR_SNAKE_HEAD);

    /* ── handle food eaten ──────────────────────────────────────── */
    if (ate_food) {
        game.score += 10;

        /* speed up every 5 foods, minimum 80ms */
        if (game.score % 50 == 0 && game.tick_ms > 80) {
            game.tick_ms -= 20;
            xTimerChangePeriod(game_timer,
                               pdMS_TO_TICKS(game.tick_ms), 0);
        }

        draw_score();
        spawn_food();
    }

    display_writeBuffer(display_handle);

    xSemaphoreGive(game_mutex);
}

/* ═══════════════════════════════════════════════════════════════════
 * draw_game_over — flash the snake red, show game over
 * ═══════════════════════════════════════════════════════════════════ */
static void draw_game_over(void)
{
    /* flash entire snake red */
    for (uint16_t i = 0; i < game.length; i++) {
        draw_cell(game.x[i], game.y[i], COLOR_DEAD);
    }
    display_writeBuffer(display_handle);

    /* small delay so player sees the flash */
    vTaskDelay(pdMS_TO_TICKS(500));

    /* TODO: draw "GAME OVER" and score text
     * using your text widget once it's ready.
     * For now clear and print to serial.      */
    printf("\n\nGAME OVER — Score: %d\n", game.score);
    printf("Press SELECT to restart\n\n");
}

/* ═══════════════════════════════════════════════════════════════════
 * draw_start_screen
 * ═══════════════════════════════════════════════════════════════════ */
static void draw_start_screen(void)
{
    display_clearBuffer(display_handle);

    /* TODO: replace with text widget calls */
    printf("\n=== SNAKE ===\n");
    printf("Press SELECT to start\n");
    printf("Arrows to move\n");

    display_writeBuffer(display_handle);
}

/* ═══════════════════════════════════════════════════════════════════
 * game_timer_cb — fires every tick_ms, calls game_tick directly
 * owned by the snake application, no event manager involvement
 * ═══════════════════════════════════════════════════════════════════ */
static void game_timer_cb(TimerHandle_t xTimer)
{
    game_tick();
}

/* ═══════════════════════════════════════════════════════════════════
 * handle_key — process a keypad press during gameplay
 * ═══════════════════════════════════════════════════════════════════ */
static void handle_key(uint8_t key)
{
    xSemaphoreTake(game_mutex, portMAX_DELAY);

    if (game.state == STATE_DEAD) {
        xSemaphoreGive(game_mutex);
        if (key == KEY_SELECT) {
            xTimerStop(game_timer, 0);
            game_init();
            xTimerStart(game_timer, 0);
        }
        return;
    }

    /* direction keys — prevent 180° reversal */
    switch (key) {
        case KEY_UP:
            if (game.dir != DIR_DOWN)  game.next_dir = DIR_UP;
            break;
        case KEY_DOWN:
            if (game.dir != DIR_UP)    game.next_dir = DIR_DOWN;
            break;
        case KEY_LEFT:
            if (game.dir != DIR_RIGHT) game.next_dir = DIR_LEFT;
            break;
        case KEY_RIGHT:
            if (game.dir != DIR_LEFT)  game.next_dir = DIR_RIGHT;
            break;
        case KEY_SELECT:
            if (game.state == STATE_RUNNING) {
                game.state = STATE_PAUSED;
                xSemaphoreGive(game_mutex);
                xTimerStop(game_timer, 0);
                printf("\nPAUSED — press SELECT to resume");
                return;
            } else if (game.state == STATE_PAUSED) {
                game.state = STATE_RUNNING;
                xSemaphoreGive(game_mutex);
                xTimerStart(game_timer, 0);
                return;
            }
            break;
        default:
            break;
    }

    xSemaphoreGive(game_mutex);
}

/* ═══════════════════════════════════════════════════════════════════
 * app_main
 * ═══════════════════════════════════════════════════════════════════ */
void app_main(void)
{
    /* ── init subsystems ────────────────────────────────────────── */
    event_manager_init();
    hardware_init();
    display_handle = get_display_handle();
    shape_init(draw_pixel_wrapper);

    /* ── register keypad only — timer is app-owned ──────────────── */
    event_manager_register_event(EVENT_TYPE_KEYPAD, &keypad_cid);

    /* ── show start screen ──────────────────────────────────────── */
    draw_start_screen();

    /* wait for SELECT to start */
    EventDescription_t ev;
    while (1) {
        event_manager_wait_event(EVENT_TYPE_KEYPAD, keypad_cid, &ev);
        if (ev.keypad.action == KEYPAD_PRESSED &&
            ev.keypad.key_number == KEY_SELECT)
            break;
    }

    /* ── init game + mutex ──────────────────────────────────────── */
    game_mutex = NULL;   /* game_init will create it */
    game_init();

    /* ── create app-owned snake timer (auto-reload) ─────────────── */
    game_timer = xTimerCreate(
        "SnakeTick",
        pdMS_TO_TICKS(game.tick_ms),
        pdTRUE,             /* auto reload          */
        NULL,
        game_timer_cb       /* calls game_tick()    */
    );
    xTimerStart(game_timer, 0);

    /* ── main loop — only handles keypad input ───────────────────── */
    while (1) {
        event_manager_wait_event(EVENT_TYPE_KEYPAD, keypad_cid, &ev);
        if (ev.keypad.action == KEYPAD_PRESSED) {
            handle_key(ev.keypad.key_number);
        }
    }
}