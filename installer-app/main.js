const { app, BrowserWindow } = require('electron')
const path = require('node:path')
// const electronDl = require("electron-dl");

// electronDl();

if (process.platform !== "win32") {
  console.log("77Z Installer only runs on windows");
  process.exit(1);
}

function createWindow () {
  const mainWindow = new BrowserWindow({
    width: 800,
    height: 600,
    webPreferences: {
      
    },
    titleBarStyle: "hidden",
    titleBarOverlay: true,
    darkTheme: true,
    minWidth: 400,
    minHeight: 400,
    alwaysOnTop: true,
    backgroundMaterial: 'tabbed',
    maximizable: false
  })


  mainWindow.setTitleBarOverlay({
    color: "#11111100",
    symbolColor: "#ffffff",
    height: 50
  });
  mainWindow.loadFile('index.html')

}

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
app.whenReady().then(() => {
  createWindow()

  app.on('activate', function () {
    // On macOS it's common to re-create a window in the app when the
    // dock icon is clicked and there are no other windows open.
    if (BrowserWindow.getAllWindows().length === 0) createWindow()
  })
})

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
app.on('window-all-closed', function () {
  if (process.platform !== 'darwin') app.quit()
})

// In this file you can include the rest of your app's specific main process
// code. You can also put them in separate files and require them here.
