let Viewer = {
  print: (function () {
    return function (text) {
      text = Array.prototype.slice.call(arguments).join(" ");
      console.log(text);
    };
  })(),
  printErr: function (text) {
    text = Array.prototype.slice.call(arguments).join(" ");
    console.error(text);
  },
  canvas: (function () {
    var canvas = document.getElementById("canvas");
    canvas.addEventListener(
      "webglcontextlost",
      function (e) {
        alert("WebGL context lost, please reload the page");
        e.preventDefault();
      },
      false
    );
    canvas.getContext("webgl2", {
      alpha: false,
      depth: true,
      antialias: false,
      preserveDrawingBuffer: true,
    });
    return canvas;
  })(),
  setStatus: function (text) {
    if (text.length) {
      console.log("Status:", text);
    }
  },
  onRuntimeInitialized: function () {
    console.log("Viewer runtime initialized.");

    let canvas = document.getElementById("canvas");

    // focus on the canvas to grab keyboard inputs.
    canvas.setAttribute("tabindex", "0");

    // grab focus when the render window region receives mouse clicks.
    canvas.addEventListener("click", () => canvas.focus());
  },
  locateFile: function (path) {
    return `/wasm/${path}`;
  },
};

createViewer(Viewer).then(() => {
  console.log("Viewer instance initialized.");

  resizeCanvas();
});

//! Check browser support.
function isWasmSupported() {
  try {
    if (
      typeof WebAssembly === "object" &&
      typeof WebAssembly.instantiate === "function"
    ) {
      const aDummyModule = new WebAssembly.Module(
        Uint8Array.of(0x0, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00)
      );
      if (aDummyModule instanceof WebAssembly.Module) {
        return (
          new WebAssembly.Instance(aDummyModule) instanceof WebAssembly.Instance
        );
      }
    }
  } catch (e) {}
  return false;
}

if (!isWasmSupported()) {
  alert(
    "Your browser cannot support WebAssembly! Please install a modern browser."
  );
}
