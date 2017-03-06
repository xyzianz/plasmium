var port = chrome.runtime.connectNative('org.kde.plasmium');
var superWindowInfo = {};
var connected = true;

port.onDisconnect.addListener(function() {
  console.info("Disconnected.");
  connected = false;
});

chrome.runtime.onSuspend.addListener(function(){
    port.postMessage({command: "quit"});
    connected = false;
    port.disconnect();
});

port.onMessage.addListener(function(msg) {
    console.info("Received" + msg.command);
    switch (msg.command) {

        case 'list top sites':
        chrome.topSites.get(function(sites){
            var topsites = [];
            sites.forEach(function(site) {
                topsites.push({'@type': 'site', uri: site.url, title: site.title});
            });
            port.postMessage({command:'list of top sites', sites: topsites});
        });
        break;

        case 'list all tabs':
        chrome.windows.getAll({populate: true}, getAllWindowsCallback);
        break;

        case 'list all audible windows':
        chrome.windows.getAll({populate: true}, function(windows){
            var audibleWindows = [];
            windows.forEach(function(window){
                try {
                    var windowTitle = '';
                    var audible = false;
                    window.tabs.forEach(function(tab) {
                        if (tab.highlighted) {
                            console.info(tab.title + ' is highlighted');
                            windowTitle = tab.title;
                        }
                        if (tab.audible) {
                            console.info(tab.title + ' may be audible...');
                        }
                        if (tab.audible && !tab.mutedInfo.muted) {
                            console.info(tab.title + ' is audible');
                            audible = true;
                        }
                        if (windowTitle && audible) {
                            audibleWindows[audibleWindows.length] = windowTitle;
                            throw "break";
                        }
                    });
                    // in case we have reached the end of all tabs, and have found an audible tab but not a highlighted
                    if (audible) {
                        audibleWindows[audibleWindows.length] = window.id;
                    }
                } catch (e) {
                    // break;
                }
            });
            if (connected) {
                port.postMessage({command: 'list of all audible windows', windows: audibleWindows});
            }
        });
        break;

        case 'mute all tabs':
        chrome.windows.getAll({populate: true}, function(windows){
            windows.forEach(function(window){
                window.tabs.forEach(function(tab) {
                    if (tab.audible && !tab.mutedInfo.muted) {
                        chrome.tabs.update(tab.id, {muted: true});
                    }
                });
            });
        });
        break;

        case 'mute all background tabs':
        chrome.windows.getLastFocused(function(focused){
            chrome.windows.getAll({populate: true}, function(windows){
                windows.forEach(function(window){
                    window.tabs.forEach(function(tab) {
                        if (tab.audible && !tab.mutedInfo.muted && !(tab.highlighted && focused.id == window.id)) {
                            chrome.tabs.update(tab.id, {muted: true});
                        }
                    });
                });
            });
        });
        break;

        case 'unmute active tab':
        chrome.windows.getLastFocused({populate: true}, function(focused){
            focused.tabs.forEach(function(tab) {
                if (tab.mutedInfo.muted && tab.highlighted) {
                    chrome.tabs.update(tab.id, {muted: false});
                }
            });
        });
        break;

        case 'unmute all tabs':
        chrome.windows.getAll({populate: true}, function(windows){
            windows.forEach(function(window){
                window.tabs.forEach(function(tab) {
                    if (tab.mutedInfo.muted) {
                        chrome.tabs.update(tab.id, {muted: false});
                    }
                });
            });
        });
        break;
        
        case 'highlight tab':
        chrome.tabs.highlight({windowId: msg.windowId, tabs: msg.tabIndex});
        chrome.windows.update(msg.windowId, {focused: true, drawAttention: true});
        break;

        case 'new active tab':
        var tabObject = {active: true};
        if (msg.uri) {
            var parser = document.createElement('a');
            if (msg.uri.indexOf('://') == -1) {
                msg.uri = 'http://' + msg.uri;
            }
            tabObject.url = msg.uri;
        }
        chrome.tabs.create(tabObject, function(tab){
            chrome.windows.update(tab.windowId, {focused: true, drawAttention: true});
        });
        break;
    }
    if (connected) {
        port.postMessage({command: "ACK", ack: msg.command});
    }
});

port.onDisconnect.addListener(function() {
  console.info("Disconnected");
});


if (connected) {
    port.postMessage({command: "Helo."});
}
var second = 1000; //milliseconds
var minute = 60; //seconds
var hour   = 60; //minutes
setInterval(
    function(){
        if (connected) {
            port.postMessage({command: "ping"});
        }
    },
    5*minute*second
);

function getAllWindowsCallback(wins) {
    var windowList = {};
    wins.forEach(function(win) {
        windowList[win.id] = {
            '@type': 'window', 
            windowInfo: win,
        };
    });
    if (connected) {
        port.postMessage({command: 'list of tabs', list: windowList})
    }
}

function getAllTabsCallback(tabs) {
}
