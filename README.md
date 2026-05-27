# LoZim

_(Still in Alpha)_

Local [ZIM file](https://wiki.openzim.org/wiki/OpenZIM) search for KRunner 6. \
Instantly search through downloaded Wikis like Wikipedia or the ArchLinux-Wiki.

- [ArchWiki](https://browse.library.kiwix.org/#lang=eng&q=arch) ~35MiB
- [Wikipedia](https://browse.library.kiwix.org/#lang=eng&q=wikipedia) ~2 GiB to ~15GiB

## Install

just run `./install.sh`, it will build and copy the `.so` to `/lib/qt6/plugins/kf6/krunner/` as well as restart krunner.

## Dependencies

- [ECM](https://github.com/KDE/extra-cmake-modules/tree/master) (kf6)
- libzim

## TODO

- [x] Add icon
- [x] Add Action to visit online website
- [x] make trigger word customizable
- [x] publish Recognized Syntax
- [x] Support multiple archives
- [ ] Support configuration for archive folder (including symlinks?)
- [ ] fix base address for online website link (needs sth like /wiki/ or /title/) 
- [ ] Add Action to visit locally saved website
- [ ] Preview first sentence of article in KRunner
- [ ] is fuzzy find for zim possible?