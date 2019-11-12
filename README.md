# EYEronic

![notificationf](https://raw.githubusercontent.com/ruinozeros/eyeronic/master/img/2019-05-28-23%3A14%3A27.png)

**Eyeronic** aims to reduce your eye strain by reminding you of taking breaks 
in periods of 20 minutes.

## Introduction

The main purpose of **Eyeronic** is to help users remind of break-taking and coffee-drinking.

> Never trust anyone who doesn't drink coffee.
> - AJ Lee


## Dependencies

- `libnotify`
- `glibmm`
- `libxss`
- `X11`
- `gio-2.0`
- `gobject-2.0`
- `glib-2.0`

Additionally, in order to show the notification itself, a notification deamon 
is needed.

The one I am using is called `dunst`.

## Building from source

I added the eclipse project files. Just import the project and build Release.
Then you should be able to run the following helper script

 ~~~ sh
  $ ./INSTALL.sh
 ~~~

This script copies the executable to `/usr/sbin/eyeronic`. It copies the example
config files to `$HOME/.config/eyeronic/`.

## Configuration
 
There is a config file in $HOME/.config/eyeronic/ which should be used to configure.

#### Polybar

For your polybar config, you could use something like this

~~~ sh
[module/coffee]
type = custom/script
exec = ~/.config/polybar/scripts/eyeronic.sh --cool
interval = 2
click-left = ~/.config/polybar/scripts/eyeronic.sh --toggle
~~~

eyeronic.sh can be found in my dotfiles repository:
[eyeronic.sh](https://github.com/ruinozeros/dotfiles/blob/master/polybar/scripts/eyeronic.sh)


## Launch the application
 
  ~~~ sh
  $ eyeronic --start
  ~~~
  
 See more commands by typing
 
  ~~~ sh
  $ eyeronic --help
  ~~~
  
## Background

The implementation consists of a deamon and a client which communicates via unix sockets. I implemented a few commands to control the deamon.
- start
- stop
- enable
- disable
- status
- toggle
- help

I found a code snippet for sending notifications to the deamon on the archlinux website:
[archlinux.org/desktop\_notifications](https://wiki.archlinux.org/index.php/Desktop_notifications#C++)


#### Created by
* Ruinozeros [github.com/ruinozeros](http://github.com/ruinozeros/)

