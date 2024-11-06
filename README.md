# bluray-chapters
The source code and building repo for the `bd_chapters` command.
This command uses `libbluray` to dump the chpater infromation.

## building / running
```sh
go-task install:pacman:devtools
go-task container:build
go-task container:run:cdrom
```
