const {app, BrowserWindow} = require("electron");
const fs = require("fs");
const nativeLink = require("./nativeLink/AXENativeLink");

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

	// window.loadURL(`${__dirname}/dom/index.html`);
	window.loadURL("https://77z.dev");
	window.webContents.on("paint", (e, dirty, image) => {
		// fs.writeFileSync("out.png", image.toPNG());
		nativeLink.sendFrame(image.toPNG());
	});

	// window.webContents.sendInputEvent({
	// 	type: "mouseDown",
	// 	x: 10,
	// 	y: 10,
	// 	button: "left",
	// })

	window.webContents.setFrameRate(60);
}

app.whenReady().then(() => {
	createWindow();
})

app.on("window-all-closed", () => {
	app.quit();
})