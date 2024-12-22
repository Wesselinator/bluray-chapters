# bluray-chapters
The source code and building repo for the `bd_chapters` command.
This command uses `libbluray` to dump the chapter infromation.

## building

Install the tools for your os
* [go-task](https://taskfile.dev/installation/)
* [podman](https://podman.io/docs/installation)

> See the values of the `install:[PACKAGE_MANGER]:devtools` in `Taskfile.yaml`

## running

* `go-task container:run:cdrom`
* `go-task container:run:cdrom -p 1`
* `go-task container:run:dot . -f ffmpeg`
* `go-task container:run:root /home/me/blurays/title`

# Note

As far as I can tell, there is no label info on these chapters.
> Not even in the raw bluray data, but you can dig into that if you want to.

You will have to change the "Chapter XX" into whatever makes sense.  
e.g. `bd_chapters /dev/cdrom | sed 's/Chapter 1/Intro/' | sed 's/Chapter 2/Opening/' > chapters.text`
