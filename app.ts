import { el, Renderer } from '@elemaudio/core';
import { dlopen, FFIType, suffix, ptr } from "bun:ffi";

const makeStr = (text: string) => ptr(Buffer.from(`${text}\0`, "utf8"))

const path = `build/myaudiolib.${suffix}`;

const {
  symbols: {
    app_init_audio,
    app_uninit_audio,
    elem_post_instructions
  },
} = dlopen(
  path,
  {
    app_init_audio: {
      args: [],
      returns: FFIType.void,
    },
    app_uninit_audio: {
      args: [],
      returns: FFIType.void,
    },
    elem_post_instructions: {
      args: [FFIType.cstring],
      returns: FFIType.void,
    },
  },
);


app_init_audio()

const renderer = new Renderer((message) => {
  if (message.length === 1 && message[0][0] === 5) return
  elem_post_instructions(makeStr(JSON.stringify(message)))
})

let stats = await renderer.render(
  el.mul(0.3, el.cycle(440)),
  el.mul(0.3, el.cycle(441)),
);


await new Promise((resolve) => setTimeout(resolve, 1000));

app_uninit_audio()

console.log("shutdown");