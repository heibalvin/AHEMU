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
            path: ".",
            exclude: ["main.cpp"],
            sources: [
                "gbcpu.cpp",
                "gbdsk.cpp",
                "gbemu.cpp",
                "gbppu.cpp",
                "sdlemu.cpp"
            ],
            cxxSettings: [
                .headerSearchPath(".")
            ],
            linkerSettings: [
                .linkedFramework("SDL3")
            ]
        ),
    ]
)