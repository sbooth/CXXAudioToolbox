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
        let converter = audio_toolbox.CAAudioConverter()
        #expect(converter.__convertToBool() == false)
    }

    @Test func audioFile() async {
        let af = audio_toolbox.CAAudioFile()
        #expect(af.__convertToBool() == false)
    }

    @Test func extAudioFile() async {
        let eaf = audio_toolbox.CAExtAudioFile()
        #expect(eaf.__convertToBool() == false)
    }

    @Test func auGraph() async {
        let graph = audio_toolbox.CAAUGraph()
        #expect(graph.__convertToBool() == false)
    }
}
