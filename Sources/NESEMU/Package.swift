// swift-tools-version:5.9
import PackageDescription

let package = Package(
    name: "NESEMU",
    platforms: [
        .macOS(.v11),
        .iOS(.v14),
    ],
    products: [
        .library(
            name: "NESEMU",
            targets: ["NESEMU"]),
    ],
    targets: [
        .target(
            name: "NESEMU",
            path: "."),
    ]
)