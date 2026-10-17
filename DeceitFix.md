# DeceitFix.dll: features

Current version: **v1.81**

`LegacyProject\DeceitFix.dll` holds the fixes and features of Legacy Project. It is loaded by the startup DLL (`dinput8.dll`) when the game starts and works **in memory only**: no file of the game is modified on disk. Before patching anything, it checks that the game files are exactly the legacy version (`Game.dll` 5.2.9): on any other version it changes nothing.

The first section lists everything the DLL does as of v1.79. Each later version adds a section at the bottom with what it adds or changes.

---

## v1.79

### Online play

- **Casual matchmaking.** The game's online button starts a search on the Legacy Project PartyServer, alone or as a group. Lobbies of 6 are formed, with the map chosen by a full group of 6, otherwise a random map. One player of the lobby hosts the match automatically and the others join it, with no manual LAN setup.
- **Ranked mode.** Separate ranked queue, with marks, ranks up to **Diamond** and the Elo leaderboard. Ranks, rank icons and Elo positions are shown in the player list, on name plates and on the loading screen. The Elo leaderboard line of the profile page shows your position and Elo.
- **Game server on the host's PC.** Each match runs in a separate game-server process started on the host's PC. The match is not tied to the host's own game: if the host leaves, the match can still be finished. The map rooms are loaded on that server so that capture zones, traps and blood bags work everywhere.
- **XP and levels.** Score lines show the XP earned ("+75 XP"), and the end-of-round bonuses count. The end screen shows Level Progress, Give Some Love and the match timeline. The loading screen shows the players' names, emblems and levels.
- **PartyServer started automatically.** The DLL starts the Legacy Project PartyServer in the background, with no window. When you close the game, Steam shows Deceit as stopped right away, even while a match you hosted is being finished.

### Profiles and loadouts

- **Your loadout is respected in matches:** terror skins and perks are no longer reset to the default by the host.
- **DLC terrors** (Werewolf, Vampire) can be selected: no padlock on the terror wheel.
- **The chosen terror is the one you transform into** (it no longer stays on the Experiment).

### Perk trees

- **3 additional perk trees** (on top of the game's 7), so that the skill tree can rotate every two weeks. Their layout is drawn correctly in the perk menu.

### Seasonal events

- **Halloween, Christmas, Easter and Summer:** decorated maps and main menu during the event, for every player of the match.
- **Halloween candies** in casual and ranked matches, converted into XP.
- **Double XP weekend:** the game's own "+100% WEEK-END EXP" banner, with the time left.
- **Event icon** in front of LOOT BOOTH during an event (can be turned off with `"eventMenuIcon": false` in `LegacyProject\config.json`).
- **Signed event files:** every event `.pak` carries the Legacy Project signature and is checked before the game opens it. A modified or unknown pak is refused.
- **Automatic download** of the event files (normally done by the startup DLL; DeceitFix.dll does it itself when the game was started by an older startup DLL).

### Diary pages

- **The diary pages (GM notes) are back** on Asylum, Forest and Arctic: one page per floor, first come first served, as in the original game. A diary you have completed cannot be picked up again.

### Main menu

- The **Discord** button asks before opening the Legacy Project contact (Legacy Helper).
- The former **Twitch** button becomes a **Buy Me a Coffee** button to support the project, with its own icon, and asks before opening the page.
- The questions are shown in the game's language (22 languages).

### Gameplay fixes

- **Terror abilities (right click) no longer stop working** after a terror was knocked down and picked up again (all 4 terrors). This also fixes the Vampire stuck in a vent (black screen, invincible, frozen cooldown).
- **Stuck in terror form after an execution** (cannot move, transform back or use abilities): fixed.
- **Infected teammate seen as innocent** (player list, red outline): fixed.
- **Double objectives:**
  - the item no longer needs to be "spammed" to be picked up from the plate;
  - the item can no longer be duplicated by two players using it at the same moment.
- **Tracker:**
  - fuse box icons no longer disappear during a blackout while a player is tracked;
  - a single tracker can no longer be placed 2 or 3 times by spamming the click.
- **No automatic health regeneration** in matches: the game's online rules apply (no regeneration, except while transforming and with the regeneration perk).
- **Inspection kit (scanner):** held correctly in first person, with the hands around it and a working screen.
- **Mouse and Escape no longer stop working** after the "game details" key was held when a match ended.
- **Terror skins:** the snowman Yeti's carrot nose (and similar head accessories) no longer appears in front of your own view.
- **Male characters with the pump shotgun** no longer slide without a walking animation (as seen by the other players).
- **Vampire movement sound:** the terror's sound plays instead of the human sprint breathing.
- **Spectating:** A / D switch to another player even when the view is on a fixed point (for example after the followed player escaped).

### Diagnostics and protection

- **Diagnostic report:** **Ctrl+Shift+F9** in game sends a report (logs) to the project, only when you choose to. A small window shows the progress. The diagnostic lines are kept in memory until then.
- **Crash files:** when the game crashes, a crash file with the call stack is written in `Deceit\logbackups`. It is sent automatically at the next launch, so that the crash can be fixed.
- **Profile protection:** the DLL gives the PartyServer a Steam proof that you are really playing (a Steam ticket made only for this purpose). The project's database uses it to undo any change made to the profile of a player who is not playing.

---

## v1.80

- **Event files from GitHub:** when DeceitFix.dll downloads the event files itself, they now come from the GitHub releases of the project. The download is accepted only if it carries the Legacy Project signature for the announced version.
- The event icon in front of LOOT BOOTH is now always shown during an event: the `eventMenuIcon` setting that hid it was removed.

## v1.81

- **Event files, stricter checks:** they are downloaded only from the GitHub hosts that serve the releases of the project (not from any `githubusercontent.com` address), and a file name that Windows would write under another name (for example ending with a dot, or a short name such as `GAME~1.DLL`) is refused.
- If the new event files cannot be put in place and the previous ones cannot be put back either, the event levels are no longer offered until the next start (instead of being offered without their files).
