// swift-tools-version: 5.9
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
	name: "CXXAudioToolbox",
	products: [
		// Products define the executables and libraries a package produces, making them visible to other packages.
		.library(
			name: "CXXAudioToolbox",
			targets: [
				"CXXAudioToolbox",
			]
		),
	],
	dependencies: [
		.package(url: "https://github.com/sbooth/CXXCoreAudio", branch: "main")
	],
	targets: [
		// Targets are the basic building blocks of a package, defining a module or a test suite.
		// Targets can depend on other targets in this package and products from dependencies.
		.target(
			name: "CXXAudioToolbox",
			dependencies: [
				"CXXCoreAudio",
			],
			cSettings: [
				.headerSearchPath("include/CXXAudioToolbox"),
			],
			linkerSettings: [
				.linkedFramework("AudioToolbox"),
			]
		),
		.testTarget(
			name: "CXXAudioToolboxTests",
			dependencies: [
				"CXXAudioToolbox",
			],
			swiftSettings: [
				.interoperabilityMode(.Cxx),
			]
		),
	],
	cxxLanguageStandard: .cxx17
)
