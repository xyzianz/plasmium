# plasmium
A Qt-based integration between chrome/chromium and the plasma desktop (as well as rofi)

State
------
This is a work-in-progress. It is constantly being rewritten on a day-by-day basis.

Youtube demos
----
[![Mute all background tabs youtube demonstration](https://i.ytimg.com/vi/jQE1cuYMpfY/maxresdefault.jpg)](https://youtu.be/jQE1cuYMpfY "Mute all background tabs youtube demonstration")

[![Switch between open tabs from rofi](https://i.ytimg.com/vi/br34XgtWE9g/maxresdefault.jpg)](https://youtu.be/br34XgtWE9g "Switch between open tabs from rofi")

[![Open new tabs via topsites or url from rofi](https://i.ytimg.com/vi/jI206-hqpao/maxresdefault.jpg)](https://youtu.be/jI206-hqpao "Open new tabs via topsites or url from rofi")

Browser support
------
* Chrome and Chromium
* Theoretically, any browser which implements WebExtensions with Chrome-compatible API:s. Opera and Firefox _may_ work, but it is not tested nor maintained.

Dependencies
------------
* Qt (5.5)
* CMake (build dependency)
* qmake (build dependency)

Build
-------
First, cd into and build /plasmium with `qmake . ; make && sudo make install`

Then (if you want plasma krunner integration) cd into and build /plasmiumrunner with `cmake . ; make ; make install`

Then (if you want rofi integration) cd into and build /plasmiumrofimodi with `qmake . ; make && sudo make install`

Then finally:
* install `/plasmiumextension/org.kde.plasmium.json` into either `/etc/chromium/native-messaging-hosts` or `/etc/opt/chrome/native-messaging-hosts` depending on your browser.
* open `chrome://extensions`, tick the `Developer mode` checkbox, press `Load unpacked extension...` and browse to the location of `plasmiumextension` and load.

What is working now
-------------------
* There is a dbus interface with working implementations to:
 * mute all tabs
 * mute all background tabs
 * unmute all tabs
 * unmute the current tab
 * Get a list of top sites from the browser
 * Get a list of open tabs and windows
 * Open a new tab with a specified URI
 * Switch to a specific tab and bring its window to the front
 * Get a list of all windows currently playing audio
* There is a rofi modi (a plugin for rofi, you will need to add it to your rofi setup/config yourself) which can open a new tab in your browser using a supplied uri, with rofi completion from the list of top sites, as well as switch active tab in the browser
* There is a non-working krunner plugin which will be able to switch between tabs in the browser

Contribute
---------
Yes, please! That's a very good idea!

Todo
-----
* Collect underpants
* Set up keybindings for plasma, gnome and xbindkeys
* Package chrome extension
* Use actual debugging instead of writing to a file
* Export bookmarks from the browser
* Export the browser history API
* Open a new window in the browser
* a taskbar plasmoid integration, which only paints the plasma 5.9 audio icons on chrome windows which actually plays sound
* a working krunner plugin to switch between tabs in the browser
* extend the rofi plugin to also complete based on browser bookmarks, the history API
* --Build a second rofi plugin which switches tabs in browser, just like the krunner plugin is supposed to do-- (Done!)
* bind the XInput media keys to playing media objects in the browser, such as youtube, spotify or google play music.
* ????
* Profit!

Other possibilities: (not sure if these are worth-while)
---------------------
* It is possible to open chrome settings directly via a simple dbus message (newTab: chrome://settings/). Subsections of the chrome settings are also possible to open, such as chrome://settings/passwords. This could be utilized in e.g. a kcm_chrome module for systemsettings or from rofi/krunner. 
