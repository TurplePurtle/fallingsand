#!/bin/bash

exported_fns_arr=(
  "_get_pixel_buffer"
  "_init"
  "_loop_tick"
  "_create_element_at"
)

exported_fns=$(IFS=,; echo "${exported_fns_arr[*]}")

emcc draw.c -O3 \
-s EXPORTED_FUNCTIONS="${exported_fns}" \
-s 'EXPORTED_RUNTIME_METHODS=["ccall","HEAPU8"]' \
-o draw.js
