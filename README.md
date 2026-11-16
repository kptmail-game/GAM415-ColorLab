# ColorLab415 — GAM 415 Week 4

## Week 4 procedural terrain

The default map is now `/Game/Week4/L_TerrainLab`. Open in Unreal Engine 5.8 and Play. WASD flies, mouse aims, E/Q moves vertically, left click digs, right click raises, and R resets the terrain. The level selects its own `TerrainLabMode`; earlier paint lab maps remain available.

`ProceduralTerrain` exposes XSize and YSize as editable cell counts, along with cell spacing, height, noise frequency/offset, and brush radius/depth. Editing properties rebuilds the preview. Perlin noise creates the initial topology. A visibility ray from the player's eyes selects the impact point; a smooth local-space brush modifies heights, refreshes normals/tangents, and updates the mesh section and collision. No projectile collision is substituted for the required player ray trace.

The blue tutorial plane has four hard-coded vertices, two triangles, and UVs. The blue cube reads LOD 0, section 0 from the project's CPU-readable copy of Epic's engine cube. Both displayed objects are procedural meshes. `Tools/build_week4_assets.py` creates the material, source cube, and generated test level; rerunning it replaces actors in that lab level. The material uses vertex color and an emissive fill for visibility without baked lighting.

Validation on November 16, 2026: Development Editor build and asset generation succeeded. Runtime counts were 3,185 vertices / 6,144 triangles for 64 x 48 cells, and 1,353 / 2,560 after resizing to 40 x 32. The copied cube contained 54 vertices / 48 triangles from its source asset. Nineteen successful player-ray edits covered digging and raising; reset and dimension restoration also completed. The screencast shows actual rendered gameplay and explains the source and asset script.

Optional `-TerrainCapture` runs the repeatable view-ray demonstration and records viewport frames in `Saved/CaptureWeek4`. This is disabled in normal play. The actor retains shared vertex/index arrays rather than duplicating triangle corners. Collision cooking is synchronous and tangent recalculation covers the full grid: this small editor-tested demonstration is not a packaged performance benchmark. Dimensions are bounded to prevent accidental oversized allocations. The optional nongraded slicing extension is not implemented.

## Previous particle lab

Unreal Engine 5.8 C++ project demonstrating color-matched Niagara paint bursts, projectile materials and impact decals, plus floating dust that changes color on player overlap. Week 2 cube and decal exercises remain available.

## Open and play

Install Unreal Engine 5.8 and its supported Visual Studio C++ toolchain. Clone with Git LFS installed, run `git lfs pull`, and open `ColorLab415.uproject`. Allow Unreal to build the editor module. Open `/Game/Week3/L_ParticleLab` to revisit the particle lab.

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
