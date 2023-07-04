const VERSION = "0.1.0";

import JSON5 from "https://deno.land/x/json5@v1.0.0/mod.ts";
import { parse } from "https://deno.land/std@0.192.0/flags/mod.ts";
import { recursiveReaddir } from "https://deno.land/x/recursive_readdir/mod.ts";

function formatUnicorn(input: string, args: Record<string, string>) {
	for (const arg in args) {
		input = input.replace(new RegExp(`\\{${arg}\\}`, "gi"), args[arg]);
	}
	return input;
}

const exists = async (filename: string): Promise<boolean> => {
	try {
		await Deno.stat(filename);
		return true;
	} catch (error) {
		if (error instanceof Deno.errors.NotFound) {
			return false;
		} else {
			throw error;
		}
	}
};

async function mkIfNotExist(path: string) {
	if (!await exists(path)) Deno.mkdirSync(path);
}

function wildcardMatch(str: string, rule: string) {
	const escapeRegex = (str: string) =>
		str.replace(/([.*+?^=!:${}()|\[\]\/\\])/g, "\\$1");
	return new RegExp("^" + rule.split("*").map(escapeRegex).join(".*") + "$")
		.test(str);
}

const cliFlags = parse(Deno.args, {
	boolean: ["help", "verbose", "version"],
	string: ["config"],
});

if (cliFlags.version) {
	console.log("Horde Build System version " + VERSION);
	Deno.exit();
}

let configFileLoc = "./config.json5";
if (cliFlags.config) configFileLoc = cliFlags.config;

const configFile = await Deno.readTextFile(configFileLoc);
const conf = JSON5.parse(configFile);

// deno-lint-ignore no-explicit-any
function PRINT(text: any) {
	if (!cliFlags.verbose) return;
	console.log(text);
}

const localVersion = conf.LocalVersion ? "-" + conf.LocalVersion : "";
const ProductVersion = conf.Version + localVersion;
const CC: string = conf.CC;
const CXX: string = conf.CXX;
const debugBuild: boolean = conf.Debug.DebugBuild;

console.log(`${conf.ProductName} ver. ${ProductVersion}`);

const buildDir = formatUnicorn(`../bin/${conf.BuildDir}`, {
	version: ProductVersion,
	cc: conf.CC,
	cxx: conf.CXX,
});

await mkIfNotExist(buildDir);

// deno-lint-ignore no-explicit-any
async function gatherSources(target: any) {
	const sources = [];
	const files = await recursiveReaddir(target.BaseDir);
	for (const file of files) {
		for (const source of target.Sources) {
			if (wildcardMatch(file.slice((target.BaseDir + "/").length), source)) {
				sources.push(file);
			}
		}

		for (const source of target.SrcExcludes) {
			if (wildcardMatch(file.slice((target.BaseDir + "/").length), source)) {
				PRINT(`Excluding ${file} from sources`);
				const index = sources.indexOf(file);
				if (index !== 1) sources.splice(index, 1);
			}
		}
	}
	return sources;
}

for (let i = 0; i < conf.targets.length; i++) {
	const target = conf.targets[i];
	const targetDir = buildDir + "/" + target.Name;

	await mkIfNotExist(targetDir);

	PRINT(`Compiling ${target.PrettyName}`);
	// for (const item of Deno.readDirSync(target.BaseDir)) {
	// 	PRINT(item);
	// }

	const sources = await gatherSources(target);

	for (const source of sources) {
		const pos = source.lastIndexOf(".");
		const inplaceObj = source.substr(0, pos < 0 ? source.length : pos) + ".o";
		const obj = targetDir + "/" +
			inplaceObj.slice((target.BaseDir + "/").length);

		const CXXFLAGS = [];
		CXXFLAGS.push("-std=" + target.std);
		CXXFLAGS.push(target.CXXFLAGS.join(" "));
		if (target.Defines.length > 0) CXXFLAGS.push("-D" + target.Defines);
		if (debugBuild) {
			CXXFLAGS.push(target.IfDebug.CXXFLAGS);
			CXXFLAGS.push("-D" + target.IfDebug.Defines);
		} else {
			CXXFLAGS.push(target.IfRelease.CXXFLAGS);
			CXXFLAGS.push("-D" + target.IfRelease.Defines);
		}
		for (const include in target.IncludeDirs) {
			CXXFLAGS.push(`-I${target.BaseDir}/${include}`);
		}
		PRINT(`${CXX} -o ${obj} -c ${source} ${CXXFLAGS.join(" ")}`);
	}
}
