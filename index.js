var Module = {
  onRuntimeInitialized: function () {
    /** @type {HTMLCanvasElement} */
    const canvas = document.getElementById("my-canvas");
    /** @type {HTMLSelectElement} */
    const elementType = document.getElementById("element-type-select");
    const stateButton = document.querySelector("#state");
    const ctx = canvas.getContext("2d");
    const WIDTH = canvas.width;
    const HEIGHT = canvas.height;
    const pixelCount = WIDTH * HEIGHT;
    const bufferPtr = Module.ccall("get_pixel_buffer", "number", [], []);
    const imageData = ctx.createImageData(WIDTH, HEIGHT);
    // View to WASM memory. Not a copy.
    const wasmBufferView = Module.HEAPU8.subarray(
      bufferPtr,
      bufferPtr + pixelCount * 4
    );

    function drawWasmFrame() {
      Module.ccall("loop_tick", null, ["number"], [frame]);
    }

    function renderBuffer() {
      imageData.data.set(wasmBufferView);
      ctx.putImageData(imageData, 0, 0);
    }

    Module.ccall("init", null, [], []);

    let animationFrame = 0;
    let frame = 0;
    let isMouseDown = false;
    let mouseX = 0;
    let mouseY = 0;

    function renderLoop() {
      frame++;
      if (isMouseDown) {
        Module.ccall(
          "create_element_at",
          null,
          ["number", "number", "number"],
          [mouseX, mouseY, Number(elementType.value)]
        );
      }
      drawWasmFrame();
      renderBuffer();
      animationFrame = requestAnimationFrame(renderLoop);
    }

    function playPause() {
      if (animationFrame) {
        cancelAnimationFrame(animationFrame);
        animationFrame = 0;
        stateButton.textContent = "Paused";
      } else {
        animationFrame = requestAnimationFrame(renderLoop);
        stateButton.textContent = "Playing";
      }
    }

    playPause();

    canvas.addEventListener("mousemove", (e) => {
      const rect = canvas.getBoundingClientRect();
      mouseX = e.clientX - rect.left;
      mouseY = e.clientY - rect.top;
    });
    canvas.addEventListener("mousedown", (e) => {
      isMouseDown = true;
    });
    window.addEventListener("mouseup", (e) => {
      isMouseDown = false;
    });

    document.addEventListener("keydown", (e) => {
      if (e.key === " ") playPause();
    });
    stateButton.addEventListener("click", (e) => {
      playPause();
    });
  },
};
