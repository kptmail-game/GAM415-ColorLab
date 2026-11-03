# ColorLab415 — GAM 415 Week 2

Unreal Engine 5.8 C++ project demonstrating per-projectile dynamic materials, color-matched impact decals, randomized splat masks, and overlap-driven cube materials.

## Open and play

Install Unreal Engine 5.8 and its supported Visual Studio C++ toolchain. Clone with Git LFS installed, run `git lfs pull`, and open `ColorLab415.uproject`. Allow Unreal to build the editor module. The startup map is `/Game/Week2/L_ColorLab`.

WASD moves, mouse aims, left click fires, Space jumps, and F toggles a repeatable firing demonstration. Walk through either side cube to randomize its color and opacity independently.

## Implementation

- `ColorProjectile` stores one random color, gives the projectile its own dynamic material, and copies that exact value into the impact decal. Each hit randomly selects one of four original mask textures.
- `CubeDMIClass` creates a separate dynamic material per cube. Player overlaps change its Color and Opacity parameters.
- `ColorLabGameMode` supplies the first-person character, controls, demonstration firing, and HUD.
- `Content/Week2` includes the test map, three materials, and four textures. `AssetSources` contains original mask PNGs.

Unreal's material and component APIs manage rendering resources through the engine's rendering abstraction; this project does not issue raw Direct3D commands. The Windows target uses DirectX 12.

## Validation

The Development Editor target compiled successfully in Unreal Engine 5.8. Runtime impact checks reported `MATCH=1` for copied color values, with all four mask indices observed. Both cube instances reported randomized color and opacity on player overlap. The submission video includes actual rendered gameplay and a source walkthrough.

Optional `-LabCapture` captures rendered demonstration frames into `Saved/Capture` for the walkthrough. Normal play does not capture images. Generated build files, caches, and recordings are excluded from version control; Unreal assets use Git LFS.
