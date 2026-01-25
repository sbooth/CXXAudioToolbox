//
// SPDX-FileCopyrightText: 2025 Stephen F. Booth <contact@sbooth.dev>
// SPDX-License-Identifier: MIT
//
// Part of https://github.com/sbooth/CXXAudioToolbox
//

import Testing
@testable import CXXAudioToolbox

@Suite struct CXXCoreAudioTests {
    @Test func audioConverter() async {
        let converter = CXXAudioToolbox.CAAudioConverter()
        #expect(converter.__convertToBool() == false)
    }

    @Test func audioFile() async {
        let af = CXXAudioToolbox.CAAudioFile()
        #expect(af.__convertToBool() == false)
    }

    @Test func extAudioFile() async {
        let eaf = CXXAudioToolbox.CAExtAudioFile()
        #expect(eaf.__convertToBool() == false)
    }

    @Test func auGraph() async {
        let graph = CXXAudioToolbox.CAAUGraph()
        #expect(graph.__convertToBool() == false)
    }
}
