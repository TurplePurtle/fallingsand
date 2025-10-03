#include "element.h"
#include "randomize.h"
#include <emscripten.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define WIDTH 320
#define HEIGHT 240

#define ELEMENT_COUNT (WIDTH * HEIGHT)

static uint32_t pixel_buffer[ELEMENT_COUNT];
static Element elements[ELEMENT_COUNT];
static int *randlet = NULL;

Element *get_element_at(int x, int y) {
  if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
    return NULL;
  }
  return &elements[y * WIDTH + x];
}

void set_element_at(int x, int y, Element el) {
  if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
    return;
  }
  elements[y * WIDTH + x] = el;
}

void swap_elements_at(int x1, int y1, int x2, int y2, int frame_number) {
  Element *el1_ptr = get_element_at(x1, y1);
  if (!el1_ptr)
    return;
  Element *el2_ptr = get_element_at(x2, y2);
  if (!el2_ptr)
    return;

  Element el1 = *el1_ptr;
  Element el2 = *el2_ptr;
  set_element_at(x1, y1, el2);
  set_element_at(x2, y2, el1);
  mark_updated_on(el1_ptr, frame_number);
  mark_updated_on(el2_ptr, frame_number);
}

ElementType type_at(int x, int y) {
  Element *el = get_element_at(x, y);
  return el ? el->type : AIR;
}

EMSCRIPTEN_KEEPALIVE
uint8_t *get_pixel_buffer() { return (uint8_t *)pixel_buffer; }

EMSCRIPTEN_KEEPALIVE
void init() {
  srand(42);
  randlet = generate_random_list(ELEMENT_COUNT);

  int y = 229;
  for (int x = 0; x < WIDTH; x++) {
    set_element_at(x, y, element(CONCRETE));
  }
}

EMSCRIPTEN_KEEPALIVE
void create_element_at(int x, int y, ElementType type) {
  set_element_at(x, y, element(type));
}

void update_elements(int frame_number) {
  for (int i = 0; i < ELEMENT_COUNT; i++) {
    int j = randlet[i];
    int y = i / WIDTH;
    int x = i % WIDTH;
    Element *el_ptr = get_element_at(x, y);
    if (!el_ptr)
      continue;
    if (was_updated_on(el_ptr, frame_number)) {
      continue;
    }
    switch (el_ptr->type) {
    case SAND:
      if (type_at(x, y + 1) == AIR || type_at(x, y + 1) == WATER) {
        swap_elements_at(x, y, x, y + 1, frame_number);
      } else if (type_at(x - 1, y + 1) == AIR ||
                 type_at(x - 1, y + 1) == WATER) {
        swap_elements_at(x, y, x - 1, y + 1, frame_number);
      } else if (type_at(x + 1, y + 1) == AIR ||
                 type_at(x + 1, y + 1) == WATER) {
        swap_elements_at(x, y, x + 1, y + 1, frame_number);
      }
      break;
    case WATER:
      if (type_at(x, y + 1) == AIR) {
        swap_elements_at(x, y, x, y + 1, frame_number);
      } else if (type_at(x - 1, y + 1) == AIR) {
        swap_elements_at(x, y, x - 1, y + 1, frame_number);
      } else if (type_at(x + 1, y + 1) == AIR) {
        swap_elements_at(x, y, x + 1, y + 1, frame_number);
      } else {
        bool has_air_left = type_at(x - 1, y) == AIR;
        bool has_air_right = type_at(x + 1, y) == AIR;
        if (has_air_left) {
          if (!has_air_right) {
            swap_elements_at(x, y, x - 1, y, frame_number);
          }
        } else if (has_air_right) {
          swap_elements_at(x, y, x + 1, y, frame_number);
        }
      }
      break;
    case AIR:
    case CONCRETE:
    case ELEMENT_TYPE_COUNT:
      break;
    }
  }
}

void render_elements() {
  for (int i = 0; i < ELEMENT_COUNT; i++) {
    pixel_buffer[i] = ELEMENT_COLORS[elements[i].type];
  }
}

EMSCRIPTEN_KEEPALIVE
void loop_tick(int frame_number) {
  update_elements(frame_number);
  render_elements();
}

int main() { return 0; }
