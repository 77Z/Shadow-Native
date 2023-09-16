import { recursiveReaddir } from "https://deno.land/x/recursive_readdir@v2.0.0/mod.ts";
import { Target } from "./Interfaces.ts";
import { HORDE_VERSION } from "./Product.ts";
import { cliFlags, debugBuild } from "./main.ts";

export const NINJA_HEADER = `#
# Horde version ${HORDE_VERSION}
# Automatically generated ninja file
#

ninja_required_version = 1.8.2

`;

// deno-lint-ignore no-explicit-any
export function PRINT(input: any): void {
	console.log(input);
}

// deno-lint-ignore no-explicit-any
export function PRINT_VERBOSE(input: any) {
	if (cliFlags.verbose) console.log(input);
}

export function printVersion() {
	console.log("Horde Build System version " + HORDE_VERSION);
}

export function formatUnicorn(input: string, args: Record<string, string>) {
	for (const arg in args) {
		input = input.replace(new RegExp(`\\{${arg}\\}`, "gi"), args[arg]);
	}
	return input;
}

export const exists = async (filename: string): Promise<boolean> => {
	try {
		await Deno.stat(filename);
		PRINT_VERBOSE(`FILE ${filename} EXISTS`);
		return true;
	} catch (error) {
		if (error instanceof Deno.errors.NotFound) {
			PRINT_VERBOSE(`FILE ${filename} DOES NOT EXIST`);
			return false;
		} else {
			throw error;
		}
	}
};

export async function mkIfNotExist(path: string) {
	if (!await exists(path)) Deno.mkdirSync(path, { recursive: true });
}

export function wildcardMatch(str: string, rule: string) {
	const escapeRegex = (str: string) =>
		str.replace(/([.*+?^=!:${}()|\[\]\/\\])/g, "\\$1");
	return new RegExp("^" + rule.split("*").map(escapeRegex).join(".*") + "$")
		.test(str);
}

export function showHelp() {
	printVersion();
	console.log("\n---- HELP ----");
	console.log(
		`
Options:
--help                : Show this message and exit
--version | -v        : Print the version string and exit
--config [file.json5] : Path to a json5 file that contains the Horde
                        configuration you'd like to use for compiling
--clean               : Deletes BuildDir
--confgen             : Generate engine configuration only`,
	);
	Deno.exit();
}

export function gatherFlags(target: Target): string {
	const flags: string[] = [];

	// C / C++ standard
	flags.push("-std=" + target.std);

	// Compiler flags
	flags.push(target.CXXFLAGS.join(" "));

	// Defines
	for (const def of target.Defines) {
		flags.push("-D" + def);
	}

	// Same thing but for debug/release exclusive args
	if (debugBuild) {
		flags.push(target.IfDebug.CXXFLAGS.join(" "));

		for (const def of target.IfDebug.Defines) {
			flags.push("-D" + def);
		}
	} else {
		flags.push(target.IfRelease.CXXFLAGS.join(" "));

		for (const def of target.IfRelease.Defines) {
			flags.push("-D" + def);
		}
	}

	// Include dirs
	for (const include of target.IncludeDirs) {
		flags.push(`-I../../../${target.BaseDir}/${include}`);
	}

	return flags.join(" ");
}

export async function gatherSources(target: Target): Promise<string[]> {
	const sources = [];
	const files = await recursiveReaddir(target.BaseDir);
	for (const rawfile of files) {
		const file = rawfile.replaceAll("\\", "/");
		// PRINT_VERBOSE(`Found file: ${file}`);
		for (const source of target.Sources) {
			if (
				wildcardMatch(
					target.BaseDir == "."
						? file
						: file.slice((target.BaseDir + "/").length),
					source,
				)
			) {
				sources.push(file);
				PRINT_VERBOSE(`Including file: ${file}`);
			}
		}

		for (const source of target.SrcExcludes) {
			if (
				wildcardMatch(
					target.BaseDir == "."
						? file
						: file.slice((target.BaseDir + "/").length),
					source,
				)
			) {
				PRINT_VERBOSE(`Excluding ${file} from sources`);
				const index = sources.indexOf(file);
				if (index !== -1) sources.splice(index, 1);
			}
		}
	}
	return sources;
}

export function replaceFileExtension(input: string, fileExt: string): string {
	const pos = input.lastIndexOf(".");
	return input.substr(0, pos < 0 ? input.length : pos) + fileExt;
}
