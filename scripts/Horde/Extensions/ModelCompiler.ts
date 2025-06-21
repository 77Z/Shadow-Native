import { mkIfNotExist, NINJA_HEADER } from "../Utilities.ts";

export async function generateModelCompilationNinjaFiles() {
	await mkIfNotExist("./bin/models");

	Deno.writeTextFileSync(
		"./bin/models/build.ninja",
		`${NINJA_HEADER}

rule modelc
 command      = ../sdw/geometryc/geometryc -f $in -o $out
 description  = compiling model $out

rule chunker
 command      = ../sdw/chunker-cli/chunker $in -o $out
 description  = packing $out

build out/test.bin : modelc ../../Models/menu.chunk/lobbhPlatform.glb
build out/monkey.bin : modelc ../../Models/menu.chunk/monkey.glb

# Chunker (combine all assets)

build test.chunk : chunker out/test.bin out/monkey.bin

`);
}