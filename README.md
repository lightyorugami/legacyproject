# Legacy Project

**Legacy Project** is a community project that brings back online play for the legacy version of **Deceit**.

This repository hosts the official release files and the update manifest. The game downloads its updates **only from here**, and refuses any update that does not carry the Legacy Project signature.

> Legacy Project is a fan project. It is not affiliated with, endorsed by or supported by the developers or publishers of Deceit. Deceit and its assets belong to their respective owners.

---

## Contents

- [Installation](#installation)
- [What is in a release](#what-is-in-a-release)
- [How updates work](#how-updates-work)
- [Security](#security)
- [Check a file yourself](#check-a-file-yourself)
- [The bootstrapper (LegacyProject.exe)](#the-bootstrapper-legacyprojectexe)
- [What Legacy Project changes on your PC](#what-legacy-project-changes-on-your-pc)
- [Features of DeceitFix.dll](DeceitFix.md)
- [Network and privacy](#network-and-privacy)
- [Antivirus warnings](#antivirus-warnings)
- [Source code](#source-code)
- [Support](#support)

---

## Installation

> **Requirement: the legacy version of Deceit must be installed**. On the live version, Legacy Project stays inactive and the game starts without it.

Two ways to install Legacy Project. Both give the same result:

- [With the installer](#with-the-installer-recommended): one file to run, recommended for most players.
- [Manual installation](#manual-installation): you copy the files and set the launch option yourself.

### With the installer (recommended)

0. If it is not already done, in Steam, right-click Deceit → Properties → Betas, and select the Legacy version. Wait for the download to finish.
1. Download `LegacyProjectInstaller.cmd` from the **latest `installer-…` release** on the [Releases](../../releases) page.
2. Double-click it. If Windows SmartScreen shows "Windows protected your PC", click **More info → Run anyway**.
3. Follow the questions in the window, then start Deceit on Steam as usual.

What the installer does:

- finds Deceit in your Steam libraries and checks that it is the legacy version;
- downloads the latest release, the .NET runtime and the events pack from this repository, and **checks their signature or SHA-256 before installing anything** (the same checks as the game's own updates);
- sets the launch option `LegacyProject %command%` for you. Steam has to be closed for this, so the installer closes it and starts it again. Your other launch options are kept, and a backup of Steam's settings file is made (`localconfig.vdf.lpbak`).

Run it again at any time to **update**, **repair** (reinstall the latest version) or **uninstall** Legacy Project. Your `config.json` and option files are kept.

It does not download Deceit, only works with Deceit installed through Steam, needs no admin rights and writes no logs.

**Why a `.cmd` file?** For transparency. A `.cmd` file is plain text, not a compiled program: right-click it → **Edit** (or open it in Notepad) and you see the whole code that runs on your PC, nothing hidden. It is a PowerShell script that uses Windows' own built-in PowerShell, so there is nothing else to install. Its SHA-256 is given in the release notes: see [Check a file yourself](#check-a-file-yourself).

### Manual installation

For players who prefer to do everything themselves:

0. If it is not already done, in Steam, right-click Deceit → Properties → Betas, and select the Legacy version. Wait for the download to finish.
1. Download the zip of the **latest `v…` release** (for example `3.2.7.zip`) from the [Releases](../../releases) page.
2. Open your Deceit folder: in Steam, right-click Deceit → Manage → Browse local files.
3. Put the **content** of the zip into that folder: open the zip (double-click), select everything inside (`LegacyProject.exe` and the `bin` folder), and drag it into the Deceit folder. Windows merges the `bin` folder with the existing one.
4. In Steam, right-click Deceit → Properties → General → **Launch Options**, and type:
   ```
   LegacyProject %command%
   ```
   This launch option is what turns Legacy Project on: without it, Steam starts the original game.
5. Start Deceit on Steam as usual.

Your Deceit folder must look like this:

```
Deceit\
├── LegacyProject.exe           ← Legacy Project
├── ...                         (game files: already there)
└── bin\
    └── win_x64\
        ├── Deceit.exe          (the game: already there)
        ├── ...                 (other game files: already there)
        └── LegacyProject\      ← Legacy Project
            ├── dinput8.dll
            ├── DeceitFix.dll
            ├── PartyServer\
            ├── repair.bat
            └── ...
```

On first launch, the game downloads by itself what it still needs: the .NET runtime (`dotnet-3.1.32` release) and the event files (`events-…` releases). You do not need to download these manually.

### Turn Legacy Project on or off

The launch option decides, nothing else:

- **On:** `LegacyProject %command%` in the Launch Options of Deceit.
- **Off:** remove it from the Launch Options. Steam starts the original game, without Legacy Project. Nothing needs to be uninstalled: add the launch option again to play with Legacy Project.

How it works: see [The bootstrapper](#the-bootstrapper-legacyprojectexe).

---

## What is in a release

| Release | File | Content | How the game checks it |
|---|---|---|---|
| `v<version>` (e.g. `v3.2.7`) | `<version>.zip` | Legacy Project: `LegacyProject.exe`, and the `bin\win_x64\LegacyProject\` folder (startup DLL, fixes, PartyServer, tools) | Legacy Project signature, for that exact version |
| `events-<N>` (e.g. `events-2`) | `events_<N>.zip` | Seasonal content (maps and menus): `.pak` data files only, no executable code | Legacy Project signature of the zip, plus a signature on every `.pak` |
| `dotnet-3.1.32` | `dotnet.zip` | Official Microsoft .NET Core 3.1 runtime (x64), unmodified | Its SHA-256, built into the game's startup DLL |

`lp_update_manifest.txt` (in this branch) only announces the current versions, for example `3.2.7_e2` (package 3.2.7, events pack 2).

All releases are **immutable**: once published, their files and tags can never be changed or replaced, not even by the project owner.

---

## How updates work

At each launch, Legacy Project reads `lp_update_manifest.txt`. When a new version is available, a window opens **before anything is downloaded**:

- **Installed version** and **new version**
- **What's new**: the text of the GitHub release
- **Update now**: downloads the release, checks its signature, installs it, then starts the game
- **View release on GitHub**: opens the release page so you can look at it first; the window stays open
- **Quit**: closes the game, nothing is changed

The **Always install updates automatically** box skips this window for future updates. To get the window back, delete `LegacyProject\autoupdate.txt`.

The event files and the .NET runtime are data or Microsoft files, checked the same way, and are installed without asking.

---

## Security

Legacy Project only installs files that were made and signed by the project:

- **Signed updates.** Every update package and events pack is signed with a Legacy Project private key (ECDSA P-256). The signature covers every byte of the zip **and** its version. The game checks it before reading anything from the zip. A modified file, a file signed by someone else, or an older signed version given in place of the announced one is refused, and nothing is touched.
- **The key is not on GitHub.** The private keys are kept offline, protected by a passphrase, with a backup key stored separately. Even someone who took over this GitHub account, the manifest or any download link could not get the game to install their own files.
- **GitHub only.** Downloads are accepted only over HTTPS from GitHub (`github.com`, `api.github.com`, `*.githubusercontent.com`), checked again after redirections.
- **Immutable releases.** A published file can never be swapped afterwards.
- **No file of the game is modified.** Legacy Project works in memory while the game runs. The game files on disk stay exactly as Steam installed them.
- **Repair tool.** `LegacyProject\repair.bat` reinstalls Legacy Project from the same releases, with the same checks.

Public keys of the update signatures (not secret; built into the startup DLL):

```
main   9bd99ab6828acdda0596f5ac05e01d89deeedeb95c903a86720ba46356dbc80f2ad970728bdbad11b11bb9d9acb7b1c554110fc5158d5c9e0d2af19dc6bcee77
backup 5dd555139a667468028e2e97fe248df11ca3d620e4d32dcde8a00b61a21f6db661bfad6bc03e842af793bbbc08dae0a1ee57c26337787886bea79f64a0a5c4d0
```

(P-256 public keys, X followed by Y, in hexadecimal.)

### How the signature is stored

The signature sits in the comment at the end of the zip (112 bytes). The zip stays an ordinary zip that opens with Windows or 7-Zip:

| Bytes | Content |
|---|---|
| 0–7 | `LPUPD001` |
| 8 | `P` (package) or `E` (events pack) |
| 9 | key: `0` main, `1` backup |
| 10 | length of the version |
| 11–42 | version (ASCII, zero padded) |
| 43–47 | zero |
| 48–111 | ECDSA P-256 signature `r‖s` |

Signed data: `"LP-UPDATE-1\0"` + bytes 0–47 of the comment + SHA-256 of every byte of the zip before the comment length field.

---

## Check a file yourself

- **SHA-256.** GitHub shows the SHA-256 of every release file next to it. On Windows:
  ```
  certutil -hashfile 3.2.7.zip SHA256
  ```
- **VirusTotal.** Search that SHA-256 on [virustotal.com](https://www.virustotal.com), or upload the file.
- **.NET runtime.** Every DLL in `dotnet.zip` carries Microsoft's digital signature (right-click → Properties → Digital Signatures).
- **Installer.** `LegacyProjectInstaller.cmd` is not signed (it is the file that checks the others), but it is plain text: read it in Notepad, and compare its SHA-256 with the one in its release notes.
- **Signature.** Anyone can check it with the public keys above and the layout described in [How the signature is stored](#how-the-signature-is-stored).

---

## The bootstrapper (LegacyProject.exe)

`LegacyProject.exe`, in the Deceit folder, is the small program that the launch option `LegacyProject %command%` makes Steam start instead of the game. Its only job is to put the startup DLL (`dinput8.dll`) in place and start the game.

**Where `dinput8.dll` is kept.** When the game is not running, `dinput8.dll` stays in `bin\win_x64\LegacyProject`, where the game never loads it. It is next to `Deceit.exe` only while a game started through the launch option is running.

**What happens when you start Deceit:**

1. Steam starts `LegacyProject.exe`, which checks the version of the game (`bin\win_x64\Game.dll`).
2. **Legacy version:** it moves `dinput8.dll` next to `Deceit.exe`, then starts `bin\win_x64\Deceit.exe` with Steam's own arguments and a marker in its environment that tells Legacy Project it was started this way.
   **Any other version (live):** it makes sure our `dinput8.dll` is not next to `Deceit.exe`, then starts Steam's command unchanged.
3. It closes right away. It never stays in the background and never waits for the game.

**When the game ends.** Since the bootstrapper is already closed, it is the PartyServer, which Legacy Project runs alongside the game, that sees the game end and moves `dinput8.dll` back into `bin\win_x64\LegacyProject`. This works however the game ends, even if it crashes while closing.

**If `dinput8.dll` was left next to `Deceit.exe`** (PC turned off during a game, for example), nothing breaks:

- a game started **without** the launch option does not turn Legacy Project on: `dinput8.dll` sees the marker is missing, does nothing, and moves itself back;
- the next start **with** the launch option simply uses it, and the bootstrapper sorts it out (an older copy is moved aside so that a newer one installed by an update takes its place);
- on the live version, the bootstrapper moves it back before starting the game.

The bootstrapper makes no network connection, writes no log, needs no administrator rights and modifies no file of the game: it only moves our own `dinput8.dll`.

### Check it yourself

Its full source code is published in [`bootstrapper/`](bootstrapper), with the exact build script (`build.sh`) and build environment (`Dockerfile`: Ubuntu 24.04, MinGW-w64 GCC 13.2.0). The build is reproducible: the same source and the same compiler give the same file, byte for byte. To check that the `LegacyProject.exe` of a release is really built from this source:

1. Build it yourself, for example with Docker:
   ```
   cd bootstrapper
   docker build -t lp-bootstrapper .
   docker run --rm -v "$PWD:/src" lp-bootstrapper
   ```
   (or in Ubuntu 24.04 / WSL: `sudo apt install g++-mingw-w64-x86-64-win32` then `sh build.sh`). It prints the SHA-256 of the file it built.
2. Compare it with the SHA-256 of the `LegacyProject.exe` from the release zip (on Windows: `certutil -hashfile LegacyProject.exe SHA256`). The SHA-256 of each release's `LegacyProject.exe` is also written in its release notes.

---

## What Legacy Project changes on your PC

| File or folder | Role |
|---|---|
| `Deceit\LegacyProject.exe` | The [bootstrapper](#the-bootstrapper-legacyprojectexe): started by Steam through the launch option, puts `dinput8.dll` in place, starts the game, then closes. |
| `bin\win_x64\LegacyProject\dinput8.dll` | Startup DLL. Next to `Deceit.exe` only while the game runs. The game loads it at start; it forwards everything to Windows' own `dinput8.dll`, checks for updates, then loads Legacy Project. In a game not started through the launch option, or on another version of the game, it does nothing. |
| `bin\win_x64\LegacyProject\DeceitFix.dll` | The game fixes and features, applied in memory: see the [full list of features](DeceitFix.md) |
| `bin\win_x64\LegacyProject\PartyServer\` | The party server, run by one of the players |
| `bin\win_x64\LegacyProject\dotnet\` | The .NET runtime used by the PartyServer only (never the one installed on your PC) |
| `bin\win_x64\LegacyProject\events\` | Event files |
| `bin\win_x64\LegacyProject\config.json` | Your settings (kept by updates) |
| `bin\win_x64\LegacyProject\repair.bat` | Reinstalls Legacy Project |
| `bin\win_x64\LegacyProject\uninstall.bat` | Removes Legacy Project |
| Steam: `userdata\<account>\config\localconfig.vdf` | Only if you use the installer: it adds the launch option of Deceit there (backup: `localconfig.vdf.lpbak`) |

The logs are written in `Deceit\logbackups` and stay on your PC.

Optional files you can create in `bin\win_x64\LegacyProject\` (kept by updates):

- `autoupdate.txt`: install updates without the update window
- `noupdate.txt`: no update check at all

To uninstall, run the [installer](#with-the-installer-recommended) and choose **Uninstall** (it also removes the launch option), or run `bin\win_x64\LegacyProject\uninstall.bat` (or delete `LegacyProject.exe` and the `bin\win_x64\LegacyProject` folder) and, in these two cases, remove `LegacyProject %command%` from the Launch Options.

---

## Network and privacy

- **Steam relays only.** Players connect to each other, and to the player who hosts the PartyServer, through Valve's Steam Datagram Relay network, addressed by Steam account and never by IP address. Your IP address is not sent to other players, including the host, and no direct connection to your PC is opened. No port forwarding is needed, and a VPN does not get in the way. Curious about how it works? See Valve's [Steam Datagram Relay documentation](https://partner.steamgames.com/doc/features/multiplayer/steamdatagramrelay).
- **Protection against a modified host.** The host is a player like any other. What a host's PartyServer sends to your game is filtered by your own Legacy Project: it cannot make your game connect to an address of its choice.
- **Stored data.** Your profile (progression, cosmetics, friends, settings) is stored in the project's database under your Steam ID. Players' IP addresses are not kept there.
- **Diagnostics.** A diagnostic report (logs) is sent when you press Ctrl+Shift+F9 in game. To help fix bugs, two kinds of logs are also sent automatically: the crash file of a game that crashed (at the next launch), and the log of the match server when you hosted a match. Only the most recent reports are kept for each player.

---

## Antivirus warnings

Some antivirus products may flag Legacy Project, usually as a generic or "heuristic" detection. Common reasons:

- `dinput8.dll` is a proxy DLL loaded by the game, a technique also used by many game mods;
- it downloads and installs updates;
- the files are new and rarely seen, and they are not code-signed with a paid certificate.

Before trusting a file, check that its SHA-256 matches the one shown on this page's releases, then look it up on VirusTotal. If your antivirus flags a file from an official release, you can report it to the antivirus vendor as a false positive.

Only download Legacy Project from this repository's [Releases](../../releases) page. Copies posted anywhere else are not supported.

---

## Source code

Legacy Project is not open source, and I do not plan to make it open source. The only exception is the bootstrapper, published in [`bootstrapper/`](bootstrapper) so that anyone can rebuild it and compare it with the released file. The installer is a readable script too: its code is the `.cmd` file itself.

That said, you are welcome to check what the files of Legacy Project do, with any tool you like: decompilers, disassemblers, antivirus scans, VirusTotal, or AI assistants. I have no problem with that.

---

## Support

For help, bug reports or a security issue, contact **legacyhelper** on Discord. Please report security issues privately rather than in public channels.
