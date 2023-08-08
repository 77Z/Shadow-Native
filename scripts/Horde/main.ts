import { parse } from "https://deno.land/std@0.192.0/flags/mod.ts";
import {
	formatUnicorn,
	gatherFlags,
	gatherSources,
	mkIfNotExist,
	NINJA_HEADER,
	PRINT,
	printVersion,
	replaceFileExtension,
	showHelp,
} from "./Utilities.ts";
import JSON5 from "https://deno.land/x/json5@v1.0.0/mod.ts";
import { BuildFile } from "./Interfaces.ts";
import { HORDE_VERSION } from "./Product.ts";
import { Target } from "./old/build.ts";

export const cliFlags = parse(Deno.args, {
	boolean: ["help", "verbose", "version", "v", "clean"],
	string: ["config"],
});

if (cliFlags.help) showHelp();

if (cliFlags.version || cliFlags.v) {
	printVersion();
	Deno.exit();
}

const conf = JSON5.parse(Deno.readTextFileSync(
	cliFlags.config ? cliFlags.config : "./build.json5",
)) as BuildFile;

const localVersion = conf.LocalVersion ? "-" + conf.LocalVersion : "";
const ProductVersion = conf.Version + localVersion;
const CC: string = conf.CC;
const CXX: string = conf.CXX;
const AR: string = conf.AR;
export const debugBuild: boolean = conf.DebugBuild;
const buildDir = formatUnicorn(`./bin/${conf.BuildDir}`, {
	version: ProductVersion,
	cc: conf.CC,
	cxx: conf.CXX,
	dbg: debugBuild ? "debug" : "release",
});

PRINT(`${conf.ProductName} ver. ${ProductVersion}`);

if (cliFlags.clean) {
	Deno.removeSync(buildDir, { recursive: true });
	console.log("Deleted BuildDir: " + buildDir);
	Deno.exit();
}

await mkIfNotExist(buildDir);

interface MakefileTarget {
	name: string;
	prettyName: string;
}
let makefileTargets: MakefileTarget[] = [];
let topLevelMakefileContent = `#
# Horde version ${HORDE_VERSION}
# Automatically generated makefile
# Run "make help" for usage info
#

NINJA=ninja

`;

for (const target of conf.Targets) {
	const targetDir = buildDir + "/" + target.Name;
	const artifactDest = targetDir + "/" + target.Artifact;

	await mkIfNotExist(targetDir);

	let ninjaTargetContent = "";

	PRINT(
		`Generating build files for ${target.PrettyName} in ${
			debugBuild ? "DEBUG" : "RELEASE"
		} mode`,
	);

	ninjaTargetContent += `# Compiler vars
flags = ${gatherFlags(target)}

`;

	ninjaTargetContent += `# Rules [${debugBuild ? "DEBUG" : "RELEASE"}]
rule cc
 command     = ${CC} $flags -c -o $out $in
 description = Compiling C object $out

rule cxx
 command     = ${CXX} $flags -c -o $out $in
 description = Compiling C++ object $out

rule static_link
 command     = ${AR} $flags $out $in $libs
 description = Linking static target $out

rule link
 command     = echo UNFINISHED!!
 description = Linking target $out

# Build files

`;

	const sources: string[] = await gatherSources(target);

	for (const source of sources) {
		const swappedExt = replaceFileExtension(source, ".o");
		let obj: string;
		if (target.BaseDir == ".") {
			obj = swappedExt;
		} else {
			obj = swappedExt.slice((target.BaseDir + "/").length);
		}

		ninjaTargetContent += `build ${obj} : ${
			target.Language == "C++" ? "cxx" : "c"
		} ../../../${source}\n`;
	}

	/* if (target.Type == "Executable") {
		ninjaTargetContent += `# Link executable

build ${artifactDest}: link`;
	} */

	Deno.writeTextFileSync(
		`${targetDir}/build.ninja`,
		NINJA_HEADER + ninjaTargetContent,
	);

	makefileTargets.push({
		name: target.Name,
		prettyName: target.PrettyName,
	});
}

const namesOnly: string[] = [];
for (const mkTarget of makefileTargets) namesOnly.push(mkTarget.name);

topLevelMakefileContent += ".PHONY: all help " + namesOnly.join(" ") + "\n\n";
topLevelMakefileContent += "all: " + namesOnly.join(" ") + "\n\n";

for (const mkTarget of makefileTargets) {
	topLevelMakefileContent += `${mkTarget.name}:
	@echo "========== Building ${mkTarget.prettyName} [${
		debugBuild ? "DEBUG" : "RELEASE"
	}] =========="
	@$(NINJA) -C ${mkTarget.name}

`;
}

topLevelMakefileContent += `help:
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@echo "    all (default)"
`;
for (const mkTarget of makefileTargets) {
	topLevelMakefileContent += `	@echo "    ${mkTarget.name}"\n`;
}

Deno.writeTextFileSync(
	buildDir + "/Makefile",
	topLevelMakefileContent,
);
