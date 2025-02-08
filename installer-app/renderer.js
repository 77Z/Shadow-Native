const AUTH_SERVER_URL = "http://localhost:8787";

fetch(AUTH_SERVER_URL + "/api/v1/versionInfo").then(async r => {
	const data = await r.json();

	const prettyDate = new Date(data.releaseDate).toDateString();

	document.getElementById("ver").innerText = data.latestVersion;
	document.getElementById("daterel").innerText = "Released " + prettyDate;

	console.log(prettyDate);
	console.log(data.latestVersion);

})