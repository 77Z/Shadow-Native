import { parse } from "https://deno.land/std@0.192.0/flags/mod.ts";
import {
	exists,
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
import { generateShadowEngineConfig } from "./ShadowConfigGen.ts";
import { generateShaderBuildFiles } from "./Shaders.ts";

export const cliFlags = parse(Deno.args, {
	boolean: ["help", "verbose", "version", "v", "clean", "confgen", "noshaders", "dumpToJson", "dumpBuildDir"],
	string: ["config", "target"],
});

if (cliFlags.help) showHelp();

if (cliFlags.version || cliFlags.v) {
	printVersion();
	Deno.exit();
}

const configFileName = cliFlags.config ? cliFlags.config : "./build.json5";
const conf = JSON5.parse(Deno.readTextFileSync(configFileName)) as BuildFile;

const localVersion = conf.LocalVersion ? "-" + conf.LocalVersion : "";
export const ProductVersion = conf.Version + localVersion;
const CC: string = conf.CC;
const CXX: string = conf.CXX;
const AR: string = conf.AR;
const compilerCacheString = conf.CompilerCaching ? "ccache " : "";
export const debugBuild: boolean = conf.DebugBuild;
const buildDir = formatUnicorn(`./bin/${conf.BuildDir}`, {
	version: ProductVersion,
	cc: conf.CC,
	cxx: conf.CXX,
	dbg: debugBuild ? "debug" : "release",
});

if (cliFlags.dumpBuildDir) {
	PRINT(buildDir);
	Deno.exit();
}

PRINT(`${conf.ProductName} ver. ${ProductVersion}`);

if (cliFlags.clean) {
	Deno.removeSync(buildDir, { recursive: true });
	PRINT("Deleted BuildDir: " + buildDir);
	Deno.exit();
}

if (cliFlags.dumpToJson) {
	Deno.writeTextFileSync("dumped.json", JSON.stringify(conf));
	PRINT("Dumped build files to JSON");
	Deno.exit();
}

await mkIfNotExist(buildDir);

await Promise.all([
	generateShadowEngineConfig(conf),
	generateShaderBuildFiles(conf, buildDir, cliFlags.noshaders),
]);

interface MakefileTarget {
	name: string;
	prettyName: string;
}
const makefileTargets: MakefileTarget[] = [];
let topLevelMakefileContent = `#
# Horde version ${HORDE_VERSION}
# Automatically generated makefile
# Run "make help" for usage info
#

NINJA=ninja

`;

for (const target of conf.Targets) {
	if (cliFlags.target !== undefined && cliFlags.target !== target.Name) {
		continue;
	}

	const targetDir = buildDir + "/" + target.Name;
	const artifactDest = targetDir + "/" + target.Artifact;

	await mkIfNotExist(targetDir);

	let ninjaTargetContent = "";

	PRINT(
		`Generating build files for ${target.PrettyName} in ${
			debugBuild ? "DEBUG" : "RELEASE"
		} mode`,
	);

	ninjaTargetContent += `# Build mode: ${debugBuild ? "DEBUG" : "RELEASE"}\n\n`;

	ninjaTargetContent += `# Compiler vars
flags = ${gatherFlags(target, false)}
ccflags = ${gatherFlags(target, true)}

`;

	ninjaTargetContent += `# Rules
rule cc
 command     = ${compilerCacheString}${CC} $ccflags -c -o $out $in
 description = Compiling C object $out

rule cxx
 command     = ${compilerCacheString}${CXX} $flags -c -o $out $in
 description = Compiling C++ object $out

rule static_link
 command     = ${AR} $flags $out $in $libs
 description = Linking static target $out

rule link
 command     = ${compilerCacheString}${CXX} -o $out $in $libs $flags
 description = Linking target $out

rule regenerate_build
 command     = ../../../Horde --config ${configFileName} --target ${target.Name}
 description = Regenerating build files

# Build files
`;

	if (conf.SelfRegeneration) {
		ninjaTargetContent += `
build build.ninja: regenerate_build ../../../${configFileName}

`
	}

	const sources: string[] = await gatherSources(target);
	const objs: string[] = [];

	for (const source of sources) {
		const swappedExt = replaceFileExtension(source, ".o");
		let obj: string;
		if (target.BaseDir == ".") {
			obj = swappedExt;
		} else {
			obj = swappedExt.slice((target.BaseDir + "/").length);
		}

		objs.push(obj);

		
		//TODO: temporary patch
		const cOrCpp = source.substring(source.lastIndexOf(".")) == ".cpp" ? "cxx" : "cc"
		ninjaTargetContent += `build ${obj} : ${
			cOrCpp
		} ../../../${source}\n`;

		/* ninjaTargetContent += `build ${obj} : ${
			target.Language == "C++" ? "cxx" : "cc"
		} ../../../${source}\n`; */
	}

	switch (target.Type) {
		case "StaticLib": {
			ninjaTargetContent += `
# Link library

build ../../../${artifactDest}: static_link ${objs.join(" ")}
 flags = -rcs

`;
			break;
		}
		case "Executable": {
			const inlineBuildLibs: string[] = [];
			for (const inlineLib of target.InlineBuildLibs) {
				inlineBuildLibs.push(`../${inlineLib}/lib${inlineLib}.a`);
			}

			ninjaTargetContent += `
# Link executable

build ../../../${artifactDest}: link ${objs.join(" ")}
 libs = ${inlineBuildLibs.join(" ")}  ${target.Libs.join(" ")}
 flags = ${target.LinkerFlags.join(" ")}
`;
			break;
		}
		case "DynamicLib":
		default:
			throw Error("Not implemented");
	}

	Deno.writeTextFileSync(
		`${targetDir}/build.ninja`,
		NINJA_HEADER + ninjaTargetContent,
	);

	makefileTargets.push({
		name: target.Name,
		prettyName: target.PrettyName,
	});

	const PS = target.PostBuild;
	if (PS) {
		PRINT(target.PrettyName + " post-setup routines");
		if (PS.Symlinks) {
			for (const link of PS.Symlinks) {
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

makefileTargets.push({
	name: "Shaders",
	prettyName: "Shaders",
});

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
