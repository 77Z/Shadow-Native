type TargetOutputType = "StaticLib" | "Executable" | "DynamicLib";

export interface Target {
	Name: string;
	PrettyName: string;
	Type: TargetOutputType;
	Language: "C++" | "C";
	BaseDir: string;
	Artifact: string;

	std: string;
	Defines: string[];
	CXXFLAGS: string[];

	IfDebug: {
		Defines: string[];
		CXXFLAGS: string[];
	};
	IfRelease: {
		Defines: string[];
		CXXFLAGS: string[];
	};

	IncludeDirs: string[];

	Sources: string[];
	SrcExcludes: string[];
	LinkerFlags: string[];
	InlineBuildLibs: string[];
	Libs: string[];
	PostBuild: {
		Symlinks: string[][];
	};
}

export interface ShadersConfig {
	Enabled: boolean;
	EmbedShaders: boolean;
	ShadersLocation: string;
	Compiler: string;
	IncludeDirs: string[];
}

export interface BuildFile {
	ProductName: string;
	Version: string;
	LocalVersion: string;
	CC: string;
	CXX: string;
	AR: string;
	CompilerCaching: boolean;
	BuildDir: string;
	DebugBuild: boolean;
	// deno-lint-ignore no-explicit-any
	EngineConfiguration: Array<Array<any>>;
	Shaders: ShadersConfig;
	Targets: Target[];
}
