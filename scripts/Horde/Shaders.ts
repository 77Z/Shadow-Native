import { recursiveReaddir } from "https://deno.land/x/recursive_readdir@v2.0.0/mod.ts";
import { BuildFile } from "./Interfaces.ts";
import {
	formatUnicorn,
	mkIfNotExist,
	NINJA_HEADER,
	PRINT,
} from "./Utilities.ts";
import { debugBuild } from "./main.ts";

type shaderPlatform =
	| "android"
	| "asm.js"
	| "ios"
	| "linux"
	| "orbis"
	| "osx"
	| "windows";

type shaderProfile =
	| "100_es" // OpenGL ES Shading Language / WebGL (ESSL)
	| "300_es"
	| "310_es"
	| "320_es"
	| "s_3_0" // High-Level Shading Language (HLSL)
	| "s_4_0"
	| "s_5_0"
	| "metal" // Metal Shading Language (MSL)
	| "pssl" // PlayStation Shader Language (PSSL)
	| "spirv" // Standard Portable Intermediate Representation - V (SPIR-V)
	| "spirv10-10"
	| "spirv13-11"
	| "spirv14-11"
	| "spirv15-12"
	| "spirv16-13"
	| "120" // OpenGL Shading Language (GLSL)
	| "130"
	| "140"
	| "150"
	| "330"
	| "400"
	| "410"
	| "420"
	| "430"
	| "440";

type shaderType = "vertex" | "fragment" | "compute";

type shaderOptimization = 0 | 1 | 2 | 3;

export async function generateShaderBuildFiles(
	conf: BuildFile,
	buildDir: string,
) {
	if (!conf.Shaders.Enabled) return;
	PRINT("Generating build files for Shaders");

	const shaderOutDir = buildDir + "/Shaders";
	const shaderInDir = conf.Shaders.ShadersLocation;
	const shaderc = formatUnicorn(conf.Shaders.Compiler, {
		buildDir: buildDir,
	});

	await mkIfNotExist(shaderOutDir);

	const dirItems = await recursiveReaddir(shaderInDir);
	const shaders = dirItems.filter((file) => {
		return (file.includes("vs_") || file.includes("fs_")) &&
			file.endsWith(".sc");
	});

	function shaderTarget(
		targetName: string,
		platform: shaderPlatform,
		profile: shaderProfile,
	): string {
		let ninja = NINJA_HEADER;

		const optimization: shaderOptimization = debugBuild ? 0 : 3;

		const incs: string[] = [];
		for (const include of conf.Shaders.IncludeDirs) {
			incs.push("-i");
			incs.push("../../../" + include);
		}

		ninja += `
platform = ${platform}
profile  = ${profile}
incs     = ${incs.join(" ")}

rule shader
 command   = ../../../${shaderc} --platform $platform -p $profile --varyingdef $vdef $incs --type $type -O ${
			String(optimization)
		} -f $in -o $out
 description = Compiling shader $out

`;

		for (const shader of shaders) {
			const blankDest = shader.substring(
				(conf.Shaders.ShadersLocation + "/").length,
			);

			ninja += `build ${blankDest}.${targetName}.bin : shader ../../../${shader}
 vdef = ../../../${
				shader.substring(shader.lastIndexOf("/"), 0) + "/varying.def.sc"
			}
 type = ${shader.includes("vs_") ? "vertex" : "fragment"}
`;
		}

		Deno.writeTextFileSync(`${shaderOutDir}/${targetName}.ninja`, ninja);

		return `subninja ${targetName}.ninja\n`;
	}

	let subninjas = "";

	subninjas += shaderTarget("glsl", "linux", "120");
	subninjas += shaderTarget("essl", "android", "100_es");
	subninjas += shaderTarget("spv", "linux", "spirv");
	subninjas += shaderTarget("mtl", "ios", "metal");

	if (Deno.build.os == "windows") {
		subninjas += shaderTarget("dx9", "windows", "s_3_0");
		subninjas += shaderTarget("dx11", "windows", "s_5_0");
	}

	Deno.writeTextFileSync(
		shaderOutDir + "/build.ninja",
		NINJA_HEADER + subninjas,
	);
}
