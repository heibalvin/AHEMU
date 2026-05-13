//
//  NESEMUHolder.swift
//  AHNESEMUMacOS
//
//  Created by Alvin HEIB on 13/05/2026.
//

import Foundation
import SwiftUI
import Combine

public final class NESEMUHolder: ObservableObject {
    let emu = NESEMU()
    private var running = false
    private var thread: Thread?
    private var cancellable: AnyCancellable?

    @Published var ups: Double = 0.0

    private var updateCount: Int = 0
    private var lastMetricsTime: TimeInterval = 0.0
    private let lock = NSLock()

    func start() {
        lastMetricsTime = CFAbsoluteTimeGetCurrent()

        running = true
        thread = Thread { [weak self] in
            guard let self else { return }
            while self.running {
                let currentTime = CFAbsoluteTimeGetCurrent()
                self.emu.update(currentTime)
                self.lock.lock()
                self.updateCount += 1
                self.lock.unlock()
            }
        }
        thread?.start()

        cancellable = Timer.publish(every: 1.0, on: .main, in: .default)
            .autoconnect()
            .sink { [weak self] _ in
                guard let self else { return }
                let now = CFAbsoluteTimeGetCurrent()
                let deltaTime = now - self.lastMetricsTime

                self.lock.lock()
                let count = self.updateCount
                self.updateCount = 0
                self.lock.unlock()

                self.ups = Double(count) / deltaTime
                self.lastMetricsTime = now
            }
    }

    func stop() {
        running = false
        cancellable = nil
    }
}
