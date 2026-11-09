# ColorLab415 — GAM 415 Week 3

Unreal Engine 5.8 C++ project demonstrating color-matched Niagara paint bursts, projectile materials and impact decals, plus floating dust that changes color on player overlap. Week 2 cube and decal exercises remain available.

## Open and play

Install Unreal Engine 5.8 and its supported Visual Studio C++ toolchain. Clone with Git LFS installed, run `git lfs pull`, and open `ColorLab415.uproject`. Allow Unreal to build the editor module. The startup map is `/Game/Week3/L_ParticleLab`.

WASD moves, mouse aims, left click fires, Space jumps, and F toggles a repeatable firing demonstration. Walk through either side cube to randomize its color and opacity independently.

Walk into the floating dust on the left side of the range to recolor it. Leave and reenter to choose another color. Each impact creates a short-lived paint burst whose material receives the same stored color as its projectile and decal.

## Implementation

- `ColorProjectile` stores one random color, gives the projectile its own dynamic material, and copies that exact value into the impact decal. Each hit randomly selects one of four original mask textures.
- `CubeDMIClass` creates a separate dynamic material per cube. Player overlaps change its Color and Opacity parameters.
- `ColorLabGameMode` supplies the first-person character, controls, demonstration firing, and HUD.
- `Content/Week2` includes the test map, three materials, and four textures. `AssetSources` contains original mask PNGs.
- `DustParticleZone` owns a trigger and Niagara component. Only player overlaps randomize its dynamic material's `ParticleColor` vector.
- `ParticleAssetTools` configures editor-only Niagara assets, the `User.ParticleMaterial` binding, paint sprite size, and compilation. Each runtime component receives its own material instance so independent shots cannot overwrite one another.
- `Tools/build_week3_assets.py` builds the particle materials, systems, and test map. The materials use the original splat mask and particle alpha, with color supplied by a dynamic material parameter. This uses a user material binding rather than a separate Niagara user color variable.
- The paint simulation adapts Epic's built-in Niagara `DirectionalBurst` system; dust adapts its `HangingParticulates` emitter. Paint uses enlarged unaligned sprites, and other template renderers are disabled. These engine templates are the simulation starting points, not original simulations.

Unreal's material and component APIs manage rendering resources through the engine's rendering abstraction; this project does not issue raw Direct3D commands. The Windows target uses DirectX 12.

## Validation

The Development Editor target compiled successfully in Unreal Engine 5.8. Runtime impact checks reported `MATCH=1` for copied color values, with all four mask indices observed. Both cube instances reported randomized color and opacity on player overlap. The submission video includes actual rendered gameplay and a source walkthrough.

Week 3 validation on November 9, 2026: the Development Editor build and asset generation completed successfully. All captured paint bursts reported `MATCH=1` and active systems. Two separate dust entries produced different colors while the system stayed active. The recording shows rendered gameplay followed by a narrated source walkthrough.

Optional `-LabCapture` captures rendered demonstration frames into `Saved/CaptureWeek3`, including two dust entries. Normal play does not capture images. Generated build files, caches, and recordings are excluded from version control; Unreal assets use Git LFS.
