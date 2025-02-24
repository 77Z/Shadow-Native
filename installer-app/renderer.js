//const AUTH_SERVER_URL = "http://localhost:8787";
const AUTH_SERVER_URL = "http://192.168.1.98:8787";
// const AUTH_SERVER_URL = "https://nexus.77z.dev";

const du = document.getElementById("downloadUpdate");

fetch(AUTH_SERVER_URL + "/api/v1/versionInfo").then(async r => {
	const data = await r.json();

	const prettyDate = new Date(data.releaseDate).toDateString();

	document.getElementById("ver").innerText = data.latestVersion;
	document.getElementById("daterel").innerText = "Released " + prettyDate;

	console.log(prettyDate);
	console.log(data.latestVersion);

})


document.getElementById("axeInstaller").addEventListener("click", () => {
	du.innerText = "Preparing to install...";

	document.getElementById("installerBottom").style.transform = "translateY(0)";
})