function wildcardMatch(str: string, rule: string) {
	const escapeRegex = (str: string) =>
		str.replace(/([.*+?^=!:${}()|\[\]\/\\])/g, "\\$1");
	return new RegExp("^" + rule.split("*").map(escapeRegex).join(".*") + "$")
		.test(str);
}

console.log(wildcardMatch(
	"lib/imgui/examples/example_win32_directx12/main.cpp",
	"lib/imgui/*.cpp",
));
