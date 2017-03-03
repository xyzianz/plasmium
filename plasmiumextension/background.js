var port = chrome.runtime.connectNative('org.kde.plasmium');
var superWindowInfo = {};

port.onMessage.addListener(function(msg) {
  console.log("Received" + msg.text);
  switch (msg.text) {
      case 'list':
        chrome.windows.getAll(getAllWindowsCallback);
        break;

      case 'activate':
        chrome.tabs.highlight({windowId: msg.windowId, tabs: msg.tabIndex});
        break;
  }
//  port.postMessage({ text: "ECHO: " + msg.text });
});

port.onDisconnect.addListener(function() {
  console.log("Disconnected");
});

port.postMessage({ text: "HELO Plasmium" });

setInterval((function(){port.postMessage({ text: "HOLA Plasmium" });}), 60000);

function getAllWindowsCallback(wins) {
    wins.forEach(function(win) {
        superWindowInfo[win.id] = win;
        chrome.tabs.getAllInWindow(win.id, getAllTabsCallback);
    });
}

function getAllTabsCallback(tabs) {
    var tabList = {'@type': 'window', 'tabs': []};
    tabs.forEach(function (tab, i) {
        tabList.tabs[tabList.tabs.length] = {id: tab.id,title:tab.title,audible:tab.audible,mutedInfo:tab.mutedInfo,index:tab.index,highlighted:tab.highlighted};
        tabList.windowId = tab.windowId;
        tabList.windowInfo = superWindowInfo[tab.windowId];
    });
    port.postMessage({text: 'list', list: tabList})
}
