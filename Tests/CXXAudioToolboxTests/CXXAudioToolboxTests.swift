import Testing
@testable import CXXAudioToolbox

@Suite struct CXXCoreAudioTests {
	@Test func audioConverter() async {
		let converter = CXXAudioToolbox.ATAudioConverter()
		#expect(converter.IsValid() == false)
	}

	@Test func audioFile() async {
		let af = CXXAudioToolbox.ATAudioFile()
		#expect(af.IsValid() == false)
	}

	@Test func auGraph() async {
		let graph = CXXAudioToolbox.ATAUGraph()
		#expect(graph.IsValid() == false)
	}
}
