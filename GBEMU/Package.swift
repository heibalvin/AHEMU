// swift-tools-version: 5.10
import PackageDescription

let package = Package(
    name: "GBEMU",
    products: [
        .library(
            name: "GBEMU",
            targets: ["GBEMU"]
        ),
    ],
    targets: [
        .target(
            name: "GBEMU",
            path: "Source",
            exclude: ["main.cpp"],
            sources: [
                "gbcpu.cpp",
                "gbdsk.cpp",
                "gbcomponent.cpp",
                "gbbus.cpp",
                "gbemu.cpp",
                "gbppu.cpp",
                "sdlemu.cpp"
            ],
            publicHeadersPath: "Include",
            cxxSettings: [
                .headerSearchPath("."),
                .headerSearchPath("Include")
            ],
            linkerSettings: [
                .linkedFramework("SDL3")
            ]
        ),
    ]
)