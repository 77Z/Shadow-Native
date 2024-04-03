const {app, BrowserWindow} = require("electron");
const fs = require("fs");
const nativeLink = require("./nativeLink/build/Release/AXENativeLink");

app.disableHardwareAcceleration();

function createWindow() {
	const window = new BrowserWindow({
		width: 800,
		height: 600,
		show: false,
		webPreferences: {
			nodeIntegration: true,
			contextIsolation: false,
			offscreen: true
		}
	})

	window.loadURL("https://77z.dev");
	window.webContents.on("paint", (e, dirty, image) => {
		// fs.writeFileSync("out.png", image.toPNG());
		
	});

	window.webContents.setFrameRate(200);
}

app.whenReady().then(() => {
	nativeLink.foobar();
	createWindow();
})

app.on("window-all-closed", () => {
	app.quit();
})