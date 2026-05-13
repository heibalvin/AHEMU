import SpriteKit
import Combine

class NESRenderScene: SKScene, ObservableObject {
    private let ppu: NESPPU
    private let screenNode = SKSpriteNode()

    // FPS counter
    @Published private(set) var fps: Double = 0.0

    private var frameCount: Int = 0
    private var lastMetricsTime: TimeInterval = 0.0
    private var cancellable: AnyCancellable?

    init(ppu: NESPPU) {
        self.ppu = ppu
        super.init(size: CGSize(width: ppu.width, height: ppu.height))

        screenNode.position = CGPoint(x: ppu.width / 2, y: ppu.height / 2)
        addChild(screenNode)
    }

    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) not implemented")
    }

    override func didMove(to view: SKView) {
        // Force the PPU rendering layer to match original NTSC NES speed
        view.preferredFramesPerSecond = 60
        
        // Debug overlays to verify your frame rate (Optional)
        view.showsFPS = true
        view.showsNodeCount = true
    }

    func startFPSTimer() {
        lastMetricsTime = CFAbsoluteTimeGetCurrent()
        frameCount = 0

        cancellable = Timer.publish(every: 1.0, on: .main, in: .default)
            .autoconnect()
            .sink { [weak self] _ in
                guard let self else { return }
                let now = CFAbsoluteTimeGetCurrent()
                let deltaTime = now - self.lastMetricsTime

                self.fps = Double(self.frameCount) / deltaTime
                self.frameCount = 0
                self.lastMetricsTime = now
            }
    }

    func stopFPSTimer() {
        cancellable?.cancel()
        cancellable = nil
    }

    override func update(_ currentTime: TimeInterval) {
        // Render every frame (no throttling)
        let texture = SKTexture(data: Data(ppu.getFrameBuffer()), size: size)
        texture.filteringMode = .nearest
        screenNode.texture = texture
        screenNode.size = CGSize(width: ppu.width, height: ppu.height)

        // Count frames for FPS calculation
        frameCount += 1
    }
}
