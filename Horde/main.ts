const VERSION = "0.2.0";

import JSON5 from "https://deno.land/x/json5@v1.0.0/mod.ts";
import { parse } from "https://deno.land/std@0.192.0/flags/mod.ts";
import { recursiveReaddir } from "https://deno.land/x/recursive_readdir@v2.0.0/mod.ts";
import { writeAllSync } from "https://deno.land/std@v0.190.0/streams/mod.ts";

interface SrcDestCombo {
	source: string;
	dest: string;
}

function printVersion() {
	console.log("Horde Build System version " + VERSION);
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
		PRINT(`FILE ${filename} EXISTS`);
		return true;
	} catch (error) {
		if (error instanceof Deno.errors.NotFound) {
			PRINT(`FILE ${filename} DOES NOT EXIST`);
			return false;
		} else {
			throw error;
		}
	}
};

async function mkIfNotExist(path: string) {
	if (!await exists(path)) Deno.mkdirSync(path, { recursive: true });
}

function wildcardMatch(str: string, rule: string) {
	const escapeRegex = (str: string) =>
		str.replace(/([.*+?^=!:${}()|\[\]\/\\])/g, "\\$1");
	return new RegExp("^" + rule.split("*").map(escapeRegex).join(".*") + "$")
		.test(str);
}

const cliFlags = parse(Deno.args, {
	boolean: ["help", "verbose", "version", "v", "clean", "confgen"],
	string: ["config", "threads"],
});

if (cliFlags.help) {
	printVersion();
	console.log("\n---- HELP ----");
	console.log(
		`
Options:
--help                : Show this message and exit
--version | -v        : Print the version string and exit
--config [file.json5] : Path to a json5 file that contains the Horde
                        configuration you'd like to use for compiling
--threads [int]       : Number of concurrent compilations you'd like to 
                        happen at once, defaults to the number of threads 
                        your CPU reports, in your case: ${navigator.hardwareConcurrency}
--clean               : Deletes BuildDir
--confgen             : Generate engine configuration only`,
	);
	Deno.exit();
}

if (cliFlags.version || cliFlags.v) {
	printVersion();
	Deno.exit();
}

const totalPromises = cliFlags.threads
	? Number(cliFlags.threads)
	: navigator.hardwareConcurrency;

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

if (cliFlags.clean) {
	Deno.removeSync(buildDir, { recursive: true });
	console.log("Deleted BuildDir: " + buildDir);
	Deno.exit();
}

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
/* function compile(target: any, args: string[]): boolean {
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
} */

// deno-lint-ignore no-explicit-any
async function compile(target: any, source: SrcDestCombo) {
	const src = source.source;
	const obj = source.dest;

	// Make sure compiler doesn't run into any issues with non-existant
	// directories at this point.
	Deno.mkdirSync(obj.substring(obj.lastIndexOf("/"), 0), {
		recursive: true,
	});

	const CXXFLAGS = gatherCflags(target);
	// PRINT(` -o ${obj} -c ${src} ${CXXFLAGS.join(" ")}`);

	const cmd = `${CXX} -o ${obj} -c ${src} ${CXXFLAGS.join(" ")}`.split(" ");

	const proc = Deno.run({ cmd, stderr: "piped", stdout: "piped" });
	const [stderr, stdout, status] = await Promise.all([
		proc.stderrOutput(),
		proc.output(),
		proc.status(),
	]);
	const ret = {
		cmd: cmd,
		code: status.code,
		stdout: new TextDecoder().decode(stdout).trim(),
		stderr: new TextDecoder().decode(stderr).trim(),
	};

	proc.close();

	writeAllSync(Deno.stdout, stdout);
	writeAllSync(Deno.stdout, stderr);

	return ret;
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

function linkExecutable(
	outputLoc: string,
	objects: string[],
	// deno-lint-ignore no-explicit-any
	target: any,
): boolean {
	const LDFLAGS = [];
	LDFLAGS.push(target.LDFLAGS);
	for (const lib of target.InlineBuildLibs) {
		LDFLAGS.push(`${buildDir}/${lib}/lib${lib}.a`);
	}
	for (const lib of target.Libs) {
		LDFLAGS.push(lib);
	}
	LDFLAGS.push(target.LinkerFlags.join(" "));

	const args = `-o ${outputLoc} ${objects.join(" ")} ${LDFLAGS.join(" ")}`
		.split(" ");

	const linker = new Deno.Command(CXX, {
		args: args.filter((i) => i),
	});

	const { code, stdout, stderr } = linker.outputSync();

	writeAllSync(Deno.stdout, stdout);
	writeAllSync(Deno.stdout, stderr);

	if (code !== 0) return false;
	return true;
}

// Config file generation
{
	await mkIfNotExist("../include/generated");

	let output = "";
	output += `/*
*
* Horde automatically generated file
* Shadow Engine Build Configuration
*
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

	Deno.removeSync("../include/generated/autoconf.h");
	Deno.writeFileSync(
		"../include/generated/autoconf.h",
		new TextEncoder().encode(output),
	);
	console.log("Generated engine configuration file");

	if (cliFlags.confgen) Deno.exit();
}

for (let i = 0; i < conf.targets.length; i++) {
	const target = conf.targets[i];
	const targetDir = buildDir + "/" + target.Name;
	const binaryLoc = targetDir + "/" + target.Target;

	await mkIfNotExist(targetDir);

	console.log(
		`Building target ${target.PrettyName} in ${
			debugBuild ? "DEBUG" : "RELEASE"
		} mode`,
	);
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

	// PRINT("SOURCES TO REBUILD:");
	// PRINT(sourcesToBuild);

	// Dynamic load balancing for compilation

	if (sourcesToBuild.length != 0) {
		hasToLink = true;

		const promises = new Array(totalPromises);

		for (let i = 0; i < totalPromises; i++) {
			promises[i] = (async () => {
				let nextIndex = i;

				while (nextIndex < sourcesToBuild.length) {
					const source = sourcesToBuild[nextIndex];
					const start = performance.now();
					await compile(target, source);
					console.log(
						`Thread ${i} finished ${source.source} in ${
							performance.now() - start
						}ms`,
					);

					// Choose the next index based on the number of promises available
					nextIndex += totalPromises;
				}

				PRINT(`Thread ${i} finished its work`);
			})();
		}

		await Promise.allSettled(promises);
	}

	if (!await exists(binaryLoc)) hasToLink = true;

	if (hasToLink) {
		const objects: string[] = [];
		for (let l = 0; l < sourcesAndObjects.length; l++) {
			objects.push(sourcesAndObjects[l].dest);
		}

		/* PRINT("OBJECTS to link");
		PRINT(objects); */

		switch (target.Type) {
			case "StaticLib":
				linkStaticLib(binaryLoc, objects);
				break;
			case "Executable":
				console.log("Linking executable " + binaryLoc);
				linkExecutable(binaryLoc, objects, target);
				break;
			case "DynamicLib":
				throw new Error("Not yet implemented!");
			default:
				throw new Error("Invalid target type!");
		}
	} else {
		console.log("Nothing to do in target " + target.PrettyName);
	}

	PRINT("Post-install routines");
	if (target.PostInstall) {
		const PI = target.PostInstall;
		if (PI.Symlinks) {
			for (const link of PI.Symlinks) {
				const dest = formatUnicorn(link[1], {
					targetDir: targetDir,
					cwd: Deno.cwd(),
				});

				if (await exists(dest)) continue;

				Deno.symlinkSync(
					formatUnicorn(link[0], {
						targetDir: targetDir,
						cwd: Deno.cwd(),
					}),
					dest,
				);
			}
		}
	}
}
