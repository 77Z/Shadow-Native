import { BuildFile } from "./Interfaces.ts";
import { HORDE_VERSION } from "./Product.ts";
import { mkIfNotExist } from "./Utilities.ts";
import { cliFlags, ProductVersion } from "./main.ts";

function isWindows() {
	if (Deno.build.os === "windows") return true;
	return false;
}

export async function generateShadowEngineConfig(conf: BuildFile) {
	await mkIfNotExist("./include/generated");

	let output = `/*
* Horde version ${HORDE_VERSION}
* Automatically generated file
* Shadow Engine Build Configuration
*/

// Shadow version info
#define SHADOW_VERSION_STRING "${ProductVersion}"
#define SHADOW_VERSION_MAJOR ${ProductVersion.split(".")[0]}
#define SHADOW_VERSION_MINOR ${ProductVersion.split(".")[1]}
#define SHADOW_VERSION_PATCH ${ProductVersion.split(".")[2]}

#define HORDE_VERSION_STRING "${HORDE_VERSION}"
#define HORDE_VERSION_MAJOR ${HORDE_VERSION.split(".")[0]}
#define HORDE_VERSION_MINOR ${HORDE_VERSION.split(".")[1]}
#define HORDE_VERSION_PATCH ${HORDE_VERSION.split(".")[2]}
#define HORDE_PROJECT_PWD "${isWindows() ? Deno.cwd().replaceAll("\\", "/") : Deno.cwd()}"

#define SHADOW_COMMIT_HASH "${new TextDecoder().decode(new Deno.Command("git", { args: ['rev-parse', 'HEAD'] }).outputSync().stdout).replaceAll('\n', '')}"

`;

	for (const entry of conf.EngineConfiguration) {
		const key = entry[0];
		const val = entry[1];

		if (key.toUpperCase() != key) {
			console.log(
				`%cWARNING: configuration key "${key}" breaks the all uppercase convention`,
				"color: yellow",
			);
		}

		output += `#define CONFIG_${key} `;

		switch (typeof val) {
			case "boolean":
				output += val ? "1" : "0";
				break;
			case "string":
				output += `"${val.replaceAll("\\", "\\\\")}"`;
				break;
			case "number":
			case "bigint":
			case "symbol":
			case "undefined":
			case "object":
			case "function":
			default:
				output += val;
		}

		output += "\n";
	}

	Deno.writeTextFileSync("./include/generated/autoconf.h", output);

	console.log("Generated engine configuration");

	if (cliFlags.confgen) Deno.exit();
}
