import { BuildFile } from "./Interfaces.ts";
import { HORDE_VERSION } from "./Product.ts";
import { mkIfNotExist } from "./Utilities.ts";
import { cliFlags } from "./main.ts";

export async function generateShadowEngineConfig(conf: BuildFile) {
	await mkIfNotExist("./include/generated");

	let output = `/*
* Horde version ${HORDE_VERSION}
* Automatically generated file
* Shadow Engine Build Configuration
*/

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
				output += `"${val}"`;
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
