# Terminal Music Visualizer (Linux)

A minimal music visualizer that runs directly in your terminal. It loads `.wav` files using SDL2 and displays real-time frequency bars using ncurses.

---

## 🛠 Libraries & Tools Used

- [SDL2](https://www.libsdl.org/) — for loading and decoding `.wav` audio files  
- [ncurses](https://invisible-island.net/ncurses/) — for terminal-based visualization  
- `xwininfo`, `xdotool` — used to dynamically fetch terminal window dimensions

---

## 🔊 Output Preview

*(Insert your preview image or GIF here)*  
![Visualizer Preview](preview.gif)

---

## ✅ TODO

- [ ] Fix high-frequency bar height issue  
   - Current average-based scaling flattens important peaks  
   - Consider using **maximum amplitude** or **median** within the frequency range instead  

---

Stay tuned for more updates and improvements!

