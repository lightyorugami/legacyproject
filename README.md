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
- [What Legacy Project changes on your PC](#what-legacy-project-changes-on-your-pc)
- [Network and privacy](#network-and-privacy)
- [Antivirus warnings](#antivirus-warnings)
- [Support](#support)

---

## Installation

1. Download the zip of the **latest `v…` release** (for example `3.2.7.zip`) from the [Releases](../../releases) page.
2. Extract it into the `bin\win_x64` folder of your Deceit installation (the folder that contains `Deceit.exe`).
3. Start Deceit as usual.

On first launch, the game downloads by itself what it still needs: the .NET runtime (`dotnet-3.1.32` release) and the event files (`events-…` releases). You do not need to download these manually.

---

## What is in a release

| Release | File | Content | How the game checks it |
|---|---|---|---|
| `v<version>` (e.g. `v3.2.7`) | `<version>.zip` | Legacy Project: startup DLL, `LegacyProject\` folder (fixes, PartyServer, tools) | Legacy Project signature, for that exact version |
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
- **Signature.** Anyone can check it with the public keys above and the layout described in [How the signature is stored](#how-the-signature-is-stored).

---

## What Legacy Project changes on your PC

Everything is in `Deceit\bin\win_x64`:

| File or folder | Role |
|---|---|
| `dinput8.dll` | Startup DLL. Deceit loads it at start; it forwards everything to Windows' own `dinput8.dll`, checks for updates, then loads Legacy Project. On any other version of the game, it does nothing. |
| `LegacyProject\DeceitFix.dll` | The game fixes and features, applied in memory |
| `LegacyProject\PartyServer\` | The party server, run by one of the players |
| `LegacyProject\dotnet\` | The .NET runtime used by the PartyServer only (never the one installed on your PC) |
| `LegacyProject\events\` | Event files |
| `LegacyProject\config.json` | Your settings (kept by updates) |
| `LegacyProject\repair.bat` | Reinstalls Legacy Project |
| `LegacyProject\uninstall.bat` | Removes Legacy Project |

The logs are written in `Deceit\logbackups` and stay on your PC.

Optional files you can create in `LegacyProject\` (kept by updates):

- `autoupdate.txt`: install updates without the update window
- `noupdate.txt`: no update check at all
- `novpncheck.txt`: skip the VPN check

To uninstall, run `LegacyProject\uninstall.bat`, or delete `dinput8.dll` and the `LegacyProject` folder.

---

## Network and privacy

- **Steam relays only.** Players connect to each other, and to the player who hosts the PartyServer, through Valve's Steam Datagram Relay network, addressed by Steam account and never by IP address. Your IP address is not sent to other players, including the host, and no direct connection to your PC is opened. No port forwarding or VPN is needed.
- **Protection against a modified host.** The host is a player like any other. What a host's PartyServer sends to your game is filtered by your own Legacy Project: it cannot make your game connect to an address of its choice.
- **Stored data.** Your profile (progression, cosmetics, friends, settings) is stored in the project's database under your Steam ID. Players' IP addresses are not kept there.
- **Diagnostics.** A diagnostic report (logs) is sent only when you choose to send one (Ctrl+Shift+F9 in game). Only the most recent reports are kept for each player.

---

## Antivirus warnings

Some antivirus products may flag Legacy Project, usually as a generic or "heuristic" detection. Common reasons:

- `dinput8.dll` is a proxy DLL loaded by the game, a technique also used by many game mods;
- it downloads and installs updates;
- the files are new and rarely seen, and they are not code-signed with a paid certificate.

Before trusting a file, check that its SHA-256 matches the one shown on this page's releases, then look it up on VirusTotal. If your antivirus flags a file from an official release, you can report it to the antivirus vendor as a false positive.

Only download Legacy Project from this repository's [Releases](../../releases) page. Copies posted anywhere else are not supported.

---

## Support

For help, bug reports or a security issue, contact **legacyhelper** on Discord. Please report security issues privately rather than in public channels.
