# Terminal Music Visualizer (Linux)

A minimal music visualizer that runs directly in your terminal. It loads `.wav` files using SDL2 and displays real-time frequency bars using ncurses.

---
## 🔊 Output Preview

*(Insert your preview image or GIF here)*  
![Visualizer Preview](preview.gif)

---

## 🛠 Libraries & Tools Used

- [SDL2](https://www.libsdl.org/) — for loading and decoding `.wav` audio files  
- [ncurses](https://invisible-island.net/ncurses/) — for terminal-based visualization  
- `xwininfo`, `xdotool` — used to dynamically fetch terminal window dimensions

## 🧰 Installation / Setup

> ⚠️ These instructions are for **Debian-based Linux systems** (e.g., Ubuntu).

### 1. Install dependencies
```bash
sudo apt update
sudo apt install libsdl2-dev libncurses5-dev x11-utils xdotool---
```
### 2. Compile the project
```bash
make
```
### 3. Run the visualizer
```bash
./bin/visualizer path/to/music_file.wav
```
---

## ✅ TODO

- [ ] Fix high-frequency bar height issue  
   - Current average-based scaling flattens important peaks  
   - Consider using **maximum amplitude** or **median** within the frequency range instead  

---

Stay tuned for more updates and improvements!

