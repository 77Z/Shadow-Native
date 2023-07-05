const VERSION = "0.1.0";

import JSON5 from "https://deno.land/x/json5@v1.0.0/mod.ts";
import { parse } from "https://deno.land/std@0.192.0/flags/mod.ts";
import { recursiveReaddir } from "https://deno.land/x/recursive_readdir@v2.0.0/mod.ts";

interface SrcDestCombo {
	source: string;
	dest: string;
}

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
const AR: string = conf.AR;
const debugBuild: boolean = conf.Debug.DebugBuild;

console.log(`${conf.ProductName} ver. ${ProductVersion}`);

const buildDir = formatUnicorn(`../bin/${conf.BuildDir}`, {
	version: ProductVersion,
	cc: conf.CC,
	cxx: conf.CXX,
	dbg: debugBuild ? "debug" : "release",
});

await mkIfNotExist(buildDir);

// deno-lint-ignore no-explicit-any
async function gatherSources(target: any): Promise<string[]> {
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

// deno-lint-ignore no-explicit-any
function gatherCflags(target: any) {
	const CXXFLAGS = [];
	CXXFLAGS.push("-std=" + target.std);
	CXXFLAGS.push(target.CXXFLAGS.join(" "));
	for (const define of target.Defines) {
		CXXFLAGS.push("-D" + define);
	}
	if (debugBuild) {
		CXXFLAGS.push(target.IfDebug.CXXFLAGS);
		for (const define of target.IfDebug.Defines) {
			CXXFLAGS.push("-D" + define);
		}
	} else {
		CXXFLAGS.push(target.IfRelease.CXXFLAGS);
		for (const define of target.IfRelease.Defines) {
			CXXFLAGS.push("-D" + define);
		}
	}
	for (const include of target.IncludeDirs) {
		CXXFLAGS.push(`-I${target.BaseDir}/${include}`);
	}
	return CXXFLAGS;
}

//Assumes source file does exist
function shouldRebuild(combined: SrcDestCombo): boolean {
	const srcFile = Deno.statSync(combined.source);
	try {
		const destFile = Deno.statSync(combined.dest);
		if (destFile.isFile) {
			// PRINT("Object exists");
			if (srcFile.mtime === null) return true;
			if (destFile.mtime === null) return true;

			if (srcFile.mtime > destFile.mtime) return true;
		}
	} catch (e) {
		if (e instanceof Deno.errors.NotFound) return true;
		else throw e;
	}

	return false;
}

// deno-lint-ignore no-explicit-any
function compile(target: any, args: string[]): boolean {
	// TODO: Make this multithreaded
	// By that, I mean create up to navigator.hardwareConcurrency number of
	// compiler processes and pop a source off the stack until it's all gone.
	// Keep spawning new compiler processes as previous ones finish, but make
	// sure there are no more than navigator.hardwareConcurrency processes

	const programName = target.Language == "C++" ? CXX : CC;

	const compiler = new Deno.Command(programName, {
		args: args,
	});

	const { code, stdout, stderr } = compiler.outputSync();

	console.log(new TextDecoder().decode(stdout));
	console.log(new TextDecoder().decode(stderr));

	if (code !== 0) return false;
	return true;
}

function linkStaticLib(outputLoc: string, objects: string[]): boolean {
	const args = `-rcs ${outputLoc} ${objects.join(" ")}`.split(" ");
	PRINT(args);
	const archiver = new Deno.Command(AR, {
		args: args,
	});

	const { code, stdout, stderr } = archiver.outputSync();

	console.log(new TextDecoder().decode(stdout));
	console.log(new TextDecoder().decode(stderr));

	if (code !== 0) return false;
	return true;
}

for (let i = 0; i < conf.targets.length; i++) {
	const target = conf.targets[i];
	const targetDir = buildDir + "/" + target.Name;
	const binaryLoc = targetDir + "/" + target.Target;

	await mkIfNotExist(targetDir);

	PRINT(`Compiling ${target.PrettyName}`);
	// for (const item of Deno.readDirSync(target.BaseDir)) {
	// 	PRINT(item);
	// }

	const sources: string[] = await gatherSources(target);
	const sourcesAndObjects: SrcDestCombo[] = [];

	const sourcesToBuild: SrcDestCombo[] = [];

	let hasToLink = false;

	for (const source of sources) {
		const pos = source.lastIndexOf(".");
		const inplaceObj = source.substr(0, pos < 0 ? source.length : pos) + ".o";
		const obj: string = targetDir + "/" +
			inplaceObj.slice((target.BaseDir + "/").length);

		const combo: SrcDestCombo = { source: source, dest: obj };
		sourcesAndObjects.push(combo);

		if (shouldRebuild(combo)) {
			sourcesToBuild.push(combo);
		}
	}

	if (sourcesToBuild.length > 0) {
		//Priming for the build step
		for (let l = 0; l < sourcesToBuild.length; l++) {
			const src = sourcesToBuild[l].source;
			const obj = sourcesToBuild[l].dest;

			// Make sure compiler doesn't run into any issues with non-existant
			// directories at this point.
			Deno.mkdirSync(obj.substring(obj.lastIndexOf("/"), 0), {
				recursive: true,
			});
			PRINT(`Building ${src}`);
			const CXXFLAGS = gatherCflags(target);
			PRINT(` -o ${obj} -c ${src} ${CXXFLAGS.join(" ")}`);

			compile(target, `-o ${obj} -c ${src} ${CXXFLAGS.join(" ")}`.split(" "));
		}
		hasToLink = true;

		// Linking
		PRINT(`${AR} -rcs `);
	}

	if (!await exists(binaryLoc)) hasToLink = true;

	if (hasToLink) {
		const objects: string[] = [];
		for (let l = 0; l < sourcesAndObjects.length; l++) {
			objects.push(sourcesAndObjects[l].dest);
		}

		switch (target.Type) {
			case "StaticLib":
				linkStaticLib(binaryLoc, objects);
				break;
			case "DynamicLib":
			case "Executable":
				throw new Error("Not yet implemented!");
			default:
				throw new Error("Invalid target type!");
		}
	} else {
		PRINT("Nothing to do in target " + target.PrettyName);
	}
}
