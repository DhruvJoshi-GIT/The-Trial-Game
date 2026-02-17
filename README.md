# NEON ASCENSION

> *An anime-inspired action combat game set in a cyberpunk utopia of 2099*

![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5.7-blue)
![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey)
![Status](https://img.shields.io/badge/Status-In%20Development-orange)

## Overview

Neon Ascension is a stylish, Dragon Ball Z-inspired action combat game where players possess godlike powers from the start. The challenge is mastering combat mechanics, not grinding for abilities.

### Core Design Philosophy
- **All powers unlocked from start** - Player is already powerful
- **HP is the only progression** - Grind to survive harder content
- **Style over stats** - Flashy, smooth, responsive combat
- **Dense verticality** - Flying and aerial combat are core

---

## Getting Started

### Prerequisites

- **Unreal Engine 5.7** (installed via Epic Games Launcher)
- **Visual Studio 2022** with C++ game development workload
- **Windows 10/11** (64-bit)
- **16GB RAM minimum** (32GB recommended)
- **GPU with DirectX 12 support**

### Installation

1. Clone this repository:
   ```bash
   git clone https://github.com/yourusername/NeonAscension.git
   ```

2. Right-click `NeonAscension.uproject` and select **"Generate Visual Studio project files"**

3. Open the generated `.sln` file in Visual Studio 2022

4. Build the project (F7) in **Development Editor** configuration

5. Launch from Visual Studio (F5) or double-click `NeonAscension.uproject`

### Running the Game

**From Editor:**
1. Open `NeonAscension.uproject` in Unreal Engine
2. Wait for shaders to compile (first time only)
3. Press **Play** in the editor

**Standalone:**
1. File → Package Project → Windows (64-bit)
2. Run the generated executable

---

## Controls

| Action | Keyboard | Gamepad |
|--------|----------|---------|
| Move | WASD | Left Stick |
| Look | Mouse | Right Stick |
| Jump | Space | A |
| Fly (hold in air) | Space | A (hold) |
| Dash | Shift | LB |
| Light Attack | LMB | X |
| Heavy Attack | RMB | Y |
| Charge Ki | E | RT |
| Beam (while charging) | RMB | Y |
| Burst (while charging) | LMB | X |

---

## Game Architecture

### High-Level System Diagram

```mermaid
graph TB
    subgraph Core["Core Systems"]
        GM[Game Mode]
        GI[Game Instance]
        PS[Player State]
    end

    subgraph Characters["Character System"]
        PC[Player Character]
        EC[Enemy Character]
        BC[Base Character]
    end

    subgraph Combat["Combat System"]
        DM[Damage Manager]
        CB[Combo System]
        HB[Hitbox Manager]
        AB[Ability System]
    end

    subgraph Effects["VFX System"]
        NI[Niagara Effects]
        AU[Aura System]
        BM[Beam Effects]
        PT[Particles]
    end

    subgraph AI["AI System"]
        BT[Behavior Tree]
        BB[Blackboard]
        AI_C[AI Controller]
    end

    subgraph UI["UI System"]
        HUD[HUD Widget]
        MM[Main Menu]
        PM[Pause Menu]
        GO[Game Over]
    end

    GM --> PC
    GM --> EC
    BC --> PC
    BC --> EC
    PC --> AB
    EC --> AB
    AB --> DM
    AB --> CB
    DM --> HB
    PC --> NI
    NI --> AU
    NI --> BM
    NI --> PT
    EC --> AI_C
    AI_C --> BT
    BT --> BB
    GM --> HUD
    PS --> HUD
```

### Character State Machine

```mermaid
stateDiagram-v2
    [*] --> Idle

    Idle --> Running: Move Input
    Running --> Idle: Stop Moving

    Idle --> Jumping: Jump
    Running --> Jumping: Jump
    Jumping --> Falling: Apex Reached
    Falling --> Idle: Land

    Jumping --> Flying: Hold Jump (Air)
    Falling --> Flying: Hold Jump
    Flying --> Falling: Release Jump
    Flying --> Falling: Stamina Empty

    Idle --> Dashing: Dash Input
    Running --> Dashing: Dash Input
    Flying --> Dashing: Dash Input
    Dashing --> Idle: Timer End (Ground)
    Dashing --> Falling: Timer End (Air)

    Idle --> Attacking: Attack Input
    Running --> Attacking: Attack Input
    Attacking --> Idle: Combo End
    Attacking --> Attacking: Continue Combo

    Idle --> Charging: Charge Input
    Charging --> Idle: Release Charge
    Charging --> BeamAttack: Heavy + Ki >= 40
    Charging --> BurstAttack: Light + Ki >= 30

    BeamAttack --> Idle: Beam End
    BurstAttack --> Idle: Burst End

    Idle --> Stunned: Take Damage
    Stunned --> Idle: Stun End

    Idle --> Dead: HP <= 0
    Running --> Dead: HP <= 0
    Flying --> Dead: HP <= 0
    Dead --> [*]
```

### Combat Flow

```mermaid
sequenceDiagram
    participant P as Player
    participant I as Input System
    participant C as Combat Manager
    participant H as Hitbox
    participant E as Enemy
    participant D as Damage System
    participant V as VFX System

    P->>I: Press Attack
    I->>C: Attack Input Event
    C->>P: Set State: Attacking
    C->>V: Play Attack Animation
    C->>H: Activate Hitbox

    H->>E: Overlap Detection
    H->>D: Calculate Damage
    D->>E: Apply Damage
    D->>E: Apply Knockback
    D->>V: Spawn Hit Effects
    V->>V: Screen Shake
    V->>V: Damage Numbers

    E->>E: Check Health
    alt HP <= 0
        E->>E: Die
        E->>V: Death Effects
    else HP > 0
        E->>E: Stagger
    end
```

### Ki Charge & Aura System

```mermaid
flowchart TD
    subgraph Input
        A[Hold E Key]
    end

    subgraph ChargeSystem
        B[Start Charging]
        C[Increase Ki]
        D[Update Charge Intensity]
    end

    subgraph AuraEffects["Niagara Aura System"]
        E[Inner Glow Sphere]
        F[Outer Aura Sphere]
        G[Flame Wisps]
        H[Lightning Bolts]
        I[Floating Particles]
        J[Ground Cracks]
        K[Rising Ring]
        L[Debris]
    end

    subgraph Parameters
        M[Charge Intensity 0-1]
        N[Flame Height]
        O[Particle Spawn Rate]
        P[Lightning Frequency]
    end

    A --> B
    B --> C
    C --> D
    D --> M
    M --> E
    M --> F
    M --> G
    M --> H
    M --> I
    M --> J
    M --> K
    M --> L
    D --> N
    D --> O
    D --> P
```

### Enemy AI Behavior Tree

```mermaid
flowchart TD
    ROOT[Root] --> SEL1[Selector]

    SEL1 --> SEQ1[Sequence: Dead Check]
    SEQ1 --> DEAD{Is Dead?}
    DEAD -->|Yes| STOP[Stop All]

    SEL1 --> SEQ2[Sequence: Attack]
    SEQ2 --> RANGE{In Attack Range?}
    RANGE -->|Yes| COOLDOWN{Cooldown Ready?}
    COOLDOWN -->|Yes| ATTACK[Perform Attack]

    SEL1 --> SEQ3[Sequence: Chase]
    SEQ3 --> DETECT{Player Detected?}
    DETECT -->|Yes| MOVE[Move To Player]

    SEL1 --> IDLE[Idle/Patrol]
```

---

## Project Structure

```
NeonAscension/
├── Config/
│   ├── DefaultEngine.ini
│   ├── DefaultGame.ini
│   ├── DefaultInput.ini
│   └── DefaultEditor.ini
├── Content/
│   ├── Characters/
│   │   ├── Player/
│   │   └── Enemies/
│   ├── Effects/
│   │   ├── Aura/
│   │   ├── Beam/
│   │   └── Particles/
│   ├── Maps/
│   │   └── CombatArena/
│   └── UI/
│       ├── HUD/
│       └── Menus/
├── Source/
│   └── NeonAscension/
│       ├── Characters/
│       │   └── NeonCharacterBase.h/.cpp
│       ├── GameModes/
│       │   └── NeonGameMode.h/.cpp
│       ├── NeonAscension.h/.cpp
│       └── NeonAscension.Build.cs
├── NeonAscension.uproject
└── README.md
```

---

## Development Workflow

```mermaid
gitGraph
    commit id: "Initial Setup"
    branch feature/character
    checkout feature/character
    commit id: "Base Character Class"
    commit id: "Movement System"
    commit id: "Combat Basics"
    checkout main
    merge feature/character
    branch feature/aura-effects
    checkout feature/aura-effects
    commit id: "Niagara Aura System"
    commit id: "Flame Wisps"
    commit id: "Lightning Effects"
    checkout main
    merge feature/aura-effects
    branch feature/enemies
    checkout feature/enemies
    commit id: "Base Enemy AI"
    commit id: "Behavior Trees"
    checkout main
    merge feature/enemies
    branch feature/arena
    checkout feature/arena
    commit id: "Combat Arena Level"
    commit id: "Wave System"
    checkout main
    merge feature/arena
    commit id: "Release v0.1"
```

---

## Implementation Roadmap

### Phase 1: Core Combat (Current)
- [x] Project Setup
- [x] Base Character Class
- [x] Game Mode
- [ ] Player Blueprint
- [ ] Movement System (Ground/Air/Flight)
- [ ] Basic Attack Combos
- [ ] Ki Charging System

### Phase 2: VFX & Polish
- [ ] Niagara Aura System
- [ ] Beam Attack Effect
- [ ] Burst Wave Effect
- [ ] Hit Effects & Screen Shake
- [ ] Character Rim Glow

### Phase 3: Enemies & AI
- [ ] Base Enemy Class
- [ ] Behavior Tree AI
- [ ] Enemy Types (Grunt, Elite, Boss)
- [ ] Difficulty Scaling

### Phase 4: Arena & Progression
- [ ] Combat Arena Level
- [ ] Wave Spawning System
- [ ] HP Upgrade System
- [ ] Game Over / Victory

### Phase 5: Polish
- [ ] Main Menu
- [ ] HUD
- [ ] Audio
- [ ] Animations

---

## Technical Notes

### Key Plugins Used
- **Enhanced Input** - Modern input handling
- **Gameplay Abilities** - Ability system framework
- **Niagara** - Advanced particle/VFX system
- **Control Rig** - Procedural animation

### Performance Targets
- 60 FPS at 1080p (minimum)
- 60 FPS at 1440p (recommended)
- Support for 10+ enemies on screen

---

## Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

---

## License

This project is for educational/personal use.

---

## Acknowledgments

- Inspired by Dragon Ball Z: Kakarot
- Built with Unreal Engine 5.7
- Combat inspired by Ghost of Tsushima & Devil May Cry

---

## Author

**Dhruv Joshi**

[![LinkedIn](https://img.shields.io/badge/LinkedIn-0A66C2?style=for-the-badge&logo=linkedin&logoColor=white)](https://www.linkedin.com/in/dhruv-joshi-52769b265/)
[![X](https://img.shields.io/badge/X-000000?style=for-the-badge&logo=x&logoColor=white)](https://x.com/mdhruvjoshi)
[![Email](https://img.shields.io/badge/Email-EA4335?style=for-the-badge&logo=gmail&logoColor=white)](mailto:mdhruvjoshi@gmail.com)
[![Discord](https://img.shields.io/badge/Discord-5865F2?style=for-the-badge&logo=discord&logoColor=white)](https://discord.com/users/dhruvjoshi.28)
