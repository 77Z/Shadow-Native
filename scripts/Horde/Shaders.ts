import { recursiveReaddir } from "https://deno.land/x/recursive_readdir@v2.0.0/mod.ts";
import { BuildFile } from "./Interfaces.ts";
import {
	formatUnicorn,
	mkIfNotExist,
	NINJA_HEADER,
	PRINT,
} from "./Utilities.ts";

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

	Deno.writeTextFileSync(
		shaderOutDir + "/build.ninja",
		NINJA_HEADER +
			``,
	);

	PRINT(shaders);
}
