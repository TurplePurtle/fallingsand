#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef enum {
  AIR = 0,
  SAND = 1,
  CONCRETE = 2,
  WATER = 3,
  ELEMENT_TYPE_COUNT
} ElementType;

typedef struct {
  ElementType type;
  int lastUpdate;
} Element;

static uint32_t ELEMENT_COLORS[ELEMENT_TYPE_COUNT] = {
    0xff000000,
    0xff00ffff,
    0xffffffff,
    0xffff5577,
};

Element element(ElementType type) {
  Element el;
  el.type = type;
  return el;
}

bool was_updated_on(Element *el, int frame) { return el->lastUpdate == frame; }
void mark_updated_on(Element *el, int frame) { el->lastUpdate = frame; }
