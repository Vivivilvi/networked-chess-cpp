---

# Networked 3D Chess Game

C++17 chess game with client-server architecture, multi-threading, and raw WinSock2 networking.

---
<img width="1919" height="1009" alt="изображение" src="https://github.com/user-attachments/assets/8b4ba6ef-ab58-4845-beea-4fa1281052b0" />

---

## 🚀 How to build and run (Windows, Visual Studio)

### 1. Open the project
- Open `K_Chess_Project.sln` in **Visual Studio 2019/2022**

### 2. Switch to x86 (important!)
- In the top toolbar, change `x64` → **`x86` (Win32)**

### 3. Build
- Press `Ctrl + Shift + B` (or Build → Build Solution)

### 4. Run the server (one terminal)
- Locate `ChessServer.exe` in `Debug/` or `Release/`
- Run:  
  `ChessServer.exe --port 8080`

### 5. Run the client (second terminal)
- Locate `Chess.exe`
- Run:  
  `Chess.exe --connect 127.0.0.1:8080`

> 💡 To play with a friend over the internet:  
> - Forward port `8080` on your router  
> - Friend runs: `Chess.exe --connect YOUR_PUBLIC_IP:8080`

## What it does

Two players connect to a server and play chess over TCP. Server validates all moves, clients only render and send input.

---

## Technical highlights

| Area | Implementation |
|------|----------------|
| Networking | WinSock2, TCP, custom binary protocol |
| Concurrency | std::thread per client + std::mutex for state |
| Architecture | Decoupled: Logic / Network / Render / StateManager |
| Rendering | 3D (OpenGL + GLUT/GLEW) |
| Validation | Server-side only — clients cannot cheat |

---

## Status

- ✅ Core chess logic complete (all FIDE rules)
- ✅ Multi-threaded server works
- ⏳ Inline comments: Ukrainian → English (in progress)

---

# ⚠️ Troubleshooting

**Error: `glut32.lib` unresolved** → You must build as **x86**, not x64.

**Error: `ChessServer.exe` not found** → Build the `ChessServer` project first.

--

## No external dependencies

- No Boost
- No Unity/Unreal
- No JSON parser — raw binary structs over sockets
- Only C++17 STL + Windows SDK

---
