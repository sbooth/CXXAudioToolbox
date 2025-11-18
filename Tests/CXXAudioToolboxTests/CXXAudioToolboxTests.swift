import Testing
@testable import CXXAudioToolbox

@Suite struct CXXCoreAudioTests {
	@Test func audioConverter() async {
		let converter = CXXAudioToolbox.CAAudioConverter()
		#expect(converter.IsValid() == false)
	}

	@Test func audioFile() async {
		let af = CXXAudioToolbox.CAAudioFile()
		#expect(af.IsValid() == false)
	}

	@Test func extAudioFile() async {
		let eaf = CXXAudioToolbox.CAExtAudioFile()
		#expect(eaf.IsValid() == false)
	}

	@Test func auGraph() async {
		let graph = CXXAudioToolbox.CAAUGraph()
		#expect(graph.IsValid() == false)
	}
}
